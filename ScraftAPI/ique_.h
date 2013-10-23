
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

struct CXX_NO_VTABLE ScraftIQue
  {
    IQue*          QueryIfs() { return (IQue*)this; }
    u32_t          AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t          Release()  { return ((IUnknown*)this)->Release(); }
    long           QueNo();
    u32_t          Millis();
    u32_t          Delta();
    u32_t          CommandTime();
    bool           IsSuspended();
    bool           IsEmpty();
    void           Dismiss();
    void           Schedule(IUnknown* command);
    void           Suspend();
    void           Resume();
    void           Clear();
    tagQueState    ExecuteNext();
    bool           CheckCond(long condno);
    void           SetCondVal(long condno,bool val);
    void           ScheduleSleep(u32_t millis);
    void           ScheduleDismiss();
    void           ScheduleHolder(IUnknown* holder);
    void           Alert();

    struct Property_dleta   { TEGGO_R_PROPERTY(ScraftIQue,u32_t,Delta,delta) } delta;
    struct Property_millis  { TEGGO_R_PROPERTY(ScraftIQue,u32_t,Millis,millis) } millis;
    struct Property_cmdtime { TEGGO_R_PROPERTY(ScraftIQue,u32_t,CommandTime,cmdtime)  } cmdtime;
    struct Property_queno   { TEGGO_R_PROPERTY(ScraftIQue,long,QueNo,queno) } queno;

  };

inline IQue* scraft_query_ifs(ScraftIQue* u)
  {
    return u->QueryIfs();
  }
