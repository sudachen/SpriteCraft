
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

#if !defined SCRAFT_INTERNAL_MARKER
#error "should be included in scraftapi.h only"
#endif

struct CXX_NO_VTABLE ScraftIConsole
  {

    IConsole*   QueryIfs() { return (IConsole*)this; }

    u32_t       AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t       Release()  { return ((IUnknown*)this)->Release(); }

    void        Println(pwide_t text);
    void        Print(pwide_t text);
    void        Clear();
    bool        ProcessInput();
    void        SetLines(long val);
    long        GetLines();
    void        SetMaxLines(long val);
    long        GetMaxLines();
    void        SetVisible(bool val);
    bool        IsVisible();
    void        SetTransparent(bool val);
    bool        IsTransparent();
    void        SetCmdLine(pwide_t cmdline);
    __ScString  GetCmdLine();

    struct Property_lines { TEGGO_PROPERTY(ScraftIConsole,long,SetLines,GetLines,lines) } lines;
    struct Property_maxlines { TEGGO_PROPERTY(ScraftIConsole,long,SetMaxLines,GetMaxLines,maxlines) } maxlines;
    struct Property_visible { TEGGO_PROPERTY(ScraftIConsole,bool,SetVisible,IsVisible,visible) } visible;
    struct Property_transp { TEGGO_PROPERTY(ScraftIConsole,bool,SetTransparent,IsTransparent,transparent) } transparent;
    struct Property_cmdline { TEGGO_S_PROPERTY(ScraftIConsole,SetCmdLine,GetCmdLine,cmdline) } cmdline;
  };

inline IConsole* scraft_query_ifs(ScraftIConsole* u)
  {
    return u->QueryIfs();
  }
