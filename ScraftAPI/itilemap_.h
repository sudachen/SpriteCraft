
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

#if !defined SCRAFT_INTERNAL_MARKER
#error "should be included in scraftapi.h only"
#endif

struct ScraftITileMap
  {
    ITileMap*      QueryIfs() { return (ITileMap*)this; }
    u32_t        AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t        Release()  { return ((IUnknown*)this)->Release(); }
    void           Dispose();
    bool           IsAlive();
    __ScString     GetName();
    bool           IsVisible();
    void           SetVisible(bool val);
    float          GetX();
    void           SetX(float val);
    float          GetY();
    void           SetY(float val);
    long           GetLayer();
    void           SetLayer(long val);
    long           GetSubLayer();
    void           SetSubLayer(long val);
    float          GetLuminance();
    void           SetLuminance(float val);
    float          GetTransparency();
    void           SetTransparency(float val);
    IColorPtr      Color();
    float          GetScaleX();
    void           SetScaleX(float val);
    float          GetScaleY();
    void           SetScaleY(float val);
    void           SetScale(float val);
    long           GetTileSize();
    void           SetTileSize(long val);
    long           GetTileWidth();
    void           SetTileWidth(long val);
    long           GetTileHeight();
    void           SetTileHeight(long val);
    long           GetRows();
    long           GetCols();
    long           AddTilesFrom(pwide_t klass,long frno=0,long count=0);
    void           Clear();
    void           SetTile(long col,long row,long tilno);
    long           GetTile(long col,long row);
    bool           IsXCycled();
    void           SetXCycled(bool val);
    bool           IsYCycled();
    void           SetYCycled(bool val);
    void           SetTileByIdx(long idx,long tilno);
    long           GetTileByIdx(long idx);
    void           Purge();
    pwide_t        GetNameAsPWIDE();

    struct Property_name {TEGGO_R_PROPERTY(ScraftITileMap,__ScString,GetName,name)};
    struct Property_layer {TEGGO_PROPERTY(ScraftITileMap,long,SetLayer,GetLayer,layer)};
    struct Property_x {TEGGO_PROPERTY(ScraftITileMap,float,SetX,GetX,x)};
    struct Property_y  {TEGGO_PROPERTY(ScraftITileMap,float,SetY,GetY,y)};
    struct Property_visible  {TEGGO_PROPERTY(ScraftITileMap,bool,SetVisible,IsVisible,visible)};
    struct Property_tileHeight {TEGGO_PROPERTY(ScraftITileMap,long,SetTileHeight,GetTileHeight,tileHeight)};
    struct Property_tileWidth {TEGGO_PROPERTY(ScraftITileMap,long,SetTileWidth,GetTileWidth,tileWidth)};
    struct Property_rows {TEGGO_R_PROPERTY(ScraftITileMap,long,GetRows,rows)};
    struct Property_cols {TEGGO_R_PROPERTY(ScraftITileMap,long,GetCols,cols)};
    struct Property_transparency {TEGGO_PROPERTY(ScraftITileMap,float,SetTransparency,GetTransparency,transparency)};
    struct Property_luminance {TEGGO_PROPERTY(ScraftITileMap,float,SetLuminance,GetLuminance,luminance)};
    struct Property_xScale {TEGGO_PROPERTY(ScraftITileMap,float,SetScaleX,GetScaleX,xScale)};
    struct Property_yScale {TEGGO_PROPERTY(ScraftITileMap,float,SetScaleY,GetScaleY,yScale)};
    struct Property_xyScale {TEGGO_W_PROPERTY(ScraftITileMap,float,SetScale,xyScale)};
    struct Property_cfilt {TEGGO_R_PROPERTY_ARROW(ScraftITileMap,IColorPtr,Color,cfilt)};
    struct Property_alive {TEGGO_R_PROPERTY(ScraftITileMap,bool,IsAlive,alive)};
    struct Property_sublayer {TEGGO_PROPERTY(ScraftITileMap,long,SetSubLayer,GetSubLayer,sublayer)};
    struct Property_xCycled  {TEGGO_PROPERTY(ScraftITileMap,bool,SetXCycled,IsXCycled,xCycled)};
    struct Property_yCycled  {TEGGO_PROPERTY(ScraftITileMap,bool,SetYCycled,IsYCycled,yCycled)};

    Property_name         name;
    Property_layer        layer;
    Property_x            x;
    Property_y            y;
    Property_visible      visible;
    Property_tileHeight   tileHeight;
    Property_tileWidth    tileWidth;
    Property_cols         cols;
    Property_rows         rows;
    Property_transparency transparency;
    Property_luminance    luminance;
    Property_xScale       xScale;
    Property_yScale       yScale;
    Property_xyScale      xyScale;
    Property_cfilt        cfilt;
    Property_alive        alive;
    Property_sublayer     sublayer;
    Property_xCycled      xCycled;
    Property_yCycled      yCycled;
  };

inline ITileMap* scraft_query_ifs(ScraftITileMap* u)
  {
    return u->QueryIfs();
  }
