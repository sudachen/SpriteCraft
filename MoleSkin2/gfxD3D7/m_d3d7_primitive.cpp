
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
#include "m_d3d7_device.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$gfxD3D7")
#endif

namespace gfxD3D7 {

static inline
D3DPRIMITIVETYPE PrimitiveToD3D(GFX3D_PRIMITIVE p)
{
  switch(p){
  case GFX3D_TRISTRIP: return D3DPT_TRIANGLESTRIP;
  case GFX3D_TRIFAN:   return D3DPT_TRIANGLEFAN;
  case GFX3D_TRILIST:  return D3DPT_TRIANGLELIST;
  default:  
    return D3DPT_FORCE_DWORD;
//    retval_because("gfxd3d7: unknown primitive",D3DPT_FORCE_DWORD);
;  }
}

inline
widestring FormatToString(unsigned fmt)
{
  widestring format;
  if ( fmt & GFX3D_VxB_VECTOR  )  format += "V";
  if ( fmt & GFX3D_VxB_NORMAL  )  format += "N";
  if ( fmt & GFX3D_VxB_TEXTURE )  format += "T";
  if ( fmt & GFX3D_VxB_STATIC  )  format += "s";
  if ( fmt & GFX3D_VxB_DYNAMIC )  format += "d";
  return format;
}

inline
unsigned FormatToD3D(unsigned fmt)
{
  unsigned format = 0;
  if ( fmt & GFX3D_VxB_VECTOR  )  format |= D3DFVF_XYZ;
  if ( fmt & GFX3D_VxB_NORMAL  )  format |= D3DFVF_NORMAL;
  else format |= D3DFVF_DIFFUSE;
  format |= D3DFVF_TEX1;
  return format;
}

inline 
int SizeOfVERTEX(unsigned format) {
  int size = 3*sizeof(float);
  if ( format & GFX3D_VxB_NORMAL   ) size += 3*sizeof(float);
  if ( format & GFX3D_VxB_COLOR    ) size += 4;
  if ( format & GFX3D_VxB_TEXTURE  ) size += 2*sizeof(float);
  return size;
}

inline 
int ROUND32(int x) {
  return (x+32-1)&+0x01f;
}

enum { MAX_VERTICES_DRAW = 4000 };
static u32_t g_local_vertices_buffer_24[6*MAX_VERTICES_DRAW];

static void FillInternalVxB2_(const VxB_VERTEX* vertices,unsigned count,unsigned flags,void* lock_mem)
{
  if ( flags & GFX3D_VxB_COLOR )
    __asm {
      mov esi,vertices
      mov edi,lock_mem
      mov edx,count
    lb_loop:
      mov eax,[esi]
      mov ecx,[esi+4]
      mov ebx,[esi+8]
      mov [edi], eax
      mov [edi+4],ecx
      mov eax, [esi+12]
      mov [edi+8],ebx
      mov ecx, [esi+16]
      mov ebx, [esi+20]
      mov [edi+16],ecx
      add esi,32
      mov [edi+20],ebx
      mov ecx,eax
      and eax,0x00ff00ff
      and ecx,0xff00ff00
      ror eax,16
      add edi,24
      or  eax,ecx
      dec edx
      mov [edi-12],eax
      jne lb_loop
    }
  else
    __asm {
      mov esi,vertices
      mov edi,lock_mem
      mov edx,count
    lb_loop1:
      mov eax,[esi]
      mov ecx,[esi+4]
      mov ebx,[esi+8]
      mov [edi], eax
      mov [edi+4],ecx
      mov eax, [esi+12]
      mov [edi+8],ebx
      mov ecx, [esi+16]
      mov [edi+12],0xffffffff
      mov ebx, [esi+20]
      mov [edi+16],ecx
      add esi,32
      mov [edi+20],ebx
      add edi,24
      dec edx
      jne lb_loop1
    }
}

enum { VX_SIZE = 24, VX_COUNT = 256 };

SCERROR GfxD3D7Device::FillInternalVxB_(const VxB_VERTEX* vertices,unsigned count,unsigned flags)
{
  HRESULT hr;
  byte_t* lock_mem;
//create_vxb:
  if ( !internal_vxb_ ) {
    long vxb_size    = VX_SIZE*VX_COUNT;
    long vxbmaxcount = VX_COUNT;
    D3DVERTEXBUFFERDESC vxbds = {0};
    vxbds.dwSize = sizeof(vxbds);
//    vxbds.dwCaps = D3DVBCAPS_WRITEONLY|D3DVBCAPS_DONOTCLIP|D3DVBCAPS_SYSTEMMEMORY;
    vxbds.dwCaps = D3DVBCAPS_WRITEONLY|D3DVBCAPS_SYSTEMMEMORY;
    vxbds.dwFVF  = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
    vxbds.dwNumVertices = vxbmaxcount;
    fail_if_dxerr ( id3d_->CreateVertexBuffer(&vxbds,&+internal_vxb_,0) );
    internal_max_vx_ = vxbmaxcount;
  }
  if ( internal_max_vx_ < count ) 
    fail_because( _S*"gfxd3d7: maximum available count is %? but requested %?" %internal_max_vx_ %count);
lock_vxb:
  hr = internal_vxb_->Lock(DDLOCK_WRITEONLY|DDLOCK_WAIT|DDLOCK_DISCARDCONTENTS,(void**)&lock_mem,0);
  internal_vxflags_ = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
  if ( SUCCEEDED(hr) ) {
//    if ( flags & GFX3D_VxB_COLOR ) {
//      for ( unsigned i = 0; i < count; ++i, lock_mem+=/*32*/24,vertices++ ) {
//        memcpy(lock_mem,vertices,3*4);
//        u32_t tmpcolor = vertices->color.C();
//        u32_t r = tmpcolor&0x00ff00ff;
//        *(u32_t*)(lock_mem+12) = (tmpcolor&0xff00ff00) | (r>>16) | (r<<16);
//        memcpy(lock_mem+16,&vertices->ts,2*4);
//        //memcpy(lock_mem+24,&vertices->ts1,2*4);
//      }
//    }else{
//      for ( unsigned i = 0; i < count; ++i, lock_mem+=32,vertices++ ) {
//        memcpy(lock_mem,vertices,3*4);
//        *(u32_t*)(lock_mem+12) = 0xffffffff;
//        memcpy(lock_mem+16,&vertices->ts,2*4);
//        //memcpy(lock_mem+24,&vertices->ts1,2*4);
//      }
//    }
    FillInternalVxB2_(vertices,count,flags,lock_mem);
    fail_if_dxerr( internal_vxb_->Unlock() );
  } 
  else if ( hr == DDERR_SURFACEBUSY ) {
    goto lock_vxb;
  }
  else fail_if_dxerr(hr);
  return SCE_OK;
}

/**
  Устанавливает текущий вертекс буффер
*/
SCERROR GfxD3D7Device::SetVxB(VxB* vxb){
  // проверим что это наш вертекс буффер
  GfxD3D7VxB* gfxvxb = Adaptate<GfxD3D7VxB>(vxb);
  if ( gfxvxb && gfxvxb->GetServer() != this ) 
    fail_because( "gfxd3d7: invalid vertices buffer" );
  // если устанновлен текущий вертекс буффер и это не тот-же самый,
  // что устанавливаем сейчас
  // то снимаем его и устанавливаем текущий
  if ( locked_vxb_ != gfxvxb ) {
    //if ( !IsNull(locked_vxb_) ) UnlockVertices(locked_vxb_);
    //if ( gfxvxb ) LockVertices(gfxvxb);
    Reset(locked_vxb_,Grab(gfxvxb));
  }
  return SCE_OK;
}

/**
  Отрисовывает текущий вертекс буффер
*/
SCERROR GfxD3D7Device::DrawVxB(GFX3D_PRIMITIVE p,int first,int count)
{
  if ( IsNull(locked_vxb_) ) 
    fail_because("gfxd3d7: vertices buffer is not set");
  if ( first < 0 || first >= locked_vxb_->count_ ) 
    fail_because("gfxd3d7: invalid first value");
  if ( count < 0 ) count = locked_vxb_->count_-first;
  fail_if ( count+first > locked_vxb_->count_ ); 
//  fail_if_err( 
//    DrawVertices(
//      p,&locked_vxb_->vertices_[0],first,count,locked_vxb_->format_,0,count) );
  return SCE_OK;
}

/**
  Отрисовывает текущий вертекс буффер по индексу вертексов
*/
SCERROR GfxD3D7Device::DrawIndexedVxB(
  GFX3D_PRIMITIVE p, const u16_t* idx,int count, unsigned vxfirst, unsigned vxcount)
{
  if ( IsNull(locked_vxb_) ) 
    fail_because("gfxd3d7: vertices buffer is not set");
  if ( idx == 0  ) 
    fail_because("gfxd3d7: invalid idx value");
  if ( count < 0 || count >= 1024*64 ) 
    fail_because("gfxd3d7: invalid count value");
//  UpdateStates();
  if ( vxcount == 0 ) vxcount = locked_vxb_->count_-vxfirst;
//  fail_if_err( 
//    DrawVertices(
//      p,&locked_vxb_->vertices_[0],vxfirst,count,locked_vxb_->format_,idx,vxcount) );
  return SCE_OK;
}


/**
  Отрисовывает массив вертексов
*/
SCERROR GfxD3D7Device::DrawPrimitive(
  GFX3D_PRIMITIVE p,const VxB_VERTEX* vertices,int count,unsigned flags)
{
//  SetVxB(0);
//  fail_if_fail(FillInternalVxB_(vertices,count,flags));
//  fail_if_dxerr( idd_->DrawPrimitiveVB(PrimitiveToD3D(p),
//    ~internal_vxb_,0,count,0) );
  if ( count > MAX_VERTICES_DRAW ) fail_because("gfxd3d: vertices limit is exceed");
  FillInternalVxB2_(vertices,count,flags,g_local_vertices_buffer_24);
  fail_if_dxerr( 
    idd_->DrawPrimitive(
      PrimitiveToD3D(p),
      D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,
      g_local_vertices_buffer_24,count,0) );
  return SCE_OK;
}

SCERROR GfxD3D7Device::DrawIndexedPrimitive(
  GFX3D_PRIMITIVE p,const VxB_VERTEX* vertices,unsigned vxcount,const u16_t* idx,int count,unsigned flags)
{
//  SetVxB(0);
//  fail_if_fail(FillInternalVxB_(vertices,vxcount,flags));
//  fail_if_dxerr( idd_->DrawIndexedPrimitiveVB(PrimitiveToD3D(p),
//    ~internal_vxb_,0,vxcount,(u16_t*)idx,count,0) );
  if ( vxcount > MAX_VERTICES_DRAW ) fail_because("gfxd3d: vertices limit is exceed");
  FillInternalVxB2_(vertices,vxcount,flags,g_local_vertices_buffer_24);
  fail_if_dxerr( 
    idd_->DrawIndexedPrimitive(
      PrimitiveToD3D(p),
      D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,
      g_local_vertices_buffer_24,vxcount,(u16_t*)idx,count,0) );
  return SCE_OK;
}

VxB* GfxD3D7Device::CreateDirtyVxB(int count,unsigned flags)
{
  if ( count <= 0 ) ret0_because( "gfxd3d7: count must be great then 0" );
  if ( (flags & GFX3D_VxB_STATIC) && (flags & GFX3D_VxB_DYNAMIC) )
    ret0_because("gfxd3d7:: vxb can't be dynamic and static at the same time");
  if ( (flags & GFX3D_VxB_NORMAL) && (flags & GFX3D_VxB_COLOR) )
    ret0_because("gfxd3d7: vxb can't contains normal and color at the same time");
  return new GfxD3D7VxB(this,count,flags);
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///
/// GfxD3D7VxB implemetation
///
/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

pchar_t const GfxD3D7VxB::TypeID__ = "#TYPEDEF#gfxD3D7#GfxD3D7VxB";
void* GfxD3D7VxB::Adaptate_(const void* id){
  if ( id == TypeID__ ) return this;
  if ( void* x = VxB::Adaptate_(id) ) return x;
  if ( void* y = GfxD3D7Object::Adaptate_(id) ) return y;
  return 0;
}

bool        GfxD3D7VxB::IsLost() const    { return  !vertices_;}
void        GfxD3D7VxB::Release_() const  { refbase::Release_(); }
void        GfxD3D7VxB::Grab_() const     { refbase::Grab_(); }
int         GfxD3D7VxB::Count() const     { return count_; }

SCERROR GfxD3D7VxB::Drop() { 
  Unrefe(vertices_); 
  return SCE_OK; 
}
void        GfxD3D7VxB::OnTermContext()   {/*Drop();*/}
void        GfxD3D7VxB::OnInitContext()   {}

GfxD3D7VxB::GfxD3D7VxB(GfxD3D7Device* device,int count,unsigned flags)
: GfxD3D7Object(device), format_(flags), count_(count)
{
  REQUIRE ( count_ > 0 );
//  vertices_ = AutoArr(new VxB_VERTEX[count_]);
//  memset( Get(vertices_),0, sizeof(VxB_VERTEX)*count_);
}

GfxD3D7VxB::~GfxD3D7VxB() {Drop();}

widestring GfxD3D7VxB::ToString_() const { 
  return widestring(
      _S*L"GFXD3D7_VxB(%?) {%?,%?}" %(VxB*)this %FormatToString(format_) %count_);
}

SCERROR GfxD3D7VxB::Restore()  { 
//  GfxD3D7Device* gfx = GetServer();
//  if ( gfx && Get(gfx->locked_vxb_) == this ) UnlockVertices(this);
  
//  bool arb_object = 
//    (GfxD3D7Device::EXTENTIONS & vertex_buffer_object_EXT) &&
//    (format_ & (GFX3D_VxB_STATIC|GFX3D_VxB_DYNAMIC)) ;
//
//  if ( arb_object ) {
//    if ( !objname_ || !GfxD3D7Device::IsBufferARB(objname_) ) {
//      fail_if_glerr( GfxD3D7Device::GenBuffersARB(1,&objname_) );
//      unsigned vertex_object_type = 
//        format_ & GFX3D_VxB_STATIC ? GL_STATIC_DRAW_ARB : GL_DYNAMIC_DRAW_ARB;
//      fail_if_glerr( GfxD3D7Device::BindBufferARB(GL_ARRAY_BUFFER_ARB,objname_) );
//      fail_if_glerr( 
//        GfxD3D7Device::BufferDataARB(
//          GL_ARRAY_BUFFER_ARB,ROUND32(SizeOfVERTEX(format_)*count_),0,vertex_object_type) );
//    }
//  }else{
//    if ( !vertices_ ) vertices_ = AutoArr(new VxB_VERTEX[count_]); 
//  }

  return SCE_OK;
}

SCERROR GfxD3D7VxB::Fill(
  const VxB_VERTEX* vertices, int first, int count)
{
//  enum { FILL_BUFFER_LENGTH = 1000 };
//  static byte_t fill_buffer[sizeof(VxB_VERTEX)*FILL_BUFFER_LENGTH];
//
//  fail_if_fail ( first >= 0 && first < count_ );
//  if ( count == -1 ) count = count_-first;
//  fail_if_fail ( count >= 0 && count <= count_-first );
// 
//  fail_if_err( Restore() );
//  
//  bool arb_object = 
//    (GfxD3D7Device::EXTENTIONS & vertex_buffer_object_EXT) &&
//    (format_ & (GFX3D_VxB_STATIC|GFX3D_VxB_DYNAMIC) );
//    
//
//  if ( arb_object )
//  {  
//    int fb_first = first;
//    int fb_count = count;
//    int fb_idx   = 0;
//    int vx_size = SizeOfVERTEX(format_);
//
//    for ( ; fb_idx < teggo_min<int>(FILL_BUFFER_LENGTH,count) ; ++fb_idx )
//    {
//      byte_t* p = fill_buffer+fb_idx*vx_size;
//      int offs = 3*sizeof(float);
//      memcpy(p,&vertices[fb_idx].vx,sizeof(float)*3);
//      if ( format_ & GFX3D_VxB_NORMAL) {
//        memcpy(p+offs,&vertices[fb_idx].nx,sizeof(float)*3);
//        offs+=sizeof(float)*3;
//      }else if ( format_ & GFX3D_VxB_COLOR) {
//        memcpy(p+offs,&vertices[fb_idx].color,4);
//        offs+=4;
//      }
//      if ( format_ & GFX3D_VxB_TEXTURE) {
//        memcpy(p+offs,&vertices[fb_idx].color,sizeof(float)*2);
//        offs+=sizeof(float)*2;
//      }
//    }
//
//  }else{
//    if ( !vertices_ ) vertices_ = AutoArr(new VxB_VERTEX[count_]); 
//    GfxD3D7Device* gfx = GetServer();
//    if ( gfx && Get(gfx->locked_vxb_) == this ) UnlockVertices(this);
//    memset( Get(vertices_),0, sizeof(VxB_VERTEX)*count_);
//    copy(vertices,vertices+count,vertices_+first);
//  }
  return SCE_OK;
}

} // namespace

