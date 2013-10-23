
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

#define _TEGGOINLINE

#include "../../classes.h"
//#include "../../Classes.SRC/lzsh.h"
#include "../../Classes.SRC/lz+.inl"

//using teggo::LZSH;
using teggo::Lz_Encode;
using teggo::Lz_Decode;

static byte_t test[8*1024*1024] = {0};
static byte_t obuf[8*1024*1024] = {0};
static byte_t rbuf[8*1024*1024] = {0};

void CXX_CDECL main(int argv, char **argc)
  {
    FILE *f = fopen(argc[1],"rb");
    if (!f) exit(0);

    memset(obuf,0xee,sizeof(obuf));
    //fseek(f,8*1024,SEEK_SET);
     u32_t ml;
    ml = teggo::GetMillis();
    int wr = fread(test,1,8*1024*1024,f);
    printf("(%d), was read %d\n",teggo::GetMillis()-ml,wr);    
    //wr = cxx_min(wr,64*1024);
    //TestDict(test,wr);
    int r;
    //int r2,succeeded;
    // u32_t ml;
    ml = teggo::GetMillis();
    memcpy(rbuf,test,sizeof(obuf));
    //r = LZPLUS::RawEncode(test+65560,wr-65560,obuf,sizeof(obuf),0,mem);
    //r = LZPLUS::RawEncode(test+131097,wr-131097,obuf,sizeof(obuf),0,mem);
    r = Lz_Encode(test,wr,obuf,cxx_min<int>(sizeof(obuf),wr));
    printf("(%d), {LZ+,ENCODE} r = %d, was %d, %d%%\n",teggo::GetMillis()-ml,r,wr,(r*100)/wr);    
    if ( r > 0 )
      {
        FILE *lzf = fopen(+(StringA(argc[1]) + ".lz+"),"w+b");
        fwrite(obuf,1,r,lzf);
        fclose(lzf);
      }
    ml = teggo::GetMillis();
    memset(rbuf,0x00,sizeof(rbuf));
    int r2 = Lz_Decode(obuf,r,rbuf,sizeof(rbuf));
    ml  = teggo::GetMillis() -ml;
    int succeeded = (r2 == wr && 0 == memcmp(test,rbuf,wr));
    printf("(%d), {LZ+,DECODE} r = %d, was %d, %s\n",ml,r2,r,(succeeded?"verified":"failed"));    
//     ml = teggo::GetMillis();
//     LZSS lzss;
//     r = lzss.Encode(test,wr,obuf,sizeof(obuf));
//     ml  = teggo::GetMillis() -ml ;
//     printf("(%d), {LZSS,ENCODE} r = %d, was %d, %d%%\n",ml,r,wr,(r*100)/wr);    
//     ml = teggo::GetMillis();
//     r2 = lzss.Decode(obuf,r,rbuf,sizeof(rbuf));
//     ml  = teggo::GetMillis() -ml ;
//     succeeded = (r2 == wr && 0 == memcmp(test,rbuf,wr));
//     printf("(%d), {LZSH,DECODE} r = %d, was %d, %s\n",ml,r2,r,(succeeded?"verified":"failed"));    
    //getchar();
  }