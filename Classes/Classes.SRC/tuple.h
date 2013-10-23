
#ifndef ___7ddf9ffd_bc93_44ea_99c1_ac70e15bdf58___
#define ___7ddf9ffd_bc93_44ea_99c1_ac70e15bdf58___

#include "_specific.h"

namespace teggo {
  template < class tA, class tB >
    struct Tuple2
      {
        tA _0;
        tB _1;
        Tuple2( tA a, tB b ) : _0(a), _1(b) {}
      };

  template < class tA, class tB >
    Tuple2<tA,tB> tuple2(tA a,tB b) { return Tuple2<tA,tB>(a,b); }

  template < class tA, class tB, class tC >
    struct Tuple3
      {
        tA _0;
        tB _1;
        tC _2;
        Tuple3( tA a, tB b, tC c ) : _0(a), _1(b), _2(c) {}
      };

  template < class tA, class tB, class tC >
    Tuple3<tA,tB,tC> tuple3(tA a,tB b,tC c) { return Tuple3<tA,tB,tC>(a,b,c); }

} // namespace teggo

#endif  // ___7ddf9ffd_bc93_44ea_99c1_ac70e15bdf58___

