
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

#if !defined SCRAFT_INTERNAL_MARKER
#error "should be included in scraftapi.h only"
#endif

template<class tTx,class tTf> struct ScraftAction;
template<class tTx,class tTf,class tTa> struct ScraftAction1;

struct CXX_NO_VTABLE ScraftIExecutor
  {

    IExecutor*     QueryIfs() { return (IExecutor*)this; }
    u32_t          AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t          Release()  { return ((IUnknown*)this)->Release(); }
    IQuePtr        CreateQueue(long queno = QueUndefined);
    IQuePtr        GetQueue(long queno);
    void           ScheduleEx(long queno,IUnknown* command);
    long           Schedule(IUnknown* command);
    void           DismissQueue(long queno);

    template<class tTx,class tTf>
      long ScheduleAction(tTx* object,tTf func)
        {
          return Schedule(RccPtr(new ScraftAction<tTx,tTf>(object,func))->__iunknown());
        }

    template<class tTx,class tTf,class tTa>
      long ScheduleAction1(tTx* object,tTf func,const tTa& a)
        {
          return Schedule(RccPtr(new ScraftAction1<tTx,tTf,tTa>(object,func,a))->__iunknown());
        }
  };

inline IExecutor* scraft_query_ifs(ScraftIExecutor* u)
  {
    return u->QueryIfs();
  }

struct CXX_NO_VTABLE ScraftExecutable : TeggoIfs0 < ICommand >
  {
    virtual u32_t Execute(ScraftIQue* que) = 0;
    virtual HRESULT __stdcall Execute(IQue*,long* sleeptime);
  };

typedef TeggoObject< ScraftExecutable > ScraftCommand;
typedef ScraftCommand ScraftTask;
