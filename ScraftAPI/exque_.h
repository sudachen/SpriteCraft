
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

template < class tTx, class tTf >
  struct ScraftAction : ScraftCommand
    {
      tTx* object_;
      tTf func_;
      ScraftAction(tTx* object,tTf func) : object_(object),func_(func) {}
      u32_t Execute(ScraftIQue* que)
        {
          return (object_->*func_)(que);
        }
    };

template < class tTx, class tTf, class A >
  struct ScraftAction1 : ScraftCommand
    {
      tTx* object_;
      tTf func_;
      A a_;
      ScraftAction1(tTx* object,tTf func,const A& a) : object_(object),func_(func),a_(a) {}
      u32_t Execute(ScraftIQue* que)
        {
          return (object_->*func_)(que,a_);
        }
    };

struct ScraftExQue
  {
    ScraftExQue() : que_ ( QueUndefined )
      {
      }

    ~ScraftExQue()
      {
        Dismiss();
      }

    template < class tTx, class tTf >
      long DoAction(tTx* object,tTf func)
        {
          return
            Dismiss(),
            que_ = Scraft::GetExecutor()->Schedule(
              RccPtr(new ScraftAction<tTx,tTf>(object,func))->__iunknown());
        }

    void Dismiss()
      {
        if ( que_ != QueUndefined )
          Scraft::GetExecutor()->DismissQueue(que_),
          que_ = QueUndefined;
      }

    void SetCondVal(unsigned no,bool val)
      {
        if ( que_ != QueUndefined )
          if ( IQuePtr que = Engine->GetExecutor()->GetQueue(que_) )
            que->SetCondVal(no,val);
          else
            que_ = QueUndefined;
      }

    bool CheckCond(unsigned no)
      {
        if ( que_ != QueUndefined )
          if ( IQuePtr que = Engine->GetExecutor()->GetQueue(que_) )
            return que->CheckCond(no);
          else
            que_ = QueUndefined;
        return false;
      }

    bool IsAlive()
      {
        if ( que_ != QueUndefined )
          if ( IQuePtr que = Engine->GetExecutor()->GetQueue(que_) )
            return true;
          else
            que_ = QueUndefined;
        return false;
      }

  private:
    long que_;
    ScraftExQue(const ScraftExQue&);
    void operator = (const ScraftExQue&);
  };
