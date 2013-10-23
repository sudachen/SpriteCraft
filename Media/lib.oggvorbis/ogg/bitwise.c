/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2002             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

  function: packing variable sized words into an octet stream
  last mod: $Id$

 ********************************************************************/

/* We're 'LSb' endian; if we write a word but read individual bits,
   then we'll read the lsb first */

#if defined __SPLICE_CODE_SEG
#pragma code_seg(".text$oggVORBIS")
#endif

#include <string.h>
#include <stdlib.h>
#include "../include/ogg.h"

#pragma warning(disable:4244)
#pragma warning(disable:4146)

#define BUFFER_INCREMENT 256

static unsigned long mask[]=
{0x00000000,0x00000001,0x00000003,0x00000007,0x0000000f,
 0x0000001f,0x0000003f,0x0000007f,0x000000ff,0x000001ff,
 0x000003ff,0x000007ff,0x00000fff,0x00001fff,0x00003fff,
 0x00007fff,0x0000ffff,0x0001ffff,0x0003ffff,0x0007ffff,
 0x000fffff,0x001fffff,0x003fffff,0x007fffff,0x00ffffff,
 0x01ffffff,0x03ffffff,0x07ffffff,0x0fffffff,0x1fffffff,
 0x3fffffff,0x7fffffff,0xffffffff };

//
//void oggpack_writeinit(oggpack_buffer *b){
//  memset(b,0,sizeof(*b));
//  b->ptr=b->buffer=_ogg_malloc(BUFFER_INCREMENT);
//  b->buffer[0]='\0';
//  b->storage=BUFFER_INCREMENT;
//}
//
//void oggpack_writetrunc(oggpack_buffer *b,long bits){
//  long bytes=bits>>3;
//  bits-=bytes*8;
//  b->ptr=b->buffer+bytes;
//  b->endbit=bits;
//  b->endbyte=bytes;
//  *b->ptr|=mask[bits];
//}
//
//void oggpack_writealign(oggpack_buffer *b){
//  int bits=8-b->endbit;
//  if(bits<8)
//    oggpack_write(b,0,bits);
//}
//
//void oggpack_writecopy(oggpack_buffer *b,void *source,long bits){
//  unsigned char *ptr=(unsigned char *)source;
//
//  long bytes=bits/8;
//  bits-=bytes*8;
//
//  if(b->endbit){
//    int i;
//    /* unaligned copy.  Do it the hard way. */
//    for(i=0;i<bytes;i++)
//      oggpack_write(b,(long)(ptr[i]),8);    
//  }else{
//    /* aligned block copy */
//    if(b->endbyte+bytes+1>=b->storage){
//      b->storage=b->endbyte+bytes+BUFFER_INCREMENT;
//      b->buffer=_ogg_realloc(b->buffer,b->storage);
//      b->ptr=b->buffer+b->endbyte;
//    }
//
//    memmove(b->ptr,source,bytes);
//    b->ptr+=bytes;
//    b->buffer+=bytes;
//    *b->ptr=0;
//
//  }
//  if(bits)
//    oggpack_write(b,(long)(ptr[bytes]),bits);    
//}

void oggpack_reset(oggpack_buffer *b){
  b->ptr=b->buffer;
  b->buffer[0]=0;
  b->endbit=b->endbyte=0;
}

//void oggpack_writeclear(oggpack_buffer *b){
//  _ogg_free(b->buffer);
//  memset(b,0,sizeof(*b));
//}

void oggpack_readinit(oggpack_buffer *b,unsigned char *buf,int bytes){
  memset(b,0,sizeof(*b));
  b->buffer=b->ptr=buf;
  b->storage=bytes;
}

///* Takes only up to 32 bits. */
//void oggpack_write(oggpack_buffer *b,unsigned long value,int bits){
//  if(b->endbyte+4>=b->storage){
//    b->buffer=_ogg_realloc(b->buffer,b->storage+BUFFER_INCREMENT);
//    b->storage+=BUFFER_INCREMENT;
//    b->ptr=b->buffer+b->endbyte;
//  }
//
//  value&=mask[bits]; 
//  bits+=b->endbit;
//
//  b->ptr[0]|=value<<b->endbit;  
//  
//  if(bits>=8){
//    b->ptr[1]=value>>(8-b->endbit);  
//    if(bits>=16){
//      b->ptr[2]=value>>(16-b->endbit);  
//      if(bits>=24){
//	b->ptr[3]=value>>(24-b->endbit);  
//	if(bits>=32){
//	  if(b->endbit)
//	    b->ptr[4]=value>>(32-b->endbit);
//	  else
//	    b->ptr[4]=0;
//	}
//      }
//    }
//  }
//
//  b->endbyte+=bits/8;
//  b->ptr+=bits/8;
//  b->endbit=bits&7;
//}

/* Read in bits without advancing the bitptr; bits <= 32 */
long oggpack_look(oggpack_buffer *b,int bits){
  unsigned long ret;
  unsigned long m=mask[bits];

  bits+=b->endbit;

  if(b->endbyte+4>=b->storage){
    /* not the main path */
    if(b->endbyte*8+bits>b->storage*8)return(-1);
  }
  
  ret=b->ptr[0]>>b->endbit;
  if(bits>8){
    ret|=b->ptr[1]<<(8-b->endbit);  
    if(bits>16){
      ret|=b->ptr[2]<<(16-b->endbit);  
      if(bits>24){
	ret|=b->ptr[3]<<(24-b->endbit);  
	if(bits>32 && b->endbit)
	  ret|=b->ptr[4]<<(32-b->endbit);
      }
    }
  }
  return(m&ret);
}

long oggpack_look1(oggpack_buffer *b){
  if(b->endbyte>=b->storage)return(-1);
  return((b->ptr[0]>>b->endbit)&1);
}

void oggpack_adv(oggpack_buffer *b,int bits){
  bits+=b->endbit;
  b->ptr+=bits/8;
  b->endbyte+=bits/8;
  b->endbit=bits&7;
}

void oggpack_adv1(oggpack_buffer *b){
  if(++(b->endbit)>7){
    b->endbit=0;
    b->ptr++;
    b->endbyte++;
  }
}

/* bits <= 32 */
long oggpack_read(oggpack_buffer *b,int bits){
  unsigned long ret;
  unsigned long m=mask[bits];

  bits+=b->endbit;

  if(b->endbyte+4>=b->storage){
    /* not the main path */
    ret=-1UL;
    if(b->endbyte*8+bits>b->storage*8)goto overflow;
  }
  
  ret=b->ptr[0]>>b->endbit;
  if(bits>8){
    ret|=b->ptr[1]<<(8-b->endbit);  
    if(bits>16){
      ret|=b->ptr[2]<<(16-b->endbit);  
      if(bits>24){
	ret|=b->ptr[3]<<(24-b->endbit);  
	if(bits>32 && b->endbit){
	  ret|=b->ptr[4]<<(32-b->endbit);
	}
      }
    }
  }
  ret&=m;
  
 overflow:

  b->ptr+=bits/8;
  b->endbyte+=bits/8;
  b->endbit=bits&7;
  return(ret);
}

long oggpack_read1(oggpack_buffer *b){
  unsigned long ret;
  
  if(b->endbyte>=b->storage){
    /* not the main path */
    ret=-1UL;
    goto overflow;
  }

  ret=(b->ptr[0]>>b->endbit)&1;
  
 overflow:

  b->endbit++;
  if(b->endbit>7){
    b->endbit=0;
    b->ptr++;
    b->endbyte++;
  }
  return(ret);
}

long oggpack_bytes(oggpack_buffer *b){
  return(b->endbyte+(b->endbit+7)/8);
}

long oggpack_bits(oggpack_buffer *b){
  return(b->endbyte*8+b->endbit);
}

unsigned char *oggpack_get_buffer(oggpack_buffer *b){
  return(b->buffer);
}


#undef BUFFER_INCREMENT
