
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

#if !defined __SpriteCraft_AnimationMgr_h__
#define __SpriteCraft_AnimationMgr_h__
  
struct AnimationMgr 
  {
  
  private:

    struct as_t
      {
        rcc_ptr<IWeakSprite> isprite_;
        rcc_ptr<IUnknown> listener_;
        u32_t start_frame_millis;
        long fps;
        long startFrno;
        long stopFrno;
        bool looping;
        float delta;
        long step;
        bool __interrupted;
        long cookie;
        struct Pred 
          {
            IWeakSprite* s_;
            Pred(IWeakSprite* s) : s_(s) {}
            bool operator() (const as_t& ast) const 
              { 
                DBG_REQUIRE( +ast.isprite_ != 0 );
                DBG_REQUIRE( s_ != 0 );
                return +ast.isprite_ == s_; 
              }
          };
      };

    typedef SlistT<as_t> aslist_t;

  private:
    
    aslist_t aslist_;

  public:

    void AddAnimatedSprite(IWeakSprite* ias, long bFrno, long eFrno, float fps, IUnknown* listener, long cookie, bool looping); 
    void StopSpriteAnimation(aslist_t::Position iter,bool interrupted); 
    void StopSpriteAnimation(IWeakSprite* ias,bool interrupted);
    void Update(); 
  };

#endif // __SpriteCraft_AnimationMgr_h__
