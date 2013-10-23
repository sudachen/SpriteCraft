/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2002             *
 * by the XIPHOPHORUS Company http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

  function: lookup based functions
  last mod: $Id$

 ********************************************************************/

#include <math.h>
#include "lookup.h"
#include "lookup_data.h"
#include "os.h"
#include "misc.h"


static float FROMdB_LOOKUP[FROMdB_LOOKUP_SZ]={
	             1.f,   0.6309573445f,   0.3981071706f,   0.2511886432f,
	   0.1584893192f,            0.1f,  0.06309573445f,  0.03981071706f,
	  0.02511886432f,  0.01584893192f,           0.01f, 0.006309573445f,
	 0.003981071706f, 0.002511886432f, 0.001584893192f,          0.001f,
	0.0006309573445f,0.0003981071706f,0.0002511886432f,0.0001584893192f,
	         0.0001f,6.309573445e-05f,3.981071706e-05f,2.511886432e-05f,
	1.584893192e-05f,          1e-05f,6.309573445e-06f,3.981071706e-06f,
	2.511886432e-06f,1.584893192e-06f,          1e-06f,6.309573445e-07f,
	3.981071706e-07f,2.511886432e-07f,1.584893192e-07f,
};

static float FROMdB2_LOOKUP[FROMdB2_LOOKUP_SZ]={
	   0.9928302478f,   0.9786445908f,   0.9646616199f,   0.9508784391f,
	   0.9372921937f,     0.92390007f,   0.9106992942f,   0.8976871324f,
	   0.8848608897f,   0.8722179097f,   0.8597555737f,   0.8474713009f,
	    0.835362547f,   0.8234268041f,   0.8116616003f,   0.8000644989f,
	   0.7886330981f,   0.7773650302f,   0.7662579617f,    0.755309592f,
	   0.7445176537f,   0.7338799116f,   0.7233941627f,   0.7130582353f,
	   0.7028699885f,   0.6928273125f,   0.6829281272f,   0.6731703824f,
	   0.6635520573f,   0.6540711597f,   0.6447257262f,   0.6355138211f,
};


#ifdef FLOAT_LOOKUP

/* interpolated lookup based cos function, domain 0 to PI only */
float vorbis_coslook(float a){
  double d=a*(.31830989*(float)COS_LOOKUP_SZ);
  int i=vorbis_ftoi(d-.5);

  return COS_LOOKUP[i]+ (d-i)*(COS_LOOKUP[i+1]-COS_LOOKUP[i]);
}

/* interpolated 1./sqrt(p) where .5 <= p < 1. */
float vorbis_invsqlook(float a){
  double d=a*(2.f*(float)INVSQ_LOOKUP_SZ)-(float)INVSQ_LOOKUP_SZ;
  int i=vorbis_ftoi(d-.5f);
  return INVSQ_LOOKUP[i]+ (d-i)*(INVSQ_LOOKUP[i+1]-INVSQ_LOOKUP[i]);
}

/* interpolated 1./sqrt(p) where .5 <= p < 1. */
float vorbis_invsq2explook(int a){
  return INVSQ2EXP_LOOKUP[a-INVSQ2EXP_LOOKUP_MIN];
}

#include <stdio.h>
/* interpolated lookup based fromdB function, domain -140dB to 0dB only */
float vorbis_fromdBlook(float a){
  int i=vorbis_ftoi(a*((float)(-(1<<FROMdB2_SHIFT)))-.5f);
  return (i<0)?1.f:
    ((i>=(FROMdB_LOOKUP_SZ<<FROMdB_SHIFT))?0.f:
     FROMdB_LOOKUP[i>>FROMdB_SHIFT]*FROMdB2_LOOKUP[i&FROMdB2_MASK]);
}

#endif

#ifdef INT_LOOKUP
/* interpolated 1./sqrt(p) where .5 <= a < 1. (.100000... to .111111...) in
   16.16 format 

   returns in m.8 format */
long vorbis_invsqlook_i(long a,long e){
  long i=(a&0x7fff)>>(INVSQ_LOOKUP_I_SHIFT-1); 
  long d=(a&INVSQ_LOOKUP_I_MASK)<<(16-INVSQ_LOOKUP_I_SHIFT); /*  0.16 */
  long val=INVSQ_LOOKUP_I[i]-                                /*  1.16 */
    (((INVSQ_LOOKUP_I[i]-INVSQ_LOOKUP_I[i+1])*               /*  0.16 */
      d)>>16);                                               /* result 1.16 */
  
  e+=32;
  if(e&1)val=(val*5792)>>13; /* multiply val by 1/sqrt(2) */
  e=(e>>1)-8;

  return(val>>e);
}

/* interpolated lookup based fromdB function, domain -140dB to 0dB only */
/* a is in n.12 format */
float vorbis_fromdBlook_i(long a){
  int i=(-a)>>(12-FROMdB2_SHIFT);
  return (i<0)?1.f:
    ((i>=(FROMdB_LOOKUP_SZ<<FROMdB_SHIFT))?0.f:
     FROMdB_LOOKUP[i>>FROMdB_SHIFT]*FROMdB2_LOOKUP[i&FROMdB2_MASK]);
}

/* interpolated lookup based cos function, domain 0 to PI only */
/* a is in 0.16 format, where 0==0, 2^^16-1==PI, return 0.14 */
long vorbis_coslook_i(long a){
  int i=a>>COS_LOOKUP_I_SHIFT;
  int d=a&COS_LOOKUP_I_MASK;
  return COS_LOOKUP_I[i]- ((d*(COS_LOOKUP_I[i]-COS_LOOKUP_I[i+1]))>>
			   COS_LOOKUP_I_SHIFT);
}

#endif
