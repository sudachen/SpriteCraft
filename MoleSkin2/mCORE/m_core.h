
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

#if !defined __m_core_h__
#define  __m_core_h__

#include "m_errors.h"
#include <setjmp.h>

NAMESPACE_MSKIN_BEGIN

//void    ResetErrorLog();
void    SetMoleSkinErrorReason(pchar_t val);
void    SetMoleSkinErrorReason(pwide_t val);

class MCORE {
public:
  static jmp_buf jmpBuf_;
  static void StartRuntime(unsigned flags=0,unsigned heapPages=0);
  static void FinishRuntime();
  static void ReStart(unsigned flags=0);
  static bool    IsLogging();
  static bool    SetLogging(bool);
  static pwide_t GetModulePath();
  static pwide_t GetModuleName();
  enum { 
    EXIT_PROCESS    = 1, 
    ABORT_PROCESS   = 2,
    DISABLE_LOGGER  = MSKIN_BIT(1),
    DISABLE_LJMP    = MSKIN_BIT(2),
  };
private:
  static void InitPath();
  static void InitializeFormater();
  static void TerminateFormater();
  static void InitializeLogger(unsigned flags);
  static void TerminateLogger();
  static void InitializeSingletons();
  static void TerminateSingletons();
};

template<class Fn> 
inline SCERROR DoMetaMain(Fn fn) {
  switch ( setjmp(MCORE::jmpBuf_) ) {
  case 0:
    MCORE::StartRuntime();
    fn();
    break;
  case MCORE::ABORT_PROCESS: // aborted
    return SCE_FAIL;
  default: ; // nothing
  }
  MCORE::FinishRuntime();
  return SCE_OK;
}

NAMESPACE_MSKIN_END

//enum { 
//  MRT_DISABLE_LOGGER  = MSKINSPACE::MCORE::DISABLE_LOGGER,
//  MRT_DISABLE_LJMP    = MSKINSPACE::MCORE::DISABLE_LJMP,
//};
//extern "C" void StartMoleSkinRuntime(unsigned flags=0,unsigned heapPages=0);
//extern "C" void FinishMoleSkinRuntime();


#endif //  __m_core_h__
