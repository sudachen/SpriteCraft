
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

#if !defined __SpriteCraft_CollideMgr_h__
#define __SpriteCraft_CollideMgr_h__

struct CollideMgr_CollideObject;

struct CollideMgr
  {
    enum {MAX_GROUPS=16,ACTIVE=0,PASSIVE=1};

    typedef SlistT<CollideMgr_CollideObject*> objects_t;
    objects_t objects_[MAX_GROUPS][2];
    u32_t collide_mask_[MAX_GROUPS];
    u32_t unique_;
    rcc_ptr<IOnCollide> listener_;
    
    struct 
      {
        bool offscreen_ : 1;
      };
    
    bool EvaluteRxy(CollideMgr_CollideObject& o); 
    bool EvaluteHit(CollideMgr_CollideObject& o, CollideMgr_CollideObject& p, bool active);
    bool EvaluteTrackCollide(CollideMgr_CollideObject& o, CollideMgr_CollideObject& p, bool active);
    bool NotifyIt(CollideMgr_CollideObject& o, CollideMgr_CollideObject& p, bool active);

    void CollideCheck();
    void AddObject(ICollidableObject*,long group,bool active,bool trackable);
    void AllowCollide(unsigned group1,unsigned group2);
    void DisallowCollide(unsigned group1,unsigned group2);
    void SetListener(IUnknown *l);
    bool GetOffscreenCollideStatus() { return offscreen_; }
    void SetOffscreenCollideStatus(bool s) { offscreen_ = s; }

    CollideMgr();
    ~CollideMgr();
  };

#endif

