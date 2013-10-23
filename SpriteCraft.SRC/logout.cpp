
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
#include "logout.h"

extern pchar_t Scraft_MainModuleFolder();
extern pchar_t Scraft_MainModuleName();

static FILE* g_logHandle = 0;
static bool _g_OpenLog_reopen = false;

static BufferT<char> Logout_AnalyzeModuleName()
  {
    BufferT<char> b;
    b.Push('0');
    return BufferT<char>(b,SWAP_CONTENT);
  }
  
int Logout_OpenLog()
  {
    if ( g_logHandle ) return 0;
    StringT<char> log_file_name (
      _S*"%?/%?-log.txt" %Scraft_MainModuleFolder() %Scraft_MainModuleName() );
    g_logHandle = fopen( +log_file_name,_g_OpenLog_reopen?"at":"w+t");
    if ( !g_logHandle )
      {
        return 0;
      }
    if (!_g_OpenLog_reopen) 
      {
        time_t t = time(0);
        fprintf(g_logHandle,":starting log: %s",ctime(&t));
        fflush(g_logHandle);
      }
    _g_OpenLog_reopen = true;
    return 1;
  }
  
void Logout_CloseLog()
  {
    if ( g_logHandle )
      {
        fclose( g_logHandle );
        g_logHandle = 0;
      }
  }

bool Logout_LogIsOpen()
  {
    return g_logHandle != 0;
  }

void Logout( pwide_t text )
  {
    if ( !g_logHandle ) return;
    fwprintf( g_logHandle, L"%ls\n", text );
    fflush(g_logHandle);
  }
  
void Logout( pchar_t text )
  {
    if ( !g_logHandle ) return;
    fprintf( g_logHandle, "%s\n", text );
    fflush(g_logHandle);
  }
  
void Logdbg( pwide_t text )
  {
    if ( !g_logHandle ) return;
    fwprintf( g_logHandle, L"[DBG] %ls\n", text );
  }
  
void Logdbg( pchar_t text )
  {
    if ( !g_logHandle ) return;
    fprintf( g_logHandle, "[DBG] %s\n", text );
  }
