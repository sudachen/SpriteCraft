
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

#include "stdafx.h"
#include "flat_e3.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$flat$display")
#endif

static Gfx3DDevice* g_gfx;
static float g_screen_width;
static float g_screen_height;
static u32_t g_render_iteration;
static bool g_blendstage;

static bool g_screen_offset = false;
static float g_screen_offset_x = 0;
static float g_screen_offset_y = 0;

enum { FLAG_ADDITIVE = 1, FLAG_FILTERING = 2, FLAG_SOLIDSPRITE = 4 };

struct RendBuffer
{
//  enum { MAX_VERTICES = 256, MAX_INDICES = MAX_VERTICES*3, MAX_RBUFFERS=1 };
  enum { MAX_VERTICES = 512, MAX_INDICES = MAX_VERTICES*3, MAX_RBUFFERS=8 };
  VxB_VERTEX  vertices_[MAX_VERTICES];
  u16_t    indices_[MAX_INDICES];
  Texture*    texture_;
  u32_t     vertices_count_;
  u32_t     indices_count_;
  u32_t     flags_;
  static RendBuffer*  rbuffers_;//[MAX_RBUFFERS];
  static float       rbuff_curr_z_;
  static RendBuffer* rbuff_curr_;
  static void        NeedSpace(int vx,int ix);
  static void        ApplaySpace(int vx,int ix);
  static void        SelectBuffer(Texture*,float z,u32_t flags);
  static VxB_VERTEX* GetVerticesPtr();
  static u16_t*   GetIndicesPtr();
  static u16_t    GetVerticesCount();
  static void        Init();
  static void        Flush();
  void Flush_();
};

static RendBuffer RendBuffer_rbuffers_[RendBuffer::MAX_RBUFFERS];
RendBuffer* RendBuffer::rbuffers_ = RendBuffer_rbuffers_;
float       RendBuffer::rbuff_curr_z_;
RendBuffer* RendBuffer::rbuff_curr_;

void RendBuffer::Init()
{
  rbuff_curr_ = 0;
  rbuff_curr_z_ = 1;
  for ( int i=0; i < MAX_RBUFFERS; ++i ) {
    rbuffers_[i].indices_count_ = 0;
    rbuffers_[i].vertices_count_ = 0;
    rbuffers_[i].texture_ = 0;
    rbuffers_[i].flags_ = 0;
  }
}

void RendBuffer::Flush()
{
  for ( int i = 0; i < MAX_RBUFFERS; ++i )
    rbuffers_[i].Flush_();
}

VxB_VERTEX* RendBuffer::GetVerticesPtr()   { return rbuff_curr_->vertices_ + rbuff_curr_->vertices_count_; }
u16_t*   RendBuffer::GetIndicesPtr()    { return rbuff_curr_->indices_ + rbuff_curr_->indices_count_; }
u16_t    RendBuffer::GetVerticesCount() { return (u16_t)rbuff_curr_->vertices_count_; }

void RendBuffer::NeedSpace(int vx,int ix)
{
  if ( vx + rbuff_curr_->vertices_count_ > MAX_VERTICES
    || ix + rbuff_curr_->indices_count_ > MAX_INDICES )
    rbuff_curr_->Flush_();
}

void RendBuffer::ApplaySpace(int vx,int ix)
{
  rbuff_curr_->vertices_count_ += vx;
  rbuff_curr_->indices_count_ += ix;
}

void RendBuffer::SelectBuffer(Texture* tex,float z,u32_t flags)
{
  if ( !rbuff_curr_ )
  {
    rbuff_curr_ = rbuffers_;
    goto lb_reset;
  }

  if ( !g_blendstage || z == rbuff_curr_z_ )
  {
    if ( tex == rbuff_curr_->texture_ && flags == rbuff_curr_->flags_ ) {
      return; // nothing changed
    }
    for ( int i = 0; i < MAX_RBUFFERS; ++i )
    {
      if ( rbuffers_[i].texture_ == tex )
      {
        rbuff_curr_ = rbuffers_ + i;
        if ( rbuff_curr_->flags_ != flags ) rbuff_curr_->Flush_();
        goto lb_reset;
      }
      if ( rbuffers_[i].vertices_count_ )
      {
        if ( rbuffers_[i].vertices_count_ > rbuff_curr_->vertices_count_ )
            rbuff_curr_ = rbuffers_+i;
      }
      else
      {
        rbuff_curr_ = rbuffers_+i;
        goto lb_reset;
      }
    }
    rbuff_curr_->Flush_();
  }
  else
  {
    Flush();
  }

lb_reset:
  rbuff_curr_z_ = z;
  rbuff_curr_->flags_ = flags;
  rbuff_curr_->texture_ = tex;
}

void RendBuffer::Flush_()
{
  if ( vertices_count_ )
  {
    g_gfx->SetTexture(texture_);

    if ( texture_ )
    {
      if ( flags_ & FLAG_FILTERING ) g_gfx->SetState(GFX3D_STATE_MINMAGFILTER,GFX3D_LINEAR);
      else                           g_gfx->SetState(GFX3D_STATE_MINMAGFILTER,GFX3D_NEAREST);
    }

    if ( g_blendstage )
    {
      if ( flags_ & FLAG_ADDITIVE )  g_gfx->SetState(GFX3D_STATE_BLEND_FUNC,GFX3D_SRC_ALPHA,GFX3D_ONE);
      else           g_gfx->SetState(GFX3D_STATE_BLEND_FUNC,GFX3D_SRC_ALPHA,GFX3D_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
      if ( flags_ & FLAG_SOLIDSPRITE ) g_gfx->SetState(GFX3D_STATE_ALPHA_TEST,GFX3D_DISABLE);
      else
        g_gfx->SetState(GFX3D_STATE_ALPHA_TEST,GFX3D_ENABLE),
        g_gfx->SetState(GFX3D_STATE_ALPHA_FUNC,GFX3D_GREATER,0.9f);
    }

    g_gfx->DrawIndexedPrimitive(GFX3D_TRILIST,vertices_,vertices_count_,indices_,indices_count_,GFX3D_VxB_VCT);
  }

  vertices_count_ = 0;
  indices_count_ = 0;
//  texture_ = 0;
//  flags_ = 0;
}

static void BeginStage(bool blendstage)
{
  g_blendstage = blendstage;
  RendBuffer::Init();
  g_gfx->SetState(GFX3D_STATE_ENVMODE,GFX3D_TEXTURE_MODULATE);
  if ( blendstage )
  {
    g_gfx->SetState(GFX3D_STATE_ALPHA_TEST,GFX3D_ENABLE);
    g_gfx->SetState(GFX3D_STATE_ALPHA_FUNC,GFX3D_GREATER,0.03f);
    g_gfx->SetState(GFX3D_STATE_DEPTH_FUNC,GFX3D_LEQUAL);
    g_gfx->SetState(GFX3D_STATE_BLENDING,GFX3D_ENABLE);
  }
  else
  {
    g_gfx->SetState(GFX3D_STATE_DEPTH_FUNC,GFX3D_LESS);
    g_gfx->SetState(GFX3D_STATE_BLENDING,GFX3D_DISABLE);
  }
}

static void FinishStage()
{
  RendBuffer::Flush();
  if ( g_blendstage ) g_gfx->SetState(GFX3D_STATE_BLENDING,GFX3D_DISABLE);
}

static void RecursiveSpriteTransform(VxB_VERTEX* vx,Sprite3D* parent,u32_t deps)
{
  if ( !deps ) return;
  if ( (deps & SPRITE_DEPS_XSCALE)  && parent->scaled_  ) {
    float scale = parent->xscale_;
    vx[0].vx *= scale;vx[1].vx *= scale;vx[2].vx *= scale;vx[3].vx *= scale;
  }
  if ( (deps & SPRITE_DEPS_YSCALE) && parent->scaled_  ) {
    float scale = parent->yscale_;
    vx[0].vy *= scale;vx[1].vy *= scale;vx[2].vy *= scale;vx[3].vy *= scale;
  }
  if ( (deps & SPRITE_DEPS_DEGREE) && parent->rotated_ ) {
    for ( int i=0; i <4; ++i ) {
      const float x0 = vx[i].vx;
      const float y0 = vx[i].vy;
      vx[i].vx = (x0*parent->cos_ + y0*parent->sin_);
      vx[i].vy = (-x0*parent->sin_ + y0*parent->cos_);
    }
  }
  if ( deps & SPRITE_DEPS_X ) {
    float x = parent->x_;
    vx[0].vx += x;vx[1].vx += x; vx[2].vx += x; vx[3].vx += x;
  }
  if ( deps & SPRITE_DEPS_Y ) {
    float y = parent->y_;
    vx[0].vy += y;vx[1].vy += y; vx[2].vy += y; vx[3].vy += y;
  }
  mrgba_t color = vx[0].color;
  if ( deps & (SPRITE_DEPS_LUMINANCE|SPRITE_DEPS_CFILT) ) {
    float R = float(color.R);
    float G = float(color.G);
    float B = float(color.B);
    if ( deps & SPRITE_DEPS_LUMINANCE ) {
      R *= parent->luminance_;
      G *= parent->luminance_;
      B *= parent->luminance_;
    }
    if ( deps & SPRITE_DEPS_CFILT ) {
      R *= float(parent->crgb_.R)*(1.0f/255.0f);
      G *= float(parent->crgb_.G)*(1.0f/255.0f);
      B *= float(parent->crgb_.B)*(1.0f/255.0f);
    }
    color = mRGBA_t(byte_t(R),byte_t(G),byte_t(B),color.A);
  }
  if ( deps & SPRITE_DEPS_TRANSPARENCY ) {
    color.A = byte_t(float(color.A) * (1.0f-parent->transparency_));
  }
  vx[3].color = vx[2].color = vx[1].color = vx[0].color = color;
  if ( parent->parent_ )
    RecursiveSpriteTransform(vx,parent->parent_,parent->parent_deps_&deps);
}

static bool FillInterParalaxedSpriteVertices(Sprite3D* sprI, KlassOfSprite3D& klass, long tex_no)
{
  return true;
}

static bool FillSpriteVertices(Sprite3D* sprI, ComplexSprite* complex_spr, KlassOfSprite3D& klass, long tex_no)
{
  // координаты полигона
  rect_t const & tex_rect = klass.textures_[tex_no].layout_EX_[klass.unisprite_?1:0].rect;
  float const tx = float(klass.unisprite_?tex_rect.left:0);
  float const ty = float(klass.unisprite_?tex_rect.top:0);
  float const txW = klass.unisprite_?float(tex_rect.right):float(klass.info_.spr_width);
  float const tyW = klass.unisprite_?float(tex_rect.bottom):float(klass.info_.spr_height);

  ComplexSprite::Info cs_info;
  u32_t fr_no;

  if ( complex_spr ) {
    cs_info.xscale = 1.0f;
    cs_info.yscale = 1.0f;
    cs_info.cosv   = 1.0f;
    cs_info.sinv   = 0.0f;
    cs_info.flags  = 0;
    cs_info.alphaCf = 1000;

    // начинает итерацию по микроспрайтам комплексного спрайта
    if ( !complex_spr->ComplexSpriteReset(g_blendstage)
      || !complex_spr->ComplexSpriteNext(&cs_info,g_blendstage)) return false;
  }

  u32_t flags = 0;
  flags |= klass.info_.additive_?FLAG_ADDITIVE:0;
  flags |= klass.info_.type == SPRITE_SOLID?FLAG_SOLIDSPRITE:0;
  flags |= klass.info_.filtering?FLAG_FILTERING:0;

  u32_t const fr_mapping_no = teggo_max<u32_t>(1,klass.fr_mapping_.size());

  // итерируем по комплексному спрайту
  do {
    if ( complex_spr ) fr_no = cs_info.fr_no;
    else fr_no = sprI->fr_no_ % klass.info_.spr_no;

    if ( complex_spr && !klass.unisprite_ )
      tex_no = klass.fr_mapping_.empty()?0:klass.fr_mapping_[fr_no%fr_mapping_no];

    // выбор кеш-буфера редерера, для группировки по текстурам и глубине
    RendBuffer::SelectBuffer(
      (klass.info_.dummy_?0:~klass.textures_[tex_no].image_),
      sprI->z_,
      flags);

    // выделение четырёх вертексов и шести индексов в кеш-буфере рендерера
    // здесь может произойти сброс буфера на GPU
    RendBuffer::NeedSpace(4,6);

    VxB_VERTEX* vx = RendBuffer::GetVerticesPtr();

    // текстурные координаты
    const SPRITE_fRect& frect = klass.unisprite_?
                            klass.spr_layout_[tex_no]
                           :klass.spr_layout_[fr_no];

    // предпроцессинг  цвета с учётом настроек спрайта
    mrgba_t color = sprI->color_;
    if ( complex_spr ) color.A = byte_t((long(color.A)*cs_info.alphaCf)/1000);
    vx[3].color = vx[2].color = vx[1].color = vx[0].color = color;

    // заполнение вертексов
    {
      float x   = tx;
      float y   = ty;
      float s = frect.s, t = frect.t;
      float ws = frect.ws, wt = frect.wt;

      if ( sprI->flip_x_ ) {
        x = klass.info_.spr_width-(tx+txW);
        teggo_swap(ws,s);
      }

      if ( sprI->flip_y_ ) {
        y = klass.info_.spr_height-(ty+tyW);
        teggo_swap(wt,t);
      }

      float xW  = x+txW;
      float yW  = y+tyW;

      vx[0].Xyz(x,yW,sprI->z_); vx[0].Tex(s,wt);
      vx[1].Xyz(x,y,sprI->z_);  vx[1].Tex(s,t);
      vx[2].Xyz(xW,yW,sprI->z_);vx[2].Tex(ws,wt);
      vx[3].Xyz(xW,y,sprI->z_); vx[3].Tex(ws,t);
    }

    // постпроцессинг вертекса с учётом настроек элемента комплексного спрайта
    if ( complex_spr )
    {
      float offsX = 0;
      float offsY = 0;

      if ( cs_info.flags & ComplexSprite::Info::IS_CENTERED )
        if ( klass.unisprite_ )
          {
            offsX = klass.info_.spr_width * 0.5f;
            offsY = klass.info_.spr_height * 0.5f;
          }
        else
          {
            offsX = txW*0.5f;
            offsY = tyW*0.5f;
          }

      // масштабирование
      if ( cs_info.flags & ComplexSprite::Info::IS_SCALED )
        {
          const float xs = cs_info.xscale;
          const float ys = cs_info.yscale;
          vx[0].vx = (vx[0].vx-offsX) * xs;
          vx[1].vx = (vx[1].vx-offsX) * xs;
          vx[2].vx = (vx[2].vx-offsX) * xs;
          vx[3].vx = (vx[3].vx-offsX) * xs;
          vx[0].vy = (vx[0].vy-offsY) * ys;
          vx[1].vy = (vx[1].vy-offsY) * ys;
          vx[2].vy = (vx[2].vy-offsY) * ys;
          vx[3].vy = (vx[3].vy-offsY) * ys;
        }
      else
        {
          vx[0].vx  -= offsX; vx[1].vx  -= offsX; vx[2].vx  -= offsX; vx[3].vx  -= offsX;
          vx[0].vy  -= offsY; vx[1].vy  -= offsY; vx[2].vy  -= offsY; vx[3].vy  -= offsY;
        }
      // поворот
      if ( cs_info.flags & ComplexSprite::Info::IS_ROTATED )
        for (int i=0;i<4;++i)
          {
            float x0 = vx[i].vx;
            float y0 = vx[i].vy;
            vx[i].vx = (x0*cs_info.cosv  + y0*cs_info.sinv);
            vx[i].vy = (-x0*cs_info.sinv + y0*cs_info.cosv);
          }
    }

    // постпроцессинг вектора для учёта активной точки (hotspot) и иерархии
    if ( !complex_spr || !(cs_info.flags&ComplexSprite::Info::IS_INDEPENDED) )
    {
      {
        const float x0 = (complex_spr?cs_info.x:-sprI->xhsp_);
        const float y0 = (complex_spr?cs_info.y:-sprI->yhsp_);
        vx[0].vx += x0; vx[1].vx += x0; vx[2].vx += x0; vx[3].vx += x0;
        vx[0].vy += y0; vx[1].vy += y0; vx[2].vy += y0; vx[3].vy += y0;
      }
      if ( sprI->scaled_ )
      {
        const float xscale = sprI->xscale_;
        vx[0].vx *= xscale; vx[1].vx *= xscale;
        vx[2].vx *= xscale; vx[3].vx *= xscale;
        const float yscale = sprI->yscale_;
        vx[0].vy *= yscale; vx[1].vy *= yscale;
        vx[2].vy *= yscale; vx[3].vy *= yscale;
      }
      if ( sprI->rotated_ ) {
        for (int i=0;i<4;++i) {
          float x0 = vx[i].vx;
          float y0 = vx[i].vy;
          vx[i].vx = (x0*sprI->cos_  + y0*sprI->sin_);
          vx[i].vy = (-x0*sprI->sin_ + y0*sprI->cos_);
        }
      }

      vx[0].vx += sprI->x_; vx[1].vx += sprI->x_; vx[2].vx += sprI->x_; vx[3].vx += sprI->x_;
      vx[0].vy += sprI->y_; vx[1].vy += sprI->y_; vx[2].vy += sprI->y_; vx[3].vy += sprI->y_;

      // постпроцессинг координат вертекса и цвета с учётом иерархии
      if ( sprI->parent_ )
        RecursiveSpriteTransform(vx,sprI->parent_,sprI->parent_deps_);

    }else{
      const float x0 = (complex_spr?cs_info.x:0);
      const float y0 = (complex_spr?cs_info.y:0);
      vx[0].vx += x0; vx[1].vx += x0; vx[2].vx += x0; vx[3].vx += x0;
      vx[0].vy += y0; vx[1].vy += y0; vx[2].vy += y0; vx[3].vy += y0;
    }

    // учёт сдвига слоя на котором находится спрайт и паралакса спрайта
    if ( sprI->layerObj_ )
    {
      const float lx = sprI->layerObj_->x + g_screen_offset_x * sprI->paralax_;
      const float ly = sprI->layerObj_->y + g_screen_offset_y * sprI->paralax_;
      vx[0].vx += lx; vx[1].vx += lx; vx[2].vx += lx; vx[3].vx += lx;
      vx[0].vy += ly; vx[1].vy += ly; vx[2].vy += ly; vx[3].vy += ly;
    }
    else if ( g_screen_offset )
    {
      const float lx = g_screen_offset_x * sprI->paralax_;
      const float ly = g_screen_offset_y * sprI->paralax_;
      vx[0].vx += lx; vx[1].vx += lx; vx[2].vx += lx; vx[3].vx += lx;
      vx[0].vy += ly; vx[1].vy += ly; vx[2].vy += ly; vx[3].vy += ly;
    }

    // надо ли отображать полигон
    if ( complex_spr || klass.unisprite_ )
    {
      // a0 = vx[0]
      // a1 = vx[3]
      // b0 = vx[1]
      // b1 = vx[2]
      float const X  = vx[0].vx - vx[1].vx; // a0 - b0
      float const Y  = vx[0].vy - vx[1].vy;
      float const A  = vx[3].vx - vx[0].vx; // a1 - a0
      float const B  = vx[3].vy - vx[0].vy;
      float const A_ = vx[2].vx - vx[1].vx;
      float const B_ = vx[2].vy - vx[1].vy;
      float const t  = ( Y*A_ - X*B_ ) / (A*B_  - B*A_);
      float const XX = t * A;
      float const YY = t * B;
      float const x = XX + vx[0].vx;
      float const y = YY + vx[0].vy;

      float R = fabs(XX) + fabs(YY);
      //float R = fabs(t);

      if ( x < -R || y < -R || x > g_screen_width + R || y > g_screen_height + R )
        continue;
    }

    // приведение вертекса в координаты экрана
    vx[0].vy = g_screen_height - vx[0].vy;
    vx[1].vy = g_screen_height - vx[1].vy;
    vx[2].vy = g_screen_height - vx[2].vy;
    vx[3].vy = g_screen_height - vx[3].vy;

    // заполнение индексного буфера
    u16_t* indices = RendBuffer::GetIndicesPtr();
    u16_t  vxi     = RendBuffer::GetVerticesCount();
    indices[0] =  vxi+1;
    indices[1] =  vxi;
    indices[2] =  vxi+3;
    indices[3] =  vxi+3;
    indices[4] =  vxi;
    indices[5] =  vxi+2;

    // применение изменений буфера
    RendBuffer::ApplaySpace(4,6);

  } while ( complex_spr && complex_spr->ComplexSpriteNext(&cs_info,g_blendstage) );

  return true;
}

static void FillTextSpriteVertices(Sprite3D* Sprite, KlassOfSprite3D& klass);

void Impl_3DSPRITES::RenderSprite(Sprite3D* sprite,ComplexSprite* complex_spr)
{
  if ( !complex_spr ) {
    if ( sprite->blended_ != g_blendstage ) return;
    if ( sprite->OutOfScreen() ) return;
  }
  else
    if ( !complex_spr->HasBlendState(g_blendstage) ) return;

  KlassOfSprite3D& klass = *sprite->klass_;
  if ( (klass.is_unloaded_ || klass.need_update_) && !klass.info_.dummy_ ) {
    u32_t millis = GetMillis();
    return_if ( RestoreKlass_(klass) != SCE_OK );
    if (!klass.provider_)
    Lout << _S*"sprites: '%?' was load in %d millis" %klass.GetName() %(GetMillis()-millis);
    REQUIRE ( !klass.is_unloaded_ );
  }

  if ( klass.info_.is_font )
    {
      FillTextSpriteVertices(sprite,klass);
    }
  else if ( klass.unisprite_ )
    {
      if ( sprite->inter_paralaxed_ )
        for ( unsigned i=0; i < klass.textures_.size(); ++i )
          FillInterParalaxedSpriteVertices(sprite,klass,i);
      else
        for ( unsigned i=0; i < klass.textures_.size(); ++i )
          FillSpriteVertices(sprite,complex_spr,klass,i);
    }
  else
    {
      if ( sprite->inter_paralaxed_ )
        FillInterParalaxedSpriteVertices(sprite,
          klass,
          (klass.fr_mapping_.empty()?0:klass.fr_mapping_[sprite->fr_no_%klass.fr_mapping_.size()]));
      else
        FillSpriteVertices(sprite,
          complex_spr,
          klass,
          (klass.fr_mapping_.empty()?0:klass.fr_mapping_[sprite->fr_no_%klass.fr_mapping_.size()]) );
    }
}

static void SpriteBlendVisibilityTest(Sprite3D* spr,u32_t deps,bool* blending,bool* visibility ) {
  bool parent_blending = false, parent_visibility = true;
  bool next_parent = false;
  if ( deps & SPRITE_DEPS_TRANSPARENCY ) {
    *blending = spr->transparency_ > 0.001f;
    next_parent = true;
  }
  if ( deps & SPRITE_DEPS_VISIBILITY ) {
    *visibility = spr->visible_;
    next_parent = true;
  }
  if ( next_parent && spr->parent_ ) {
    SpriteBlendVisibilityTest(spr->parent_,spr->parent_deps_,&parent_blending,&parent_visibility);
    *blending = *blending || parent_blending;
    *visibility = *visibility && parent_visibility;
  }
}

struct TextRendBuffer
{
  enum { MAX_SYMBOLS=256, MAX_VERTICES = 256*4, MAX_INDICES = 256*6 };
  static VxB_VERTEX vertices_[MAX_VERTICES];
  static u16_t   indices_[MAX_INDICES];
  static u32_t    vertices_count_;
  static Texture*   texture_;
  static u32_t    flags_;

  static void Init(u32_t flags)
  {
    static bool indices_initialized = false;
    if ( !indices_initialized ) {
      for ( int i = 0; i < MAX_SYMBOLS; ++i ) {
        u16_t vxi = i*4;
        u16_t* indices = &indices_[i*6];
        indices[0] =  vxi;
        indices[1] =  vxi+1;
        indices[2] =  vxi+2;
        indices[3] =  vxi+2;
        indices[4] =  vxi+3;
        indices[5] =  vxi;
      }
      indices_initialized = true;
    }
    if ( g_blendstage ) RendBuffer::Flush();
    flags_ = flags;
    vertices_count_ = 0;
    texture_ = 0;
  }

  static VxB_VERTEX* Quad(Texture* texture)
  {
    if ( vertices_count_+4 >= MAX_VERTICES || texture != texture_ ) Flush();
    texture_ = texture;
    return vertices_ + vertices_count_;
  }

  static void ApplayQuad()
  {
    vertices_count_ += 4;
  }

  static void Flush()
  {
    if ( vertices_count_ )
    {
      g_gfx->SetTexture(texture_);

      if ( flags_ & FLAG_FILTERING ) g_gfx->SetState(GFX3D_STATE_MINMAGFILTER,GFX3D_LINEAR);
      else                           g_gfx->SetState(GFX3D_STATE_MINMAGFILTER,GFX3D_NEAREST);

      if ( g_blendstage )
        if ( flags_ & FLAG_ADDITIVE ) g_gfx->SetState(GFX3D_STATE_BLEND_FUNC,GFX3D_SRC_ALPHA,GFX3D_ONE);
        else          g_gfx->SetState(GFX3D_STATE_BLEND_FUNC,GFX3D_SRC_ALPHA,GFX3D_ONE_MINUS_SRC_ALPHA);
      else
        if ( flags_ & FLAG_SOLIDSPRITE ) g_gfx->SetState(GFX3D_STATE_ALPHA_TEST,GFX3D_DISABLE);
        else
          g_gfx->SetState(GFX3D_STATE_ALPHA_TEST,GFX3D_ENABLE),
          g_gfx->SetState(GFX3D_STATE_ALPHA_FUNC,GFX3D_GREATER,0.9f);

      g_gfx->DrawIndexedPrimitive(GFX3D_TRILIST,vertices_,vertices_count_,indices_,vertices_count_/4*6,GFX3D_VxB_VCT);
      vertices_count_ = 0;
    }
  }
};

VxB_VERTEX TextRendBuffer::vertices_[MAX_VERTICES];
u16_t   TextRendBuffer::indices_[MAX_INDICES];
u32_t    TextRendBuffer::vertices_count_ = 0;
Texture*   TextRendBuffer::texture_ = 0;
u32_t    TextRendBuffer::flags_ = 0;

static void PrepareParentTransforms(Sprite3D* parent,u32_t deps,mrgba_t* xcolor)
{
  if ( !deps ) return;
  {
    mrgba_t color = *xcolor;
    if ( deps & (SPRITE_DEPS_LUMINANCE|SPRITE_DEPS_CFILT) ) {
      float R = float(color.R);
      float G = float(color.G);
      float B = float(color.B);
      if ( deps & SPRITE_DEPS_LUMINANCE ) {
        R *= parent->luminance_;
        G *= parent->luminance_;
        B *= parent->luminance_;
      }
      if ( deps & SPRITE_DEPS_CFILT ) {
        R *= float(parent->crgb_.R)*(1.0f/255.0f);
        G *= float(parent->crgb_.G)*(1.0f/255.0f);
        B *= float(parent->crgb_.B)*(1.0f/255.0f);
      }
      color = mRGBA_t(byte_t(R),byte_t(G),byte_t(B),color.A);
    }
    if ( deps & SPRITE_DEPS_TRANSPARENCY ) {
      color.A = byte_t(float(color.A) * (1.0f-parent->transparency_));
    }
    *xcolor = color;
  }

  if ( parent->parent_ )
    PrepareParentTransforms(parent->parent_,parent->parent_deps_&deps,xcolor);

  if ( deps & (SPRITE_DEPS_X|SPRITE_DEPS_Y) ) {
    float x = (deps & SPRITE_DEPS_X) ? parent->x_ : 0;
    float y = (deps & SPRITE_DEPS_Y) ? -parent->y_ : 0;
    g_gfx->Translate(x,y,0);
  }
  if ( (deps & (SPRITE_DEPS_XSCALE|SPRITE_DEPS_YSCALE)) && parent->scaled_ ) {
    float xscale = 1.0f,yscale = 1.0f;
    if (deps & SPRITE_DEPS_XSCALE) xscale = parent->xscale_;
    if (deps & SPRITE_DEPS_YSCALE) yscale = parent->yscale_;
    g_gfx->Scale(xscale,yscale,1.0f);
  }
  if ( (deps & SPRITE_DEPS_DEGREE) && parent->rotated_ ) {
    g_gfx->RotateRad( parent->radians_ , 0,0, 1);
  }
}

static void FillTextSpriteVertices(Sprite3D* sprite, KlassOfSprite3D& klass)
{
  Sprite3D_Text* text = ~sprite->text_;
  if ( !text || !text->SymbolReset() ) return;

  u32_t flags = 0;
  flags |= klass.info_.additive_?FLAG_ADDITIVE:0;
  flags |= klass.info_.type == SPRITE_SOLID?FLAG_SOLIDSPRITE:0;
  flags |= klass.info_.filtering?FLAG_FILTERING:0;

  TextRendBuffer::Init(flags);

  mrgba_t color = sprite->color_;
  g_gfx->PushTransform();
  g_gfx->Translate(0,g_screen_height,0);
  if ( sprite->layerObj_ )
    g_gfx->Translate(
      sprite->layerObj_->x + g_screen_offset_x * sprite->paralax_,
      -sprite->layerObj_->y - g_screen_offset_y * sprite->paralax_,
      0);
  else
    g_gfx->Translate(
      g_screen_offset_x * sprite->paralax_,
      - (g_screen_offset_y * sprite->paralax_),
      0);

  if ( sprite->parent_ )
   PrepareParentTransforms(sprite->parent_,sprite->parent_deps_,&color);

  g_gfx->Translate(sprite->x_,-sprite->y_,0);
  if (sprite->rotated_) g_gfx->Rotate(Degree(sprite->radians_),AXIS_Z);
  if (sprite->scaled_)  g_gfx->Scale(sprite->xscale_,sprite->yscale_,1);
  g_gfx->Translate(-sprite->xhsp_,sprite->yhsp_,0);

  const TexFont::SYMBOL* symbol;
  float fntheight = (float)klass.font_->GetTextHeight();
  Sprite3D::TextView* view = ~sprite->textview_;
  if ( view && !view->active ) view = 0;

  const frect_t* frect = view?&view->frect:0;
  if ( frect ) g_gfx->Translate(-frect->left,frect->top,0);

  while ( (symbol = text->SymbolNext()) != 0 )
  {
    const float symy = symbol->y;
    const float symx = symbol->x;
    const float symw = symbol->width;

    if ( frect ) {
      if ( symy+fntheight < frect->top || symy > frect->bottom ) continue;
      if ( symx+symw < frect->left || symx > frect->right ) continue;
    }

    Texture* texture = klass.font_->GetSymbolTexture(symbol->symbol);
    VxB_VERTEX* vx = TextRendBuffer::Quad(texture);

    klass.font_->FillQuad(vx,symbol->symbol,symx,-symy,frect);
    vx[3].color = vx[2].color = vx[1].color = vx[0].color = color;
    vx[3].vz = vx[2].vz = vx[1].vz = vx[0].vz = sprite->z_;
    TextRendBuffer::ApplayQuad();
  }

  TextRendBuffer::Flush();
  g_gfx->PopTransform();
}

void Impl_3DSPRITES::RenderSprite2(Sprite3D* sprite,TriangledSprite* triangled)
  {

    SPRITE_IMAGE_TYPE spritetype;
    triangled->TriangledImageType(&spritetype);

    if ( spritetype == SPRITE_INVALID_TYPE )
      {
        spritetype = sprite->klass_->info_.type;
        if ( spritetype == SPRITE_BLEND && sprite->klass_->info_.additive_ )
          spritetype = SPRITE_ADDITIVE;
      }

    if ( g_blendstage !=
          (spritetype==SPRITE_ADDITIVE||spritetype==SPRITE_BLEND||sprite->blended_) )
            return;

    KlassOfSprite3D& klass = *sprite->klass_;

    if ( (klass.is_unloaded_ || klass.need_update_) && !klass.info_.dummy_ )
      {
        u32_t millis = GetMillis();
        return_if ( RestoreKlass_(klass) != SCE_OK );
        Lout << _S*"sprites: '%?' was load in %? millis"
            % klass.GetName() % (GetMillis()-millis);
        REQUIRE ( !klass.is_unloaded_ );
      }

    if ( klass.info_.is_font || klass.unisprite_ )
      return; // !!!!

    bool colorized;

    if ( !sprite->triangled_vertices_ )
      sprite->triangled_vertices_ = AutoMem( new BufferT<VxB_VERTEX>() );

    bool updated =
      triangled->TriangledUpdate(&spritetype,&colorized,*sprite->triangled_vertices_);

    if ( !sprite->triangled_vertices_->Empty() )
    {
      if ( g_blendstage )
        RendBuffer::Flush();

      mrgba_t color = sprite->color_;
      g_gfx->PushTransform();
      g_gfx->Translate(0,g_screen_height,0);

      if ( sprite->layerObj_ )
        g_gfx->Translate(
          sprite->layerObj_->x+g_screen_offset_x,
          -sprite->layerObj_->y-g_screen_offset_y,
          0);
      else
        g_gfx->Translate(
          g_screen_offset_x,
          -g_screen_offset_y,
          0);

      if ( sprite->parent_ )
       PrepareParentTransforms(sprite->parent_,sprite->parent_deps_,&color);

      long tex_no = klass.fr_mapping_.empty()?0:
        klass.fr_mapping_[sprite->fr_no_%klass.fr_mapping_.size()];

      if ( updated || sprite->triangled_color_ != color)
        {
          VxB_VERTEX* i = sprite->triangled_vertices_->Begin();
          VxB_VERTEX* iE = sprite->triangled_vertices_->End();
          sprite->triangled_color_ = color;
          if ( colorized )
            for ( ; i!=iE; ++i )
              {
                float R = float(i->color.R);
                float G = float(i->color.G);
                float B = float(i->color.B);
                float A = float(i->color.A);
                R *= float(color.R)*(1.0f/255.0f);
                G *= float(color.G)*(1.0f/255.0f);
                B *= float(color.B)*(1.0f/255.0f);
                A *= float(color.A)*(1.0f/255.0f);
                i->color = mRGBA_t(byte_t(R),byte_t(G),byte_t(B),color.A);
                i->vz = 0;
              }
          else
            for ( ; i!=iE; ++i )
              i->color = color, i->vz = 0;

          if ( !klass.info_.dummy_ && !klass.info_.compress_it )
            {
              float h1 = klass.info_.spr_height/klass.textures_[tex_no].height_;
              float w1 = klass.info_.spr_width/klass.textures_[tex_no].width_;
              for ( i = sprite->triangled_vertices_->Begin(); i!=iE; ++i )
                i->ts *= w1, i->tt *= h1;
            }
        }

      g_gfx->Translate(sprite->x_,-sprite->y_,0);

      if (sprite->rotated_) g_gfx->Rotate(Degree(sprite->radians_),AXIS_Z);
      if (sprite->scaled_)  g_gfx->Scale(sprite->xscale_,sprite->yscale_,1);

      g_gfx->Translate(-sprite->xhsp_,sprite->yhsp_,0);
      g_gfx->Translate(0,0,sprite->z_);

      g_gfx->SetTexture(klass.info_.dummy_?0:~klass.textures_[tex_no].image_);

      if ( klass.info_.filtering )
        g_gfx->SetState(GFX3D_STATE_MINMAGFILTER,GFX3D_LINEAR);
      else
        g_gfx->SetState(GFX3D_STATE_MINMAGFILTER,GFX3D_NEAREST);

      if ( g_blendstage )
        {
          g_gfx->SetState(GFX3D_STATE_ALPHA_TEST,GFX3D_ENABLE);
          g_gfx->SetState(GFX3D_STATE_ALPHA_FUNC,GFX3D_GREATER,0.03f);
          if ( spritetype == SPRITE_ADDITIVE )
            g_gfx->SetState(GFX3D_STATE_BLEND_FUNC,GFX3D_SRC_ALPHA,GFX3D_ONE);
          else
            g_gfx->SetState(GFX3D_STATE_BLEND_FUNC,GFX3D_SRC_ALPHA,GFX3D_ONE_MINUS_SRC_ALPHA);
        }
      else
        if ( spritetype == SPRITE_SOLID )
          g_gfx->SetState(GFX3D_STATE_ALPHA_TEST,GFX3D_DISABLE);
        else
          g_gfx->SetState(GFX3D_STATE_ALPHA_TEST,GFX3D_ENABLE),
          g_gfx->SetState(GFX3D_STATE_ALPHA_FUNC,GFX3D_GREATER,0.9f);

      g_gfx->DrawPrimitive(GFX3D_TRILIST,
        &(*sprite->triangled_vertices_)[0],
        sprite->triangled_vertices_->Count(),
        GFX3D_VxB_VCT);

      g_gfx->PopTransform();
    }
  }

SCERROR Impl_3DSPRITES::Display2()
{
  g_gfx = SCREEN::Get3DDevice();
  g_screen_height = (float)SCREEN::WndHeight();
  g_screen_width  = (float)SCREEN::WndWidth();

  g_screen_offset_x = floorf(SCREEN::OffsetX());
  g_screen_offset_y = floorf(SCREEN::OffsetY());

  g_screen_offset = fabsf(g_screen_offset_x) > 0.5 || fabsf(g_screen_offset_y) > 0.5;

  fail_if (!g_gfx);
  // плоский режим отображения
  g_gfx->Ortho2D(0,0,-DEPTH_STEP_ON_SPRITE,1);
  // установки рендерера
  g_gfx->SetState(GFX3D_STATE_CULLFACE_MODE,GFX3D_BACK);
  g_gfx->SetState(GFX3D_STATE_LIGHTING,GFX3D_DISABLE);
  g_gfx->SetState(GFX3D_STATE_WRAPST,GFX3D_TEXTURE_REPEAT);
  g_gfx->SetState(GFX3D_STATE_BLENDING,GFX3D_DISABLE);
//  g_gfx->SetState(GFX3D_STATE_MIPMAPPING,GFX3D_DISABLE);
  g_gfx->SetState(GFX3D_STATE_MIPMAPPING,GFX3D_ENABLE);
  g_gfx->SetState(GFX3D_STATE_ENVMODE,GFX3D_TEXTURE_MODULATE);
  g_gfx->SetState(GFX3D_STATE_WRITE_DEPTH,GFX3D_ENABLE);
  g_gfx->PushTransform();

  if ( (SCREEN::GetCaps() & GFX_CAP_TEXNORMALIZE) != 0 )
    g_gfx->Translate(-0.5f,0.5f,0);

  ++g_render_iteration;

  BeginStage(false);
  {
    float depth = DEPTH_STEP_ON_SPRITE;
    long layer = LONG_MIN;
    long sublayer = LONG_MIN;

    VISIBILITY_SET::iterator i = layers_.begin();
    VISIBILITY_SET::iterator iE = layers_.end();
    for ( ; i!=iE ; ++i ) {

      Sprite3D* sprite = (*i)->layered_sprite_;
      if ( sprite->layerObj_ && !sprite->layerObj_->visible )
      {
        sprite->z_ = 1;
        continue;
      }

      bool parent_blending = false,parent_visibility = true;
      if ( sprite->parent_ )
        SpriteBlendVisibilityTest(sprite,sprite->parent_deps_,&parent_blending,&parent_visibility);
      sprite->blended_ = parent_blending || sprite->transparency_ > 0.1 || sprite->klass_->info_.type == SPRITE_BLEND;

      if ( !parent_visibility )
      {
        sprite->z_ = 1;
        continue;
      }

      if ( layer != sprite->layered_.layer_ || sublayer != sprite->layered_.layer_cheat_) {
        layer = sprite->layered_.layer_;
        sublayer = sprite->layered_.layer_cheat_;
        depth+=DEPTH_STEP_ON_SPRITE;
      }

      sprite->z_ = -depth;

      if ( sprite->complex_ ) RenderSprite(sprite,Adaptate<ComplexSprite>(sprite->subimpl_));
      else if ( sprite->triangled_ ) RenderSprite2(sprite,Adaptate<TriangledSprite>(sprite->subimpl_));
      else if ( !sprite->blended_ ) RenderSprite(sprite,0);
    }
    visiblity_changed_ = false;
  }
  FinishStage();

  BeginStage(true);
  {
    Sprite3D_Layered **i = layers_.End()-1;
    Sprite3D_Layered **iE = layers_.Begin()-1;
    for ( ; i!=iE ; --i ) {
      Sprite3D* sprite = (*i)->layered_sprite_;
      if ( sprite->z_ < 0 ) {
        if ( sprite->complex_ ) RenderSprite(sprite,Adaptate<ComplexSprite>(sprite->subimpl_));
        else if ( sprite->triangled_ ) RenderSprite2(sprite,Adaptate<TriangledSprite>(sprite->subimpl_));
        else if ( sprite->blended_ ) RenderSprite(sprite,0);
      }
    }
  }
  FinishStage();

  SetNeedRefresh(false);
  g_gfx->PopTransform();
  return SCE_OK;
}
