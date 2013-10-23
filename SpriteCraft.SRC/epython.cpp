
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
#include "../ScraftAPI/spritecraft.h"
#include "../lib.kaa/com_python.h"

#include "Engine.h"
#include "SpriteHelper.h"
#include "SpritesList.h"
#include "SpritesEnumerator.h"
#include "TileMap.h"
#include "Randomizer.h"
#include "console.h"
#include "scXMLite.h"
#include "Particles.h"
#include "ParticlesEnumerator.h"

static PyMethodDef module_methods[] = 
  {
      {NULL}  /* Sentinel */
  };

int PyDispatch_InitType(PyObject *module);
int GenericExecutor_PyInit(PyObject *module,pchar_t modulename,pchar_t name);
int GenericExecutorQue_PyInit(PyObject *module,pchar_t modulename,pchar_t name);

PyMODINIT_FUNC
initscraft(void)
  {
    pchar_t modulename = "scraft";

    PyObject* m = _Py_External_Module->Init(
      modulename, module_methods,"Teggo SpriteCraft Engine for Python.");

    if (m == NULL)
      return;

    STRICT_ENSURE( 0 == PyTypeInfoFactory().ExportEnumNames(m,modulename) );
    STRICT_ENSURE( 0 == SpriteCraftEngine::PyAddTypeToModule(m,modulename,"_Engine") );
    
    SpriteHelper        ::PyAddTypeToModule(m,modulename,"Sprite");
    SpritesList         ::PyAddTypeToModule(m,modulename,"SpritesList");
    TileMapHelper       ::PyAddTypeToModule(m,modulename,"TileMap");
    RandomizerHelper    ::PyAddTypeToModule(m,modulename,"Randomizer");
    XMLNodeHelper       ::PyAddTypeToModule(m,modulename,"Xdata");
    ParticlesHelper     ::PyAddTypeToModule(m,modulename,"Particles");
    SpritesEnumerator   ::PyAddTypeToModule(m,modulename,"_SpritesSequence");
    SpritePrimitive     ::PyAddTypeToModule(m,modulename,"_Primitive");
    SpriteLwsArray      ::PyAddTypeToModule(m,modulename,"_LwsArray");
    SpriteTextView      ::PyAddTypeToModule(m,modulename,"_TextView");
    Hierarchy           ::PyAddTypeToModule(m,modulename,"_Hierarchy");
    Console             ::PyAddTypeToModule(m,modulename,"_Console");
    XMLNodeIterator     ::PyAddTypeToModule(m,modulename,"_XdataIterator");
    ParticlesEnumerator ::PyAddTypeToModule(m,modulename,"_ParticlesSequence");
    GenericExecutor_PyInit(m,modulename,"_Executor");
    GenericExecutorQue_PyInit(m,modulename,"_Que");

    rcc_ptr<IEngine> oE;
    rcc_ptr<IComPyUnknown> py_oE;
    REQ_S_OK( CreateEngine_(&+oE) );
    REQ_S_OK( +oE->QueryInterface( TEGGO_GUIDOF(IComPyUnknown), (void**)&+py_oE) );

    STRICT_ENSURE( 0 ==
      _Py_External_Module->Module_AddObject(
        m,"engine",(PyObject*)py_oE->IComPyUnknown_Get(true)) );

    PyDispatch_InitType(m);
  }

