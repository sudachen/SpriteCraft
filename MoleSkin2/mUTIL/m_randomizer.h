
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
                      
#include "../mcore/m_autoconf.h"
#include "../mcore/m_time.h"
#include <time.h>

#if !defined __m_util_randomizer_h__
#define __m_util_randomizer_h__

NAMESPACE_MSKIN_BEGIN

class Randomizer{
  unsigned state_;
public:
  static inline u32_t GetStarter() {
    return ( u32_t(time(0)) + GetMillis() );
  }
  Randomizer(unsigned state=GetStarter()):state_(state){}
  void Reset(unsigned state=GetStarter()) {state_=state;}
  unsigned Random();
  unsigned Get();
  float RandomF();
  float GetF();
  ~Randomizer(){}
  unsigned operator()(unsigned mod) { return Random()%mod; }
};

inline unsigned Randomizer::Random() 
{
  state_ = 1664525U*state_+1013904223U;
  return state_;
}

inline unsigned Randomizer::Get()
{
  return state_;
}

inline float Randomizer::RandomF(){
  unsigned f = Random();
  f = 0x3f800000U|(0x007fffffU&f);
  float val = *(float*)&f - 1.f;
  STRICT_REQUIRE(!(val<0||val>=1));
  return val;
}

inline float Randomizer::GetF()
{
  unsigned f = 0x3f800000U|(0x007fffffU&Get());
  float val = *(float*)&f - 1.f;
  STRICT_REQUIRE(!(val<0||val>=1));
  return val;
}

unsigned MOLE_FUNCALL Random();

NAMESPACE_MSKIN_END

#endif // __m_util_randomizer_h__

