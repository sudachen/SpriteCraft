
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

#include "../mcore/m_autoconf.h"
#include "m_surface.h"
#include "m_color.h"
#include "../mstreams/m_dsource.h"

#if !defined __m_loadsurf_h__
#define __m_loadsurf_h__

NAMESPACE_MSKIN_BEGIN

/// Поток данных для загрузки изображений
class ImageDataSource : public StreamedDataSource {
public:
  /// Описание изображения
  struct ImageInfo{
    int width;              /// ширина изображения в пикселах
    int height;             /// высота изображения в пикселах
    int stride;             /// ширина строки пикселов в байтах
    int bpp;                /// глубина цвета 8/16/24/32
    bool updown;            /// порядок чтения строк снизу вверх
    bool alphabits;         /// пикселы содежат альфаканал
    bool shortgreen;        /// для 16бит , короткая зеленая компонента
    int palsize;            /// количество используемых элементов палитры
    int frames;             /// количество кадров в анимации
    BufferT<mrgba_t> pal;    /// для палитровых, палитра изображения
    BufferT<float> duration; /// для анимаций, длительность кадров
  };
  /// читает заголовок изображения
  virtual SCERROR ReadPicInfo (ImageInfo&) = 0;
  /// читает строку пикселов
  /// строка содержит компененты пиксела в порядке BGRA
  /// для непалитровых изображений 32/24 формат пиксела 888[8] 
  /// для 16-битных изображений формат пиксела 565 или 555 если установлен shortgreen
  /// если установлен alphabits то пиксел содердит альфаканал
  virtual SCERROR ReadRow(BufferT<byte_t>& row) = 0;
protected:
  ImageDataSource(DataSource* ds) : StreamedDataSource(ds){}
  ImageDataSource(InputStreamEx* is, pwide_t name = L"<unknown>") : StreamedDataSource(is,name){}
  virtual ~ImageDataSource() {}
};
typedef auto_ref<ImageDataSource> AutoImageDataSource;

/// Загружает изображение на сурфейс
SCERROR MOLE_FUNCALL LoadSurfaceFrom(pwide_t source,Surface* surf);

/// Загружает изображение на сурфейс
/// если сурфейс не создан, то создает необходимый сурфейс с указанным форматом
SCERROR MOLE_FUNCALL LoadSurfaceFrom(pwide_t source,AutoSurface& surf,unsigned format);

/// Загружает изображение на сурфейс
SCERROR MOLE_FUNCALL LoadSurfaceFrom(DataSource* ds,Surface* surf);

/// Загружает изображение на сурфейс
/// если сурфейс не создан, то создает необходимый сурфейс с указанным форматом
SCERROR MOLE_FUNCALL LoadSurfaceFrom(DataSource* ds,AutoSurface& surf,unsigned format);

/// Загружает изображение на сурфейс
/// если сурфейс не создан, то создает необходимый сурфейс с указанным форматом
SCERROR MOLE_FUNCALL LoadSurfaceFrom(ImageDataSource* ds,AutoSurface& surf,unsigned fmt);

/// Создает сурфейс на базе изображения
Surface*    MOLE_FUNCALL CreateSurfaceFrom(DataSource*,unsigned = Surface::PREFERRED);

/// Создает сурфейс на базе изображения
Surface*    MOLE_FUNCALL CreateSurfaceFrom(pwide_t resname,unsigned fmt = Surface::PREFERRED);

/// Создает поток данных для загрузки изображения в формате MS BMP
ImageDataSource* MOLE_FUNCALL MakeBMPImageDataSource(DataSource*);
ImageDataSource* MOLE_FUNCALL MakeTGAImageDataSource(DataSource*);
ImageDataSource* MOLE_FUNCALL MakeJPGImageDataSource(DataSource*);
ImageDataSource* MOLE_FUNCALL MakePNGImageDataSource(DataSource*);
ImageDataSource* MOLE_FUNCALL MakeTAFImageDataSource(DataSource*);

/// Распознает формат изображения и создает поток данных для загрузки
ImageDataSource* MOLE_FUNCALL MakeImageDataSourceFrom(DataSource*);

typedef ImageDataSource* (__cdecl *LPFN_IMAGING)(DataSource*);
SCERROR MOLE_FUNCALL AddImagingEXT(LPFN_IMAGING cb);
SCERROR MOLE_FUNCALL RemoveImagingEXT(LPFN_IMAGING cb);


NAMESPACE_MSKIN_END

#endif // __m_loadsurf_h__
