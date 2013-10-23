
#ifndef ___4d2f6b6f_455c_47a2_8076_d6bbf42aeef8___
#define ___4d2f6b6f_455c_47a2_8076_d6bbf42aeef8___

namespace eng_particles {

  enum { CF_COUNT = 16 };

  struct EMITTER_CONTEXT;
  struct PARTICLE;

  struct EMITTER_OPS
    {
      void (__cdecl *opClear)(EMITTER_CONTEXT*);
      void (__cdecl *opEmit)(EMITTER_CONTEXT*,PARTICLE*,int);
      void (__cdecl *opUpdate)(EMITTER_CONTEXT*,PARTICLE*,long ms,int);
      void (__cdecl *opSimulate)(EMITTER_CONTEXT*,PARTICLE*,long ms,int fps,int);
      bool (__cdecl *opProgram)(EMITTER_CONTEXT*,wchar_t const *program,wchar_t *error_text_buffer /*128b*/);
    };

  struct EMITTER_CONTEXT
    {
      EMITTER_OPS ops;

      enum
        {
          REG_CF        = 0,
          REG_CF_COUNT  = CF_COUNT,
          REG_CF_END    = REG_CF + CF_COUNT*2,
          REG_SHADOW00  = REG_CF_END,
          REG_SHADOW01,
          REG_SHADOW1,
          REG_SHADOW1_HI,
          REG_A,
          REG_A0,
          REG_Ai_COUNT  = 4,
          REG_A_END     = REG_A0 + REG_Ai_COUNT*2,
          REG_DT        = REG_A_END,
          REG_IDX,
          REG_T,
          REG_COUNT,
        };

      float rg[REG_COUNT];
      long  quantity_cf[2];
      long  period_cf[2];
      long  timelimit_cf[2];
      long  timelimit;
      long  lastemission;
      long  nextemission;

      void  *sprite;
    };

  inline void Clear( EMITTER_CONTEXT *em )
    {
      memset(em->rg,0,sizeof(em->rg));
      memset(em->quantity_cf,0,sizeof(em->quantity_cf));
      memset(em->period_cf,0,sizeof(em->period_cf));
      memset(em->timelimit_cf,0,sizeof(em->timelimit_cf));
      em->sprite = 0;
      //em->rg[EmCfInitScale*2] = em->rg[EmCfInitScale*2+1] = 100.0f;
      if (em->ops.opClear) em->ops.opClear(em);
    }

  struct PARTICLE
    {
      long  flags;
      float x[2];
      float scale;
      float cosin[2];
      float trans;
      long  frno;
      // -- 32 bytes
      float v[2];
      float r[2];
      float f;
      long  epi;
      EMITTER_CONTEXT* context_;
      float angle;
    };

  struct PARTICLES_ENGINE
    {
       EMITTER_CONTEXT *(__cdecl *allocEmitter)(PARTICLES_ENGINE *);
       void (__cdecl *freeEmitter)(PARTICLES_ENGINE *,EMITTER_CONTEXT *);
       void (__cdecl *release)(PARTICLES_ENGINE *);
       void (__cdecl *addRef)(PARTICLES_ENGINE *);
    };

} // name space

#endif /*___4d2f6b6f_455c_47a2_8076_d6bbf42aeef8___*/
