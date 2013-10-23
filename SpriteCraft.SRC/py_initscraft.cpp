
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

#include "stdafx.h"
#include "../python_over_com/python_over_com.h"
#include "../ScraftAPI/spritecraft.h"

HRESULT __stdcall PyDispatch_RegisterPyType();
HRESULT __stdcall Engine_RegisterPyType();
HRESULT __stdcall CreateEngine_(IEngine** pI);
HRESULT __stdcall GenericExecutorQue_RegisterPyType();
HRESULT __stdcall GenericExecutor_RegiterPyType();
HRESULT __stdcall Xdata_RegisterPyType();
HRESULT __stdcall Sprite_RegisterPyType();
HRESULT __stdcall Surface_RegisterPyType();


extern "C" void __cdecl initscraft()
  {
    if ( FAILED(PythonOverCom_Init("scraft", "Teggo.FF Scraft.4 Game Engine for Python.")) )
      return;

    STRICT_ENSURE( SUCCEEDED(PythonOverCom_ExportEnumNames()) );

    rcc_ptr<IEngine> oE;
    //rcc_ptr<IComPyUnknown> py_oE;
    REQ_S_OK( CreateEngine_(&+oE) );
    STRICT_ENSURE( SUCCEEDED(Engine_RegisterPyType()) );
    STRICT_ENSURE( SUCCEEDED(PythonOverCom_AddObject("engine",+oE)) );
    STRICT_ENSURE( SUCCEEDED(PyDispatch_RegisterPyType()) );
    STRICT_ENSURE( SUCCEEDED(GenericExecutorQue_RegisterPyType()) );
    STRICT_ENSURE( SUCCEEDED(GenericExecutor_RegiterPyType()) );
    STRICT_ENSURE( SUCCEEDED(Xdata_RegisterPyType()) );
    STRICT_ENSURE( SUCCEEDED(Sprite_RegisterPyType()) );
    STRICT_ENSURE( SUCCEEDED(Surface_RegisterPyType()) );
    PythonOverCom_GlobalizeMember("OpenLog",+oE);
  }
