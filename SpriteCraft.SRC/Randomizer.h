
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


#if !defined __230178DA_539B_46AB_9F87_E66C97F2133B__
#define __230178DA_539B_46AB_9F87_E66C97F2133B__

struct RandomizerHelper : 
  TeggoObject<
    TeggoPyDispImpl<IRandomizer,0, TeggoISupportErrorInfo> >
  {
    teggo::randizer_t randomizer_;
    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid)
      {
        return IsEqualGUID(TEGGO_GUIDOF(IRandomizer),riid)?S_OK:S_FALSE;
      }

	  HRESULT __stdcall Next(/*[out,retval]*/ float* pVal);
	  HRESULT __stdcall Reset(/*[in]*/ long cookie);
  };


#endif // __230178DA_539B_46AB_9F87_E66C97F2133B__
