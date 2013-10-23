
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

#if !defined _GFXNOJPG

#include "tffmedia.h"
#pragma code_seg(".text$tffmedia")

extern "C" {
#include "lib.jpeg/jinclude.h"
#include "lib.jpeg/jpeglib.h"
#include "lib.jpeg/jerror.h"
}

#include <setjmp.h>

static jmp_buf tffmedia_jpeg_ljmpbuf;

struct TFFt_JPG_SOURCE 
  {
    struct jpeg_source_mgr pub; /* public fields */
    DataStream *infile;    /* source stream */
    JOCTET *buffer;    /* start of buffer */
    boolean start_of_file;  /* have we gotten any data yet? */
  };

enum { INPUT_BUF_SIZE = 4096  /* choose an efficiently fread'able size */ };

static void TFF__InitJpegSource (j_decompress_ptr cinfo)
  {
    TFFt_JPG_SOURCE *src = (TFFt_JPG_SOURCE *)cinfo->src;

    /* We reset the empty-input-file flag for each image,
     * but we don't clear the input buffer.
     * This is correct behavior for reading a series of images from one source.
     */
    src->start_of_file = TRUE;
  }

static boolean TFF__FillJpegBuffer(j_decompress_ptr cinfo)
  {
    TFFt_JPG_SOURCE *src = (TFFt_JPG_SOURCE *)cinfo->src;
    size_t nbytes;

    nbytes = src->infile->Read((byte_t*)src->buffer, INPUT_BUF_SIZE);

    if (nbytes <= 0) 
      {
        if (src->start_of_file) /* Treat empty input file as fatal error */
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

static void TFF__SkipJpegData(j_decompress_ptr cinfo, long num_bytes)
  {
    TFFt_JPG_SOURCE *src = (TFFt_JPG_SOURCE *)cinfo->src;

    /* Just a dumb implementation for now.  Could use fseek() except
     * it doesn't work on pipes.  Not clear that being smart is worth
     * any trouble anyway --- large skips are infrequent.
     */
    if (num_bytes > 0) 
      {
        while (num_bytes > (long) src->pub.bytes_in_buffer) 
          {
            num_bytes -= (long) src->pub.bytes_in_buffer;
            (void)TFF__FillJpegBuffer(cinfo);
            /* note we assume that fill_input_buffer will never return FALSE,
             * so suspension need not be handled.
             */
          }
        src->pub.next_input_byte += (size_t) num_bytes;
        src->pub.bytes_in_buffer -= (size_t) num_bytes;
      }
  }

static void TFF__TermJpegSource(j_decompress_ptr cinfo)
  {
    /* no work necessary here */
  }

static void TFF__JpegIO(j_decompress_ptr cinfo, DataStream *infile)
  {
    TFFt_JPG_SOURCE *src;

    if (cinfo->src == NULL) 
      { /* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
          (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
              SIZEOF(TFFt_JPG_SOURCE));
        src = (TFFt_JPG_SOURCE*) cinfo->src;
        src->buffer = (JOCTET *)
          (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
              INPUT_BUF_SIZE * SIZEOF(JOCTET));
      }

    src = (TFFt_JPG_SOURCE *)cinfo->src;
    src->pub.init_source = &TFF__InitJpegSource;
    src->pub.fill_input_buffer = &TFF__FillJpegBuffer;
    src->pub.skip_input_data = &TFF__SkipJpegData;
    src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->pub.term_source = &TFF__TermJpegSource;
    src->infile = infile;
    src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
    src->pub.next_input_byte = NULL; /* until buffer loaded */
  }

static void TFF__JpegRaiseError(j_common_ptr) 
  {
    longjmp(tffmedia_jpeg_ljmpbuf,-1);
  }

struct TFFt_PICTURE_JPEG : TFFt_PICTURE
  {
    rcc_ptr<DataStream> is_;

    u32_t img_width_;
    u32_t img_height_;
    BufferT<byte_t> row_;
    int stride_;
    bool info_loaded_;
    long bpp_;

    bool decompressing_started;
    jpeg_error_mgr err_mgr;
    jpeg_decompress_struct cinfo;


    TFFt_CopyRowXtoX copyf8_;

    TFFt_PICTURE_JPEG(rcc_ptr<DataStream> const &ds);
    virtual ~TFFt_PICTURE_JPEG();
    
    bool ReadPicInfo();

    virtual TFFMt_E GetInfo(TFFt_PICTURE::INFO *);
    virtual TFFMt_E NextRow8(rgba8_t *row,int shift,int cnt);
  };

TFFt_PICTURE_JPEG::TFFt_PICTURE_JPEG(rcc_ptr<DataStream> const &ds) 
  : is_(ds), info_loaded_(false)
  {
    decompressing_started = false;
    memset(&err_mgr,0,sizeof(err_mgr));
    memset(&cinfo,0,sizeof(cinfo));
    cinfo.err = jpeg_std_error(&err_mgr);
    err_mgr.error_exit = &TFF__JpegRaiseError;
  }

TFFt_PICTURE_JPEG::~TFFt_PICTURE_JPEG()
  {
    if ( setjmp(tffmedia_jpeg_ljmpbuf) )
      Lerr << _S*"[MEDIA ERROR] failed to dispose JPEG decoder of '%?'" %is_->Identifier();
    jpeg_destroy_decompress(&cinfo);
  }

TFFMt_E TFFt_PICTURE_JPEG::GetInfo(TFFt_PICTURE::INFO *picnfo)
  {
    if ( info_loaded_ )
      {
        picnfo->alphabits = false;
        picnfo->bpp       = 3*8;
        picnfo->frames    = 1;
        picnfo->height    = this->img_height_;
        picnfo->width     = this->img_width_;
        picnfo->palsize   = 0;
        picnfo->updown    = 1;
        return TFFM_OK;
      }
    else
      return TFFM_FAIL;
  }

bool TFFt_PICTURE_JPEG::ReadPicInfo()
  {
    if ( setjmp(tffmedia_jpeg_ljmpbuf) )
      {
        return false;
      }

    jpeg_create_decompress(&cinfo);
    TFF__JpegIO(&cinfo,+is_);
    jpeg_read_header(&cinfo,TRUE);
    jpeg_start_decompress(&cinfo);
    decompressing_started = true;
    img_width_ = cinfo.output_width;
    img_height_ = cinfo.output_height;
    stride_ = img_width_*3; 
    info_loaded_ = true;

    copyf8_ = TffmCopyRowXtoX(TFF_PF_RGB8|TFF_PF_BGR,TFF_PF_RGBA8);

    return true;
  }

TFFMt_E TFFt_PICTURE_JPEG::NextRow8(rgba8_t *row,int shift,int cnt)
  {
    row_.Resize(stride_);
    byte_t *r[] = {&row_[0]};

    if ( cnt+shift > img_width_ )
      return TFFM_BADARGUMENT;

    if ( setjmp(tffmedia_jpeg_ljmpbuf) )
      {
        Lerr << _S*"[MEDIA ERROR] failed to read JPEG row from '%?'" %is_->Identifier();
        return TFFM_FAIL;
      }

    jpeg_read_scanlines(&cinfo,r,1);
    if (cinfo.output_scanline == cinfo.output_height) 
      {
        jpeg_finish_decompress(&cinfo);
        decompressing_started = false;
      }

    byte_t *p = &row_[0]+shift*3;
    copyf8_(p,row,cnt);

    return TFFM_OK;
  }

mem_ptr<TFFt_PICTURE> CXX_STDCALL TffmJpgCreatePicFrom( rcc_ptr<DataStream> const &ds )
  { 
    mem_ptr<TFFt_PICTURE_JPEG> bmp = MemPtr( new TFFt_PICTURE_JPEG(ds));
    if ( bmp->ReadPicInfo() )
      return mem_ptr<TFFt_PICTURE>(bmp.forget_());
    else
      return mem_ptr<TFFt_PICTURE>(0);
  }

#endif // _GFXNOJPG
