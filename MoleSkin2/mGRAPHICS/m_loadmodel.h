
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


#include "../mcore/m_autoconf.h"
#include "../mstreams/m_dsource.h"
#include "m_surface.h"
#include "m_color.h"
#include "m_gfx3d.h"

#if !defined __m_loadmodel_h__
#define __m_loadmodel_h__

NAMESPACE_MSKIN_BEGIN

Model3D* MOLE_FUNCALL CreateModelFrom(pchar_t name);
Model3D* MOLE_FUNCALL CreateModelFrom(DataSource* source);

Model3D* MOLE_FUNCALL CreateModelFrom_3DS(DataSource* source);

NAMESPACE_MSKIN_END

#endif  // __m_loadmodel_h__
