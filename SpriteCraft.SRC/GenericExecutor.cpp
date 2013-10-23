
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

#include "StdAfx.h"
#include "../ScraftAPI/spritecraft.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Engine")
#endif

enum
  {
    QueOrderCommon      = 0,
    QueOrderPostUpdate  = 1,
    QueOrderMandatory   = 0xffffffff
  };

struct CxxCommand : TeggoIfs0<ICommand>
  {
    CxxCommand()
      {
      }
    virtual ~CxxCommand()
      {
      }

    virtual HRESULT __stdcall Execute(IQue* que,long* sleeptime)
      {
        *sleeptime = ExecuteSelf(que);
        return S_OK;
      }
    virtual long ExecuteSelf(IQue* que) = 0;
  };

struct CmdDismissQueue__ : TeggoObject<CxxCommand>
  {
    CmdDismissQueue__()
      {
      }
    virtual ~CmdDismissQueue__()
      {
      }

    virtual long ExecuteSelf(IQue* que)
      {
        que->Dismiss();
        return long(CommandStateEnd);
      }
  };

struct CmdWait__ : public TeggoObject<CxxCommand>
  {
    u32_t timeout_;

    CmdWait__(u32_t timeout) : timeout_(timeout)
      {
      }
    virtual ~CmdWait__()
      {
      }

    virtual long ExecuteSelf(IQue*)
      {
        u32_t tout = timeout_;
        timeout_ = u32_t(CommandStateEnd);
        return tout;
      }
  };

struct CmdHolder__ : public TeggoObject<CxxCommand>
  {
    rcc_ptr<IUnknown> _;

    CmdHolder__(IUnknown* t) : _(Refe(t))
      {
      }
    virtual ~CmdHolder__()
      {
      }

    virtual long ExecuteSelf(IQue* que)
      {
        return CommandStateEnd;
      }
  };

struct CmdDispatch__ : TeggoObject<CxxCommand>
  {
    rcc_ptr<IDispatch> _;

    CmdDispatch__(IDispatch* t) : _(Refe(t))
      {
      }
    virtual ~CmdDispatch__()
      {
      }

    virtual long ExecuteSelf(IQue* que)
      {
        DispResult r = *(DispInvoker(+_,L"ICommand_Execute") % que);

        if ( r.IsSuccess() && r.HasResult() )
          return r.ToLong();
        else
          return CommandStateEnd;
      }
  };

struct CmdExecute__ : TeggoObject< TeggoIfs0<ICommand> >
  {
    rcc_ptr<IDispatch> _;

    CmdExecute__(IDispatch* t) : _(Refe(t))
      {
      }
    virtual ~CmdExecute__()
      {
      }

    virtual HRESULT __stdcall Execute(IQue* que,long* sleeptime)
      {
        DispResult r = *(DispInvoker(+_,1) % que);

        if ( r.IsSuccess() && r.HasResult() )
          *sleeptime = r.ToLong();
        else
          *sleeptime = CommandStateEnd;
        return S_OK;
      }
  };


static int que_unique_no = QueUndefined;

struct GenericExecutor;

struct GenericExecutionQue : public TeggoObject< TeggoPyDispImpl<IQue,0> >
  {
    long que_no_;
    bool paused_;
    bool scheduling_diabled_;
    u32_t nextclock_;
    u32_t lastclock_;
    u32_t pausclock_;
    u32_t cmd_startclock_;
    u32_t condmap_;
    GenericExecutor* master_;

    rcc_ptr<ICommand> current_;
    SlistT< rcc_ptr<ICommand> > que_;

    GenericExecutionQue(long name,GenericExecutor* executor);
    virtual ~GenericExecutionQue();

    virtual HRESULT __stdcall get_delta(long *pVal);
    virtual HRESULT __stdcall Alert();
    virtual HRESULT __stdcall get_queno (long * pVal);
    virtual HRESULT __stdcall get_millis (long * pVal);
    virtual HRESULT __stdcall get_cmdtime (long * pVal);
    virtual HRESULT __stdcall IsSuspended (VARIANT_BOOL* pVal);
    virtual HRESULT __stdcall IsEmpty (VARIANT_BOOL* pVal);
    virtual HRESULT __stdcall Dismiss ();
    virtual HRESULT __stdcall Schedule_ (struct ICommand* cmd);
    virtual HRESULT __stdcall ScheduleHolder (struct IUnknown* pI);
    virtual HRESULT __stdcall Schedule (struct IUnknown* pI);
    virtual HRESULT __stdcall ScheduleSleep (long millis);
    virtual HRESULT __stdcall ScheduleDismiss ();
    virtual HRESULT __stdcall Suspend ();
    virtual HRESULT __stdcall Resume ();
    virtual HRESULT __stdcall Clear ();
    virtual HRESULT __stdcall ExecuteNext (enum tagQueState* pVal);
    virtual HRESULT __stdcall CheckCond (long cond,VARIANT_BOOL * pVal);
    virtual HRESULT __stdcall SetCondVal (long cond,VARIANT_BOOL val);
    virtual HRESULT __stdcall CreateAnother(long queno,long order,IQue** pI);

    bool IsEmpty_();
    tagQueState ExecuteNext_();

    void ForgetMaster_()
    {
      master_ = 0;
    }

  };

struct GenericExecutor :
  TeggoPyDispImpl< IExecutor, 0,
    TeggoCopntImpl< &DIID__IQueueEvents, &CLSID__CoQueueConnectionPoint,
      TeggoISupportErrorInfo > >
  {
    long queue_no_;

    typedef GenericExecutionQue Queue;

    struct QOrder : Refcounted
      {
        Queue* q;
        long qno;
        long order;
        QOrder(Queue* qq,long no,long o) : q(qq),qno(no),order(o)
          {
          }
        ~QOrder()
          {
            q->Release();
          }
      };

    struct QLessP
      {
        bool operator() (rcc_ptr<QOrder> const &qo0,rcc_ptr<QOrder> const &qo1) const
          {
            return qo0->order < qo1->order
                    ? true
                    : qo0->order == qo1->order
                      ? qo0->q < qo1->q
                      : false;
          }
      };

    typedef ArrayT< rcc_ptr<QOrder> > ORDER__;
    typedef FlatmapT< long, rcc_ptr<QOrder> > QUEUES__;

    ORDER__ order_;
    QUEUES__ queues_;

    virtual ~GenericExecutor();

    QOrder* GetQOrder(long qno);
    virtual HRESULT __stdcall _LogQueues();
    virtual HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);
    virtual HRESULT __stdcall CreateQueue (long que,struct IQue** pI);
    virtual HRESULT __stdcall CreateQueueEx (long que,long order,struct IQue** pI);
    virtual HRESULT __stdcall GetQueue (long que,struct IQue** pI);
    virtual HRESULT __stdcall ScheduleEx (long que, struct IUnknown * pI );
    virtual HRESULT __stdcall _Execute (long stage);
    virtual HRESULT __stdcall ExecuteMandatory (long que);
    virtual HRESULT __stdcall DismissQueue (long que);
    virtual HRESULT __stdcall CreateAnother(IExecutor** pI);
    virtual HRESULT __stdcall _Purge();
    virtual HRESULT __stdcall Schedule(IUnknown* cmd,long* queno);
    virtual HRESULT InternalAdvise(IUnknown* dispifs,u32_t* cookie);
    virtual HRESULT InternalUnadvise(u32_t cookie);
    void DismissQueue_(long no);
  };

HRESULT __stdcall GenericExecutor_RegiterPyType()
  {
    return GenericExecutor::RegisterPyType("_Executor");
  }

HRESULT __stdcall GenericExecutorQue_RegisterPyType()
  {
    return GenericExecutionQue::RegisterPyType("_Que");
  }

HRESULT __stdcall GenericExecutionQue::IsEmpty (VARIANT_BOOL* pVal)
  {
    _SYNCHRONIZE_;
    *pVal = IsEmpty_()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::ScheduleHolder (struct IUnknown* pI)
  {
    // synhronized in schedule_
    return Schedule_( +rcc_ptr<ICommand>(new CmdHolder__(pI)) );
  }

HRESULT __stdcall GenericExecutionQue::Schedule (struct IUnknown* pI)
  {
    // synhronized in schedule_
    rcc_ptr<IUnknown> ppI(pI,true);

    if ( rcc_ptr<ICommand> cI =  teggo::ifs_cast<ICommand>(ppI) )
      {
        return Schedule_( +cI );
      }

    if ( rcc_ptr<IDispatch> dI = teggo::ifs_cast<IDispatch>(ppI) )
      {
        return Schedule_( +rcc_ptr<ICommand>(new CmdDispatch__(+dI)) );
      }

    return ScraftError(L"unknown type of command");
  }

HRESULT __stdcall GenericExecutionQue::ScheduleSleep (long millis)
  {
    // synhronized in schedule_
    return Schedule_( +rcc_ptr<ICommand>(new CmdWait__(millis)) );
  }

HRESULT __stdcall GenericExecutionQue::ScheduleDismiss ()
  {
    // synhronized in schedule_
    return Schedule_( +rcc_ptr<ICommand>(new CmdDismissQueue__) );
  }

HRESULT __stdcall GenericExecutionQue::ExecuteNext (tagQueState* pVal)
  {
    _SYNCHRONIZE_;

    if ( master_ == 0 )
      return GenSCWideError(L"queue dismissed",L":ExecuteNext:");

    *pVal = ExecuteNext_();

    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::get_queno (long * pVal)
  {
    _SYNCHRONIZE_;

    *pVal = que_no_;
    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::get_millis (long * pVal)
  {
    _SYNCHRONIZE_;

    *pVal = nextclock_;
    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::IsSuspended (VARIANT_BOOL* pVal)
  {
    _SYNCHRONIZE_;

    if ( master_ == 0 )
      return GenSCWideError(L"queue dismissed",L":IsSuspended:");

    *pVal = paused_?VARIANT_TRUE:VARIANT_FALSE;

    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::Dismiss ()
  {
    _SYNCHRONIZE_;

    Xdbg( Ldbg << _S*"GenericExecutionQue::Dismiss %p" %this );

    if ( master_ )
      master_->DismissQueue_(que_no_);

    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::Schedule_ (ICommand* cmd)
  {
    _SYNCHRONIZE_;

    if ( scheduling_diabled_ || !master_ )
      return ScraftError(L"scheduling disbaled now");

    if ( que_.Empty() && !current_ )
      nextclock_ = GetMillis_OnFrame()+1;

    que_.Append(RccRefe(cmd));

    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::Suspend ()
  {
    _SYNCHRONIZE_;

    if ( master_ == 0 )
      return ScraftError(L"queue dismissed");

    if ( !paused_ )
      {
        paused_ = true;
        pausclock_ = GetMillis_OnFrame();
      }

    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::Resume ()
  {
    _SYNCHRONIZE_;

    if ( master_ == 0 )
      return ScraftError(L"queue dismissed");

    if ( paused_ )
      {
        paused_ = false;
        u32_t pc = GetMillis_OnFrame() - pausclock_;
        nextclock_ = nextclock_ + pc;
        lastclock_ = lastclock_ + pc;
      }

    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::Clear ()
  {
    _SYNCHRONIZE_;

    scheduling_diabled_ = true;

    nextclock_ = 0;
    Unrefe(current_);
    que_.Clear();

    scheduling_diabled_ = false;

    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::CheckCond (long cond,VARIANT_BOOL * pVal)
  {
    _SYNCHRONIZE_;

    if ( master_ == 0 )
      return ScraftError(L"queue dismissed");

    if ( cond < 0 || cond >= 32 )
      return ScraftError(_S*L"invalid condition index %d" %cond);

    *pVal = VARIANT_FALSE;

    if ( (condmap_ & (1 << cond)) != 0 )
      *pVal = VARIANT_TRUE;

    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::SetCondVal (long cond,VARIANT_BOOL val)
  {
    _SYNCHRONIZE_;

    if ( master_ == 0 )
      return ScraftError(L"queue dismissed");

    if ( cond < 0 || cond >= 32 )
      ScraftError(_S*L"invalid condition index %d" %cond);

    u32_t val_mask = (val!=VARIANT_FALSE?1:0)<<cond;
    condmap_ = (condmap_ & ~(1<<cond)) | val_mask;

    return S_OK;
  }

bool GenericExecutionQue::IsEmpty_()
  {
    // no need _SYNCHRONIZE_;
    return !current_ && que_.Empty();
  }

tagQueState GenericExecutionQue::ExecuteNext_()
  {
    // no need _SYNCHRONIZE_;

    if ( IsEmpty_() )
      return QueStateEmpty;

    if ( paused_ )
      return QueStateScheduled;

    unsigned millis = GetMillis_OnFrame();
    REQUIRE ( millis < 0x80000000UL );

    if ( millis >= nextclock_ )
      {
        if ( !current_ )
          {
            current_ = que_.First();
            que_.Pop();
            cmd_startclock_ = millis;
            REQUIRE ( !!current_ );
          }

        u32_t tout = CommandStateRepeat;
        u32_t tickdelta = long(millis-lastclock_);
        HRESULT hr = current_->Execute(this,(long*)&tout);

        lastclock_ = millis;
        REQUIRE ( lastclock_ < 0x80000000UL );

        if ( !FAILED(hr) && tout != u32_t(CommandStateEnd))
          {
            STRICT_REQUIRE ( tout < 0x80000000UL );
            if ( tout >= 0x80000000UL )
              {
                Lout << _S*"executor: "
                           "ICommand_Execute(QUE:%?) invalid returned timeout 0x%08x"
                          %que_no_ %tout;
                goto purge_the_command;
              }
            nextclock_ = millis+tout;
            STRICT_REQUIRE ( nextclock_ < 0x80000000UL );
            if ( nextclock_ >= 0x80000000UL )
              {
                Lout <<
                  _S*"executor: "
                     "ICommand_Execute(QUE:%?) bad next invokation time 0x%08x"
                    %que_no_ %nextclock_;
                goto purge_the_command;
              }
            return QueStateScheduled;
          }
        else
          {
            if ( FAILED(hr) )
              {
                Lout << _S*"executor: "
                           "ICommand_Execute(QUE:%?):deleta=%? failed, HRESUT = 0x%08x"
                          %que_no_ %tickdelta %hr;
              }
            goto purge_the_command;
          }

        purge_the_command:
            Unrefe(current_);
            return que_.Empty()?QueStateEmpty:QueStateContinue;
      }
    else
      {
        return QueStateScheduled;
      }
  }

GenericExecutionQue::GenericExecutionQue(long qno,GenericExecutor* master)
  : paused_(0),scheduling_diabled_(false),
    nextclock_(0),condmap_(0),cmd_startclock_(0)
  {
    lastclock_ = GetMillis_OnFrame();
    que_no_ = qno;
    master_ = master;
  }

GenericExecutionQue::~GenericExecutionQue()
  {
    _SYNCHRONIZE_;
    Clear();
  }

HRESULT __stdcall GenericExecutionQue::CreateAnother(long queno,long order,IQue** pI)
  {
    _SYNCHRONIZE_;

    if ( master_ == 0 )
      return ScraftError(L"queue dismissed");

    return master_->CreateQueueEx(queno,order,pI);
  }

HRESULT __stdcall GenericExecutionQue::Alert()
  {
    _SYNCHRONIZE_;

    nextclock_ = GetMillis_OnFrame();
    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::get_cmdtime(long *pVal)
  {
    _SYNCHRONIZE_;

    *pVal = teggo_max<long>(long(GetMillis_OnFrame()-cmd_startclock_),0);
    return S_OK;
  }

HRESULT __stdcall GenericExecutionQue::get_delta(long *pVal)
  {
    _SYNCHRONIZE_;

    *pVal = teggo_max<long>(long(GetMillis_OnFrame()-lastclock_),0);
    return S_OK;
  }


IExecutor* CreateGenericExecutor()
  {
    return new TeggoObject<GenericExecutor>();
  }

GenericExecutor::QOrder* GenericExecutor::GetQOrder(long qno)
  {
    if ( rcc_ptr<QOrder> *q = queues_.Get(qno) )
      return +*q;
    else
      return 0;
  }

HRESULT __stdcall GenericExecutor::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(IExecutor),riid)?S_OK:S_FALSE;
  }

HRESULT __stdcall GenericExecutor::CreateQueue(long que,struct IQue** pI)
  {
    // synhronized in CreateQueueEx
    return CreateQueueEx(que,(que>=0?QueOrderCommon:QueOrderPostUpdate),pI);
  }

HRESULT __stdcall GenericExecutor::CreateQueueEx (long que,long order,struct IQue** pI)
  {
    _SYNCHRONIZE_;

    if ( que == QueUndefined )
      {
        if ( que_unique_no == INT_MAX )
          que_unique_no = QueUndefined ;

        do
          que = ++que_unique_no;
        while ( GetQOrder(que) != 0 );
      }

    rcc_ptr<QOrder> qo(new QOrder( new Queue(que,0), que, order ));

    if (  queues_.Get(que) )
      return ScraftError(_S*L"que %? already exists" %que);

    queues_.Put(que,qo);
    qo->q->master_ = this;
    order_.Binsert(qo,QLessP());

    return qo->q->QueryInterface(TEGGO_GUIDOF(IQue),(void**)pI);
  }

HRESULT __stdcall GenericExecutor::GetQueue(long que,struct IQue** pI)
  {
    _SYNCHRONIZE_;

    if ( QOrder* qo = GetQOrder(que) )
      return qo->q->QueryInterface(TEGGO_GUIDOF(IQue),(void**)pI);
    else
      return *pI = 0, S_OK;
  }

HRESULT __stdcall GenericExecutor::ScheduleEx(long que, struct IUnknown * pI )
  {
    _SYNCHRONIZE_;

    if ( QOrder* qo = GetQOrder(que) )
      return qo->q->Schedule(pI);
    else
      return ScraftError(_S*L"que %? is not exists" %que);
  }

HRESULT __stdcall GenericExecutor::_Execute (long stage)
  {
    _SYNCHRONIZE_;

    ArrayT< rcc_ptr<QOrder> > ov(order_.Begin(),order_.End());
      // !!! Á‡ÏÂÌËÚ¸ ‚ ·Û‰Û˛˘ÂÏ Ì‡ LocalArrayT

    for ( rcc_ptr<QOrder>* i = ov.Begin(); i != ov.End(); ++i )
      {
        if (  ( stage == 0 && (*i)->order == QueOrderCommon )
            ||( stage == 1 && (*i)->order == QueOrderPostUpdate ) )
          {
            if ( !(*i)->q->IsEmpty_() )
              while ( (*i)->q->ExecuteNext_() == QueStateContinue )
                {/*nothing*/}
          }
      }

    return S_OK;
  }

HRESULT __stdcall GenericExecutor::ExecuteMandatory (long que)
  {
    _SYNCHRONIZE_;
    if ( rcc_ptr<QOrder> qo = RccRefe(GetQOrder(que)) )
      {
        if ( !qo->q->IsEmpty_() )
          while ( qo->q->ExecuteNext_() == QueStateContinue )
            {/*nothing*/}
          return S_OK;
      }
    else
      return ScraftError(_S*L"que %? is not exists" %que);
  }

HRESULT __stdcall GenericExecutor::DismissQueue (long que)
  {
    _SYNCHRONIZE_;

    DismissQueue_(que);
    return S_OK;
  }

void GenericExecutor::DismissQueue_(long no)
  {
    _SYNCHRONIZE_;

    if ( QOrder *qo = GetQOrder(no) )
      {
        qo->q->Clear();
        qo->q->ForgetMaster_();
        order_.Berase(RccRefe(qo),QLessP());
        queues_.Erase(no);
      }
  }

HRESULT __stdcall GenericExecutor::CreateAnother(IExecutor** pI)
  {
    _SYNCHRONIZE_;

    *pI = new TeggoObject<GenericExecutor>();
    return S_OK;
  }

GenericExecutor::~GenericExecutor()
  {
    _SYNCHRONIZE_;

    _Purge();
  }

HRESULT __stdcall GenericExecutor::_Purge()
  {
    _SYNCHRONIZE_;

    for ( rcc_ptr<QOrder> *i = order_.Begin(); i != order_.End(); ++i )
      (*i)->q->Clear();

    order_.Purge();
    queues_.Purge();

    return S_OK;
  }

HRESULT __stdcall GenericExecutor::Schedule(IUnknown* cmd,long* queno)
  {
    _SYNCHRONIZE_;

    rcc_ptr<IQue> que;

    if ( CreateQueue(QueUndefined,&+que) == S_OK )
      {
        if ( que->Schedule(cmd) )
          {
            que->Dismiss();
            return E_FAIL;
          }
        que->ScheduleDismiss();
        que->get_queno(queno);
        return S_OK;
      }
    else
      return E_FAIL;
  }

HRESULT GenericExecutor::InternalAdvise(IUnknown* dispifs,u32_t* cookie)
  {
    _SYNCHRONIZE_;

    rcc_ptr<CmdExecute__> cmd ( new CmdExecute__((IDispatch*)dispifs) );
    rcc_ptr<IQue> que;

    if ( CreateQueue(QueUndefined,&+que) == S_OK )
      {
        que->get_queno((long*)cookie);
        if ( que->Schedule((ICommand*)+cmd) )
          {
            que->Dismiss();
            return E_FAIL;
          }
        que->ScheduleDismiss();
        return S_OK;
      }
    else
      return E_FAIL;
  }

HRESULT GenericExecutor::InternalUnadvise(u32_t cookie)
  {
    _SYNCHRONIZE_;

    return DismissQueue(cookie);
  }


HRESULT __stdcall GenericExecutor::_LogQueues()
  {
    Lout << "engine: *EXECUTOR/QUEUES*";
    if ( order_.Empty() )
      Lout << "engine:   none scheduled";
    else
      {
        Lout << _S*"engine: frameticks %d" %GetMillis_OnFrame();
        for ( rcc_ptr<QOrder> *i = order_.Begin(); i != order_.End(); ++i )
          {
            QOrder* qo = +(*i);
            Lout << _S*"engine: [queue] %d (%s) length %d, lastFT %d, nextFT %d"
                    %qo->qno
                    %( qo->order == QueOrderCommon ? "common" :
                       qo->order == QueOrderPostUpdate ? "post" :
                       "mandatory" )
                    %qo->q->que_.Count()
                    %qo->q->lastclock_
                    %qo->q->nextclock_;
          }
      }
    return S_OK;
  }
