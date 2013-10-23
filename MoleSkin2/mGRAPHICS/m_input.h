
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
#include "../mcore/m_debug.h"
//#include "../mcore/m_irefobj.h"

#if !defined __m_input_h__
#define __m_input_h__

NAMESPACE_MSKIN_BEGIN

/// Коды клвиш для устройства ввода
struct KEYVAL
{
  /// Тип кода клавиши, испльзуется в событии
  enum{
    NONE    = 0,
    EVK_FLAGKEY = 0x7fffffU,
    EVK_CTRLF   = 0x010000U,
    EVK_SHIFTF  = 0x020000U,
    EVK_LBUTTONF= 0x040000U,
    EVK_RBUTTONF= 0x080000U,
    EVK_MBUTTONF= 0x100000U,
    EVK_FLAGMASK= 0xff0000U,
    EVK_KEYMASK = 0x00ffffU,
    EVK_ESC     = 255,
    EVK_UP      = 200,
    EVK_RIGHT   = 201,
    EVK_DOWN    = 202,
    EVK_LEFT    = 203,
    EVK_LBUTTON = 210,
    EVK_RBUTTON = 211,
    EVK_MBUTTON = 212,
    EVK_F1      = 151,
    EVK_F2      = 152,
    EVK_F3      = 153,
    EVK_F4      = 154,
    EVK_F5      = 155,
    EVK_F6      = 156,
    EVK_F7      = 157,
    EVK_F8      = 158,
    EVK_F9      = 159,
    EVK_F10     = 160,
    EVK_F11     = 161,
    EVK_F12     = 162,
    EVK_ENTER   = '\n',
    EVK_BACKSPACE = '\b',
    EVK_TAB     = '\t',
    EVK_SPACE   = ' ',
    EVK_A       = 'A',
    EVK_B       = 'B',
    EVK_C       = 'C',
    EVK_D       = 'D',
    EVK_E       = 'E',
    EVK_F       = 'D',
    EVK_G       = 'G',
    EVK_H       = 'H',
    EVK_I       = 'I',
    EVK_J       = 'J',
    EVK_K       = 'K',
    EVK_L       = 'L',
    EVK_M       = 'M',
    EVK_N       = 'N',
    EVK_O       = 'O',
    EVK_P       = 'P',
    EVK_Q       = 'Q',
    EVK_R       = 'R',
    EVK_S       = 'S',
    EVK_T       = 'T',
    EVK_U       = 'U',
    EVK_V       = 'V',
    EVK_W       = 'W',
    EVK_X       = 'X',
    EVK_Y       = 'Y',
    EVK_Z       = 'Z',
    EVK_COMMA   = ',',
    EVK_PERIOD  = '.',
    EVK_0       = '0',
    EVK_1       = '1',
    EVK_2       = '2',
    EVK_3       = '3',
    EVK_4       = '4',
    EVK_5       = '5',
    EVK_6       = '6',
    EVK_7       = '7',
    EVK_8       = '8',
    EVK_9       = '9',
    EVK_PLUS    = '+',
    EVK_MINUS   = '-'
  };
  /// Возвращает код цифровой клавиши
  static u32_t Number(u32_t v) {
    STRICT_REQUIRE ( v < 10 );
    return u32_t(EVK_0+v);
  }
};

/// описание состояни аналогова устройства ввода - мышь/джойстик
struct POINTVAL
{
  int X;
  int Y;
  int Z;
};

/// Абстракция события для устройства ввода
struct EVENT{
  /// Тип события
  enum EventType {
    NONE          =0, ///< Нет события
    KEY_DOWN      =1, ///< Нажтие клавиши
    KEY_UP        =2, ///< Отпускани клавиши
    POINTER_DOWN  =3, ///< Нажтие клавиши мыши/джойстика
    POINTER_UP    =4, ///< Отпускани клавиши мыши/джойстика
    POINTER_MOVE  =5, ///< Изменение состояния мыши/джойстика
    QUIT          =6, ///< Закрытие окна/приложения
    CONTEXT_RESET =7,
    POINTER_LEAVE =8, ///< Изменение состояния мыши
    CHARKEY       =9, ///<
    WINDOWACTIVE  =10,
    WINDOWINACTIVE=11
  };

  /// Тип события
  EventType type;
  union {
    /// код клавиши
    u32_t key;
    /// символ
    wchar_t ckey;
  };
  /// состояние мыши/джойстика
  POINTVAL point;

  static EVENT MakeWindowActive(bool active) {
    EVENT e; e.type = (active?EVENT::WINDOWACTIVE:EVENT::WINDOWINACTIVE); return e;
  }

  /// Создает событие NONE
  static EVENT MakeNoneEvent() { EVENT e; e.type = EVENT::NONE; return e;}
  /// Создает событие QUIT
  static EVENT MakeQuitEvent() { EVENT e; e.type = EVENT::QUIT; return e;}
  /// Создает событие KEY_UP
  static EVENT MakeKeyUpEvent(u32_t key) {
    EVENT e; e.type = EVENT::KEY_UP; e.key = key; return e;
  }
  /// Создает событие KEY_DOWN
  static EVENT MakeKeyDownEvent(u32_t key) {
    EVENT e; e.type = EVENT::KEY_DOWN; e.key = key; return e;
  }
  ///
  static EVENT MakeCharKeyEvent(wchar_t key) {
    EVENT e; e.type = EVENT::CHARKEY; e.ckey = key; return e;
  }
  /// Создает событие POINTER_UP
  static EVENT MakePointUpEvent(u32_t key, int x, int y) {
    EVENT e;
    e.type = EVENT::POINTER_UP;
    e.key = key;
    e.point.X = x; e.point.Y = y;
    return e;
  }
  /// Создает событие POINTER_DOWN
  static EVENT MakePointDownEvent(u32_t key, int x, int y) {
    EVENT e;
    e.type = EVENT::POINTER_DOWN;
    e.key = key;
    e.point.X = x; e.point.Y = y;
    return e;
  }
  /// Создает событие POINTER_MOVE
  static EVENT MakePointMoveEvent(u32_t keysf,int x, int y, int z) {
    EVENT e;
    e.key = keysf;
    e.type = EVENT::POINTER_MOVE;
    e.point.X = x; e.point.Y = y; e.point.Z = z;
    return e;
  }
  static EVENT MakePointLeaveEvent(u32_t keysf,int x, int y, int z) {
    EVENT e;
    e.key = keysf;
    e.type = EVENT::POINTER_LEAVE;
    e.point.X = x; e.point.Y = y; e.point.Z = z;
    return e;
  }
};

NAMESPACE_MSKIN_END

#endif
