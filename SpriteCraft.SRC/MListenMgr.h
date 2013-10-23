
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

#if !defined __SpriteCraft_MListenMgr_h__
#define __SpriteCraft_MListenMgr_h__
  
struct MListenMgr 
  {
   
    struct s_t 
      {
        rcc_ptr<IWeakSpriteEx> isprite;
        struct Pred 
          {
            IWeakSpriteEx* s_;
            Pred(IWeakSpriteEx* s) : s_(s) {}
            bool operator() ( const s_t& s) const 
              {
                return +s.isprite ==  s_;
              }
          };
      };
  
    typedef SlistT<s_t> SprLst;

    SprLst slist_;
    rcc_ptr<IWeakSpriteEx> locked_;
    rcc_ptr<IWeakSpriteEx> selected_;
    long nearLayer_;
    long farLayer_;

    MListenMgr();
    ~MListenMgr();
    void Add(IWeakSpriteEx* spr);
    void Remove(IWeakSpriteEx* spr);
    void GrabMouse(IWeakSpriteEx* spr);
    void SetMouseNotifierRange(long nearLayer,long farLayer); 
    void Update(); 
    void Notify_MouseOver(IWeakSpriteEx* iex,bool inArea);
    void Notify_MouseMove(float x,float y);
    void Notify_MouseDown(float x,float y,long button);
    void Notify_MouseUp(float x,float y,long button);
    void Notify_MouseClick(float x,float y,long button);
  };

#endif