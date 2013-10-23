
#include "../stdafx.h"
#include "../../ScraftAPI/spritecraft.h"
#include "./defs.h"
#include "./cops.h"

using namespace eng_particles;

#include "./default_ps.h"
extern "C" const wchar_t particles_default_program[];

namespace default_ps {

  void __cdecl _OpClear(EMITTER_CONTEXT *ctx)
    {
      ((DPF_EMMITER_CONTEXT*)ctx)->program.Clear();
    }

  void __cdecl _OpUpdate(EMITTER_CONTEXT *ctx,PARTICLE *p,long tickdelta,int idx)
    {
      DPF_EMMITER_CONTEXT *c = ((DPF_EMMITER_CONTEXT*)ctx);
      if ( tickdelta && c->program.Count() ) Execute(c,*p,tickdelta,idx);
    }

  bool __cdecl _OpProgram(EMITTER_CONTEXT *ctx,wchar_t const *program,wchar_t *error_text_buffer /*128 chars*/)
    {
      DPF_EMMITER_CONTEXT *c = ((DPF_EMMITER_CONTEXT*)ctx);
      StringT<wchar_t> errorS;
      if ( !PspCompile(program,c->program,errorS) )
        {
          if ( error_text_buffer )
            {
              error_text_buffer[127] = 0;
              wcsncpy(error_text_buffer,+errorS,127);
            }
          return false;
        }
      return true;
    }

  DPF_EMMITER_CONTEXT::DPF_EMMITER_CONTEXT(DPF_PARTICLES_ENGINE *e) : e_(Refe(e))
    {
      memset(&ops,sizeof(ops),0);
      this->ops.opClear    = &_OpClear;
      //this->ops.opSimulate = &_OpSimulate;
      //this->ops.opEmit   = &_OpEmit;
      Clear(this);
      this->ops.opUpdate   = &_OpUpdate;
      this->ops.opProgram  = &_OpProgram;
      _OpProgram(this,particles_default_program,0);
    }

  struct DPF_PARTICLES_ENGINE: PARTICLES_ENGINE
    {
      unsigned refcount_;
      DPF_PARTICLES_ENGINE();
      void AddRef()
        {
          ++refcount_;
        }
      void Release()
        {
          if ( ! --refcount_ )
            delete this;
        }
    };

  EMITTER_CONTEXT * __cdecl _peAllocEmitter(PARTICLES_ENGINE *e)
    {
      DPF_PARTICLES_ENGINE *eng = (DPF_PARTICLES_ENGINE*)e;
      DPF_EMMITER_CONTEXT *c = new DPF_EMMITER_CONTEXT(eng);
      return c;
    }

  void __cdecl _peFreeEmitter(PARTICLES_ENGINE *e,EMITTER_CONTEXT *ctx)
    {
      DPF_EMMITER_CONTEXT *c = ((DPF_EMMITER_CONTEXT*)ctx);
      delete c;
    }

  void __cdecl _peRelease(PARTICLES_ENGINE *e)
    {
      ((DPF_PARTICLES_ENGINE*)e)->Release();
    }

  void __cdecl _peAddRef(PARTICLES_ENGINE *e)
    {
      ((DPF_PARTICLES_ENGINE*)e)->AddRef();
    }

  DPF_PARTICLES_ENGINE::DPF_PARTICLES_ENGINE() : refcount_(1)
    {
      memset((PARTICLES_ENGINE*)this,0,sizeof(PARTICLES_ENGINE));
      allocEmitter = _peAllocEmitter;
      freeEmitter  = _peFreeEmitter;
      release      = _peRelease;
      addRef       = _peAddRef;
    }
}

extern "C" PARTICLES_ENGINE *MakeDefaultPsEngine()
 {
   return new default_ps::DPF_PARTICLES_ENGINE();
 }
