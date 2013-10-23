
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

struct SVALUE;
typedef SVALUE const * const * SVALUEPP;

struct SVALUE
  {
    static const SVALUE EMPTY;

    SVALUE()
      {
        type_ = TYPE_EMPTY;
        owner_ = 0;
      };

    ~SVALUE()
      {
        Clean_();
      }

    SVALUE(SVALUE const &val,bool copy = true)
      {
        if ( copy )
          Copy2_(val);
        else
          Alias_(val);
      }

    SVALUE& operator=(SVALUE const &val)
      {
        Clean_();
        Copy_(val);
        return *this;
      }

    SVALUE Copy() const
      {
        return SVALUE().Copy_(*this);
      }

    void Clean_();
    SVALUE& Copy_(SVALUE const &val);  // Copy2_ + copy pwide_t as owned object
    SVALUE& Copy2_(SVALUE const &val); // owned objects copy/control
    SVALUE& Swap_(SVALUE &val);
    SVALUE& Alias_(SVALUE const &val); // not owned copy

    void Dismiss_()
      {
        type_ = TYPE_EMPTY;
        owner_ = 0;
      }

    explicit  SVALUE(long val)
      {
        type_ = TYPE_NUMBER;
        owner_ = 0;
        number_ = double(val);
      }

    explicit  SVALUE(double val)
      {
        type_ = TYPE_NUMBER;
        owner_ = 0;
        number_ = val;
      }

    explicit  SVALUE(bool val)
      {
        type_ = TYPE_BOOLEAN;
        owner_ = 0;
        boolean_ = val;
      }

    explicit  SVALUE(pwide_t val)
      {
        type_ = TYPE_STRING;
        owner_ = 0;
        string_ = (BSTR)val;
      }

    explicit  SVALUE(IScriptObject* val)
      {
        type_ = TYPE_OBJECT;
        owner_ = 0;
        object_ = val;
      }

    SVALUE(pwide_t s,int)
      {
        SVALUE v(s);
        Copy_(v);
      }

    SVALUE(IScriptObject* o,int)
      {
        SVALUE v(o);
        Copy_(v);
      }

    bool Empty()
      {
        return type_ == TYPE_EMPTY;
      }

  #if defined SCRAFT_INTERNAL_MARKER
    explicit  SVALUE(IScriptObjectPtr const &val);
    explicit  SVALUE(IScriptObjectPtr const &o,int)
      {
        SVALUE v(o);
        Copy_(v);
      }
    double      Number() const;
    bool        Bool()   const;
    pwide_t     Str()    const;
    widestring  ToStr()  const;
    pwide_t     ConvertToStr();
    IScriptObjectPtr Obj() const;
  #endif

    enum TYPE_ENUM
      {
        TYPE_EMPTY       = 0,
        TYPE_NUMBER      = 1,
        TYPE_BOOLEAN     = 2,
        TYPE_STRING      = 3,
        TYPE_OBJECT      = 4,
        TYPE_PAIR        = 5  // spritecraft internal
      };

    struct
      {
        unsigned type_ : 4;
        unsigned owner_: 1;
        unsigned reserve_: 27;
      };

    union
      {
        double        number_;
        bool          boolean_;
        BSTR          string_;
        IScriptObject *object_;
        struct { void* _0; void* _1; } pair_;
      };
  };
