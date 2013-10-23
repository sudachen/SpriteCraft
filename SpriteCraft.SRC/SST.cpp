
#include "stdafx.h"
#include "SST.h"
#include "Logout.h"
#include "../ScraftAPI/spritecraft.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE
#pragma code_seg(".text$rcsman$sst")
#endif

bool IsSstKlassVaild(Xnode *node)
  {
    if ( !node ) return false;
    return true;
  }

teggo::Xdocument_ *LoadSST_into_XDoc(DataStream &ds)
  {
    teggo::Xdocument_ *doc = 0;
    static char const format[] = "failed to load sst-file, %s";

    XdocumentBuilder db;
    BufferT<char> line(258,0);
    BufferT<char> klass(256);
    line.End()[-2] = '\n';
    line.Resize(line.Count()-2);
    if ( !ds.ReadLine_(+line,line.Count()) || strncmp(+line,"#!SST",5) )
      return _E.Fail(doc,_S*format %"isn't sst stream");

    db.BeginDocument();
    db.Source(ds.Identifier());

    while( !ds.Eof() )
      {
  loop_next_record:
        klass[0] = 0;
        bool in_block = false;
        for(;;)
          {
            if ( ds.Eof() ) goto none_records_more;
            line.Truncate();
            if ( TEGGOt_E e = ds.ReadLineEx(line) )
              return _E.Fail(doc,_S*format %TeggoErrorText(e));
            line.Append('\n');
            line.Append('\0');

            if ( !in_block )
              {
                if ( sscanf(&line[0]," klass %s",&klass[0]) == 1
                  || sscanf(&line[0]," sprite %s",&klass[0]) == 1 )
                  {
                    in_block = true;
                    db.BeginTag1(L"sprite");
                    db.Body1(+StringT<wchar_t,32>(&klass[0]).Trim());
                  }
                else if ( sscanf(&line[0]," sound %s",&klass[0]) == 1 )
                  {
                    in_block = true;
                    db.BeginTag1(L"sound");
                    db.Body1(+StringT<wchar_t,32>(&klass[0]).Trim());
                  }
                else if ( sscanf(&line[0]," font %s",&klass[0]) == 1 )
                  {
                    in_block = true;
                    db.BeginTag1(L"font");
                    db.Body1(+StringT<wchar_t,32>(&klass[0]).Trim());
                  }
                else if ( sscanf(&line[0]," tilemap %s",&klass[0]) == 1 )
                  {
                    in_block = true;
                    db.BeginTag1(L"font");
                    db.Body1(+StringT<wchar_t,32>(&klass[0]).Trim());
                  }
                else if ( !in_block && !line.Empty() && line[0] == '#' )
                  {
                    BufferT<char> include(128*2,'\0');
                    if ( 1 == sscanf(&line[0],"#!include %127[^\n]",&include[0]) )
                      {
                        char* p0 = strchr(&include[0],'"');
                        char* p1 = strrchr(&include[0],'"');
                        if ( p0 == p1 )
                          return _E.Fail(doc,_S*format %"bad file name in 'include' directive");
                        ++p0;
                        memmove(&include[0],p0,p1-p0);
                        *(&include[0]+(p1-p0)) = 0;
                        db.BeginTag1(L"include");
                        pwide_t wi = (pwide_t)&include[0];
                        teggo::InplaceCharToWide(&include[0],-1);
                        db.Body1(wi);
                      }
                  }
              }
            else
              {
                char *dilim = teggo::linear_find(line.Begin(),line.End(),'=');
                if ( dilim != line.End() )
                  {
                    teggo::replace(line.Begin(),line.End(),'\t',' ');
                    teggo::transform(line.Begin(),dilim,line.Begin(),tolower);
                    *teggo::remove(line.Begin(),dilim,' ') = 0;
                    if ( line[0] == '#' || line[0] == 0 ) continue;
                    db.Param2(+StringT<wchar_t,32>(&line[0]),+StringT<wchar_t,64>(&*(dilim+1)));
                  }
                else
                  {
                    char c;
                    if ( sscanf(&line[0]," }%c",&c) == 1 )
                      {
                        db.EndCurTag();
                        goto loop_next_record;
                      }
                  }
              }
          }
      }
  none_records_more:;

    db.EndDocument();
    return db.ProduceDocument().forget_();
  }

teggo::Xdocument_ *LoadSST_into_XDoc_FormatDetect(DataStream &ds)
  {
    StringW const &source = ds.Identifier();
    if ( source.EndsWith(L".xml") )
      return XopenXmlDocument(ds).forget_();
    else if ( source.EndsWith(L".def") )
      return XopenDefDocument(ds).forget_();
    else
      return LoadSST_into_XDoc(ds);
  }

SCERROR RSCMAN_Inst_LoadSST(DataStream &ds,CollectionT<Xnode*>& klasses,Xdocument &doc)
  {
    ArrayT<widestring> includes;
    Xdocument loaded ( LoadSST_into_XDoc_FormatDetect(ds) );

    bool has_errors = false;

    if ( !loaded )
      return SCE_FAIL;

    Xnode *root = doc->GetRoot();
    Xiterator i = loaded->GetRoot()->Iterate();
    while ( i.Next() )
      {
        pwide_t Linclude = loaded->LookupLiteral(L"include");
        Xnode *n = i.Get();
        pwide_t content = n->GetContent();
        if ( Linclude && n->GetName()== Linclude )
          includes.Append(content);
        else
          if ( klasses.Get(content) )
            has_errors = true,
            _E.Fail(SCE_FAIL,"failed to register klass"),
            Lout << _S*"[SST] failed to register klass '%s', klass alrady registered" %content;
          else
            {
              n = root->InsertCopyOf(n,true);
              klasses.Put(content,n);
            }
      }

    return has_errors?SCE_FAIL:SCE_OK;
  }

Xnode *SST_::GetKlass(pwide_t klass)
  {
    Xnode ** node = klasses_.Get(klass);
    if ( !node ) node = base_klasses_.Get(klass);
    return node?*node:0;
  }

SCERROR SST_::UndefKlass(pwide_t klass)
  {
    if ( Xnode **node = klasses_.Get(klass) ) {
      doc_->freezed_ = false;
      (*node)->Erase();
      klasses_.Erase(klass);
      doc_->freezed_ = true;
    }
    return SCE_OK;
  }

SCERROR SST_::DefKlass(pwide_t klass,Xnode *node)
  {
    if ( klass && node )
      {
        if ( klasses_.Get(klass) )
          fail_because(_S*"sstmanager: klass %s already exists"%klass);
        if ( node && node == node->GetRoot() )
          node = node->GetSubNode();
        if ( !IsSstKlassVaild(node) )
          fail_because(_S*"sstmanager: klass %s is not valid SST klass"%klass);
        doc_->freezed_ = false;
        Xnode *xnode = doc_->GetRoot()->Insert(node->GetName());
        xnode->SetContent(klass);
        const Xparam *attr = node->GetFirstParam();
        for ( ; attr != 0 ; attr = attr->next  )
          {
            xnode->SetParam(attr->name,attr->GetValue());
          }
        doc_->freezed_ = true;
        klasses_.Put(klass,xnode);
      }
    return SCE_OK;
  }

SCERROR SST_::Load(pwide_t source,u32_t flags)
  {
    CollectionT<Xnode*> &klss = (flags & SstLoadBack)?base_klasses_:klasses_;
    CollectionIteratorT<Xnode*> it = klss.Iterate();
    while ( it.Next() ) (*it)->Erase();

    if ( flags & SstLoadRewrite )
      klss.Clear();

    if ( source && wcslen(source) != 0 )
      {
        if ( rcc_ptr<DataStream> ds = teggo::DataSource->Open(source) )
          {
            doc_->freezed_ = false;
            SCERROR ecode = RSCMAN_Inst_LoadSST(*ds,klss,doc_);
            doc_->freezed_ = true;
            if ( SCE_OK != ecode )
              return SCE_FAIL;
          }
        else
          return _E.Fail(SCE_FAIL,_S*"failed to open sst file '%s'"%source);
      }
    return SCE_OK;
  }

SST_::SST_() : doc_( Xdocument_New())
  {
    doc_->freezed_ = true;
  }

SST_::~SST_()
  {
  }
