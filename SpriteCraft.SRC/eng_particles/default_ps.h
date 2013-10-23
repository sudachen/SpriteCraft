
#ifndef ___c522a747_6a89_4db5_bce5_691c453a897b___
#define ___c522a747_6a89_4db5_bce5_691c453a897b___

namespace default_ps {

  struct DPF_PARTICLES_ENGINE;

  struct DPF_EMMITER_CONTEXT: EMITTER_CONTEXT
    {
      rcc_ptr<DPF_PARTICLES_ENGINE> e_;
      BufferT<u32_t> program;
      DPF_EMMITER_CONTEXT(DPF_PARTICLES_ENGINE *e);
      Randomizer randomizer_;
    };

  void Execute(DPF_EMMITER_CONTEXT *, PARTICLE &particle, u32_t tickdelta, int idx);
  bool PspCompile(pwide_t source, BufferT<u32_t> &program, BaseStringT<wchar_t> &errorS);
}

#endif /*___c522a747_6a89_4db5_bce5_691c453a897b___*/
