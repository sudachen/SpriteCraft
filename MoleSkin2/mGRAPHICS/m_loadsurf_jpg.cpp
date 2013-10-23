
/*

Copyright © 2003-2013, Alexéy Sudachén, alexey@sudachen.name

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/


#include "precompile.h"
#include "m_surface.h"
#include "m_loadsurf.h"

extern "C" {
#include "../../Media/lib.jpeg/jinclude.h"
#include "../../Media/lib.jpeg/jpeglib.h"
#include "../../Media/lib.jpeg/jerror.h"
}


#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

typedef struct {
  struct jpeg_source_mgr pub;	/* public fields */

  DataSource * infile;		/* source stream */
  JOCTET * buffer;		/* start of buffer */
  boolean start_of_file;	/* have we gotten any data yet? */
} mskinjpg_source_mgr;

typedef mskinjpg_source_mgr * mskinjpg_src_ptr;

#define INPUT_BUF_SIZE  4096	/* choose an efficiently fread'able size */

static void mskinjpg_init_source (j_decompress_ptr cinfo)
{
  mskinjpg_src_ptr src = (mskinjpg_src_ptr) cinfo->src;

  /* We reset the empty-input-file flag for each image,
   * but we don't clear the input buffer.
   * This is correct behavior for reading a series of images from one source.
   */
  src->start_of_file = TRUE;
}

static boolean mskinjpg_fill_input_buffer(j_decompress_ptr cinfo)
{
  mskinjpg_src_ptr src = (mskinjpg_src_ptr) cinfo->src;
  size_t nbytes;

  nbytes = src->infile->Read((byte_t*)src->buffer, INPUT_BUF_SIZE);

  if (nbytes <= 0) {
    if (src->start_of_file)	/* Treat empty input file as fatal error */
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
    WARNMS(cinfo, JWRN_JPEG_EOF);
    /* Insert a fake EOI marker */
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nbytes;
  src->start_of_file = FALSE;

  return TRUE;
}


static void mskinjpg_skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  mskinjpg_src_ptr src = (mskinjpg_src_ptr) cinfo->src;

  /* Just a dumb implementation for now.  Could use fseek() except
   * it doesn't work on pipes.  Not clear that being smart is worth
   * any trouble anyway --- large skips are infrequent.
   */
  if (num_bytes > 0) {
    while (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      (void) mskinjpg_fill_input_buffer(cinfo);
      /* note we assume that fill_input_buffer will never return FALSE,
       * so suspension need not be handled.
       */
    }
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}


static void mskinjpg_term_source (j_decompress_ptr cinfo)
{
  /* no work necessary here */
}


static void mskinjpg_io_src (j_decompress_ptr cinfo, DataSource * infile)
{
  mskinjpg_src_ptr src;

  if (cinfo->src == NULL) {	/* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  SIZEOF(mskinjpg_source_mgr));
    src = (mskinjpg_src_ptr) cinfo->src;
    src->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  INPUT_BUF_SIZE * SIZEOF(JOCTET));
  }

  src = (mskinjpg_src_ptr) cinfo->src;
  src->pub.init_source = mskinjpg_init_source;
  src->pub.fill_input_buffer = mskinjpg_fill_input_buffer;
  src->pub.skip_input_data = mskinjpg_skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = mskinjpg_term_source;
  src->infile = infile;
  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = NULL; /* until buffer loaded */
}


#define __JPEG_SYNC__
static const char ERROR__jpeg_common[] = ":JPGImageDataSource: JPEG Error";
static void mskinjpg_RaiseError(j_common_ptr) {
  __debug_break__();
  Lerr << ERROR__jpeg_common;
  ETH_THROW0(ERROR__jpeg_common);
}

class JPGImageDataSource : public ImageDataSource {
  u32_t img_width_;
  u32_t img_height_;
  BufferT<byte_t> rowbits_;
  int stride_;
  int row_no_;
  bool info_loaded_;
  bool updown_;
  bool leftright_;
  bool decompressing_started;
	jpeg_error_mgr err_mgr;
	jpeg_decompress_struct cinfo;
  SCERROR ReadNextRow(byte_t* row);
  SCERROR ReadHeader();
public:
  virtual SCERROR ReadPicInfo (ImageInfo&);
  virtual SCERROR ReadRow(BufferT<byte_t>& row);
  JPGImageDataSource(DataSource* ds) : ImageDataSource(ds) , stride_(0), row_no_(0), info_loaded_(false) {
    decompressing_started = false;
    memset(&err_mgr,0,sizeof(err_mgr));
    memset(&cinfo,0,sizeof(cinfo));
		cinfo.err = jpeg_std_error(&err_mgr);
    err_mgr.error_exit = mskinjpg_RaiseError;
  }
  virtual ~JPGImageDataSource() {
    __ETH_BEGIN
    jpeg_destroy_decompress(&cinfo);
    __ETH_END
  }
};

SCERROR JPGImageDataSource::ReadPicInfo (ImageInfo& info) {
  if ( !info_loaded_ ) {
    __JPEG_SYNC__;
    fail_if_fail(ReadHeader());
		fail_if ( cinfo.output_components != 3 );
  }

  info.width   = img_width_;
  info.height  = img_height_;
  info.updown  = updown_;
  info.stride  = stride_;
  info.bpp     = 24;
  info.palsize = 0; 
  info.shortgreen = false;
  info.alphabits  = false;
  info.pal.clear();

  return SCE_OK;
}

SCERROR JPGImageDataSource::ReadHeader()
{
  __ETH_BEGIN
	jpeg_create_decompress(&cinfo);
	mskinjpg_io_src(&cinfo,this);
	jpeg_read_header(&cinfo,TRUE);
  jpeg_start_decompress(&cinfo);
  decompressing_started = true;
  img_width_ = cinfo.output_width;
  img_height_ = cinfo.output_height;
  updown_ = true;
  stride_ = img_width_*3; 
  __ETH_END;
  if ( __eth_caught() ) fail_because( __eth_message() );
  return SCE_OK;
}

SCERROR JPGImageDataSource::ReadNextRow(byte_t* row)
{
  __ETH_BEGIN
	jpeg_read_scanlines(&cinfo,&row,1);
  if (cinfo.output_scanline == cinfo.output_height) {
  	jpeg_finish_decompress(&cinfo);
    decompressing_started = false;
  }
  __ETH_END
  if ( __eth_caught() ) fail_because( __eth_message() );
  return SCE_OK;
}

SCERROR JPGImageDataSource::ReadRow(BufferT<byte_t>& row) {
  reterr_if ( eof2ok(this->PeekError()) );
  retval_if ( row_no_ >= signed(img_height_) , SCE_EOF ); 
  row.resize(stride_);

  {
    __JPEG_SYNC__;
    fail_if_fail(ReadNextRow(&row[0]));
  }

  ++row_no_;
  return this->PeekError();
}

ImageDataSource* MOLE_FUNCALL MakeJPGImageDataSource(DataSource* ds) {
  return new JPGImageDataSource(Grab(ds));
}

NAMESPACE_MSKIN_END
