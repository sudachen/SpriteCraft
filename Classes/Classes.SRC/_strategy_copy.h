
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

namespace teggo
{
  template < unsigned tUseExceptions >
    struct Teggo_Internal_NPCpS_
      {
        template < class tTx >
          static void CXX_STDCALL MoveRight_( tTx *begin, tTx *&end, unsigned number )
            {
              tTx *from = end -1;
              end = end + number;
              tTx *to   = end -1;

              CXX_TRY
                {
                  for ( ; from != begin-1 ; --from, --to )
                    {
                      teggo_new_and_move_data(to,from);
                      from->~tTx();
                    }
                }
              CXX_CATCH_ALL
                {
                  --to;
                  for ( ; to != end; )
                    {
                      to->~tTx();
                    }
                  from->~tTx();
                  end = from;
                  CXX_RETHROW;
                }
            }

        template < class tTx >
          static void CXX_STDCALL MoveRight( tTx *begin, tTx *&end, unsigned number, tTx const *from )
            {
              if ( begin != end )
                {
                  tTx *to = begin;
                  MoveRight_( begin, end, number );
                  CXX_TRY
                    {
                      for ( tTx const *where = to+number ; to != where; ++to )
                        new (to) tTx(*(from++)); // filling
                    }
                  CXX_CATCH_ALL
                    {
                      for ( tTx *where = begin + number; where != end; ++where )
                        where->~tTx();

                      end = to;

                      CXX_RETHROW;
                    }
                }
              else
                {
                  for ( tTx *where = end+number ; end != where; ++end )
                    new (end) tTx( *(from++) ); // filling
                }
            }

        template < class tTx >
          static void CXX_STDCALL MoveRight( tTx *begin, tTx *&end, unsigned number, tTx const& filler )
            {
              if ( begin != end )
                {
                  tTx *to = begin;
                  MoveRight_( begin, end, number );
                  CXX_TRY
                    {
                      for ( tTx *where = to+number ; to != where; ++to )
                        new (to) tTx( filler );
                    }
                  CXX_CATCH_ALL
                    {
                      for ( tTx *where = begin + number; where != end; ++where )
                        where->~tTx();

                      end = to;

                      CXX_RETHROW;
                    }
                }
              else
                {
                  while ( number-- )
                    {
                      new (end) tTx( filler );
                      ++end;
                    }
                }
            }

        template < class tTx >
          static void CXX_STDCALL MoveLeft( tTx *begin, tTx *&end, unsigned number )
            {
              tTx *nend = end - number;

              if ( begin != end )
                {
                  tTx *from   = begin + number;
                  CXX_TRY
                    {
                      for ( ; from != end ; ++from, ++begin )
                        {
                          begin->~tTx();
                          //new (begin) tTx (*from);
                          teggo_new_and_move_data(begin,from);
                        }
                    }
                  CXX_CATCH_ALL
                    {
                      for ( ; from != end; ++from )
                        from->~tTx();
                      end = begin;
                      CXX_RETHROW;
                    }
                }

              for ( ; end != nend ; --end )
                (end-1)->~tTx();
            }
      };

  typedef Teggo_Internal_NPCpS_<CXX_USES_EXCEPTIONS> NonPodCopyStrategy;

  struct PodCopyStrategy
    {
      template < class tTx >
        static void CXX_STDCALL MoveRight( tTx *begin, tTx *&end, unsigned number, tTx const *from )
          {
            if ( begin != end )
              memmove( begin + number, begin, (end-begin) * sizeof ( tTx ) );
            memcpy( begin, from, number * sizeof ( tTx ) );
            end += number;
          }

      template < class tTx >
        static void CXX_STDCALL MoveRight( tTx *begin, tTx *&end, unsigned number, tTx const& filler )
          {
            if ( begin != end )
              memmove( begin + number, begin, (end-begin) * sizeof ( tTx ) );
            end += number;
            while( number-- )
              memcpy( begin++, &filler, sizeof(tTx) );
          }

      template < class tTx >
        static void CXX_STDCALL MoveLeft( tTx *begin, tTx *&end, unsigned number )
          {
            if ( begin != end )
              memmove( begin, begin+number, ((end-begin)-number) * sizeof(tTx));
            end -= number;
          }
    };

} // namespace
