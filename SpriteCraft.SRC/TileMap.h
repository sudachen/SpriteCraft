
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

#if !defined __4B42FFA7_CCE5_482F_9DAA_2180B23F2C5B__
#define __4B42FFA7_CCE5_482F_9DAA_2180B23F2C5B__

#include "RBGFilter.h"
#include "flat_e0.h"

struct TileMapHelper;

/*

Реализация логики спрайтов.
Используется для предоставления рендереру спрайтов интерфейса SpriteComplex  
     
*/

struct TileMapHelper_ComplexTile: public SpriteSubImpl, public ComplexSprite, private refbase 
  {
    Sprite* spr_;
    TileMapHelper* impl_;
    float width_,height_;
    long row_,col_;
    long row_first_,row_last_;
    long col_first_,col_last_;
    float xfix_,yfix_;
  
    virtual void Release_() const; 
    virtual void Grab_() const; 
    void Clear(); 

    bool NextTile(long* col,long* row,long* frno);
    bool ComplexSpriteReset(bool blendstate);
    bool ComplexSpriteNext(ComplexSprite::Info* info, bool blendstate);
    bool HasBlendState(bool blendstate);

    virtual void SetSpritePtr(Sprite* spr); 
    virtual void* Adaptate_(const void* id); 
    TileMapHelper_ComplexTile(TileMapHelper* impl);
    ~TileMapHelper_ComplexTile(); 
  };

/* 

  Менеджер тайловых карт.
  Служит для создания, поиска и перебора таловых карт.
  Существует пока существует хотя бы одна тайловая карта

*/

struct TileMapManager : refbase
  {
    struct NameIsLess 
      { 
        bool operator()(const TileMapHelper* a,const TileMapHelper* b) const ;  
      };

    CollectionT< TileMapHelper* > tmaps_;
    static TileMapManager* manager_;
    static HRESULT DoMake(pwide_t name,unsigned rows,unsigned columns,unsigned tileSize,int layer,rcc_ptr<TileMapHelper>* pVal);
    static TileMapHelper* Find(pwide_t name);
    static HRESULT Enumerate(ArrayT< rcc_ptr<TileMapHelper> >& e);
    static void LogTileMaps();
    static void Purge();

    void Forget( pwide_t );   
    TileMapManager();
    ~TileMapManager();
  };

/*

  Реализация тайловой карты.
  Основывается на спрайте как надоре талов содержащихся в карте.
  Создаются карты через менеджер карт который является так же и их холдером. 
  
*/

HRESULT __stdcall Engne_NewTileMap_(ITileMap **pI);
HRESULT __stdcall PyCreateTileMap_(ITileMap **pI,PyObject*,PyObject*);

struct TileMapHelper : 
  TeggoPyDispImpl<ITileMap,(PythonOverCom_Make3)PyCreateTileMap_,
    TeggoIfs<IColorHelper,TeggoISupportErrorInfo> >
  {
    typedef TileMapHelper_ComplexTile ComplexTile;
    typedef auto_ref<ComplexTile> ComplexTilePtr;
    
    struct TileDesc 
      { 
        ComplexTile* holder; 
        long frno; 
      };

    template<class F1,class A1,class A2>
      void DoIt_(F1 f,A1 a1,A2 a2)
        {
          CollectionIteratorT<ComplexTilePtr> i = tiles_.Iterate();
            while ( i.Next() ) 
            { 
              ComplexTilePtr& val = *i; 
              if ( val->spr_ ) 
                (val->spr_->*f)(a1,a2);
            }
        }

    template<class F1,class A1>
      void DoIt_(F1 f,A1 a1)
        {
          CollectionIteratorT<ComplexTilePtr> i = tiles_.Iterate();
          while ( i.Next() ) 
            { 
              ComplexTilePtr& val = *i; 
              if ( val->spr_ ) 
                (val->spr_->*f)(a1);
            }
        }

    template<class F1> 
      void DoIt_(F1 f)
        {
          CollectionIteratorT<ComplexTilePtr> i = tiles_.Iterate();
          while ( i.Next() ) 
            { 
              ComplexTilePtr& val = *i; 
              if ( val->spr_ ) 
                (val->spr_->*f)();
            }
        }

    // через эту ссылку карта удерживает менеджер в памяти. 
    // Пока существует хотя бы одна карта, существует и менеджер карт
    auto_ref<TileMapManager> manager_;

    // набор тайлов которые могут быть размещены на карте
    // доступ по имени класса спрайта содежащего тайлы
    CollectionT<ComplexTilePtr> tiles_;

    widestring        name_;            // имя тайловой карты
    unsigned          rows_;            // количество тайлов по вертикали
    unsigned          cols_;            // количество тайлов по горизонтали
    unsigned          x_tilsize_;       // базовый размер тайла
    unsigned          y_tilsize_;       // базовый размер тайла
    long              layer_,sublayer_; // слой на котором находится карта
    long              x_,y_;            // позиция левого верхнего угла карты в пиксилах
    float             transparency_;    // прозрачность спрайтов
    float             luminance_;       // мощнось свечения спрайтов
    float             scale_x_;         // масштаб по горизонтали
    float             scale_y_;         // масштаб по вернтикли
    mrgb_t            cfilt_;           // цветовой фильтер
    BufferT<byte_t>   map_;             // карта тайлов

    TileDesc          tsetmapper_[256]; // отображение номера тайла на спрайт содержащий тайлы
    long              tsetcount_;       // количество назначеных тайлов

    enum { HEREIS_NO_TILE = 0x0ff };    // поля карты не содержащие тайл заполняются этим значением

    // флаги
    struct 
      {
        bool visible_              : 1; // флаг видимости карты
        bool xcycled_              : 1; // тайловая карта зациклена по горизонтали
        bool ycycled_              : 1; // тайловая карта зациклена по вертикали
        bool has_blending_sprites_ : 1;
        // карта не имеет имени, что означает, что она сущетствует, пока на нее ссылаюся извне
        bool noname_               : 1; 
      };

    TileMapHelper();
    virtual ~TileMapHelper();

    virtual void Grab_() const; 
    virtual void Release_() const;

    virtual u32_t __stdcall Release() = 0;
    virtual u32_t __stdcall AddRef()  = 0;
    void CheckDispose() { if ( ~manager_ && noname_ ) Dispose_(); }
    virtual void  XComObject__check() = 0;
    void Init_(const auto_ref<TileMapManager>& manager,pwide_t name,bool noname,unsigned rows,unsigned columns,unsigned tileSize,int layer);
    void Purge_();
    void Show_();
    void Hide_();
    void Dispose_();
    void SetSubImpl_(IUnknown*);
    HRESULT SetTile_(long idx,long tilno);
    HRESULT UseTilesFrom_(pwide_t klass,long frno,long count,long* no);
  public:
    virtual mrgb_t IColorHelper_GetColor() const;
    virtual void   IColorHelper_SetColor(mrgb_t c);
  public:
	  HRESULT __stdcall get_style(/*[out, retval]*/ enum tagStyle *pVal);
	  HRESULT __stdcall put_style(/*[in]*/ enum tagStyle newVal);
	  HRESULT __stdcall get_sublayer(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall put_sublayer(/*[in]*/ long newVal);
	  HRESULT __stdcall get_alive(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall GetNameAsPWIDE(/*[out,retval]*/ wchar_t** p);
	  HRESULT __stdcall get_tileHeight(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall put_tileHeight(/*[in]*/ long newVal);
	  HRESULT __stdcall get_tileWidth(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall put_tileWidth(/*[in]*/ long newVal);
	  HRESULT __stdcall Purge();
	  HRESULT __stdcall get_yCycled(/*[out, retval]*/ VARIANT_BOOL *pVal);
	  HRESULT __stdcall put_yCycled(/*[in]*/ VARIANT_BOOL newVal);
	  HRESULT __stdcall get_xCycled(/*[out, retval]*/ VARIANT_BOOL *pVal);
	  HRESULT __stdcall put_xCycled(/*[in]*/ VARIANT_BOOL newVal);
	  HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);
	  HRESULT __stdcall GetTileByIdx(/*[in]*/ long idx, /*[out,retval]*/ long* tileno);
	  HRESULT __stdcall SetTileByIdx(/*[in]*/ long idx,/*[in]*/ long tileno);
	  HRESULT __stdcall GetTile(/*[in]*/ long col ,/*[in]*/ long row, /*[out,retval]*/ long* tileno);
	  HRESULT __stdcall SetTile(/*[in]*/ long col,/*[in]*/ long row,/*[in]*/ long tileno);
	  HRESULT __stdcall Clear();
	  HRESULT __stdcall AddTilesFrom(BSTR klass, long frno, long count, long *no);
	  HRESULT __stdcall get_yScale(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_yScale(/*[in]*/ float newVal);
	  HRESULT __stdcall get_xScale(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_xScale(/*[in]*/ float newVal);
	  HRESULT __stdcall put_xyScale(/*[in]*/ float newVal);
	  HRESULT __stdcall get_cols(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall get_rows(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall get_tileSize(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall put_tileSize(/*[in]*/ long newVal);
	  HRESULT __stdcall get_visible(/*[out, retval]*/ VARIANT_BOOL *pVal);
	  HRESULT __stdcall put_visible(/*[in]*/ VARIANT_BOOL newVal);
	  HRESULT __stdcall get_y(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_y(/*[in]*/ float newVal);
	  HRESULT __stdcall get_x(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_x(/*[in]*/ float newVal);
	  HRESULT __stdcall get_cfilt(/*[out, retval]*/ IRGBFilter* *pVal);
	  HRESULT __stdcall get_transparency(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_transparency(/*[in]*/ float newVal);
	  HRESULT __stdcall get_luminance(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_luminance(/*[in]*/ float newVal);
	  HRESULT __stdcall put_layer(/*[out, retval]*/ long val);
	  HRESULT __stdcall get_layer(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall get_name(/*[out, retval]*/ BSTR *pVal);
	  HRESULT __stdcall Dispose();
  };

#endif // __4B42FFA7_CCE5_482F_9DAA_2180B23F2C5B__


