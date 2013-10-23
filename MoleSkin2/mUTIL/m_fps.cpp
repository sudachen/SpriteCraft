
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

#include "precompile.h"
#include "m_fps.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mUTIL")
#endif

NAMESPACE_MSKIN_BEGIN

#define FPS_INTERVAL 0.3f

static unsigned long last_fps_clock = 0;
static unsigned long last_fps_count = 0;
static float last_fps = 0;
static bool calc_fps = 0;

void MOLE_FUNCALL FPS_StartClock() 
{
	calc_fps = true;
	last_fps_clock = GetMillis();
	last_fps_count = 0;
}
 
void MOLE_FUNCALL FPS_StopClock() 
{
	calc_fps = false;
}

void MOLE_FUNCALL FPS_Update()
{
	if ( calc_fps )
	{
		unsigned long clock = GetMillis();
		++last_fps_count; 
		float tdiff = ((float)(clock-last_fps_clock))/1000.0f;
		if ( tdiff >= FPS_INTERVAL )
		{
			last_fps = ((float)last_fps_count)/tdiff;
			last_fps_clock = clock;
			last_fps_count = 0;
		}
	}
}

float MOLE_FUNCALL FPS_GetFPS() {return last_fps;}

NAMESPACE_MSKIN_END
