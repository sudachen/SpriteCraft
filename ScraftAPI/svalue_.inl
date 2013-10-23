
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

const SVALUE SVALUE::EMPTY;

void SVALUE::Clean_()
  {
    if ( owner_ )
      if ( type_ == TYPE_OBJECT && object_ )
        object_->Release(), object_ = 0;
      else if ( type_ == TYPE_STRING && string_ )
        SysFreeString( string_ ), string_ = 0;
    type_ = TYPE_EMPTY; owner_ = 0;
  }

SVALUE& SVALUE::Copy2_(SVALUE const &val)
  {
    memcpy(this,&val,sizeof(SVALUE));
    if ( owner_ )
      if ( type_ == TYPE_OBJECT && object_ )
        object_->AddRef();
      else if ( type_ == TYPE_STRING && string_ )
        string_ = SysAllocString(string_);
    return *this;
  }

SVALUE& SVALUE::Copy_(SVALUE const &val)
  {
    Copy2_(val);
    if ( !owner_ )
      if( type_ == TYPE_STRING && string_ )
        string_ = SysAllocString(string_), owner_ = 1;
      else if ( type_ == TYPE_OBJECT && object_ )
        object_->AddRef(), owner_ = 1;
    return *this;
  }

SVALUE& SVALUE::Swap_(SVALUE &val)
  {
    byte_t buf[sizeof(SVALUE)];
    memcpy(buf,&val,sizeof(SVALUE));
    memcpy(&val,this,sizeof(SVALUE));
    memcpy(this,buf,sizeof(SVALUE));
    return *this;
  }

SVALUE& SVALUE::Alias_(SVALUE const &val)
  {
    memcpy(this,&val,sizeof(SVALUE));
    owner_ = 0;
    return *this;
  }
