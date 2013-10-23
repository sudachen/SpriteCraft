
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

#if !defined __8FD636CA_1811_44A4_AEC2_96F47A575EE1__
#define __8FD636CA_1811_44A4_AEC2_96F47A575EE1__

struct Console :
  TeggoObject<
    TeggoPyDispImpl<IConsole, 0, TeggoISupportErrorInfo> >
  {
    void Display();

    Console();
    ~Console();

    void Reinit();

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);
    HRESULT __stdcall put_visible(/*[in]*/ VARIANT_BOOL);
    HRESULT __stdcall put_transparent(/*[in]*/ VARIANT_BOOL);
    HRESULT __stdcall get_visible(/*[in]*/ VARIANT_BOOL*);
    HRESULT __stdcall get_transparent(/*[in]*/ VARIANT_BOOL*);
    HRESULT __stdcall Println(/*[in]*/ BSTR text);
    HRESULT __stdcall Print(/*[in]*/ BSTR text);
    HRESULT __stdcall put_cmdline(/*[in]*/ BSTR text);
    HRESULT __stdcall get_cmdline(/*[out,retval]*/ BSTR* text);
    HRESULT __stdcall Clear();
    HRESULT __stdcall ProcessInput(/*[out,retval]*/ VARIANT_BOOL* cmdEnter);
    HRESULT __stdcall put_lines(/*[in]*/ long lines);
    HRESULT __stdcall get_lines(/*[in]*/ long* lines);
    HRESULT __stdcall put_maxlines(/*[in]*/ long lines);
    HRESULT __stdcall get_maxlines(/*[in]*/ long* lines);

    unsigned lines_;
    unsigned maxlines_;
    bool visible_;
    bool transparent_;

    //widestring cmdline_;
    SlistT<widestring> content_;

    rcc_ptr< ISprite > con_content_;
    rcc_ptr< ISprite > con_cmdline_;
    rcc_ptr< ISprite > con_background_;

    void UpdateContent();
    void Print_(pwide_t text);
  };

#endif // __8FD636CA_1811_44A4_AEC2_96F47A575EE1__
