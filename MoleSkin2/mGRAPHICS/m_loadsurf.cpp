
/*

Copyright В© 2003-2013, AlexГ©y SudachГ©n, alexey@sudachen.name

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
#include "m_graphics.h"
#include "../mcore/m_time.h"
#include "../mstreams/m_igzstream.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

//typedef vector<LPFN_IMAGING> ImagingVec;
//static Singleton<ImagingVec> imaging_extensions;
//
//SCERROR MOLE_FUNCALL AddImagingEXT(LPFN_IMAGING cb) {
//  fail_if ( cb == 0 );
//  ImagingVec* v = imaging_extensions.Instance();
//  ImagingVec::iterator i = teggo::linear_find(v->begin(),v->end(),cb);
//  if ( i == v->end() ) v->push_back(cb);
//  return SCE_OK;
//}
//
//SCERROR MOLE_FUNCALL RemoveImagingEXT(LPFN_IMAGING cb) {
//  fail_if ( cb == 0 );
//  ImagingVec* v = imaging_extensions.Instance();
//  ImagingVec::iterator i = teggo::linear_find(v->begin(),v->end(),cb);
//  if ( i != v->end() ) v->erase(i);
//  return SCE_OK;
//}

SCERROR MOLE_FUNCALL LoadSurfaceFrom(ImageDataSource* ds,AutoSurface& surf,unsigned fmt) 
{
  fail_if ( ds == 0 );

  //XDbg(u32_t tm = GetMillis());
#if defined MSKIN_DEBUG
  u32_t tm = GetMillis();
#endif
  
  BufferT<byte_t>  rowbits;
  BufferT<u16_t> c5x5_pal;
  ImageDataSource::ImageInfo info;
  unsigned fmtbits;
  bool     alphalayer   = false;
  bool     reverse_img  = false;
  bool     shortgreen   = false;

  fail_if_err(ds->ReadPicInfo(info));

  if ( IsNull(surf) ) {
    if ( Surface::GET_PIXFORMAT(fmt) == Surface::AUTODETECT ) {
      switch ( info.bpp ) {
      case 32:
      case 24: fmt = Surface::SET_PIXFORMAT(fmt,Surface::RGBx32); break;
      case 16: fmt = Surface::SET_PIXFORMAT(fmt,Surface::RGB5x5); break;
      case 8:  fmt = Surface::SET_PIXFORMAT(fmt,Surface::PAL); break;
      default: 
        fail_if("INVALID SURFACE FORMAT");
      }
    }
    fail_if_fail(surf = AutoRef(SCREEN::CreateDirtySurface(info.width,info.height,fmt)));
  }

  fmtbits     = Surface::GET_PIXFORMAT(surf->GetFormat());
  shortgreen  = (Surface::SHORTGREEN & surf->GetFormat()) != 0;
  reverse_img = (surf->GetFormat() & Surface::DOWNUP) != 0; 
  
  fail_if_fail ( 
    fmtbits == Surface::RGB5x5 || 
    fmtbits == Surface::RGBx32 || 
    fmtbits == Surface::PAL || 
    fmtbits == Surface::ALPHA );
  
  if ( info.bpp != 8 &&  fmtbits == Surface::PAL ) 
  {
    Lerr << _S*"surfloader: '%?' : non paletted image can't be loaded on paletted surface" %ds->GetName();
    return SCE_FAIL;
  }

  // сурфейс имеет дополнительный слой для альфаканала
  if ( fmtbits != Surface::ALPHA )
    alphalayer = (surf->GetFormat()&Surface::ALPHALAYER) != 0 && fmtbits != Surface::RGBx32;

  if ( info.bpp == 8 ) {
    if ( fmtbits == Surface::RGB5x5 ) {
      c5x5_pal.resize(256);
      if ( !shortgreen ) {
        for ( unsigned i = 0 ; i < teggo_min<unsigned>(256,info.pal.size()); ++i )
          c5x5_pal[i] = xxx_to_c565(info.pal[i]);
      }else{
        for ( unsigned i = 0 ; i < teggo_min<unsigned>(256,info.pal.size()); ++i )
          c5x5_pal[i] = xxx_to_c555(info.pal[i]);
      }
    }
    else if (fmtbits == Surface::RGBx32) {
      for ( unsigned i = 0 ; i < teggo_min<unsigned>(256,info.pal.size()); ++i )
        info.pal[i].A = 255;
    }
  }

  {
    rect_t surf_rect = surf->GetRect();
    int count = teggo_min<int>(surf_rect.Width(),info.width); 
    if ( count == 0 ) return SCE_OK;
    
    SurfLock lsurf(surf,Surface::DONOTRESTORECONTENT); 
    SurfLock lasurf;
    if ( alphalayer ) {
      if ( MultiLayerSurface* mls = Adaptate<MultiLayerSurface>(surf) ) {
        lasurf.Lock(mls->GetAlphaSurface(),Surface::DONOTRESTORECONTENT);
      }
    }
    
    int stride,astride;   
    byte_t *mem,*amem;

    mem = (byte_t*)lsurf->GetMemory(stride);
    amem = (byte_t*)lasurf->GetMemory(astride);

    int surf_row_step;
    int surf_row;
    int rows_number = teggo_min<long>(info.height,surf_rect.Height());
    if ( info.updown != reverse_img ) {
      surf_row = 0;
      surf_row_step = 1;
    }else{
      surf_row = rows_number-1;
      surf_row_step = -1;
    }

    if ( info.bpp == 8 && fmtbits == Surface::PAL ) {
      surf->SetPal(info.pal);
    }

    for ( int i_row = 0; i_row < rows_number; ++i_row, surf_row += surf_row_step ) {

      fail_if_fail(eof2ok(ds->ReadRow(rowbits)));
     
      if ( info.bpp == 32 ) {
        const mrgba_t* pixels = (const mrgba_t*)&rowbits[0];
        if ( fmtbits == Surface::RGB5x5 ) {
          u16_t* d =  (u16_t*)(mem+surf_row*stride);
          u16_t* const dE = d+count;
          if ( amem ) {
            byte_t* da =  (byte_t*)(amem+surf_row*astride);
            if ( shortgreen ) 
              for ( ; d < dE ; ++pixels ) {
                *d++= xxx_to_c555( *pixels );*da++ = (*pixels).A;
              }
            else
              for ( ; d < dE ; ++pixels ) {
                *d++= xxx_to_c565( *pixels );*da++ = (*pixels).A;
              }
          }else {
            if ( shortgreen ) 
              while ( d < dE ) *d++ = xxx_to_c555( *pixels++ );
            else
              while ( d < dE ) *d++ = xxx_to_c565( *pixels++ );
          }
        }else if ( fmtbits == Surface::RGBx32 ) {
          mrgba_t* d =  (mrgba_t*)(mem+surf_row*stride);
          //mrgba_t* const dE = d+count;
          //for ( ; d < dE ;++d,++pixels) *d = *pixels;
/* !!!! Intel specific */
          memcpy( d, pixels, count*4 );
        }else if ( fmtbits == Surface::ALPHA ) {
          byte_t* d =  (byte_t*)(mem+surf_row*stride);
          byte_t* const dE = d+count;
          while ( d < dE ) *d++ = (*pixels++).A;
        }else if ( fmtbits == Surface::PAL ) {
          retval_if ("UNSUPPRTED FOR PALETTED SURFACE",SCE_FAIL);
        }
      }
      else if ( info.bpp == 24 ) {
        const mrgb_t* pixels = (const mrgb_t*)&rowbits[0];
        if ( fmtbits == Surface::RGB5x5 ) {
          u16_t* d =  (u16_t*)(mem+surf_row*stride);
          u16_t* const dE = d+count;
          if ( shortgreen ) 
            while ( d < dE ) *d++ = xxx_to_c555( *pixels++ );
          else
            while ( d < dE ) *d++ = xxx_to_c565( *pixels++ );
        }else if ( fmtbits == Surface::RGBx32 ) {
          mrgba_t* d =  (mrgba_t*)(mem+surf_row*stride);
          mrgba_t* const dE = d+count;
          while ( d < dE ) *d++ = xxx_to_mrgba(*pixels++);
          //memcpy(d,pixels,count*3);
        }else if ( fmtbits == Surface::ALPHA ) {
          byte_t* d =  (byte_t*)(mem+surf_row*stride);
          byte_t* const dE = d+count;
          while ( d < dE ) *d++ = (*pixels++).R;
        }else if ( fmtbits == Surface::PAL ) {
          retval_if ("UNSUPPRTED FOR PALETTED SURFACE",SCE_FAIL);
        }
      }
      else if ( info.bpp == 16 ) {
        const u16_t* pixels = (const u16_t*)&rowbits[0];
        if ( fmtbits == Surface::RGB5x5 ) {
          u16_t* d =  (u16_t*)(mem+surf_row*stride);
          u16_t* const dE = d+count;
          if ( info.shortgreen == shortgreen )
            while ( d < dE ) *d++ = ReadShort_((byte_t*)(pixels++));
          else if ( info.shortgreen ) 
            while ( d < dE ) *d++ = c555_to_c565(ReadShort_((byte_t*)(pixels++)));
          else
            while ( d < dE ) *d++ = c565_to_c555(ReadShort_((byte_t*)(pixels++)));
          //memcpy( d, pixels, count*2 ); 
        }else if ( fmtbits == Surface::RGBx32 ) {
          mrgba_t* d =  (mrgba_t*)(mem+surf_row*stride);
          mrgba_t* const dE = d+count;
          if ( info.shortgreen ) 
            while ( d < dE ) *d++ = c555_to_mrgba( ReadShort_((byte_t*)(pixels++)) );
          else
            while ( d < dE ) *d++ = c565_to_mrgba( ReadShort_((byte_t*)(pixels++)) );
        }else if ( fmtbits == Surface::ALPHA ) {
          byte_t* d =  (byte_t*)(mem+surf_row*stride);
          byte_t* const dE = d+count;
          if ( info.shortgreen )
            while ( d < dE ) *d++ = c555_to_mrgba( ReadShort_((byte_t*)(pixels++)) ).R;
          else
            while ( d < dE ) *d++ = c565_to_mrgba( ReadShort_((byte_t*)(pixels++)) ).R;
        }else if ( fmtbits == Surface::PAL ) {
          retval_if ("UNSUPPRTED FOR PALETTED SURFACE",SCE_FAIL);
        }
      }
      else if ( info.bpp == 8 ) {
        const byte_t* pixels = (const byte_t*)&rowbits[0];
        if ( fmtbits == Surface::RGB5x5 ) {
          u16_t* d =  (u16_t*)(mem+surf_row*stride);
          u16_t* const dE = d+count;
          u16_t* palx = &c5x5_pal[0];
          while ( d < dE ) *d++ = palx[*pixels++];
        }else if ( fmtbits == Surface::RGBx32 ) {
          mrgba_t* d =  (mrgba_t*)(mem+surf_row*stride);
          mrgba_t* const dE = d+count;
          mrgba_t* x32_pal = &info.pal[0];
          while ( d < dE ) *d++ = x32_pal[ *pixels++ ];
        }else if ( fmtbits == Surface::PAL ) {
          byte_t* d =  (byte_t*)(mem+surf_row*stride);
          memcpy ( d, pixels, count );
        }else if ( fmtbits == Surface::ALPHA ) {
          byte_t* d =  (byte_t*)(mem+surf_row*stride);
          byte_t* const dE = d+count;
          mrgba_t* x32_pal = &info.pal[0];
          while ( d < dE ) *d++ = x32_pal[ *pixels++ ].R;
        }
      }
    }
  }

  Xdbg( Ldbg << _S*"surfloader: loading %? in %? millis" %(ds->GetName()) %(GetMillis()-tm) );
  return SCE_OK;
}

// общее правило - Make* всегда увеличивает счетчик сылок аргумета, конструктор никогда
ImageDataSource* MOLE_FUNCALL MakeImageDataSourceFrom(DataSource* source) 
{
  ret0_if ( !source );
  u32_t magic = source->PeekLong();
  retval_if ( source->PeekError() != SCE_OK , 0);
  if ( (magic&0x0ffff) == 0x4d42 ) 
  {
    return MakeBMPImageDataSource(source);
  }
  else
  if ( magic == 0x31464154 ) 
  {
    return MakeTAFImageDataSource(source);
  }
  else if ( (magic&0x0ffff) == 0x8b1f) 
  {
    auto_ref<DataSource> gz_source 
      (new StreamedDataSource(new GZipInputStream(Grab(source)),source->GetName()));
    return MakeImageDataSourceFrom(gz_source);
  }
  else{
    widestring sname = source->GetName();
    if ( sname.EndsWith(L".tga") || sname.EndsWith(L".tga.gz") )
    {
      return MakeTGAImageDataSource(source);
    }
    else
    if ( sname.EndsWith(L".jpg") 
        || sname.EndsWith(L".jpg.gz") 
        || sname.EndsWith(L".jpeg")
        || sname.EndsWith(L".jpeg.gz") )
    {
      return MakeJPGImageDataSource(source);
    }
    else
    if ( sname.EndsWith(L".png") || sname.EndsWith(L".png.gz") )
    {
      return MakePNGImageDataSource(source);
    }
    else{
      //ImagingVec* v = imaging_extensions.Instance();
      //ImagingVec::iterator i = v->begin(), iE = v->end();
      //for ( ; i != iE ; ++i ) {
      //  ImageDataSource* ids = (*i)(source);
      //  if ( ids ) return ids;
      //}
      Lerr << _S*":SurfaceLoader: invalid content format of %?" %source->GetName();
      return 0;
    }
  }
}

SCERROR MOLE_FUNCALL LoadSurfaceFrom(DataSource* source,AutoSurface& surf,unsigned fmt)
{
  auto_ref<ImageDataSource> imgs ( MakeImageDataSourceFrom(source) );
  fail_if ( IsNull( imgs) );
  return LoadSurfaceFrom(imgs,surf,fmt);
}

SCERROR Surface::Load_(Surface* surf,DataSource* source)
{
  retval_if( !surf, SCE_INVALIDARG);
  retval_if( !source , SCE_BAD_STREAM );
  AutoSurface asurf ( Grab(surf) );
  return LoadSurfaceFrom(source,asurf,Surface::AUTODETECT);
}

Surface* MOLE_FUNCALL CreateSurfaceFrom(pwide_t resname,unsigned fmt)
{
  auto_ref<DataSource> source( RSCMAN::CreateSource(resname) );
  retval_if( IsNull(source) , 0 );
  return CreateSurfaceFrom(source,fmt);
}

Surface* MOLE_FUNCALL CreateSurfaceFrom(DataSource* source,unsigned fmt)
{
  AutoSurface surf;
  retval_if( !source , 0 );
  retval_if ( LoadSurfaceFrom(source,surf,fmt) != SCE_OK, 0 );
  return Forget(surf);
}

SCERROR MOLE_FUNCALL LoadSurfaceFrom(DataSource* source,Surface* surf)
{
  AutoSurface asurf ( Grab(surf) );
  return LoadSurfaceFrom(source,asurf,Surface::AUTODETECT);
}

SCERROR MOLE_FUNCALL LoadSurfaceFrom(pwide_t source,Surface* surf)
{
  AutoDataSource ds( RSCMAN::CreateSource(source) );
  return LoadSurfaceFrom(ds,surf);
}

SCERROR MOLE_FUNCALL LoadSurfaceFrom(pwide_t source,AutoSurface& surf,unsigned format)
{
  AutoDataSource ds( RSCMAN::CreateSource(source) );
  return LoadSurfaceFrom(ds,surf,format);
}

NAMESPACE_MSKIN_END
