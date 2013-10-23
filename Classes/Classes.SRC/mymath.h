
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

#if !defined __ED377106_626B_434d_8DDD_F6089E43BA37__
#define __ED377106_626B_434d_8DDD_F6089E43BA37__

#include "_specific.h"

namespace teggo
{

  static const float  F_PI  = float(3.14159265358979323846264338327950288419716939937510582);
  static const double LF_PI = double(3.14159265358979323846264338327950288419716939937510582);
  static const float  F_EPSILON = float(1e-6);
  static const double LF_EPSILON = double(1e-6);

  template<class real> inline
    bool CXX_STDCALL in_epsilon(real val) { return fabs(val) < real(1e-6); }
  template<class real> inline
    bool CXX_STDCALL is_fequal(real a,real b) { return fabs(a-b) < real(1e-6); }

  template <class real>
    int CXX_STDCALL SolveQuadratic(real A,real B,real C,real (&solution)[2])
      {
        real D = B*B - 4*A*C;

        if ( D <  0 )
          return 0;

        if ( D == 0 )
          return solution[0] = -B/(2*A), 1;
        else
          {
            real sqrtD = sqrt(D);
            solution[0] = (-B + sqrtD)/(2*A);
            solution[1] = (-B - sqrtD)/(2*A);
            return 2;
          }
      }

} // namespace

#endif // __ED377106_626B_434d_8DDD_F6089E43BA37__
