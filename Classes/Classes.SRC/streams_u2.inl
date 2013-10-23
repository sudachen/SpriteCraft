
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

#ifndef ___87975bda_a67d_4053_9d8f_a1501f601d38___
#define ___87975bda_a67d_4053_9d8f_a1501f601d38___

#include "streams.h"

#if defined _TEGGO_STREAMS_HERE
# define _TEGGO_STREAMS_FAKE_INLINE _TEGGO_EXPORTABLE
#else
# define _TEGGO_STREAMS_FAKE_INLINE CXX_FAKE_INLINE
#endif

namespace teggo {

#if !defined _TEGGO_STREAMS_HERE && !defined _TEGGOINLINE
  _TEGGO_EXPORTABLE int CXX_STDCALL UtfGetCodeLength(int c);
  _TEGGO_EXPORTABLE char * CXX_STDCALL UtfEncode( char* bf, wchar_t c, int *cnt );
#else
  _TEGGO_STREAMS_FAKE_INLINE
    int CXX_STDCALL UtfGetCodeLength(int c)
      {
        static char length[256] =
          {
            /* Map UTF-8 encoded prefix byte to sequence length.  zero means
               illegal prefix.  see RFC 2279 for details */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 0, 0
          };
        return length[c];
      }

  _TEGGO_STREAMS_FAKE_INLINE
    char * CXX_STDCALL UtfEncode( char* bf, wchar_t c, int *cnt )
      {
        int l = 0;
        if ( c < 0x80 ) { *bf++ = (char)c; l = 1; }
        else if ( c < 0x0800 )
          {
            *bf++ = (char)(0xc0 | (c >> 6));
            *bf++ = (char)(0x80 | (c & 0x3f));
            l = 2;
          }
        else
          {
            *bf++ = (char)(0xe0 | (c >> 12));
            *bf++ = (char)(0x80 | ((c >> 6) & 0x3f));
            *bf++ = (char)(0x80 | (c & 0x3f));
            l = 3;
          }
        if ( cnt ) *cnt += l;
        return bf;
      }
#endif
}

namespace teggo {

  struct TextBuffer
    {
      DataStream &ds_;
      byte_t bS_[128];
      byte_t *bp_;
      int count_;
      int clen_;
      int const part_;

//       int Get_()
//         {
//           return --count_,*bp_++;
//         }

      _TEGGO_EXPORTABLE TextBuffer(DataStream &ds);
      _TEGGO_EXPORTABLE ~TextBuffer();
      _TEGGO_EXPORTABLE int Pump();
      _TEGGO_EXPORTABLE int GetC();
    };

#if defined _TEGGO_STREAMS_HERE || defined _TEGGOINLINE
  _TEGGO_STREAMS_FAKE_INLINE
    int TextBuffer::Pump()
      {
        int nread;
        int n = part_?sizeof(bS_)-count_:1;
        if ( count_ ) memmove(bS_,bp_,count_);
        bp_ = bS_;
        nread = ds_.Read(bp_+count_,n);
        count_ += nread;
        //Lout << _S*"pump %d bytes" % nread;
        return count_;
      }

  _TEGGO_STREAMS_FAKE_INLINE
    int TextBuffer::GetC()
      {
        int clen;
        //int cc;

        wchar_t c = 0;

      repeat:

        clen = 0;

        do {
          if ( !count_ )
            if ( !Pump() )
              return -1;
          ((byte_t*)&c)[clen] = *bp_++; --count_;
        } while ( ++clen <= clen_ );

        if ( c == '\r' )
          goto repeat;

        return (int)c;
      }

  _TEGGO_STREAMS_FAKE_INLINE
    TextBuffer::TextBuffer(DataStream &ds)
      : ds_(ds), bp_(bS_), count_(0), clen_(0), part_(ds.Features()&DataStream::CAN_SEEK)
      {
        if ( ds_.Features() & DataStream::IS_UNICODE_MEMSTREAM )
          clen_ = sizeof(wchar_t) - 1;
      }

  _TEGGO_STREAMS_FAKE_INLINE
    TextBuffer::~TextBuffer()
      {
        if ( count_ && part_ )
          ds_.Seek(-count_,SEEK_CUR);
      }
#endif

#if !defined _TEGGO_STREAMS_HERE && !defined _TEGGOINLINE
  _TEGGO_EXPORTABLE int CXX_STDCALL TextBuffer_LocaleGetW(TextBuffer &tbf);
  _TEGGO_EXPORTABLE int CXX_STDCALL TextBuffer_UtfGetW(TextBuffer &tbf);
#else
  _TEGGO_STREAMS_FAKE_INLINE
    int CXX_STDCALL TextBuffer_LocaleGetW(TextBuffer &tbf)
      {
        int c = tbf.GetC();
        if ( tbf.clen_ )
          return c;
        if ( c > 0 )
          return (u16_t)ucs2_btowc(c);
        return c;
      }

  _TEGGO_STREAMS_FAKE_INLINE
    int CXX_STDCALL TextBuffer_UtfGetW(TextBuffer &tbf)
      {
        int c = -1;
        int c0 = tbf.GetC();
        if ( c0 > 0 )
          {
            if (c0 < 0x80) return c0;
            int c1 = 0;
            int c2 = 0;
            int c3 = 0;
            int l = UtfGetCodeLength(c0);
            switch ( l )
              {
                case 2:
                  if ( (c1 = tbf.GetC()) > 0 )
                    c = ((c0 & 0x1f) << 6) + (c1 & 0x3f);
                  break;
                case 3:
                  if ( (c1 = tbf.GetC()) > 0
                    && (c2 = tbf.GetC()) > 0 )
                    c = ((c0 & 0x0f) << 12) + ((c1 & 0x3f) << 6) + (c2 & 0x3f);
                  break;
                case 4: // hm, UCS4 ????
                  if ( (c1 = tbf.GetC()) > 0
                    && (c2 = tbf.GetC()) > 0
                    && (c3 = tbf.GetC()) > 0 )
                    c = ((c0 & 0x7) << 18) + ((c0 & 0x3f) << 12) +
                        ((c2 & 0x3f) << 6) + (c3 & 0x3f);
                  break;
                default:
                  break;
              }
          }
        return c;
      }
#endif

  struct UtfTextBuffer
    {
      TextBuffer tbf;
      UtfTextBuffer(DataStream &ds) : tbf(ds) {}
      int GetC() { return TextBuffer_UtfGetW(tbf); }
    };
}



#endif // ___87975bda_a67d_4053_9d8f_a1501f601d38___
