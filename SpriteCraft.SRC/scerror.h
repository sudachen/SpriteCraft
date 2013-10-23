
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

#if !defined ___4f75bf3e_0bea_45b4_94e1_5b567475fb63___
#define ___4f75bf3e_0bea_45b4_94e1_5b567475fb63___

enum SCERROR {
  SCE_OK               =          0,
  SCE_INVALIDARG       =     0x0001,
  SCE_OUT_OF_MEMORY    =     0x0002, 
  SCE_OUT_OF_RANGE     =     0x0003,
  SCE_OUT_OF_STREAM    =     0x0004,
  SCE_BAD_FORMAT       =     0x0005,
  SCE_BAD_STREAM       =     0x0006,
  SCE_BAD_IMAGE        =     0x0007,
  SCE_BAD_STATE        =     0x0008,
  SCE_EOF              =     0x0009,
  SCE_UNSUPPORTED      =     0x000a,
  SCE_UNKNOWN          =     0x000b,
  SCE_SURFLOST         =     0x000c,
  SCE_INVALIDRECT      =     0x000d,
  SCE_GZIPFAIL         =     0x000e,
  SCE_FINISHED         =     0x000f,
  SCE_FAIL             =     0x7fff
};

inline SCERROR eof2err(SCERROR e , SCERROR err) { return e == SCE_EOF ? err : e ; }
inline SCERROR eof2fail(SCERROR e) { return e == SCE_EOF ? SCE_FAIL : e ; }
inline SCERROR eof2ok(SCERROR e ) { return e == SCE_EOF ? SCE_OK : e ; }
pchar_t TranslateECode(SCERROR);
inline bool Mok( SCERROR e ) { return e == SCE_OK; }


//enum Lerr_ { Lerr };
//template < class tTx > inline void operator , ( Lerr_, tTx const &x ) { x.__there_is_invalid_operator_comma__(); }

inline void AppendErrorInfo(pwide_t) {}
inline void AppendErrorInfo(pchar_t) {}
inline pwide_t GetErrorLog() { return L""; }
inline void ResetErrorLog() {}

template < class tTc > void AppendErrInfoAndLog( tTc const *p ) 
  {
    AppendErrorInfo(p);
    teggo::Logout(p);
  } 

// template <class tTx>
//   inline Lerr_ operator << ( Lerr_ , teggo::FormatT<tTx> const& fmt) 
//     { 
//       AppendErrInfoAndLog( fmt.Str() ); 
//       return Lerr; 
//     }
// 
// template <class tTx>
//   inline Lerr_ operator << ( Lerr_ , tTx const *text) 
//     { 
//       AppendErrInfoAndLog( text ); 
//       return Lerr; 
//     }

#if defined MSKIN_DEBUG
# define Xdbg(x)              x

inline void __debug_break__() 
  {
    __asm int 3;
  }
  
#else
# define Xdbg(x)              (void)0
inline void __debug_break__() {}
#endif

inline void warning_if_impl(bool expr,pchar_t text, pchar_t fname,int lineno)
  {
    if (expr) 
      Lout << _S*"[!warn] %? at %?:%?" %text %fname %lineno;
  }

inline bool if_impl(bool expr,pchar_t text,pchar_t val, pchar_t fname,int lineno)
  {
    if (expr) 
      {
        Lerr << _S*"[!retif] value %? because %? at %?:%?" %val %text %fname %lineno;
        __debug_break__();
      }
    return expr;
  }

inline bool iferr_impl(bool expr,pchar_t text,SCERROR ecode, pchar_t fname,int lineno)
  {
    if (expr) 
      {
        Lerr << _S*"[!iferr] on err %? because %? at %?:%?" %TranslateECode(ecode) %text %fname %lineno;
        __debug_break__();  
      }
    return expr;
  }

inline bool gotoif_impl(bool expr,pchar_t text,pchar_t label, pchar_t fname,int lineno)
  {
    if (expr) 
      {
        Lerr << _S*"[!goto] label %? because %? at %?:%?" %label %text %fname %lineno;
        __debug_break__();
      }
    return expr;
  }

template<class T>
  inline T* fatal_if_null_impl(T* expr,pchar_t text,pchar_t fname,int lineno)
    {
      if (!expr) 
        EMERGENCY( +~(_S*L"null pointer %? at %?:%?" %text %fname %lineno) );
      return expr;
    }

template<class T>
  inline T* nonull(T* expr)
    {
      if (!expr) 
        EMERGENCY( L"null pointer" );
      return expr;
    }


#define warning_if(expr)    warning_if_impl((expr)!=0,#expr,__FILE__,__LINE__)  
#define return_if(expr)     do { if (if_impl((expr)!=0,#expr,"<void>",__FILE__,__LINE__)) return; } while(0)
#define retval_if(expr,val) do { if (if_impl((expr)!=0,#expr,#val,__FILE__,__LINE__)) return (val); } while(0)
#define goto_if(expr,label) do { if (gotoif_impl((expr)!=0,#expr,#label,__FILE__,__LINE__)) goto label; } while(0)
#define reterr_if(expr)     do { SCERROR reterr_if__err = (expr); if ( iferr_impl(reterr_if__err!=SCE_OK,#expr" != SCE_OK",reterr_if__err,__FILE__,__LINE__) ) return reterr_if__err; } while(0)
#define fatal_if_null(expr) fatal_if_null_impl(expr,#expr,__FILE__,__LINE__)

#if defined _DEBUG
#define broken_because(text)       do { Lerr << _S*"[!broken] because %?"%(text); __debug_break__(); goto broken; } while(0) 
#define goto_because(text,label)   do { Lerr << _S*"[!goto] lavle %? because %?" %#label %(text); __debug_break__(); goto label; } while(0) 
#define return_because(text)       do { Lerr << _S*"[!return] because %?" %(text); __debug_break__(); return; } while(0) 
#define retval_because(text,val)   do { Lerr << _S*"[!retval] value %? because %?" %#val %(text); __debug_break__(); return (val);} while(0) 
#else
#define broken_because(text)       do { Lerr << _S*"[!err] %?"%(text); __debug_break__(); goto broken; } while(0) 
#define goto_because(text,label)   do { Lerr << _S*"[!err] %?"%(text); __debug_break__(); goto label; } while(0) 
#define return_because(text)       do { Lerr << _S*"[!err] %?"%(text); __debug_break__(); return; } while(0) 
#define retval_because(text,val)   do { Lerr << _S*"[!err] %?"%(text); __debug_break__(); return (val);} while(0) 
#endif

#define ret0_because(text)         retval_because(text,0)
#define fail_because(text)         retval_because(text,SCE_FAIL)

template<class T> inline bool SCERROR_CHECK_OK(const T& x) {return !!x;}
template<> inline bool SCERROR_CHECK_OK<SCERROR>(const SCERROR& e) { return e == SCE_OK; }

#define retval_if_fail(x,val) retval_if( !SCERROR_CHECK_OK(x) , val )
#define return_if_fail(x)     return_if( !SCERROR_CHECK_OK(x) )
#define fail_if(x)            retval_if( (x) , SCE_FAIL )
#define fail_if_fail(x)       retval_if( !SCERROR_CHECK_OK(x) , SCE_FAIL )
#define fail_if_err(x)        retval_if( (x)!=SCE_OK , SCE_FAIL )
#define ret0_if(x)            retval_if( (x) , 0 )
#define ret0_if_err(x)        retval_if( (x)!=SCE_OK , 0 )
#define ret0_if_fail(x)       retval_if( !SCERROR_CHECK_OK(x) , 0 )
#define broken_if(x)          goto_if ( x, broken )
#define broken_if_fail(x)     goto_if ( !SCERROR_CHECK_OK(x), broken )
#define goto_if_fail(x,label) goto_if ( !SCERROR_CHECK_OK(x), label )

HRESULT GenSCWideError(pwide_t info,pwide_t reason_prefix=0);
HRESULT GenSCWideError(FormatT<wchar_t> const&,pwide_t reason_prefix=0);
HRESULT GenSCWideError(pwide_t info, FormatT<wchar_t> const &reason_prefix);

#endif // ___4f75bf3e_0bea_45b4_94e1_5b567475fb63___
