
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

#if !defined ___AD932EF9_5DEE_4d98_8241_F82BFB15F25E___
#define ___AD932EF9_5DEE_4d98_8241_F82BFB15F25E___
#define SCRAFT_INTERNAL_MARKER

#if defined _MSC_VER
#pragma warning(disable:4355)
#pragma warning(disable:4786)
#pragma warning(disable:4244)
#endif

#if defined __INTEL_COMPILER
#pragma warning(disable:1125) // function ... is hidden by ...
#endif

#include "scraftapi_.h"

struct Scraft;

struct Scraft_Engine_Instance
  {
    Scraft* operator ->()
      {
        return (Scraft*)0;
      }
  };

static Scraft_Engine_Instance Engine;

struct Scraft;
struct ScraftIXdataIter;
struct ScraftIXdata;
struct ScraftISprite;
struct ScraftISpriteEnum;
struct ScraftISpritesList;
struct ScraftITileMap;
struct ScraftIParticles;
struct ScraftIQue;
struct ScraftIExecutor;
struct ScraftIColor;
struct ScraftIHierarchy;
struct ScraftILwsArray;
struct ScraftITextView;
struct ScraftIRandomizer;
struct ScraftISpritePrimitive;
struct ScraftIConsole;
struct ScraftIScriptSpace;
struct ScraftIScriptObject;
struct ScraftIScriptCode;
struct ScraftSprite;
struct ScraftScripted;

typedef teggo::rcc_ptr<ScraftIXdata>        IXdataPtr;
typedef teggo::rcc_ptr<ScraftIXdataIter>    IXdataIterPtr;
typedef teggo::rcc_ptr<ScraftISprite>       ISpritePtr;
typedef teggo::rcc_ptr<ScraftISpriteEnum>   ISpriteEnumPtr;
typedef teggo::rcc_ptr<ScraftISpritesList>  ISpritesListPtr;
typedef teggo::rcc_ptr<ScraftITileMap>      ITileMapPtr;
typedef teggo::rcc_ptr<ScraftIQue>          IQuePtr;
typedef teggo::rcc_ptr<ScraftIExecutor>     IExecutorPtr;
typedef teggo::rcc_ptr<ScraftIParticles>    IParticlesPtr;
typedef teggo::rcc_ptr<ScraftIColor>        IColorPtr;
typedef teggo::rcc_ptr<ScraftIHierarchy>    IHierarchyPtr;
typedef teggo::rcc_ptr<ScraftILwsArray>     ILwsArrayPtr;
typedef teggo::rcc_ptr<ScraftITextView>     ITextViewPtr;
typedef teggo::rcc_ptr<ScraftIRandomizer>   IRandomizerPtr;
typedef teggo::rcc_ptr<ScraftISpritePrimitive> ISpritePrimitivePtr;
typedef teggo::rcc_ptr<ScraftIConsole>      IConsolePtr;
typedef teggo::rcc_ptr<ScraftIScriptSpace>  IScriptSpacePtr;
typedef teggo::rcc_ptr<ScraftIScriptObject> IScriptObjectPtr;
typedef teggo::rcc_ptr<ScraftIScriptCode>   IScriptCodePtr;

#include "icolor_.h"
#include "ihierarchy_.h"
#include "ilwsa_.h"
#include "iprimitive_.h"
#include "itextview_.h"
#include "isprite_.h"
#include "ique_.h"
#include "iexecutor_.h"
#include "itilemap_.h"
#include "iparticles_.h"
#include "ixml_.h"
#include "ixxx_.h"
#include "iconsole_.h"
#include "scraft_.h"
#include "exque_.h"
#include "on_xxx_.h"
#include "sprite_.h"
//#include "scrafttalk_.h"

#undef SCRAFT_INTERNAL_MARKER

#if defined _MSC_VER
#pragma warning(default:4244)
#endif

#endif // ___AD932EF9_5DEE_4d98_8241_F82BFB15F25E___
