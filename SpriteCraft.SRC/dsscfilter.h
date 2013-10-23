
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

#ifndef ___0f198e4e_3b1a_4231_8c96_ca8fe7594429___
#define ___0f198e4e_3b1a_4231_8c96_ca8fe7594429___

struct DataSourceScraftFilter_ : DataSourceFilter
  {
    static DataSourceScraftFilter_ *instance_;
    struct Gate { DataSourceScraftFilter_ *operator->() const { return instance_; } };
    void SetDefaultSearchPath(StringParam path);
    void Mount(StringParam vol,StringParam path);
    static void Init();
    DataSourceScraftFilter_();
    ~DataSourceScraftFilter_();

    virtual rcc_ptr<DataStream> Open( pwide_t identifier, unsigned flgs );
    virtual int   Exists( pwide_t identifier );
    virtual u32_t LastMtimeOf( pwide_t identifier );

    typedef SlistT<StringW> PATH_LIST_;
    CollectionT< PATH_LIST_ > volumes_;

    bool Resolve_(pwide_t id, teggo::BaseStringT<wchar_t> *fullpath, pwide_t vol);
    bool Prepare_(pwide_t id, teggo::BaseStringT<wchar_t> *fullpath);

  };

static const DataSourceScraftFilter_::Gate DataSourceScraftFilter = DataSourceScraftFilter_::Gate();

#endif // ___0f198e4e_3b1a_4231_8c96_ca8fe7594429___
