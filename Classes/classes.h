
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

#if !defined ___8b26359a_427b_42e9_b63d_afe88156eb1e___
#define ___8b26359a_427b_42e9_b63d_afe88156eb1e___

#include "Classes.SRC/_specific.h"

#if !defined CXX_NO_GUID_SUPPORT
#include "Classes.SRC/guid.h"
using teggo::GuidT;
#endif // CXX_NO_GUID_SUPPORT

#include "Classes.SRC/hinstance.h"
#include "Classes.SRC/ptr_mem.h"
#include "Classes.SRC/ptr_rcc.h"
#include "Classes.SRC/refcounted.h"
#include "Classes.SRC/string.h"
#include "Classes.SRC/splitter.h"
#include "Classes.SRC/format.h"
#include "Classes.SRC/wexcept.h"
#include "Classes.SRC/slist.h"
#include "Classes.SRC/keylist.h"
#include "Classes.SRC/array.h"
#include "Classes.SRC/stack.h"
#include "Classes.SRC/flatmap.h"
#include "Classes.SRC/flatset.h"
#include "Classes.SRC/collection.h"
#include "Classes.SRC/genericio.h"
#include "Classes.SRC/sysutil.h"
#include "Classes.SRC/tuple.h"

#include "Classes.SRC/com_generic.h"
#include "Classes.SRC/com_dynamic.h"
#include "Classes.SRC/com_register.h"

#include "Classes.SRC/weaklink.h"
#include "Classes.SRC/logger.h"
#include "Classes.SRC/streams.h"
#include "Classes.SRC/symboltable.h"
#include "Classes.SRC/xdom.h"
#include "Classes.SRC/saxparser.h"
#include "Classes.SRC/defparser.h"
#include "Classes.SRC/messages.h"

#include "Classes.SRC/rectangle.h"
#include "Classes.SRC/vector.h"
#include "Classes.SRC/polygon.h"

#include "Classes.SRC/newdes.h"
//#include "Classes.SRC/lzss.h"
#include "Classes.SRC/lz+.h"
#include "Classes.SRC/md5.h"
#include "Classes.SRC/mymath.h"

#include "Classes.SRC/threads.h"

using teggo::FIXEDSIZE;
using teggo::RESIZABLE;
using teggo::REUSE;
using teggo::SWAP_CONTENT;
using teggo::Uncopyable;
using teggo::NonPodCopyStrategy;
using teggo::PodCopyStrategy;
using teggo::Qsort;
using teggo::Isort;
using teggo::Shash;

using teggo::mem_ptr;
using teggo::mem_arr;
using teggo::MemPtr;
using teggo::MemArr;
using teggo::rcc_ptr;
using teggo::RccPtr;
using teggo::RccRefe;
using teggo::Refe;
using teggo::Unrefe;
using teggo::Refcounted;
using teggo::RefcountedT;
using teggo::Ireferred;
using teggo::InternList;
using teggo::WeakLink;
using teggo::WeakLinkHolder;

using teggo::LOWERTRIMMED;
using teggo::LOWERCOPY;
using teggo::TRIMMED;
using teggo::CONCATSTRINGS;
using teggo::BaseStringT;
using teggo::StringT;
using teggo::StringW;
using teggo::StringA;
using teggo::widestring;
using teggo::asciistring;
using teggo::SplitterT;
using teggo::widesplitter;
using teggo::asciisplitter;
using teggo::FormatT;
using teggo::StringParam;

#if CXX_USES_EXCEPTIONS
using teggo::WideException;
#endif

using teggo::FlatmapT;
using teggo::FlatmapIteratorT;
using teggo::FlatsetT;
using teggo::FlatsetIteratorT;
using teggo::SlistT;
using teggo::KeyListT;
using teggo::ArrayT;
using teggo::BufferT;
using teggo::StackT;
using teggo::CollectionT;
using teggo::CollectionIteratorT;
using teggo::Tuple2;
using teggo::tuple2;
using teggo::Tuple3;
using teggo::tuple3;
using teggo::Hinstance;
using teggo::SprintF;
using teggo::VsprintF;
using teggo::Itoa;
using teggo::Itow;
using teggo::Ftoa;
using teggo::Ftow;

using teggo::GetModuleVersion;
using teggo::SysFormatError;
using teggo::ProcessYield;
using teggo::GetMillis;
using teggo::ResetMillis;
using teggo::SysMmapFile;
using teggo::SysMmapFileByHandle;
using teggo::SysUnmmapFile;

using teggo::IDispatchPtr;
using teggo::IUnknownPtr;
using teggo::IStreamPtr;
using teggo::ISeqStreamPtr;
using teggo::DispResult;
using teggo::DispInvoker;

using teggo::OpenLog;
using teggo::CloseLog;
using teggo::LogIsOpen;
using teggo::LogLevelGeq;

using teggo::DataSource;
using teggo::DataStream;
using teggo::NullDataStream;
using teggo::LogDataStream;
using teggo::FileDataStream;
using teggo::MemDataStream;
using teggo::DataSourceFilter;
using teggo::DataStreamPtr;

using teggo::OpenFile; // compatibility
using teggo::OpenStream;

using teggo::SymbolTable;

using teggo::Xnode;
using teggo::XnodeObserver;
using teggo::Xparam;
using teggo::Xiterator;
using teggo::Xdocument;
using teggo::XopenXmlDocument;
using teggo::XopenDefDocument;
using teggo::Xdocument_New;

using teggo::XMLdocumentBuilder;
using teggo::XdocumentBuilder;
using teggo::SAXparse;

using teggo::rect_t;
using teggo::frect_t;
using teggo::point_t;
using teggo::fpoint_t;
using teggo::size2_t;
using teggo::fsize2_t;

using teggo::Crc32;
using teggo::Adler32;
using teggo::NEWDES_Cipher;
using teggo::MD5_Hash;
//using teggo::LZSS;

using teggo::LZ_ENCODE_MEMORY;
using teggo::LZ_DECODE_MEMORY;
using teggo::Lz_Encode;
using teggo::Lz_Decode;

//using teggo::LineT;
//using teggo::TriangleT;
//using teggo::PolygonT;

using teggo::Tlocker;
using teggo::TsafeHolder;

#endif // ___8b26359a_427b_42e9_b63d_afe88156eb1e___
