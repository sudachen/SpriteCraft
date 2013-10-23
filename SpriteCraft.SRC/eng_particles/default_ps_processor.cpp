
#include "../stdafx.h"
#include "../../ScraftAPI/spritecraft.h"
#include "./defs.h"
#include "./cops.h"
#include "../flat_e0.h"

using namespace eng_particles;
#include "./default_ps.h"

namespace default_ps {

  void Execute(DPF_EMMITER_CONTEXT *emcont, PARTICLE &particle, u32_t tickdelta, int idx)
    {
      BufferT<u32_t> &program = emcont->program;
      u32_t* p = &program[0]+particle.epi;
      u32_t* pE = &program[0]+program.Count();
      float* rg = emcont->rg;
      rg[EMITTER_CONTEXT::REG_DT]  = float(tickdelta)/1000.f;
      rg[EMITTER_CONTEXT::REG_IDX] = float(idx);
      while ( p < pE )
        {
          u32_t c = *p++;
          if ( c & 0x80000000UL )
            {
              switch ( c )
                {
                case CMD_LDfl0: rg[EMITTER_CONTEXT::REG_SHADOW00] = *(float*)p++; break;
                case CMD_LDfl1: rg[EMITTER_CONTEXT::REG_SHADOW01] = *(float*)p++; break;
                case CMD_LDfv:
                  rg[EMITTER_CONTEXT::REG_SHADOW1+0] = *(float*)p++;
                  rg[EMITTER_CONTEXT::REG_SHADOW1+1] = *(float*)p++;
                  break;
                case CMD_LDpx: memcpy(&rg[EMITTER_CONTEXT::REG_SHADOW1],&particle.x,sizeof(float)*2); break;
                case CMD_STpx: memcpy(&particle.x,&rg[EMITTER_CONTEXT::REG_SHADOW1],sizeof(float)*2); break;
                case CMD_STpx0: particle.x[0] = rg[EMITTER_CONTEXT::REG_SHADOW1+0]; break;
                case CMD_STpx1: particle.x[1] = rg[EMITTER_CONTEXT::REG_SHADOW1+1]; break;
                case CMD_LDpv: memcpy(&rg[EMITTER_CONTEXT::REG_SHADOW1],&particle.v,sizeof(float)*2); break;
                case CMD_STpv: memcpy(&particle.v,&rg[EMITTER_CONTEXT::REG_SHADOW1],sizeof(float)*2); break;
                case CMD_STpv0: particle.v[0] = rg[EMITTER_CONTEXT::REG_SHADOW1+0]; break;
                case CMD_STpv1: particle.v[1] = rg[EMITTER_CONTEXT::REG_SHADOW1+1]; break;
                case CMD_LDpr: memcpy(&rg[EMITTER_CONTEXT::REG_SHADOW1],&particle.r,sizeof(float)*2); break;
                case CMD_STpr: memcpy(&particle.r,&rg[EMITTER_CONTEXT::REG_SHADOW1],sizeof(float)*2); break;
                case CMD_STpr0: particle.r[0] = rg[EMITTER_CONTEXT::REG_SHADOW1+0]; break;
                case CMD_STpr1: particle.r[1] = rg[EMITTER_CONTEXT::REG_SHADOW1+1]; break;
                case CMD_LDpa: rg[EMITTER_CONTEXT::REG_SHADOW1] = particle.angle; break;
                case CMD_STpa:
                  particle.angle = rg[EMITTER_CONTEXT::REG_SHADOW1];
                  particle.cosin[0] = cosf(particle.angle);
                  particle.cosin[1] = sinf(particle.angle);
                  break;
                case CMD_LDpfr: rg[EMITTER_CONTEXT::REG_SHADOW1] = float(particle.frno); break;
                case CMD_STpfr: particle.frno = long(rg[EMITTER_CONTEXT::REG_SHADOW1]); break;
                case CMD_LDpf:  rg[EMITTER_CONTEXT::REG_SHADOW1] = particle.f; break;
                case CMD_STpf:  particle.f = rg[EMITTER_CONTEXT::REG_SHADOW1]; break;
                case CMD_LDps:  rg[EMITTER_CONTEXT::REG_SHADOW1] = particle.scale*100.f; break;
                case CMD_STps:  particle.scale = rg[EMITTER_CONTEXT::REG_SHADOW1]*0.01f; break;
                case CMD_LDpt:  rg[EMITTER_CONTEXT::REG_SHADOW1] = particle.trans; break;
                case CMD_STpt:
                  particle.trans = rg[EMITTER_CONTEXT::REG_SHADOW1];
                  if ( particle.trans < 0 ) particle.trans = 0;
                  else if ( particle.trans > 100 ) particle.trans = 100;
                  break;
                case CMD_INDEP:
                  {
                    if ( Sprite *s = (Sprite*)emcont->sprite )
                      {
                        long layer = s->GetLayer();
                        particle.flags |= ComplexSprite::Info::IS_INDEPENDED;
                        particle.x[0] += s->GetScrX() - SPRITES::GetLayerX(layer);
                        particle.x[1] += s->GetScrY() - SPRITES::GetLayerY(layer);
                      }
                  }
                  break;
                case CMD_UPDAT: particle.epi = p-&program[0]; break;
                case CMD_UPDATONLY: particle.epi = p-&program[0]; goto lb_ret;
                }
            }
          else
            {
              float* vl0 = rg + (c & 0x0ff); c >>= 8;
              float* vl1 = rg + (c & 0x0ff); c >>= 8;
              float* res = rg + (c & 0x0ff); c >>= 8;
              switch ( c )
                {
                case CMD_NEGs:
                  *res = -*vl0; break;
                case CMD_NEGv:
                  *res = -*vl0; *(res+1) = -*(vl0+1);  break;
                case CMD_ADDvv:
                  *res = *vl0 + *vl1; *(res+1) = *(vl0+1) + *(vl1+1);  break;
                case CMD_ADDvs:
                  *res = *vl0 + *vl1; *(res+1) = *(vl0+1) + *vl1;  break;
                case CMD_ADDss:
                  *res = *vl0 + *vl1;  break;
                case CMD_MULvs:
                  *res = *vl0 * *vl1; *(res+1) = *(vl0+1) * *vl1;  break;
                case CMD_MULss:
                  *res = *vl0 * *vl1;  break;
                case CMD_MODvs:
                  *res = float(long(*vl0) % long(*vl1)); *(res+1) = float(long(*(vl0+1)) % long(*vl1));  break;
                case CMD_MODss:
                  *res = float(long(*vl0) % long(*vl1));  break;
                case CMD_DIVvs:
                  *res = *vl0 / *vl1; *(res+1) = *(vl0+1) / *vl1;  break;
                case CMD_DIVss:
                  *res = *vl0 / *vl1;  break;
                case CMD_ROTvs:
                  {
                    const float cosval = cosf(*vl1);
                    const float sinval = sinf(*vl1);
                    const float x = vl0[0];
                    const float y = vl0[1];
                    res[0] = (x*cosval  + y*sinval);
                    res[1] = (-x*sinval + y*cosval);
                    break;
                  }
                case CMD_RND:
                  {
                    float d = *vl1-*vl0;
                    *res = fabs(d) > 0.0001f ? emcont->randomizer_.RandomF()*d + *vl0 : *vl0;
                    break;
                  }
                case CMD_MOVs:
                  *res = *vl0; break;
                case CMD_MOVv:
                  *res = *vl0; *(res+1) = *(vl0+1); break;
                }
            }
        }
    lb_ret:;
    }

}