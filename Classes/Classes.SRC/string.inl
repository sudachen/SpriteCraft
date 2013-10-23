
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

#if !defined ___661c37ea_99f1_46a6_a6e2_30dad0ee66f6___
#define ___661c37ea_99f1_46a6_a6e2_30dad0ee66f6___

#include "string.h"

#if defined _TEGGO_STRING_HERE
# define _TEGGO_STRING_FAKE_INLINE _TEGGO_EXPORTABLE
#else
# define _TEGGO_STRING_FAKE_INLINE CXX_FAKE_INLINE
#endif

namespace teggo {

  _TEGGO_STRING_FAKE_INLINE
  void CXX_STDCALL widestring_free(void *p)
    {
#if defined TEGGO_USING_EXPORTABLE_DELETER
      if (p) (*((widestring_deleter_t*)p-1))((byte_t*)((widestring_deleter_t*)p-1));
#else
      if ( p ) delete[] (byte_t*) p;
#endif
    }

  _TEGGO_STRING_FAKE_INLINE
  void* CXX_STDCALL widestring_malloc(unsigned sz)
    {
#if defined TEGGO_USING_EXPORTABLE_DELETER
      byte_t *p = new byte_t[sz+sizeof(widestring_deleter_t)];
      *(widestring_deleter_t*)p = &widestring_deleter;
      return (widestring_deleter_t*)p+1;
#else
      return new byte_t[sz];
#endif
    }


  _TEGGO_STRING_FAKE_INLINE
  void CXX_STDCALL InplaceWideToChar(void* wp,int charcount,bool zterminate)
    {
      wchar_t* from = (wchar_t*)wp;
      char* to = (char*)wp;

      if ( charcount < 0 )
        charcount = int(wcslen(from));

      for ( int i = 0; i < charcount; ++i )
        to[i] = ucs2_wctob(from[i]);

      if ( zterminate )
        to[charcount] = 0;
    }

  _TEGGO_STRING_FAKE_INLINE
  void CXX_STDCALL InplaceCharToWide(void* wp,int charcount,bool zterminate)
    {
      wchar_t* to = (wchar_t*)wp;
      char* from = (char*)wp;

      if ( charcount < 0 )
        charcount = int(strlen(from));

      for ( int i = charcount-1; i >= 0 ; --i )
        to[i] = ucs2_btowc(from[i]);

      if ( zterminate )
        to[charcount] = 0;
    }

  _TEGGO_STRING_FAKE_INLINE
  wchar_t* CXX_STDCALL widestrdup(pwide_t val)
    {
      if ( val )
        if ( size_t len = wcslen(val) )
          {
            wchar_t* mem = new wchar_t[len+1];
            memcpy(mem,val,sizeof(wchar_t)*(len+1));
            return mem;
          }
      return 0;
    }

  _TEGGO_STRING_FAKE_INLINE
  wchar_t* CXX_STDCALL widestrdup(pchar_t val)
    {
      if ( val )
        if ( size_t len = strlen(val) )
          {
            wchar_t* mem = new wchar_t[len+1];
            StrCopyN(mem,val,len);
            return mem;
          }
      return 0;
    }

  template<class tTchr>
  typename BaseStringT<tTchr>::Tstr BaseStringT<tTchr>::Substr(
                                                               int first,
                                                               int len,
                                                               BaseStringT<tTchr> &result) const
    {
      int self_len = Length();
      if ( first < 0 || first >= self_len || !len )
        return result.Set((wchar_t*)0);
      else
        {
          if ( first + len > self_len )
            len = self_len - first;
          return result.SetRange(Str()+first,len);
        }
    }

  template <class tTchr>
  unsigned BaseStringT<tTchr>::ResizeBuffer(unsigned charcount)
    {
      if ( (size_bits_length_[1]&BaseStringT<tTchr>::FIXED_BIT_MASK) == 0 )
        {
          unsigned oldsz = BufferSize();
          if ( oldsz >= charcount )
            return oldsz;

          unsigned buffSize = MEM_BLOCK_ROUND(charcount);
          tTchr* temp = (tTchr*)widestring_malloc(buffSize*sizeof(tTchr));
          memcpy(temp,s_,oldsz*sizeof(tTchr));

          if ( IsOwner_() )
            widestring_free(s_);

          size_bits_length_[1] |= BaseStringT<tTchr>::OWNER_BIT_MASK;
          s_ = temp;
          SetSize_(buffSize);
        }
      return BufferSize();
    }

  template<class tTchr> CXX_NO_INLINE
  void BaseStringT<tTchr>::InitReuse(unsigned charcount,resize_policy_t rpt,void* buf)
    {
      size_bits_length_[0] = 0;
      size_bits_length_[1] = 0;
      s_ = (tTchr*)buf;
      *s_ = 0;

      SetSize_(charcount);

      if ( rpt == FIXEDSIZE )
        size_bits_length_[1] |= BaseStringT<tTchr>::FIXED_BIT_MASK;
    }

  template<class tTchr>
  BaseStringT<tTchr>& BaseStringT<tTchr>::ToLower()
    {
      for ( int i = 0, iE = Length(); i < iE; ++i )
        s_[i] = ChrLower(s_[i]);
      return *this;
    }

  template<class tTchr>
  BaseStringT<tTchr>& BaseStringT<tTchr>::ToUpper()
    {
      for ( int i = 0, iE = Length(); i < iE; ++i )
        s_[i] = ChrUpper(s_[i]);
      return *this;
    }

  template<class tTchr>  CXX_NO_INLINE
  BaseStringT<tTchr>& BaseStringT<tTchr>::Replace(const tTchr* from, tTchr to)
    {
      unsigned from_len = StrLen(from);
      for ( unsigned i = 0, iE = Length(); i < iE; ++i )
        for ( unsigned j = 0; j < from_len; ++j )
          if ( s_[i] == from[j] )
            s_[i] = to;
      return *this;
    }

  template<class tTchr>  CXX_NO_INLINE
  BaseStringT<tTchr> &BaseStringT<tTchr>::RemoveExtraWS()
    {
      int iE = Length(), iS = 0, i = 0;
      while ( i < iE  )
        {
          REQUIRE ( i >= iS );
          s_[iS] = s_[i++];
          if ( IsWS_(s_[iS]) )
            {
              s_[iS] = ' ';
              while ( i < iE && IsWS_(s_[i]) ) ++i;
            }
          ++iS;
        }
      Resize(iS);
      return *this;
    }

  template<class tTchr>  CXX_NO_INLINE
  int BaseStringT<tTchr>::StripCR2( tTchr *text, int len)
    {
      if ( len < 0 )
        len = StrLen(text);
      len = remove(text,text+len,tTchr(char_cr_))-text;
      text[len] = 0;
      return len;
    }

  template<class tTchr> CXX_NO_INLINE
  BaseStringT<tTchr>& BaseStringT<tTchr>::RestoreCRLF()
    {
      unsigned len = Length();
      unsigned ln_count = 0;

      for ( Tstr t = s_, tE = s_+len; t != tE; ++t )
        if ( *t == char_ln_ && t != s_ && *(t-1) != char_cr_ )
          ++ln_count;

      if ( ln_count )
        {
          Resize( len + ln_count );
          tTchr* j = s_+(len+ln_count-1);
          for ( Tstr i = s_+len-1, iE = s_-1; i != iE; --i, --j )
            {
              *j = *i;
              if ( *i == char_ln_ && (i == s_ || *(i-1) != char_cr_)  )
                --j, *j = tTchr(char_cr_);
            }
        }

      return *this;
    }

  template<class tTchr>
  BaseStringT<tTchr>& BaseStringT<tTchr>::Chomp( int chomp )
    {
      int l = (int)Length();
      if ( chomp > 0 )
        {
          if ( l > chomp )
            Resize(l-chomp);
          else
            Resize(0);
        }
      else if ( chomp < 0 )
        {
          chomp = -chomp;
          if ( l > chomp )
            {
              memmove(s_,s_+chomp,l-chomp);
              Resize(l-chomp);
            }
          else
            Resize(0);
        }
      return *this;
    }

  template <class tTchr>  CXX_NO_INLINE
  BaseStringT<tTchr>& BaseStringT<tTchr>::Insert(long idx, Tstr data, int count)
    {
      if ( idx < 0 )
        AppendRange(data,count);
      else
        {
          if ( (unsigned)idx > Length() )
            {
              Resize(idx);
              AppendRange(data,count);
            }
          else
            {
              unsigned len  = count < 0 ? StrLen(data) : count;
              unsigned olen = Length();
              Resize(olen+len);
              memmove((byte_t*)s_+(idx+len)*sizeof(tTchr),s_+idx*sizeof(tTchr),(olen-idx)*sizeof(tTchr));
              memcpy((byte_t*)s_+idx*sizeof(tTchr), data, len*sizeof(tTchr));
            }
        }
      return *this;
    }

  template <class tTchr>  CXX_NO_INLINE
  BaseStringT<tTchr>& BaseStringT<tTchr>::InsertChr(long idx, Tchar data)
    {
      if ( idx < 0 )
        Append(data);
      else
        {
          if ( (unsigned)idx > Length() )
            {
              Resize(idx);
              Append(data);
            }
          else
            {
              unsigned olen = Length();
              Resize(olen+1);
              memmove((byte_t*)s_+(idx+1)*sizeof(tTchr),s_+idx*sizeof(tTchr),(olen-idx)*sizeof(tTchr));
              s_[idx] = data;
            }
        }
      return *this;
    }

  template<class tTchr>  CXX_NO_INLINE
  BaseStringT<tTchr>& BaseStringT<tTchr>::PosixPath()
    {
      tTchr* i = s_;
      tTchr* iE = s_+Length();

      for ( ; i != iE; ++i )
        if ( *i == tTchr(char_back_slash_) )
          *i = tTchr(char_slash_);

      return *this;
    }

  template<class tTchr>  CXX_NO_INLINE
  BaseStringT<tTchr>& BaseStringT<tTchr>::ReplaceAfter(tTchr after, Tstr data)
    {
      int p = FindRChar(after);
      return  ( p < 0 ) ? Append(data) : ReplaceRight(p+1,data);
    }

  template<class tTchr> CXX_NO_INLINE
  BaseStringT<tTchr>& BaseStringT<tTchr>::ReplaceRight(unsigned after, Tstr data)
    {
      if ( after < Length() )
        Resize(after);
      return Append(data);
    }



  template < class tTchr, class _Ux > inline
  tTchr const* CXX_STDCALL BaseStringT_SetRange__(BaseStringT<tTchr> &self, _Ux const *txt,int l)
    {
      unsigned length = l<0?(txt?StrLen(txt):0):l;
      if ( length )
        {
          self.Resize(length);
          if ( txt )
            StrCopyN(self.s_,txt,length);
        }
      else
        {
          self.s_[0] = 0;
          self.SetLength_(0);
        }
      return self.s_;
    }

  _TEGGO_STRING_FAKE_INLINE
  wchar_t const* BaseStringT<wchar_t>::SetRange(char const *val,int l)
    {return BaseStringT_SetRange__(*this,val,l);}
  _TEGGO_STRING_FAKE_INLINE
  wchar_t const* BaseStringT<wchar_t>::SetRange(wchar_t const *val,int l)
    {return BaseStringT_SetRange__(*this,val,l);}
  _TEGGO_STRING_FAKE_INLINE
  char const* BaseStringT<char>::SetRange(char const *val,int l)
    {return BaseStringT_SetRange__(*this,val,l);}
  _TEGGO_STRING_FAKE_INLINE
  char const* BaseStringT<char>::SetRange(wchar_t const *val,int l)
    {return BaseStringT_SetRange__(*this,val,l);}

  template < class tTchr, class _Ux >  inline
  BaseStringT<tTchr>& CXX_STDCALL BaseStringT_AppendRange__(BaseStringT<tTchr> &self, _Ux const *val,int l)
    {
      if ( l == -1 )
        l = StrLen(val);
      if ( l > 0 )
        {
          unsigned offs = self.Length();
          if ( self.Resize(offs+l) >= offs+l )
            StrCopyN(self.s_+offs,val,l);
        }
      return self;
    }

  _TEGGO_STRING_FAKE_INLINE
  BaseStringT<wchar_t>& BaseStringT<wchar_t>::AppendRange(char const *val,int l)
    {return BaseStringT_AppendRange__(*this,val,l);}
  _TEGGO_STRING_FAKE_INLINE
  BaseStringT<wchar_t>& BaseStringT<wchar_t>::AppendRange(wchar_t const *val,int l)
    {return BaseStringT_AppendRange__(*this,val,l);}
  _TEGGO_STRING_FAKE_INLINE
  BaseStringT<char>& BaseStringT<char>::AppendRange(char const *val,int l)
    {return BaseStringT_AppendRange__(*this,val,l);}
  _TEGGO_STRING_FAKE_INLINE
  BaseStringT<char>& BaseStringT<char>::AppendRange(wchar_t const *val,int l)
    {return BaseStringT_AppendRange__(*this,val,l);}

  template<class tTchr> CXX_NO_INLINE
  unsigned BaseStringT<tTchr>::Resize(unsigned newlen)
    {
      unsigned ln = Length();
      if ( newlen >= ln )
        {
          if ( newlen >= BufferSize() )
            REQUIRE( ResizeBuffer(newlen+1) >= newlen+1 );
          memset(s_+ln,0,sizeof(tTchr)*(newlen-ln));
        }
      s_[newlen] = 0;
      SetLength_(newlen);
      return Length();
    }

  template<class tTchr> CXX_NO_INLINE
  void BaseStringT<tTchr>::Term()
    {
      if ( IsOwner_() )
        {
          widestring_free(s_);
        }
      size_bits_length_[0] = 0;
      size_bits_length_[1] = 0;
      s_ = (tTchr*)&size_bits_length_[1];
    }

  template<class tTchr> CXX_NO_INLINE
  int BaseStringT<tTchr>::FindChar(tTchr c, int from) const
    {
      unsigned length = Length();
      if ( !length )
        return -1;
      tTchr const *p = s_+from, *pE = s_+length;
      for(;p<pE;++p)
        if ( *p == c )
          return int(p-s_);
      return -1;
    }

  template<class tTchr> CXX_NO_INLINE
  int BaseStringT<tTchr>::FindRChar(tTchr c,int from) const
    {
      unsigned length = Length();
      if ( !length )
        return -1;
      if ( from == -1 || unsigned(from) >= length )
        from = length-1;
      tTchr const *p = s_+from;
      do
        if ( *p == c )
          return int(p-s_);
      while( --p >= s_);
      return -1;
    }

  template<class tTchr> CXX_NO_INLINE
  void BaseStringT<tTchr>::Init(unsigned R,resize_policy_t rpt)
    {
      size_bits_length_[0] = 0;
      size_bits_length_[1] = 0;
      if ( !R )
        s_ = (tTchr*)&size_bits_length_[1];
      else
        {
          s_ = (tTchr*)&size_bits_length_[2]; // out of array, size_bits_length_[2] - length
          *s_ = 0;

          if ( rpt == FIXEDSIZE )
            size_bits_length_[1] |= BaseStringT<tTchr>::FIXED_BIT_MASK;

          SetSize_(R+1);
        }
    }

  template<class tTchr> CXX_NO_INLINE
  bool BaseStringT<tTchr>::Expand2()
    {
      u32_t sz = BufferSize();
      if ( sz == 0 ) sz = 16;
      return ResizeBuffer(sz*2) > sz;
    }

  template<class tTchr> inline
  BaseStringT<tTchr>& BaseStringT<tTchr>::Expand(unsigned extent)
    {
      u32_t sz = BufferSize()+extent;
      REQUIRE ( ResizeBuffer(sz) >= sz );
      return *this;
    }

  template<class tTchr> CXX_NO_INLINE
  BaseStringT<tTchr>& BaseStringT<tTchr>::LeftTrim()
    {
      int i = 0;
      int iE = Length();

      for ( ; i < iE; ++i )
        if ( !IsWS_(s_[i]) )
          break;

      if ( i != iE && i != 0 )
        memmove(&s_[0],&s_[i],(iE-i)*sizeof(tTchr));

      Resize(iE-i);

      return *this;
    }

  template<class tTchr> CXX_NO_INLINE
  BaseStringT<tTchr>& BaseStringT<tTchr>::Remove(long idx, long count)
    {
      int i = idx;
      int iS = idx+count;
      int iE = Length();
      if ( iS > iE ) iS = iE;
      if ( i > iS ) i = iS;
      memmove(&s_[i],&s_[iS],(iE-iS)*sizeof(tTchr));
      Resize(i+(iE-iS));
      return *this;
    }

  template<class tTchr> CXX_NO_INLINE
  BaseStringT<tTchr>& BaseStringT<tTchr>::RightTrim()
    {
      tTchr* i = s_+Length()-1;
      tTchr* iE = s_-1;

      for ( ; i!=iE; --i )
        if ( !IsWS_(*i)  )
          break;

      Resize(int(i-iE));
      return *this;
    }

  _TEGGO_STRING_FAKE_INLINE
  bool CXX_STDCALL ToBool__(BaseStringT<wchar_t> const &p)
    {
      if ( !p )
        return false;
      if ( p == L"on" || p == L"yes" || p == L"true" )
        return true;
      if ( p.Length() == 1 && p[0] == L'1' )
        return true;
      return false;
    }

  template < class tTchr, class _Ux >
  void CXX_STDCALL BaseStringT_InitValue__(BaseStringT<tTchr> &self,unsigned R,resize_policy_t rpt,_Ux const *value,int len)
    {
      self.Init(R,rpt);
      if ( value )
        self.SetRange(value,len);
      else
        if ( len > 0 )
          self.Resize(len);
    }

  _TEGGO_STRING_FAKE_INLINE
  void BaseStringT<char>::InitValue(unsigned R,resize_policy_t rpt,char const *value,int len)
    {BaseStringT_InitValue__(*this,R,rpt,value,len);}
  _TEGGO_STRING_FAKE_INLINE
  void BaseStringT<char>::InitValue(unsigned R,resize_policy_t rpt,wchar_t const *value,int len)
    {BaseStringT_InitValue__(*this,R,rpt,value,len);}
  _TEGGO_STRING_FAKE_INLINE
  void BaseStringT<wchar_t>::InitValue(unsigned R,resize_policy_t rpt,char const *value,int len)
    {BaseStringT_InitValue__(*this,R,rpt,value,len);}
  _TEGGO_STRING_FAKE_INLINE
  void BaseStringT<wchar_t>::InitValue(unsigned R,resize_policy_t rpt,wchar_t const *value,int len)
    {BaseStringT_InitValue__(*this,R,rpt,value,len);}

  template < class tTchr >
  BaseStringT<tTchr> &BaseStringT<tTchr>::FixCstr()
    {
      if ( unsigned l = Length() )
        {
          tTchr *p = s_+l;
          while ( p != s_ && *(--p) == 0 )
            --l;
          SetLength_(l);
        }
      return *this;
    }

  _TEGGO_STRING_FAKE_INLINE
  void CXX_STDCALL StringParam_::_DeletBuffer(StringT<wchar_t> *buffer)
    { delete buffer; }

  _TEGGO_STRING_FAKE_INLINE
  StringParam_::StringParam_(pchar_t f) : buffer_(new StringT<wchar_t>(f))
    { value_ = +*buffer_; };

  _TEGGO_STRING_FAKE_INLINE
  StringParam_::StringParam_(BaseStringT<char> const &f) : buffer_(new StringT<wchar_t>(f))
    { value_ = +*buffer_; }

#if defined _TEGGO_STRING_HERE
  void __export_string_functions()
    {
      typedef void (BaseStringT<char>::*t0)();
      typedef void (BaseStringT<wchar_t>::*t1)();
      volatile t0 *_t0 = 0;
      volatile t1 *_t1 = 0;

      *_t0 = (t0)&BaseStringT<char>::Substr;
      *_t1 = (t1)&BaseStringT<wchar_t>::Substr;
      *_t0 = (t0)&BaseStringT<char>::ResizeBuffer;
      *_t1 = (t1)&BaseStringT<wchar_t>::ResizeBuffer;
      *_t0 = (t0)&BaseStringT<char>::Resize;
      *_t1 = (t1)&BaseStringT<wchar_t>::Resize;
      *_t0 = (t0)&BaseStringT<char>::Init;
      *_t1 = (t1)&BaseStringT<wchar_t>::Init;
      *_t0 = (t0)&BaseStringT<char>::InitReuse;
      *_t1 = (t1)&BaseStringT<wchar_t>::InitReuse;
      *_t0 = (t0)&BaseStringT<char>::ToLower;
      *_t1 = (t1)&BaseStringT<wchar_t>::ToLower;
      *_t0 = (t0)&BaseStringT<char>::ToUpper;
      *_t1 = (t1)&BaseStringT<wchar_t>::ToUpper;
      *_t0 = (t0)&BaseStringT<char>::Replace;
      *_t1 = (t1)&BaseStringT<wchar_t>::Replace;
      *_t0 = (t0)&BaseStringT<char>::RemoveExtraWS;
      *_t1 = (t1)&BaseStringT<wchar_t>::RemoveExtraWS;
      *_t0 = (t0)&BaseStringT<char>::StripCR;
      *_t1 = (t1)&BaseStringT<wchar_t>::StripCR;
      *_t0 = (t0)&BaseStringT<char>::RestoreCRLF;
      *_t1 = (t1)&BaseStringT<wchar_t>::RestoreCRLF;
      *_t0 = (t0)&BaseStringT<char>::Chomp;
      *_t1 = (t1)&BaseStringT<wchar_t>::Chomp;
      *_t0 = (t0)&BaseStringT<char>::Insert;
      *_t1 = (t1)&BaseStringT<wchar_t>::Insert;
      *_t0 = (t0)&BaseStringT<char>::InsertChr;
      *_t1 = (t1)&BaseStringT<wchar_t>::InsertChr;
      *_t0 = (t0)&BaseStringT<char>::PosixPath;
      *_t1 = (t1)&BaseStringT<wchar_t>::PosixPath;
      *_t0 = (t0)&BaseStringT<char>::ReplaceAfter;
      *_t1 = (t1)&BaseStringT<wchar_t>::ReplaceAfter;
      *_t0 = (t0)&BaseStringT<char>::ReplaceRight;
      *_t1 = (t1)&BaseStringT<wchar_t>::ReplaceRight;
      *_t0 = (t0)&BaseStringT<char>::Term;
      *_t1 = (t1)&BaseStringT<wchar_t>::Term;
      *_t0 = (t0)&BaseStringT<char>::FindChar;
      *_t1 = (t1)&BaseStringT<wchar_t>::FindChar;
      *_t0 = (t0)&BaseStringT<char>::FindRChar;
      *_t1 = (t1)&BaseStringT<wchar_t>::FindRChar;
      *_t0 = (t0)&BaseStringT<char>::Expand;
      *_t1 = (t1)&BaseStringT<wchar_t>::Expand;
      *_t0 = (t0)&BaseStringT<char>::Expand2;
      *_t1 = (t1)&BaseStringT<wchar_t>::Expand2;
      *_t0 = (t0)&BaseStringT<char>::LeftTrim;
      *_t1 = (t1)&BaseStringT<wchar_t>::LeftTrim;
      *_t0 = (t0)&BaseStringT<char>::RightTrim;
      *_t1 = (t1)&BaseStringT<wchar_t>::RightTrim;
      *_t0 = (t0)&BaseStringT<char>::FixCstr;
      *_t1 = (t1)&BaseStringT<wchar_t>::FixCstr;
      *_t0 = (t0)&BaseStringT<char>::Remove;
      *_t1 = (t1)&BaseStringT<wchar_t>::Remove;

    }
#endif

} // namespace


#endif // ___661c37ea_99f1_46a6_a6e2_30dad0ee66f6___
