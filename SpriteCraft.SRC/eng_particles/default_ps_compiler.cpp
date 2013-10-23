

#include "../stdafx.h"
#include "../../ScraftAPI/spritecraft.h"
#include "./defs.h"
#include "./cops.h"

#define SCANF_REGFMT L"%31[-a-z0-9.();]"

using namespace eng_particles;
#include "./default_ps.h"

namespace default_ps {

  struct REGISTER
    {
      enum
        {
          IS_VALUE    = (1UL << 8),
          IS_HIPART   = (1UL << 9),
          IS_VECTOR   = (1UL << 10),
          IS_PARTICLE = (1UL << 11),
          IS_VALUE1   = (1UL << 12),
          PAR_REG_X   = (1UL << 0),
          PAR_REG_V   = (1UL << 1),
          PAR_REG_R   = (1UL << 2),
          PAR_REG_MASK = PAR_REG_X|PAR_REG_V|PAR_REG_R
        };
      u32_t flags;
      u32_t value;
      u32_t value1;
      long    regno;
      u32_t ldcmd;
      u32_t stcmd;
      u32_t operator *()
        {
          return regno;
        }
    };

  bool RegResolve(pwide_t name, REGISTER *reg)
    {
      long r0,r1;
      wchar_t c;
      float fval;
      float fval1;
      memset( reg, 0, sizeof(REGISTER) );
      /**/ if ( !wcscmp(name,L"a") )
        {
          reg->regno = EMITTER_CONTEXT::REG_A;
        }
      else if ( !wcscmp(name,L"v") )
        {
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1;
          reg->flags = REGISTER::IS_VECTOR|REGISTER::IS_PARTICLE|REGISTER::PAR_REG_V;
          reg->ldcmd = CMD_LDpv;
          reg->stcmd = CMD_STpv;
        }
      else if ( !wcscmp(name,L"x") )
        {
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1;
          reg->flags = REGISTER::IS_VECTOR|REGISTER::IS_PARTICLE|REGISTER::PAR_REG_X;
          reg->ldcmd = CMD_LDpx;
          reg->stcmd = CMD_STpx;
        }
      else if ( !wcscmp(name,L"r") )
        {
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1;
          reg->flags = REGISTER::IS_VECTOR|REGISTER::IS_PARTICLE|REGISTER::PAR_REG_R;
          reg->ldcmd = CMD_LDpr;
          reg->stcmd = CMD_STpr;
        }
      else if ( !wcscmp(name,L"t") )  { reg->regno = EMITTER_CONTEXT::REG_T;}
      else if ( !wcscmp(name,L"dt") ) { reg->regno = EMITTER_CONTEXT::REG_DT; }
      else if ( !wcscmp(name,L"idx") ) { reg->regno = EMITTER_CONTEXT::REG_IDX; }
      else if ( !wcscmp(name,L"trans") )
        {
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1;
          reg->flags = REGISTER::IS_PARTICLE;
          reg->ldcmd = CMD_LDpt;
          reg->stcmd = CMD_STpt;
        }
      else if ( !wcscmp(name,L"scale") )
        {
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1;
          reg->flags = REGISTER::IS_PARTICLE;
          reg->ldcmd = CMD_LDps;
          reg->stcmd = CMD_STps;
        }
      else if ( !wcscmp(name,L"angle") )
        {
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1;
          reg->flags = REGISTER::IS_PARTICLE;
          reg->ldcmd = CMD_LDpa;
          reg->stcmd = CMD_STpa;
        }
      else if ( !wcscmp(name,L"frno") )
        {
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1;
          reg->flags = REGISTER::IS_PARTICLE;
          reg->ldcmd = CMD_LDpfr;
          reg->stcmd = CMD_STpfr;
        }
      else if ( !wcscmp(name,L"f") )
        {
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1;
          reg->flags = REGISTER::IS_PARTICLE;
          reg->ldcmd = CMD_LDpf;
          reg->stcmd = CMD_STpf;
        }
      else if ( !wcscmp(name,L"r") )
        {
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1;
          reg->flags = REGISTER::IS_VECTOR|REGISTER::IS_PARTICLE;
          reg->ldcmd = CMD_LDpr;
          reg->stcmd = CMD_STpr;
        }
      else if ( 2 == swscanf(name,L"a%d.%d%c",&r0,&r1,&c) )
        {
          if ( r0  < 0 || r0  > EMITTER_CONTEXT::REG_Ai_COUNT ) return false;
          if ( r1 != 0 && r1 != 1 ) return false;
          reg->regno = EMITTER_CONTEXT::REG_A0+r0*2+r1;
        }
      else if ( 1 == swscanf(name,L"a%d%c",&r0,&c) )
        {
          if ( r0  < 0 || r0  > EMITTER_CONTEXT::REG_Ai_COUNT ) return false;
          reg->regno = EMITTER_CONTEXT::REG_A0+r0*2;
          reg->flags = REGISTER::IS_VECTOR;
        }
      else if ( 2 == swscanf(name,L"em%d.%d%c",&r0,&r1,&c) )
        {
          if ( r0  < 0 || r0  > EMITTER_CONTEXT::REG_CF_COUNT ) return false;
          if ( r1 != 0 && r1 != 1 ) return false;
          reg->regno = EMITTER_CONTEXT::REG_CF+r0*2+r1;
        }
      else if ( 1 == swscanf(name,L"em%d%c",&r0,&c) )
        {
          if ( r0  < 0 || r0  > EMITTER_CONTEXT::REG_CF_COUNT ) return false;
          reg->regno = EMITTER_CONTEXT::REG_CF+r0*2;
          reg->flags = REGISTER::IS_VECTOR;
        }
      else if ( 1 == swscanf(name,L"v.%d%c",&r1,&c) )
        {
          if ( r1 != 0 && r1 != 1 ) return false;
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1+r1;
          reg->flags = REGISTER::IS_PARTICLE|REGISTER::PAR_REG_V;
          reg->ldcmd = CMD_LDpv;
          reg->stcmd = r1?CMD_STpv1:CMD_STpv0;
        }
      else if ( 1 == swscanf(name,L"x.%d%c",&r1,&c) )
        {
          if ( r1 != 0 && r1 != 1 ) return false;
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1+r1;
          reg->flags = REGISTER::IS_PARTICLE|REGISTER::PAR_REG_X;
          reg->ldcmd = CMD_LDpx;
          reg->stcmd = r1?CMD_STpx1:CMD_STpx0;
        }
      else if ( 1 == swscanf(name,L"r.%d%c",&r1,&c) )
        {
          if ( r1 != 0 && r1 != 1 ) return false;
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1+r1;
          reg->flags = REGISTER::IS_PARTICLE|REGISTER::PAR_REG_X;
          reg->ldcmd = CMD_LDpr;
          reg->stcmd = r1?CMD_STpr1:CMD_STpr0;
        }
      else if ( 1 == swscanf(name,L"%f%c",&fval,&c) )
        {
          reg->regno = EMITTER_CONTEXT::REG_SHADOW00;
          reg->flags = REGISTER::IS_VALUE;
          reg->ldcmd = CMD_LDfl0;
          reg->value = *(u32_t*)&fval;
        }
      else if ( 2 == swscanf(name,L"(%f;%f)%c",&fval,&fval1,&c) )
        {
          reg->regno = EMITTER_CONTEXT::REG_SHADOW1;
          reg->flags = REGISTER::IS_VALUE|REGISTER::IS_VALUE1|REGISTER::IS_VECTOR;
          reg->ldcmd = CMD_LDfv;
          reg->value = *(u32_t*)&fval;
          reg->value1 = *(u32_t*)&fval1;
        }
      else
        return false;

      return true;
    }

  u32_t inline MK_PSPCMD(u32_t cmd,u32_t r0,u32_t r1,u32_t r2)
    {
      return r0 | ( r1 << 8 ) | ( r2 << 16 ) | ( cmd << 24 );
    }

  bool PspCompile(pwide_t source, BufferT<u32_t> &program, BaseStringT<wchar_t> &errorS)
    {
      enum
        {
          XUPDAT,
          XUPDATO,
          XINDEP,
          ARGS2_CMD,
          XMOV,
          XNEG,
          ARGS3_CMD,
          XADD,
          XSUB,
          XDIV,
          XMUL,
          XMOD,
          XROT,
          XRND
        };

      widestring sourceS(source);
      source = sourceS.ToLower().Trim().Str();
      program.Resize(0);
      u32_t cmd;
      wchar_t r0[32] = {0};
      wchar_t r1[32] = {0};
      wchar_t r2[32] = {0};
      REGISTER reg0,reg1,reg2;
      wchar_t c;
      pwide_t s  = source;
      long lineno = 1;

      while ( *s )
        {
          while ( *s && (*s == L'\t' || *s == L' ' || *s == L'\r') ) ++s;

          if ( *s == '\n' )
            {
              ++s;
              ++lineno;
              continue;
            }

          if ( *s )
            {
              if ( *s == L';' ) { while ( *s && *s++ != L'\n' ) {} ++lineno; continue; }
              else if ( swscanf(s,L"updateonl%c",&c) == 1 && c == L'y' ) cmd = XUPDATO;
              else if ( swscanf(s,L"updat%c",&c) == 1 && c == L'e' ) cmd = XUPDAT;
              else if ( swscanf(s,L"inde%c",&c) == 1 && c == L'p' ) cmd = XINDEP;
              else if ( swscanf(s,L"add " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XADD;
              else if ( swscanf(s,L"rnd " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XRND;
              else if ( swscanf(s,L"sub " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XSUB;
              else if ( swscanf(s,L"mul " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XMUL;
              else if ( swscanf(s,L"mod " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XMOD;
              else if ( swscanf(s,L"div " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XDIV;
              else if ( swscanf(s,L"rot " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XROT;
              else if ( swscanf(s,L"mov " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r2) == 2 ) cmd = XMOV;
              else if ( swscanf(s,L"neg " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r2) == 2 ) cmd = XNEG;
              else
                return errorS.Set(_S*L"synatx error, line %d" %lineno),false;
            }

          if ( cmd > ARGS2_CMD )
            {
              if ( !RegResolve(r0,&reg0) )
                return errorS.Set(_S*L"invalid first source register/value, line %d" %lineno),false;
              if ( (cmd > ARGS3_CMD)  && !RegResolve(r1,&reg1) )
                return errorS.Set(_S*L"invalid target register, line %d" %lineno),false;
              if ( !RegResolve(r2,&reg2) )
                return errorS.Set(_S*L"invalid target register, line %d" %lineno),false;

              if ( reg2.flags&REGISTER::IS_VALUE )
                return errorS.Set(_S*L"invalid target register, line %d" %lineno),false;
              if ( reg0.flags&(REGISTER::IS_PARTICLE|REGISTER::IS_VALUE) )
                {
                  program.Push(reg0.ldcmd);
                  if ( reg0.flags&REGISTER::IS_VALUE)  program.Push(reg0.value);
                  if ( reg0.flags&REGISTER::IS_VALUE1) program.Push(reg0.value1);
                }
              if ( cmd > ARGS3_CMD )
                {
                  if ( reg1.flags&(REGISTER::IS_PARTICLE|REGISTER::IS_VALUE) )
                    {
                      if ( (reg0.flags&REGISTER::IS_VALUE) && reg0.ldcmd == CMD_LDfl0 && reg1.ldcmd == CMD_LDfl0 )
                        reg1.ldcmd = CMD_LDfl1, ++reg1.regno;
                      program.Push(reg1.ldcmd);
                      if ( reg1.flags&REGISTER::IS_VALUE) program.Push(reg1.value);
                    }
                }
              if ( (reg0.flags & REGISTER::IS_VECTOR) != (reg2.flags & REGISTER::IS_VECTOR) )
                return errorS.Set(_S*L"invalid target register type, line %d" %lineno),false;
            }

          bool is_val0v = (reg0.flags & REGISTER::IS_VECTOR) != 0;
          bool is_val1v = (reg1.flags & REGISTER::IS_VECTOR) != 0;

          /**/ if ( cmd == XNEG && is_val0v )
            program.Push( MK_PSPCMD( CMD_NEGv, *reg0, 0, *reg2 ) );
          else if ( cmd == XNEG && !is_val0v )
            program.Push( MK_PSPCMD( CMD_NEGs, *reg0, 0, *reg2 ) );
          else if ( cmd == XMOV && is_val0v )   program.Push( MK_PSPCMD( CMD_MOVv, *reg0, 0, *reg2 ) );
          else if ( cmd == XMOV && !is_val0v )  program.Push( MK_PSPCMD( CMD_MOVs, *reg0, 0, *reg2 ) );
          else if ( cmd == XSUB || cmd == XADD )
            {
              if ( !is_val0v && is_val1v ) return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
              if ( cmd == XSUB ) program.Push( MK_PSPCMD( (is_val1v?CMD_NEGv:CMD_NEGs), *reg0,0,*reg0 ) );
              if ( is_val0v ) program.Push( MK_PSPCMD( (is_val1v?CMD_ADDvv:CMD_ADDvs), *reg0,*reg1,*reg2 ) );
              else            program.Push( MK_PSPCMD( CMD_ADDss, *reg0,*reg1,*reg2 ) );
            }
          else if ( cmd == XMUL )
            {
              if ( is_val1v )
                return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
              program.Push( MK_PSPCMD( (is_val0v?CMD_MULvs:CMD_MULss), *reg0,*reg1,*reg2 ) );
            }
          else if ( cmd == XMOD )
            {
              if ( is_val1v )
                return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
              program.Push( MK_PSPCMD( (is_val0v?CMD_MODvs:CMD_MODss), *reg0,*reg1,*reg2 ) );
            }
          else if ( cmd == XDIV )
            {
              if ( is_val1v )
                return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
              program.Push( MK_PSPCMD( (is_val0v?CMD_DIVvs:CMD_DIVss), *reg0,*reg1,*reg2 ) );
            }
          else if ( cmd == XRND )
            {
              if ( is_val0v ) return errorS.Set(_S*L"invalid first register type, line %d" %lineno),false;
              if ( is_val1v ) return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
              program.Push( MK_PSPCMD( CMD_RND, *reg0,*reg1,*reg2 ) );
            }
          else if ( cmd == XROT)
            {
              if ( !is_val0v ) return errorS.Set(_S*L"invalid first register type, line %d" %lineno),false;
              if ( is_val1v )  return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
              program.Push( MK_PSPCMD( CMD_ROTvs, *reg0,*reg1,*reg2 ) );
            }
          else if ( cmd == XUPDAT ) program.Push(CMD_UPDAT);
          else if ( cmd == XUPDATO ) program.Push(CMD_UPDATONLY);
          else if ( cmd == XINDEP ) program.Push(CMD_INDEP);
          else
            return errorS.Set(_S*L"invalid command, line %d" %lineno),false;

          if ( cmd > ARGS2_CMD && (reg2.flags&REGISTER::IS_PARTICLE) )
            program.Push(reg2.stcmd);
          while ( *s && *s++ != L'\n' ) {}
          ++lineno;
        } // end while ( *s )

      //widestring disasm;
      //XDbg(Decompile(program,disasm));
      //XDbg(WideLogF(L"Particles Program Disassembler\n%s",disasm.Str()));
      return true;
    }

  pwide_t RegToSS(u32_t reg, BaseStringT<wchar_t> &ws)
    {
      if ( reg >= EMITTER_CONTEXT::REG_CF &&  reg < EMITTER_CONTEXT::REG_CF_END )
        return ws.Set(_S*L"em%d.%d" %(reg/2) %(reg%2));
      if ( reg >= EMITTER_CONTEXT::REG_SHADOW00 && reg < EMITTER_CONTEXT::REG_A )
        return ws.Set(_S*L"s%d.%d" %((reg-EMITTER_CONTEXT::REG_SHADOW00)/2) %((reg-EMITTER_CONTEXT::REG_SHADOW00)%2));
      if ( reg == EMITTER_CONTEXT::REG_A )
        return ws.Set(L"a");
      if ( reg >= EMITTER_CONTEXT::REG_A0 && reg < EMITTER_CONTEXT::REG_A_END )
        return ws.Set(_S*L"a%d.%d" %((reg-EMITTER_CONTEXT::REG_A0)/2) %((reg-EMITTER_CONTEXT::REG_A0)%2));
      if ( reg == EMITTER_CONTEXT::REG_DT )
        return ws.Set(L"dt");
      if ( reg == EMITTER_CONTEXT::REG_IDX )
        return ws.Set(L"idx");
      if ( reg == EMITTER_CONTEXT::REG_T )
        return ws.Set(L"t");
      return ws.Set(L"###");
    }

  pwide_t RegToVS(u32_t reg, BaseStringT<wchar_t> &ws)
    {
      if ( reg >= EMITTER_CONTEXT::REG_CF &&  reg < EMITTER_CONTEXT::REG_CF_END )
        return ws.Set(_S*L"em%d" %(reg/2));
      if ( reg >= EMITTER_CONTEXT::REG_SHADOW00 && reg < EMITTER_CONTEXT::REG_A )
        return ws.Set(_S*L"s%d" %((reg-EMITTER_CONTEXT::REG_SHADOW00)/2));
      if ( reg >= EMITTER_CONTEXT::REG_A0 && reg < EMITTER_CONTEXT::REG_A_END )
        return ws.Set(_S*L"a%d" %((reg-EMITTER_CONTEXT::REG_A0)/2));
      if ( reg == EMITTER_CONTEXT::REG_DT)  return ws.Set(L"dt");
      if ( reg == EMITTER_CONTEXT::REG_T)   return ws.Set(L"t");
      if ( reg == EMITTER_CONTEXT::REG_IDX) return ws.Set(L"idx");
      return ws.Set(L"###");
    }

  void Decompile(BufferT<u32_t> const &program, BaseStringT<wchar_t> &sbuf)
    {
      const u32_t* p = &program[0];
      while ( p != &program[0] + program.Count() )
        {
          sbuf << _S*L"%04d:" %(p-&program[0]);
          u32_t c = *p++;
          if ( c & 0x80000000UL )
            {
              switch ( c )
                {
                case CMD_LDfl0: sbuf<< _S*L"ldfl0\t%f\n" %*(float*)p++; break;
                case CMD_LDfl1: sbuf<< _S*L"ldfl1\t%f\n" %*(float*)p++; break;
                case CMD_LDfv:
                  {
                    float val0 = *(float*)p++;
                    float val1 = *(float*)p++;
                    sbuf<< _S*L"ldfv\t(%f;%f)\n" %val0 %val1; break;
                  }
                case CMD_LDpx:  sbuf.Append(L"ldpx\n"); break;
                case CMD_STpx:  sbuf.Append(L"stpx\n"); break;
                case CMD_STpx0: sbuf.Append(L"stpx0\n"); break;
                case CMD_STpx1: sbuf.Append(L"stpx1\n"); break;
                case CMD_LDpv:  sbuf.Append(L"ldpv\n"); break;
                case CMD_STpv:  sbuf.Append(L"stpv\n"); break;
                case CMD_STpv0: sbuf.Append(L"stpv0\n"); break;
                case CMD_STpv1: sbuf.Append(L"stpv1\n"); break;
                case CMD_LDpr:  sbuf.Append(L"ldpr\n"); break;
                case CMD_STpr:  sbuf.Append(L"stpr\n"); break;
                case CMD_STpr0: sbuf.Append(L"stpr0\n"); break;
                case CMD_STpr1: sbuf.Append(L"stpr1\n"); break;
                case CMD_LDpt:  sbuf.Append(L"ldpt\n"); break;
                case CMD_STpt:  sbuf.Append(L"stpt\n"); break;
                case CMD_LDpa:  sbuf.Append(L"ldpa\n"); break;
                case CMD_STpa:  sbuf.Append(L"stpa\n"); break;
                case CMD_LDpf:  sbuf.Append(L"ldpf\n"); break;
                case CMD_STpf:  sbuf.Append(L"stpf\n"); break;
                case CMD_LDpfr: sbuf.Append(L"ldpfr\n"); break;
                case CMD_STpfr: sbuf.Append(L"stpfr\n"); break;
                case CMD_LDps:  sbuf.Append(L"ldps\n"); break;
                case CMD_STps:  sbuf.Append(L"stps\n"); break;
                case CMD_INDEP: sbuf.Append(L"indep\n"); break;
                case CMD_UPDAT: sbuf.Append(L"update\n"); break;
                case CMD_UPDATONLY: sbuf.Append(L"updateonly\n"); break;
                default: sbuf<< _S*L"%08x\n" %c;
                }
            }
          else
            {
              u32_t vl0 = (c & 0x0ff); c >>= 8;
              u32_t vl1 = (c & 0x0ff); c >>= 8;
              u32_t res = (c & 0x0ff); c >>= 8;
              widestring w0,w1,w2;
              switch (c )
                {
                case CMD_NEGs:
                  sbuf<< _S*L"negs\t%s,%s\n" %RegToSS(vl0,w0) %RegToSS(res,w2); break;
                case CMD_NEGv:
                  sbuf<< _S*L"negv\t%s,%s\n" %RegToVS(vl0,w0) %RegToVS(res,w2); break;
                case CMD_RND:   sbuf<< _S*L"rnd\t%s,%s,%s\n" %RegToSS(vl0,w0) %RegToSS(vl1,w1) %RegToSS(res,w2);  break;
                case CMD_ADDvv: sbuf<< _S*L"addvv\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToVS(vl1,w1) %RegToVS(res,w2);  break;
                case CMD_ADDvs: sbuf<< _S*L"addvs\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToSS(vl1,w1) %RegToVS(res,w2);  break;
                case CMD_ADDss: sbuf<< _S*L"addss\t%s,%s,%s\n" %RegToSS(vl0,w0) %RegToSS(vl1,w1) %RegToSS(res,w2);  break;
                case CMD_MULvs: sbuf<< _S*L"mulvs\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToSS(vl1,w1) %RegToVS(res,w2);  break;
                case CMD_MULss: sbuf<< _S*L"mulss\t%s,%s,%s\n" %RegToSS(vl0,w0) %RegToSS(vl1,w1) %RegToSS(res,w2);  break;
                case CMD_MODvs: sbuf<< _S*L"modvs\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToSS(vl1,w1) %RegToVS(res,w2);  break;
                case CMD_MODss: sbuf<< _S*L"modss\t%s,%s,%s\n" %RegToSS(vl0,w0) %RegToSS(vl1,w1) %RegToSS(res,w2);  break;
                case CMD_DIVvs: sbuf<< _S*L"divvs\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToSS(vl1,w1) %RegToVS(res,w2);  break;
                case CMD_DIVss: sbuf<< _S*L"divss\t%s,%s,%s\n" %RegToSS(vl0,w0) %RegToSS(vl1,w1) %RegToSS(res,w2);  break;
                case CMD_ROTvs: sbuf<< _S*L"rotvs\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToSS(vl1,w1) %RegToVS(res,w2);  break;
                case CMD_MOVs:  sbuf<< _S*L"movs\t%s,%s\n" %RegToSS(vl0,w0) %RegToSS(res,w2);  break;
                case CMD_MOVv:  sbuf<< _S*L"movv\t%s,%s\n" %RegToVS(vl0,w0) %RegToVS(res,w2);  break;
                default: sbuf<< _S*L"%08x\n" %c;
                }
            }
        }
    }
}
