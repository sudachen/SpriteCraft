
/*

Copyright ¬© 2003-2013, Alex√©y Sudach√©n, alexey@sudachen.name

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



#if !defined __m_log_h__
#define __m_log_h__

#include "m_autoconf.h"

/*-=-=-=-=-=-=--=-=-=--=-=-=--=-=-=--=-=-=--=-=-=--=-=-=--=-=-=--=-=-=--=-=-=-
* ‘ÛÌÍˆËË ÎÓ„ËÓ‚‡ÌË
*/

NAMESPACE_MSKIN_BEGIN

#if !defined MSKIN_DISABLE_LOGGER
pwide_t GetLogFilename();
//EXTERN_C void Log(pchar_t);
//EXTERN_C void WideLog(pwide_t);
void Log_andErrReason(pchar_t);
void Log_andErrReason(pwide_t);

template < class _Tchr >
void Log_andErrReason(FormatT<_Tchr> const &fmt) { Log_andErrReason(fmt.Str()); }

//void WideLogF_va(pwide_t text,va_list va);
//void LogF_va(pchar_t text,va_list va);
#else // logger disabled
static inline pchar_t GetLogFilename() {return "";}
static inline void Log(pchar_t) {}
static inline void WideLog(pwide_t) {}
static inline void WideLogF_va(pwide_t text,va_list va) {}
static inline void LogF_va(pchar_t text,va_list va) {}
#endif

template <class _Tchar>
inline void Log(teggo::FormatT<_Tchar> const& fmt) { Log( fmt.Str() ); }

//EXTERN_C void LogF(pchar_t,...);
//EXTERN_C void WideLogF(pwide_t,...);

//static inline void Log(pwide_t text) { WideLog(text); }

NAMESPACE_MSKIN_END

#endif // __mskin_log_h__

