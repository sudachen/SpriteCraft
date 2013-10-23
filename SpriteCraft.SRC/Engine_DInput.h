
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

#if !defined __54CEB12B_F6D9_4BA9_BA9C_4AE758BC3FFD__
#define __54CEB12B_F6D9_4BA9_BA9C_4AE758BC3FFD__

struct Engine_DInput_Impl;

struct Engine_DInput
  {
    Engine_DInput_Impl* impl_;
    void Terminate();

    Engine_DInput();
    ~Engine_DInput();
    void RefreshInput();
    bool IsPressed(tagKeyVal);
    bool IsPressedNow(tagKeyVal);
    bool IsReleasedNow(tagKeyVal);
    void ResetContext(bool do_init=false);
  };

#endif // __54CEB12B_F6D9_4BA9_BA9C_4AE758BC3FFD__
