
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
#pragma code_seg(".text$flat$collide")
#endif

static inline float xydistance2(float x0,float y0,float x1,float y1) {
  const float d0 = x0-x1;
  const float d1 = y0-y1;
  return d0*d0 + d1*d1;
}

static void TranslateToScreen(const Sprite3D* spr,fpoint_t* p,long count, u32_t deps = SPRITE_DEPS_X|SPRITE_DEPS_Y|SPRITE_DEPS_XSCALE|SPRITE_DEPS_YSCALE|SPRITE_DEPS_DEGREE)
{
  for ( int i=0; i<count; ++i ) {
    if ( spr->scaled_ && (deps & (SPRITE_DEPS_XSCALE|SPRITE_DEPS_YSCALE) ) ) {
      if ( deps & SPRITE_DEPS_XSCALE ) p[i].x  *= spr->xscale_;
      if ( deps & SPRITE_DEPS_YSCALE ) p[i].y *= spr->yscale_;
    }
    if ( spr->rotated_ && (deps & SPRITE_DEPS_DEGREE) ) {
      const float x0 = p[i].x;
      const float y0 = p[i].y;
      p[i].x =  (x0*spr->cos_  + y0*spr->sin_);
      p[i].y = (-x0*spr->sin_ + y0*spr->cos_);
    }
    if ( deps & SPRITE_DEPS_X ) p[i].x += spr->x_;
    if ( deps & SPRITE_DEPS_Y ) p[i].y += spr->y_;
  }

  const float scrOffX = SCREEN::OffsetX() * spr->paralax_;
  const float scrOffY = SCREEN::OffsetY() * spr->paralax_;
  if ( spr->parent_) {
    const u32_t d = deps&spr->parent_deps_;
    if (d) TranslateToScreen(spr->parent_,p,count,d);
  }
  else if ( spr->layerObj_ ) {
    for ( int i=0; i<count; ++i ) {
      p[i].x += spr->layerObj_->x + scrOffX;
      p[i].y += spr->layerObj_->y + scrOffY;
    }
  }
  else {
    for ( int i=0; i<count; ++i ) {
      p[i].x +=  scrOffX;
      p[i].y += scrOffY;
    }
  }
}

static void TranslateToSprite(const Sprite3D* spr,fpoint_t* p,long count, u32_t deps = SPRITE_DEPS_X|SPRITE_DEPS_Y|SPRITE_DEPS_XSCALE|SPRITE_DEPS_YSCALE|SPRITE_DEPS_DEGREE)
{
  const float scrOffX = SCREEN::OffsetX() * spr->paralax_;
  const float scrOffY = SCREEN::OffsetY() * spr->paralax_;
  if ( spr->parent_) {
    const u32_t d = deps&spr->parent_deps_;
    if (d) TranslateToSprite(spr->parent_,p,count,d);
  }
  else if ( spr->layerObj_ ) {
    for ( int i=0; i<count; ++i ) {
      p[i].x -= spr->layerObj_->x + scrOffX;
      p[i].y -= spr->layerObj_->y + scrOffY;
    }
  }
  else {
    for ( int i=0; i<count; ++i ) {
      p[i].x -= scrOffX;
      p[i].y -= scrOffY;
    }
  }
  for ( int i=0; i<count; ++i ) {
    if ( deps & SPRITE_DEPS_X ) p[i].x -= spr->x_;
    if ( deps & SPRITE_DEPS_Y ) p[i].y -= spr->y_;
    if ( spr->rotated_ && (deps & SPRITE_DEPS_DEGREE) ) {
      const float x0 = p[i].x;
      const float y0 = p[i].y;
      p[i].x =  (x0*spr->cos_ - y0*spr->sin_);
      p[i].y = (x0*spr->sin_ + y0*spr->cos_);
    }
    if ( spr->scaled_ && (deps & (SPRITE_DEPS_XSCALE|SPRITE_DEPS_YSCALE) ) ) {
      if ( deps & SPRITE_DEPS_XSCALE ) p[i].x /= spr->xscale_;
      if ( deps & SPRITE_DEPS_YSCALE ) p[i].y /= spr->yscale_;
    }
  }
}

float Sprite3D::Distance(Sprite* spr) const
{
  const Sprite3D* s1 = Adaptate<Sprite3D>((Adaptable*)spr);
  if ( !s1 ) retval_because("target is not 3D Sprite",false);
  fpoint_t p1(0,0); TranslateToScreen(s1,&p1,1);
  fpoint_t p0(0,0); TranslateToScreen(this,&p0,1);
  const float xx = p1.x-p0.x;
  const float yy = p1.y-p0.y;
  return sqrtf( xx*xx + yy*yy );
}

static inline float CalculateAngle(fpoint_t p) {
  if ( EPSILON_EQUAL0(p.x) ) {
    if ( p.y < 0 ) return nv_pi;
    else return 0;
  }
  else {
    const float angle = atan(p.y/fabs(p.x));
    if ( p.x < 0 ) return nv_pi*2 + angle;
    else return angle;
  }    
};

float Sprite3D::AngleWith(Sprite* spr) const
{
  //const Sprite3D* s0 = this;
  const Sprite3D* s1 = Adaptate<Sprite3D>((Adaptable*)spr);
  if ( !s1 ) retval_because("target is not 3D Sprite",false);
  fpoint_t p(0,0); 
  TranslateToScreen(s1,&p,1);
  TranslateToSprite(this,&p,1);
  return CalculateAngle(p);
}

float Sprite3D::GetScrAngle() const {
  fpoint_t p[2] = { fpoint_t(0,0), fpoint_t(0,-1) };
  TranslateToScreen(this,p,2);
  p[1].x -= p[0].x;
  p[1].y -= p[0].y;
  return CalculateAngle(p[1]);
}

void Sprite3D::SpriteToScreen(float* x,float* y)
{
  fpoint_t p(*x-xhsp_,*y-yhsp_);
  TranslateToScreen(this,&p,1);
  *x = p.x;
  *y = p.y;
}

void Sprite3D::ScreenToSprite(float* x,float* y)
{
  fpoint_t p(*x,*y);
  TranslateToSprite(this,&p,1);
  *x = p.x+xhsp_;
  *y = p.y+yhsp_;
}

float Sprite3D::GetScrX() const { 
  fpoint_t p(0,0);
  TranslateToScreen(this,&p,1);
  return p.x; 
}

float Sprite3D::GetScrY() const { 
  fpoint_t p(0,0);
  TranslateToScreen(this,&p,1);
  return p.y; 
}

bool Sprite3D::OutOfScreen() const {
  float r,x,y;
  GetOBR_(&r,&x,&y);
  return ( 
    x < -r || x > SCREEN::WndWidth()+r ||
    y < -r || y > SCREEN::WndHeight()+r );
}

static bool Sprite3D_Collide_Pix2Point(const Sprite3D* s0, float x, float y);
static bool Sprite3D_Collide_Rect2Rect(const Sprite3D* s0, const Sprite3D* s1);
static bool Sprite3D_Collide_Rect2Rad(const Sprite3D* s0, const Sprite3D* s1);
static bool Sprite3D_Collide_Rect2Pix(const Sprite3D* s0, const Sprite3D* s1);
static bool Sprite3D_Collide_Rad2Pix(const Sprite3D* s0, const Sprite3D* s1);
static bool Sprite3D_Collide_Pix2Pix(const Sprite3D* s0, const Sprite3D* s1);

// проверяет что точка принадлежит хит-области спрайта
// точка задана в системе координат спрайта с началом в активной точке
bool Sprite3D::HitPoint(float xX,float yY) const
{
  const SPRITE_Info& info = klass_->info_;
  SPRITE_COLLIDE_TYPE cldt = info.collide;
  if ( cldt == SPRITE_COLLIDE_RECTANGLE ) {
    const float w = Sprite3D::GetWidth();
    const float h = Sprite3D::GetHeight();
    return ( 
      -xhsp_ <= xX && xX <= w-xhsp_ 
      && -yhsp_ <= yY && yY <= h-yhsp_ );
  } 
  else if ( cldt == SPRITE_COLLIDE_RADIUS ) {
    return (info.collideRadius*info.collideRadius) >= (xX*xX + yY*yY);
  } 
  else if ( cldt == SPRITE_COLLIDE_BITMASK ) {
    return Sprite3D_Collide_Pix2Point(this,xX,yY);
  } 
  else retval_because("invalid sprite collide type",false);
}

bool Sprite3D::ContainsPoint(float* xX,float *yY) const 
{
  fpoint_t xy(*xX,*yY);
  TranslateToSprite(this,&xy,1);
  *xX = xy.x;
  *yY = xy.y;
  return HitPoint(xy.x,xy.y);
}

bool Sprite3D::Contains(float xX,float yY) const{
  fpoint_t xy(xX,yY);
  TranslateToSprite(this,&xy,1);
// #if defined MSKIN_DEBUG
//   fpoint_t xy0 = xy;
//   TranslateToScreen(this,&xy0,1);
//   REQUIRE ( abs( xX - xy0.x) < 2 );
//   REQUIRE ( abs(yY - xy0.y) < 2 );
// #endif
  return HitPoint(xy.x,xy.y);
}

SPRITE_COLLIDE_TYPE Sprite3D::GetCollideType() const
{
  return klass_->info_.collide;
}

void Sprite3D::GetOBR_(float* r, float* x, float* y) const
{
  //const SPRITE_Info& info = klass_->info_;
  const float w = Sprite3D::GetWidth()*0.5f;
  const float h = Sprite3D::GetHeight()*0.5f;
  fpoint_t xy[2] = { 
    fpoint_t( w-xhsp_, h-yhsp_ ),
    fpoint_t( 2*w-xhsp_,2*h-yhsp_) };
  TranslateToScreen(this,xy,2);
  *x = xy[0].x;
  *y = xy[0].y;
  const float ww = xy[0].x  - xy[1].x;
  const float hh = xy[0].y - xy[1].y;
  *r = sqrtf((ww*ww) + (hh*hh));
}

void Sprite3D::GetRadius_(float* r, float* x, float* y,const Sprite3D* targ = 0) const
{
  const SPRITE_Info& info = klass_->info_;
  if ( info.collide == SPRITE_COLLIDE_RADIUS ) {
    fpoint_t xy ( 0,0 );
    TranslateToScreen(this,&xy,1);
    if ( targ ) TranslateToSprite(targ,&xy,1);
    *x = xy.x;
    *y = xy.y;
    const float R = info.collideRadius;
    if ( R <=1 ) *r = 0;
    else if ( scaled_ || 
      (parent_ && (parent_deps_ & (SPRITE_DEPS_XSCALE|SPRITE_DEPS_YSCALE))) ) 
    {
      fpoint_t rr ( 0 + R, 0);
      TranslateToScreen(this,&rr,1);
      if ( targ ) TranslateToSprite(targ,&rr,1);
      const float ww = rr.x - xy.x;
      const float hh = rr.y - xy.y;
      *r = sqrtf((ww*ww)+(hh*hh));
    } 
    else *r = R;
  } 
  else {
    const float w = Sprite3D::GetWidth()*0.5f;
    const float h = Sprite3D::GetHeight()*0.5f;
    fpoint_t xy[2] = { 
      fpoint_t( w-xhsp_, h-yhsp_ ),
      fpoint_t( 2*w-xhsp_,2*h-yhsp_) };
    TranslateToScreen(this,xy,2);
    if ( targ ) TranslateToSprite(targ,xy,2);
    *x = xy[0].x;
    *y = xy[0].y;
    const float ww = xy[0].x  - xy[1].x;
    const float hh = xy[0].y - xy[1].y;
    *r = sqrtf((ww*ww) + (hh*hh));
  }
}

void Sprite3D::GetRadius(float* r, float* x, float* y) const
{
  GetRadius_(r,x,y,0);
}

void Sprite3D::GetRect(float* x, float* y) const
{
  const float w = Sprite3D::GetWidth();
  const float h = Sprite3D::GetHeight();
  fpoint_t xy[4] = { 
    fpoint_t(-xhsp_,-yhsp_), 
    fpoint_t(w-xhsp_,-yhsp_), 
    fpoint_t(w-xhsp_,h-yhsp_), 
    fpoint_t(-xhsp_,h-yhsp_) };
  TranslateToScreen(this,xy,4);
  for ( int i = 0; i < 4; ++i ) {
    x[i] = xy[i].x;
    y[i] = xy[i].y;
  }
}

bool Sprite3D::Hit(const Sprite* spr2,bool guard) const
{
  const Sprite3D* s0 = this;
  const Sprite3D* s1 = Adaptate<Sprite3D>((Adaptable*)spr2);
  if ( !s1 ) retval_because("hit target is not 3D Sprite",false);
  SPRITE_COLLIDE_TYPE s0collide = s0->klass_->info_.collide;
  SPRITE_COLLIDE_TYPE s1collide = s1->klass_->info_.collide;
  bool r2r_collide = ( s0collide == s1collide && s0collide == SPRITE_COLLIDE_RADIUS );
  if ( guard || r2r_collide ) {
    float R0,x0,y0;
    float R1,x1,y1;
    s0->GetRadius(&R0,&x0,&y0);
    s1->GetRadius(&R1,&x1,&y1);
    if ( sqrtf(xydistance2(x0,y0,x1,y1)) - (R0+R1) > 0.5f ) return false;
    if ( r2r_collide ) return true;
  }
  if ( s0collide == SPRITE_COLLIDE_RECTANGLE ) {
    if ( s1collide == SPRITE_COLLIDE_RECTANGLE ) 
      return Sprite3D_Collide_Rect2Rect(s0,s1);
    if ( s1collide == SPRITE_COLLIDE_RADIUS )
      return Sprite3D_Collide_Rect2Rad(s0,s1);
    if ( s1collide == SPRITE_COLLIDE_BITMASK )
      return Sprite3D_Collide_Rect2Pix(s0,s1);
  }
  else if ( s0collide == SPRITE_COLLIDE_RADIUS ) {
    if ( s1collide == SPRITE_COLLIDE_RECTANGLE ) 
      return Sprite3D_Collide_Rect2Rad(s1,s0);
    if ( s1collide == SPRITE_COLLIDE_BITMASK )
      return Sprite3D_Collide_Rad2Pix(s0,s1);
  }
  if ( s0collide == SPRITE_COLLIDE_BITMASK ) {
    if ( s1collide == SPRITE_COLLIDE_RECTANGLE ) 
      return Sprite3D_Collide_Rect2Pix(s1,s0);
    if ( s1collide == SPRITE_COLLIDE_RADIUS )
      return Sprite3D_Collide_Rad2Pix(s1,s0);
    if ( s1collide == SPRITE_COLLIDE_BITMASK )
      return Sprite3D_Collide_Pix2Pix(s0,s1);
  }
  retval_because("invalid collision type",false);
}


static bool IsSegmentInsectV( 
  float x, float y0, float y1, 
  fpoint_t xy0, fpoint_t xy1 )
{
  if ( xy0.x < xy1.x )
  {if ( x < xy0.x || xy1.x < x ) return false;}
  else  {if ( x < xy1.x || xy0.x < x ) return false;}
  const float xx = xy0.x-xy1.x;
  if ( xx < 0.5f && xx > -0.5f ) {
    if ( xy0.y < xy1.y ) 
    { if( xy0.y > y1 || xy1.y < y0 ) return false;}
    else { if( xy1.y > y1 || xy0.y < y0 ) return false;}
  }else{
    const float yy = xy0.y-xy1.y;
    const float a = yy/xx;
    const float C = xy0.y - xy0.x*a;
    const float y = a * x + C;
    if ( y < y0 || y1 < y ) return false;
  }
  return true;
}

static bool IsSegmentInsectH( 
  float y, float x0, float x1, 
  fpoint_t xy0, fpoint_t xy1 )
{
  if ( xy0.y < xy1.y )
  {if ( y < xy0.y || xy1.y < y ) return false;}
  else  {if ( y < xy1.y || xy0.y < y ) return false;}
  const float yy = xy0.y-xy1.y;
  if ( yy < 0.5f && yy > -0.5f ) {
    if ( xy0.x < xy1.x ) 
    { if( xy0.x > x1 || xy1.x < x0 ) return false;}
    else { if( xy1.x > x1 || xy0.x < x0 ) return false;}
  }else{
    const float xx = xy0.x-xy1.x;
    const float a = xx/yy;
    const float C = xy0.x - xy0.y*a;
    const float x = a * y + C;
    if ( x < x0 || x1 < x ) return false;
  }
  return true;
}

static bool Sprite3D_Collide_Rect2Rect(const Sprite3D* s0, const Sprite3D* s1,fpoint_t (&xy1)[4])
{
  const float w1 = s1->GetWidth();
  const float h1 = s1->GetHeight();
  xy1[0] = fpoint_t(-s1->xhsp_,-s1->yhsp_),
  xy1[1] = fpoint_t(w1-s1->xhsp_,-s1->yhsp_),
  xy1[2] = fpoint_t(w1-s1->xhsp_,h1-s1->yhsp_),
  xy1[3] = fpoint_t(-s1->xhsp_,h1-s1->yhsp_);
  TranslateToScreen(s1,xy1,4);
  TranslateToSprite(s0,xy1,4);
  const float left = -s0->xhsp_;
  const float top = -s0->yhsp_;
  const float right = s0->GetWidth()+left;
  const float bottom = s0->GetHeight()+top;
  // прверяем включает ли первый прямоугольник точки второго
  {for ( int i=0; i < 4; ++i ) {
    if ( left <= xy1[i].x  && xy1[i].x  <= right &&
         top  <= xy1[i].y && xy1[i].y <= bottom ) return true;
  }}
  // пересекаем отрезки
  {for ( int i=0; i < 4; ++i ) {
    int j = (i+1) % 4;
    if ( IsSegmentInsectV( left, top, bottom, xy1[i],xy1[j] ) )   return true;
    if ( IsSegmentInsectV( right, top, bottom, xy1[i],xy1[j] ) )  return true;
    if ( IsSegmentInsectH( top, left, right, xy1[i],xy1[j] ) )    return true;
    if ( IsSegmentInsectH( bottom, left, right, xy1[i],xy1[j] ) ) return true;
  }}
  fpoint_t xy[4];
  xy[0] = fpoint_t(left,top);
  xy[1] = fpoint_t(right,top);
  xy[2] = fpoint_t(right,bottom);
  xy[3] = fpoint_t(left,bottom);
  TranslateToScreen(s0,xy,4);
  TranslateToSprite(s1,xy,4);
  const float left1   = -s1->xhsp_;
  const float top1    = -s1->yhsp_;
  const float right1  = w1+left1;
  const float bottom1 = h1+top1;
  // прверяем включает ли второй прямоугольник точки первого
  {for ( int i=0; i < 4; ++i ) {
    if ( left1 <= xy[i].x  && xy[i].x  <= right1 &&
         top1  <= xy[i].y && xy[i].y <= bottom1 ) return true;
  }}
  return false;
}

static bool Sprite3D_Collide_Rect2Rect(const Sprite3D* s0, const Sprite3D* s1)
{
  fpoint_t xy[4];
  return Sprite3D_Collide_Rect2Rect(s0,s1,xy);
}

static bool Sprite3D_Collide_Rect2Rad(const Sprite3D* s0, const Sprite3D* s1)
{
  float R,x,y;
  s1->GetRadius_(&R,&x,&y,s0);
  if ( R <= 1 ) return s0->HitPoint(x,y);
  const float w2 = s0->GetWidth()*0.5f;
  const float h2 = s0->GetHeight()*0.5f;
  //x -= w2;
  //y -= h2;
  if ( x > 0 ) { // right zone
    if ( y < 0 ) { // up-right zone
      if ( x < w2 ) // in vertical track
        return ( y > -h2-R );
      else if ( y > -h2 ) // in horizontal track
        return ( x < w2+R );
      else {
        const float xx = x-w2;
        const float yy = y+h2;
        return R*R > xx*xx + yy*yy;
      }
    }else{ // down-right
      if ( x < w2 ) // in vertical track
        return ( y < h2+R );
      else if ( y < h2 ) // in horizontal track
        return ( x < w2+R );
      else {
        const float xx = x-w2;
        const float yy = y-h2;
        return R*R > xx*xx + yy*yy;
      }
    }
  }
  else { // left zone
    if ( y < 0 ) { // up-left zone
      if ( x > -w2 ) // in vertical track
        return ( y > -h2-R );
      else if ( y > -h2 ) // in horizontal track
        return ( x > -w2-R );
      else {
        const float xx = x+w2;
        const float yy = y+h2;
        return R*R > xx*xx + yy*yy;
      }
    }else{ // down-left
      if ( x > -w2 ) // in vertical track
        return ( y < h2+R );
      else if ( y < h2 ) // in horizontal track
        return ( x > -w2-R );
      else {
        const float xx = x+w2;
        const float yy = y-h2;
        return R*R > xx*xx + yy*yy;
      }
    }
  }
  //return false;
}

static bool Sprite3D_Collide_Rect2Pix(const Sprite3D* s0, const Sprite3D* s1)
{
  return Sprite3D_Collide_Rect2Rect(s0,s1); //FAKE
}

static bool Sprite3D_Collide_Rad2Pix(const Sprite3D* s0, const Sprite3D* s1)
{
  float R,x,y;
  s0->GetRadius_(&R,&x,&y,s1);
  if ( R <= 1 ) return Sprite3D_Collide_Pix2Point(s1,x,y);   
  return Sprite3D_Collide_Rect2Rad(s1,s0); //FAKE
}

static bool Sprite3D_Collide_Pix2Pix(const Sprite3D* s0, const Sprite3D* s1)
{
  fpoint_t xy[4];
  const float w = s0->GetWidth(); 
  const float h = s0->GetHeight(); 
  const float xhsp_s0 = s0->xhsp_; 
  const float yhsp_s0 = s0->yhsp_; 
  if ( !Sprite3D_Collide_Rect2Rect(s0,s1,xy) ) return false;
  {for ( int i = 0; i < 4; ++i ) { xy[i].x += xhsp_s0; xy[i].y += yhsp_s0; }}
  STRICT_REQUIRE ( w*h > 0.5 );
  const u32_t* s1_bitmask = ~s1->klass_->collide_bitmask_;
  const u32_t* s0_bitmask = ~s0->klass_->collide_bitmask_;
  if ( !s1_bitmask || !s0_bitmask ) return true;
  long ux_rows0 = s0->klass_->cbm_rows;
  long ux_cols0 = s0->klass_->cbm_cols;
  long ux_rows1 = s1->klass_->cbm_rows;
  long ux_cols1 = s1->klass_->cbm_cols;
  s1_bitmask += ux_rows1*ux_cols1*(s1->fr_no_%s1->klass_->info_.spr_no);
  s0_bitmask += ux_rows0*ux_cols0*(s0->fr_no_%s0->klass_->info_.spr_no);
  const float fhdx = xy[1].x - xy[0].x;
  const float fhdy = xy[1].y - xy[0].y;
  const float fvdx = xy[3].x - xy[0].x;
  const float fvdy = xy[3].y - xy[0].y;
  const float wceff = 1.0f/(w/(32.0f*ux_cols0));
  const float hceff = 1.0f/(h/ux_rows0);
  const float ux_cols1_32 = ux_cols1 *32;
  const long hdx = long((fhdx * 256.0f / ux_cols1_32 ) * wceff);
  const long vdx = long((fvdx * 256.0f / ux_rows1 ) * wceff);
  const long hdy = long((fhdy * 256.0f / ux_cols1_32 ) * hceff);
  const long vdy = long((fvdy * 256.0f / ux_rows1 ) * hceff);
  long row_x = long( xy[0].x * 256 * wceff );
  long row_y = long( xy[0].y * 256 * hceff );
  long row = 0;
  const long max_x = ux_cols0*32 << 8;
  const long max_y = ux_rows0 << 8;
  for (; row < 32; ++row, row_x += vdx, row_y += vdy) {
    long x = row_x;
    long y = row_y;
    for ( int c__ = 0; c__ < ux_cols1; ++c__ ) {
      u32_t bitmask = s1_bitmask[row*ux_cols1 + c__];
      if ( !bitmask ) continue;
      long bit_no = 0;
      u32_t bit = 1;
      for (; bit_no < 32; ++bit_no, bit <<= 1, x += hdx, y += hdy) {
        if ( bitmask & bit ) {
          if ( x < 0 || y < 0 ) continue;
          if ( x >= max_x || y >= max_y ) continue;
          if ( ux_cols0 > 1 ) {
            long bitshift = x >> 8;
            long col = bitshift >> 5;
            bitshift = bitshift & 31;
            if (  (1UL << bitshift ) & s0_bitmask[(y>>8)*ux_cols0+col] ) return true;
          }else{
            if (  (1UL << (x >> 8) ) & s0_bitmask[(y>>8)] ) return true;
          }
        }
      }
    }
  }
  return false;
}

// X,Y в системе координат спрайта
static bool Sprite3D_Collide_Pix2Point(const Sprite3D* s0, float x, float y)
{
  const float w = s0->GetWidth(); 
  const float h = s0->GetHeight(); 
  const float xhsp_s0 = s0->xhsp_; 
  const float yhsp_s0 = s0->yhsp_; 
  x += xhsp_s0; 
  y += yhsp_s0;
  STRICT_REQUIRE ( w*h > 0.5 );
  if ( x < 0 || y < 0 || x > w || y > h ) return false;
  const u32_t* s0_bitmask = ~s0->klass_->collide_bitmask_;
  if ( !s0_bitmask ) return true;
  long yy = long(y * s0->klass_->cbm_rows / h);
  long xx = long(x * (s0->klass_->cbm_cols * 32) / w);
  u32_t bitmask = s0_bitmask[yy*s0->klass_->cbm_cols + (xx>>5)];
  return ( ((1 << (xx&0x01f) ) & bitmask) != 0 );
}

bool Sprite3D::IntersectLine(float x0,float y0, float x1,float y1) const
{
  fpoint_t xy1[2] = {fpoint_t(x0,y0),fpoint_t(x1,y1) };
  TranslateToSprite(this,xy1,2);
  switch ( klass_->info_.collide ) {
  case SPRITE_COLLIDE_BITMASK:
    //break; fake
  case SPRITE_COLLIDE_RADIUS:
    //break; fake
  case SPRITE_COLLIDE_RECTANGLE:
    {
      const float left   = -xhsp_;
      const float top    = -yhsp_;
      const float right  = Sprite3D::GetWidth()+left;
      const float bottom = Sprite3D::GetHeight()+top;
      // пересекаем отрезки
      if ( IsSegmentInsectV( left,  top,  bottom, xy1[0],xy1[1] ) ) return true;
      if ( IsSegmentInsectV( right, top,  bottom, xy1[0],xy1[1] ) ) return true;
      if ( IsSegmentInsectH( top,   left, right,  xy1[0],xy1[1] ) ) return true;
      if ( IsSegmentInsectH( bottom,left, right,  xy1[0],xy1[1] ) ) return true;
      return false;
    }
    break;
  }
  return false;
}

