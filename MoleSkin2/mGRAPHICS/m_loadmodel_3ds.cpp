
/*

Copyright В© 2003-2013, AlexГ©y SudachГ©n, alexey@sudachen.name

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

#include "precompile.h"
#include "../mcore/m_time.h"
#include "../mstreams/m_igzstream.h"
#include "../nvmath/algebra.h"
#include "m_loadmodel.h"
#include "m_gfx3d.h"

#include <limits.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

using nvmath::vec3;
using nvmath::mat3;
using nvmath::mat4_id;

#define DEBUG_x3DSLOADER
#define x3DSLOADER ":x3DSloader: "

#if defined DEBUG_x3DSLOADER
# define XXMessage XMessage
#else
# define XXMessage(x)
#endif

static const char* INVISIBLE = "#hidden#";

// на основе статьи из книги "Энциклопедия форматов файлов"
// BHV, Киев, 1997

struct x_3DS_CHUNK {
  u16_t  ID;
  u32_t   length;
};

static const u16_t CHID_NULL           = 0x0000;
static const u16_t CHID_VERSION        = 0x0002;
static const u16_t CHID_COLORF         = 0x0010; // rgb float
static const u16_t CHID_COLOR24        = 0x0011; // rgb byte
static const u16_t CHID_IPERCENT       = 0x0030; // rgb byte
static const u16_t CHID_FPERCENT       = 0x0031; // rgb byte
static const u16_t CHID_MASTERSCALE    = 0x0100; // float
static const u16_t CHID_CHUNKTYPE      = 0x0995; 
static const u16_t CHID_CHUNKUNIQUE    = 0x0996;
static const u16_t CHID_NOTCHUNK       = 0x0997;
static const u16_t CHID_CONTAINER      = 0x0998;
static const u16_t CHID_ISCHUNK        = 0x0995;
static const u16_t CHID_BITMAP         = 0x1100; // char[]
static const u16_t CHID_MDATA          = 0x3d3d; // --
static const u16_t CHID_MVERSION       = 0x3d3e; // 
static const u16_t CHID_NAMEDOBJECT    = 0x4000; // char[]
static const u16_t CHID_OBJMESH        = 0x4100; 
static const u16_t CHID_POINTARRAY     = 0x4110; // short,point[]
static const u16_t CHID_FACEARRAY      = 0x4120; // short,face[]
static const u16_t CHID_FACEMATMAP     = 0x4130;
static const u16_t CHID_TEXCOORD       = 0x4140; // short,vertex[]
static const u16_t CHID_SMOOTHGROUP    = 0x4150; // short[]
static const u16_t CHID_MATRIX         = 0x4160; // float[12]
static const u16_t CHID_M3DMAGIC       = 0x4d4d; // --
static const u16_t CHID_MATNAME        = 0xa000; // char[]
static const u16_t CHID_MATAMBIENT     = 0xa010; // 
static const u16_t CHID_MATDIFFUSE     = 0xa020; // 
static const u16_t CHID_MATSPECULAR    = 0xa030; // 
static const u16_t CHID_MATSHININESS   = 0xa040; // 
//static const u16_t CHID_MATSHIN2PCT    = 0xa041; // 
static const u16_t CHID_MATTRANSPARENCY= 0xa050; // 
//static const u16_t CHID_MATXPFALL      = 0xa052; // 
static const u16_t CHID_MATTEXMAP      = 0xa200; // --
static const u16_t CHID_MATTEXNAME     = 0xa300; // char[]

static const u16_t CHID_MAPENTRY       = 0xafff; // --
static const u16_t CHID_KFDATA         = 0xb000; // --
static const u16_t CHID_KFHDR          = 0xb00a; // --
static const u16_t CHID_KFSEG          = 0xb008; // short[2]
static const u16_t CHID_KFCURTIME      = 0xb009; // short
static const u16_t CHID_KFNODETAG      = 0xb002; // --
static const u16_t CHID_KFNODEID       = 0xb030; // short
static const u16_t CHID_KFNODEHDR      = 0xb010; // char[],short[3]
static const u16_t CHID_KFINSTANCE     = 0xb011;
static const u16_t CHID_KFPIVOT        = 0xb013; // float[3]
static const u16_t CHID_KFPOSTRACK     = 0xb020;
static const u16_t CHID_KFROTTRACK     = 0xb021;
static const u16_t CHID_KFSCLTRACK     = 0xb022;

struct x_3DS_TRACKPOINT {
  u16_t frame;
  vec3 point;
  float angle;
};
typedef BufferT<x_3DS_TRACKPOINT> TRACK;

struct SMOOTH_INFO{ 
  u16_t next; 
  u32_t bits; 
};

struct MATMAPINFO{
  BufferT<u16_t> faces;
  Model3D::SLICE* slice;
};

static SCERROR Read_3DS_CHUNK(DataSource* source,x_3DS_CHUNK* chunk);
static SCERROR ReadASCIIZ(InputStream* is,BufferT<char>& asciiz);
static SCERROR ReadColorF(DataSource* source,frgba_t& color,x_3DS_CHUNK chunk);
static SCERROR ReadPercent(DataSource* source,float& percent,x_3DS_CHUNK chunk);
static SCERROR LoadKFTrack(DataSource* source,TRACK& track,x_3DS_CHUNK chunk);
static SCERROR GenKFNodeMatrices(u16_t,Object3D*,const TRACK&,const TRACK&,const TRACK&);
static SCERROR LoadKFNode(DataSource*,Model3D*,x_3DS_CHUNK,BufferT<Object3D*>&);
static SCERROR LoadKFInfo(DataSource* source,Model3D* model,x_3DS_CHUNK chunk);
static void FixOordByPivot(Model3D* model);
static void DumpHierarchy(const BufferT<Object3D*>& objects,pchar_t prefix);
static SCERROR LoadMapEntry(DataSource* source,Model3D* model,x_3DS_CHUNK chunk,u32_t& textime);
static SCERROR LoadObject3D(DataSource* source,Model3D* model,x_3DS_CHUNK chunk);

Model3D* MOLE_FUNCALL CreateModelFrom_3DS(DataSource* source)
{
  u32_t millis = GetMillis();
  u32_t textime = 0;
  ret0_if ( !source );
  AutoModel3D model ( new Model3D );
  model->frames_ = 0;
  x_3DS_CHUNK chunk;
  ret0_if_err ( Read_3DS_CHUNK(source,&chunk) );
  if ( chunk.ID != CHID_M3DMAGIC ) ret0_because (x3DSLOADER"invalid 3ds file");
  u32_t endof3ds = chunk.length + source->WasRead();
  // читаем основную порцию данных
  while ( source->WasRead() < endof3ds ) {
    ret0_if_err ( Read_3DS_CHUNK(source,&chunk) );
    if ( chunk.ID == CHID_VERSION ) {
      XXMessage((x3DSLOADER" 3ds file version %d",source->PeekLong()));
      ret0_if ( source->Skip(chunk.length) != chunk.length );
    }else if ( chunk.ID == CHID_MDATA ) {
      u32_t endofmdata = chunk.length + source->WasRead();
      while ( source->WasRead() < endofmdata ) {
        ret0_if_err ( Read_3DS_CHUNK(source,&chunk) );
        if ( chunk.ID == CHID_MVERSION ) {
          XXMessage((x3DSLOADER" 3ds mesh version %d",source->PeekLong()));
          ret0_if ( source->Skip(chunk.length) != chunk.length );
        }else if ( chunk.ID == CHID_MASTERSCALE ) {
          model->scale_ = source->ReadFloat();
          XXMessage((x3DSLOADER" master scale %f",model->scale_));
        }else if ( chunk.ID == CHID_NAMEDOBJECT ) {
          ret0_if_err ( LoadObject3D(source,model,chunk) );
        }else if (chunk.ID == CHID_MAPENTRY) {
          ret0_if_err ( LoadMapEntry(source,model,chunk,textime) );
        }else{
          XXMessage((x3DSLOADER" chunk 0x%hx, size %d",chunk.ID,chunk.length ));
          ret0_if ( source->Skip(chunk.length) != chunk.length );
        }
      }
    }else if (chunk.ID == CHID_KFDATA) {
      ret0_if_err( LoadKFInfo(source,model,chunk) );
    }else {
      XXMessage((x3DSLOADER" chunk 0x%hx, size %d",chunk.ID,chunk.length ));
      ret0_if ( source->Skip(chunk.length) != chunk.length );
    }
  }

  XMessage((x3DSLOADER" vertices %d/%d",model->vertices_.size(),model->vertices_.capacity()));
  {XDbg( for ( unsigned i=0; i < model->slice_.size(); ++i ) ){
    XDbg(
      XMessage((x3DSLOADER" material %s, trilist size(%d)/faces(%d)/capacity(%d)",
        model->slice_[i].GetMaterialName().c_str(),
        model->slice_[i].indices_.size(),
        model->slice_[i].indices_.size()/3,
        model->slice_[i].indices_.capacity())));
  }}
  XMessage((x3DSLOADER" objects  %d/%d",model->objects_.size(),model->objects_.capacity()));

  DumpHierarchy(model->hierarchy_,"$KFnode$ ");

  millis = GetMillis()-millis;
  LogF(x3DSLOADER"loading %s in %ld ms (textures %ld ms, model %ld ms) ",
    source->GetName(),millis,
    textime,millis-textime);
  return Forget(model);
}

static void DumpHierarchy(const BufferT<Object3D*>& objects,const string& prefix)
{
  Object3D **i  = objects.Begin();
  Object3D **iE = objects.End();
  for ( ; i != iE; ++i ) {
    XMessage((x3DSLOADER"%sobject('%s')",
      prefix.c_str(),(*i)->name_.c_str()));
    DumpHierarchy((*i)->hierarchy_,prefix+"   ");
  }
}

/*

  работа с обьектами модели

*/

static SCERROR LoadObject3D(DataSource* source,Model3D* model,x_3DS_CHUNK chunk) 
{
  unsigned endofobject = chunk.length+source->WasRead();
  vector<char> name_of_object(128);
  AutoObject3D obj ( new Object3D );
  vector<u16_t> indices;
  vector<MATMAPINFO> matmap;
  unsigned ix_count = 0;
  unsigned vx_count = 0;
  unsigned vx_first = model->vertices_.size();
  bool invisible = false;

  fail_if_err ( ReadASCIIZ(source,name_of_object) );
  XXMessage((x3DSLOADER"+object('%s'), %d",&name_of_object[0],chunk.length));
  obj->name_  = &name_of_object[0];
  obj->model_ = model;

  while ( source->WasRead() < endofobject ) {
    fail_if_err ( Read_3DS_CHUNK(source,&chunk) );
    if ( chunk.ID == CHID_OBJMESH ) {
      vx_count = 0;

      unsigned endofmesh = chunk.length+source->WasRead();
      while ( source->WasRead() < endofmesh ) {
/*VERTICES*/
        fail_if_err ( Read_3DS_CHUNK(source,&chunk) );
        if ( chunk.ID == CHID_POINTARRAY ) {
          unsigned elements = source->ReadShort();
          model->vertices_.resize(vx_first+elements,VxB_VERTEX::EMPTY);
          if ( vx_count != 0 && vx_count != elements )
            fail_because("count of vertices defined but invalid");
          vx_count = elements;
          XXMessage((x3DSLOADER" point_arry(%d)", elements));
          for ( unsigned i=0; i < elements ; ++i) {
            VxB_VERTEX& vertex = model->vertices_[vx_first+i];
            vertex.vx = source->ReadFloat();
            vertex.vy = source->ReadFloat();
            vertex.vz = source->ReadFloat();
          }
          
        }else if ( chunk.ID == CHID_TEXCOORD) {
/*TEXCOORD*/
          unsigned elements = source->ReadShort();
          model->vertices_.resize(vx_first+elements,VxB_VERTEX::EMPTY);
          if ( vx_count != 0 && vx_count != elements )
            fail_because("count of vertices defined but invalid");
          vx_count = elements;
          XXMessage((x3DSLOADER" texcoord_arry(%d)", elements));
          for ( unsigned i=0 ; i < elements ; ++i) {
            VxB_VERTEX& vertex = model->vertices_[vx_first+i];
            vertex.ts = source->ReadFloat();
            vertex.tt = 1-source->ReadFloat();
          }
        }else if ( chunk.ID == CHID_FACEARRAY ) {
/*FACES*/
          ix_count = source->ReadShort()*3;
          indices.resize(ix_count);
          XXMessage((x3DSLOADER" face_arry(%d/%d)", ix_count,ix_count/3));
          for ( unsigned i = 0; i < ix_count ; i+=3) {
            indices[i+0] = source->ReadShort()+vx_first;
            indices[i+1] = source->ReadShort()+vx_first;
            indices[i+2] = source->ReadShort()+vx_first;
            source->ReadShort(); // skip face flag
          }
        }else if ( chunk.ID == CHID_FACEMATMAP ) {
/*MATERIALSMAP*/
          vector<char> matname;
          fail_if_err ( ReadASCIIZ(source,matname) );
          u16_t number_of_entries = source->ReadShort();
          fail_if ( source->PeekError() );
          if ( stricmp(&matname[0],INVISIBLE) == 0 ) {
            XXMessage((x3DSLOADER" hidden object (%d faces)",number_of_entries));
            invisible = true;
            source->Skip(2*number_of_entries);
          }else{
            XXMessage((x3DSLOADER" material '%s' mapping (%d faces)",&matname[0],number_of_entries));
            Model3D::SLICE* slice = model->FindSliceByMaterial(&matname[0]);
            if ( slice == 0 ) fail_because("can't found associated material");
            matmap.resize(matmap.size()+1);
            MATMAPINFO& info = matmap.back();
            info.faces.resize(number_of_entries);
            info.slice = slice;
            {for ( int i=0; i < number_of_entries; ++i ){
              info.faces[i] = source->ReadShort();
            }}
          }
          fail_if ( source->PeekError() );
        }else if ( chunk.ID == CHID_SMOOTHGROUP ) {
/*SMOOTHGROUP*/
          XXMessage((x3DSLOADER"SMOOTH_GROUP size %d",chunk.length));
          vector<SMOOTH_INFO> smoothing;
          smoothing.reserve(vx_count+vx_count/2);
          smoothing.resize(vx_count);
          memset( &smoothing[0], 0 , vx_count*sizeof(SMOOTH_INFO) );
          fail_if ( chunk.length/4 != ix_count/3 );
          {for ( u32_t i = 0; i < chunk.length/4; ++i ) {
            u32_t  bits  = source->ReadLong();
            for ( int j = 0; j < 3; ++j ) {
              u16_t ix0 = indices[i*3+j];
              int idx = ix0 - vx_first;
              for (;;) {
                if ( smoothing[idx].bits == 0 ) {
                  smoothing[idx].bits = bits;
                  break;
                }else{
                  if ( smoothing[idx].bits & bits ) {
                    smoothing[idx].bits &= bits;
                    indices[i*3+j] = vx_first+idx;
                    break;
                  }else {
                    if ( !smoothing[idx].next ) {
                      //XXMessage(("duplicate vertex with smg %x as smg %x",smoothing[idx].bits,bits));
                      SMOOTH_INFO sminfo = { 0, bits };
                      smoothing.push_back(sminfo);
                      smoothing[idx].next = smoothing.size()-1;
                      model->vertices_.push_back(model->vertices_[ix0]);
                      ++vx_count;
                      indices[i*3+j] = vx_first+smoothing[idx].next;
                      break;
                    }
                    idx = smoothing[idx].next;
                    // continue;
                  }
              }} 
            } 
          }}
          fail_if ( ix_count%3 != 0 );
        }else if ( chunk.ID == CHID_MATRIX ) {
/*TRANSFORMATION*/
          XXMessage((x3DSLOADER" object transformation chunk"));

          mat4 mt (nvmath::mat4_id);

          // read rotation
          mt.a00 = source->ReadFloat();
          mt.a10 = source->ReadFloat();
          mt.a20 = source->ReadFloat();
          mt.a01 = source->ReadFloat();
          mt.a11 = source->ReadFloat();
          mt.a21 = source->ReadFloat();
          mt.a02 = source->ReadFloat();
          mt.a12 = source->ReadFloat();
          mt.a22 = source->ReadFloat();
          
          // read translation
          mt.a03 = source->ReadFloat();
          mt.a13 = source->ReadFloat();
          mt.a23 = source->ReadFloat();
          
          nvmath::invert(obj->oord_,mt); 
          
        }else{
          XXMessage((x3DSLOADER" chunk 0x%hx, size %d",chunk.ID,chunk.length ));
          fail_if ( source->Skip(chunk.length) != chunk.length );  
        }
      }
/* slice by materials */
      if ( !invisible )
      {for ( int i=0, iE=matmap.size(); i < iE; ++i ){
        Model3D::SLICE* slice = matmap[i].slice;
        const vector<u16_t>& faces = matmap[i].faces;
        unsigned const number_of_entries = faces.size();
        Object3D::SLICE oslice;
        oslice.ixfirst_  = slice->indices_.size();
        oslice.ixcount_  = number_of_entries*3;
        oslice.slice_    = slice-&model->slice_[0];
        slice->indices_.reserve(slice->indices_.size()+oslice.ixcount_);
        // зачитываем номер треугольника и записываем в слайс три вертекс индекса
        {for ( unsigned i=0; i < number_of_entries; ++i ) {
          u16_t face = faces[i];
          slice->indices_.push_back(indices[face*3+0]);
          slice->indices_.push_back(indices[face*3+1]);
          slice->indices_.push_back(indices[face*3+2]);
        }}
        obj->faces_.push_back(oslice);
      }}
      else // invisible
      {
        model->vertices_.resize(vx_first);
        vx_first = 0;
        vx_count = 0;
      }
      obj->vxfirst_ = vx_first;
      obj->vxcount_ = vx_count;
      model->objects_.push_back(obj);
    }else{
      XXMessage((x3DSLOADER" chunk 0x%hx, size %d",chunk.ID,chunk.length ));
      fail_if ( source->Skip(chunk.length) != chunk.length );  
    }
  }
  return SCE_OK;
}

/*

  работа с материалами

*/

static SCERROR LoadMapEntry(DataSource* source,Model3D* model,x_3DS_CHUNK chunk,u32_t& textime)
{
  AutoMaterialInfo material ( new MaterialInfo );
  memset(&material->colors_,0,sizeof(material->colors_));
  u32_t endofme = chunk.length + source->WasRead();
  while ( source->WasRead() < endofme ) {
    fail_if_err ( Read_3DS_CHUNK(source,&chunk) );
    if ( chunk.ID == CHID_MATNAME ) {
/*MATERIALNAME*/
      vector<char> matname;
      fail_if_err ( ReadASCIIZ(source,matname) );
      material->name_ = string(&matname[0]);
      XXMessage((x3DSLOADER" +material '%s'", &matname[0]) );
    //}else if ( chunk.ID == CHID_MATAMBIENT ) { skipped
    }else if ( chunk.ID == CHID_MATDIFFUSE ) {
/*DIFFUSE*/
      fail_if_err ( ReadColorF(source,material->colors_.diffuse,chunk) );
      XDbg( frgba_t& c = material->colors_.diffuse );
      XXMessage((x3DSLOADER"  diffuse %f,%f,%f,%f",c.R,c.G,c.B,c.A));
    }else if ( chunk.ID == CHID_MATSPECULAR ) {
/*SPECULAR*/
      fail_if_err ( ReadColorF(source,material->colors_.specular,chunk) );
      XDbg( frgba_t& c = material->colors_.specular );
      XXMessage((x3DSLOADER"  specular %f,%f,%f,%f",c.R,c.G,c.B,c.A));
    }else if ( chunk.ID == CHID_MATSHININESS ) {
/*SHININESS*/
      fail_if_err ( ReadPercent(source,material->colors_.shininess,chunk) );
      XXMessage((x3DSLOADER"  shininess %f",material->colors_.shininess));
    }else if ( chunk.ID == CHID_MATTEXMAP ) {
/*TEXTURE*/
      u32_t endoftmap = chunk.length + source->WasRead();
      while ( source->WasRead() < endoftmap ) {
        fail_if_err ( Read_3DS_CHUNK(source,&chunk) );
        if ( chunk.ID == CHID_MATTEXNAME ) {
          vector<char> texname;
          fail_if_err ( ReadASCIIZ(source,texname) );
          XXMessage((x3DSLOADER"  texture '%s'", &texname[0]) );
          string texnameS ( source->GetAsciiName() );
          size_t ipos = teggo_max<int> ( texnameS.find_last_of('/'),texnameS.find_last_of('\\') );
          if ( ipos != string::npos ) {
            texnameS.resize(ipos+1); texnameS.append(&texname[0]);
          }else texnameS = &texname[0];
          u32_t textime0 = GetMillis();
          material->texture_ = AutoRef(TextureSet::Acquire(texnameS.c_str()));
          textime += GetMillis()-textime0;
          if ( IsNull(material->texture_) ) 
            fail_because(
              Format("Material '%s' is broken because texture '%s' is not found",
                material->name_.c_str(),
                texnameS.c_str()));
        }else{
          //XXMessage((x3DSLOADER"  chunk 0x%hx, size %d",chunk.ID,chunk.length ));
          fail_if ( source->Skip(chunk.length) != chunk.length );
        }
      }
    }else{
      //XXMessage((x3DSLOADER" chunk 0x%hx, size %d",chunk.ID,chunk.length ));
      fail_if ( source->Skip(chunk.length) != chunk.length );
    }
  }
  if ( stricmp( material->name_.c_str(),INVISIBLE ) != 0 ) {
    Model3D::SLICE slice;
    slice.material_ = material;
    model->slice_.push_back(slice);
  }
  return SCE_OK;
}

/*

  Load & Prepare Key frames 

*/

// загружем анимацию
static SCERROR LoadKFInfo(DataSource* source,Model3D* model,x_3DS_CHUNK chunk)
{
  XXMessage((x3DSLOADER"KFdata"));
  vector<Object3D*> nodes; nodes.reserve(model->objects_.size());
  u32_t endofkfdata = chunk.length + source->WasRead();
  while ( source->WasRead() < endofkfdata ) {
    fail_if_err ( Read_3DS_CHUNK(source,&chunk) );
    if ( chunk.ID == CHID_KFHDR ) {
      u16_t ver;
      ver = source->ReadShort();
      vector<char> fname; 
      fail_if_err( ReadASCIIZ(source,fname) );
      u16_t anim_length = source->ReadShort();
      XXMessage((x3DSLOADER"KFhdr(%d/%d) ver %hd, len %hd, '%s'",
        (fname.size()+4),chunk.length,ver,anim_length,&fname[0]));
      source->Skip(chunk.length-(fname.size()+4));
      model->frames_ = anim_length;
    }else if ( chunk.ID == CHID_KFNODETAG ) {
      fail_if_err(LoadKFNode(source,model,chunk,nodes));
    }else{
      XXMessage((x3DSLOADER" chunk 0x%hx, size %d",chunk.ID,chunk.length ));
      fail_if ( source->Skip(chunk.length) != chunk.length );
    }
  }
  FixOordByPivot(model);
  return SCE_OK;
}

// загружаем узел иерархии анимационных треков
static SCERROR 
LoadKFNode(
  DataSource* source,Model3D* model,x_3DS_CHUNK chunk,vector<Object3D*>& nodes)
{
  TRACK pos_track;
  TRACK rot_track;
  TRACK scl_track;
  u16_t noid = 0;
  Object3D* object = 0;
      
  u32_t endofkfntag = chunk.length + source->WasRead();
  while ( source->WasRead() < endofkfntag ) {
    fail_if_err ( Read_3DS_CHUNK(source,&chunk) );
    if ( chunk.ID == CHID_KFNODEID ) {
      noid = source->ReadShort();
    }else if ( chunk.ID == CHID_KFNODEHDR ) {
      vector<char> node_name;
      fail_if_err( ReadASCIIZ(source,node_name) );
      XXMessage((x3DSLOADER" KFnode '%s' ID %hd",&node_name[0],noid));
      object = model->FindObjectByName(&node_name[0]);
      if ( !object ) 
        fail_because(Format(
          "invalid kfnode for object '%s' object is not exists",
          &node_name[0]));
      if ( object->track_.size() ) {
        //fail_because("referenced objects is not supported");
        Object3D* referenced_object = object;
        object = new Object3D;
        model->objects_.push_back(AutoRef(object));
        object->name_  = FormatS("%s@%hd",referenced_object->name_.c_str(),noid);
        object->refer_ = referenced_object;
        object->model_ = model;
        object->vxcount_ = 0;
        object->vxfirst_ = 0;
        object->oord_ = referenced_object->oord_;
      }
      
      source->ReadShort(); // skip flag 0
      source->ReadShort(); // skip flag 1
      u16_t hierarchy = source->ReadShort();
      //fail_if_fail ( model->objects_.size() > noid );
      if ( hierarchy == 0xffff ) {
        model->hierarchy_.push_back(AutoRef(Grab(object)));
        object->parent_ = 0;
      }else {
        fail_if_fail ( model->objects_.size() > hierarchy );
        nodes[hierarchy]->hierarchy_.push_back(AutoRef(Grab(object)));
        object->parent_ = nodes[hierarchy];
      }
      if ( noid <= nodes.size() ) nodes.resize(noid+1,0);
      nodes[noid] = object;
    }else if ( chunk.ID == CHID_KFPIVOT ) {
      object->pivot_.x = source->ReadFloat();
      object->pivot_.y = source->ReadFloat();
      object->pivot_.z = source->ReadFloat();
    }else if ( chunk.ID == CHID_KFPOSTRACK ) {
      fail_if_err (LoadKFTrack(source,pos_track,chunk));
    }else if ( chunk.ID == CHID_KFROTTRACK ) {
      fail_if_err (LoadKFTrack(source,rot_track,chunk));
    }else if ( chunk.ID == CHID_KFSCLTRACK ) {
      fail_if_err (LoadKFTrack(source,scl_track,chunk));
    }else if ( chunk.ID == CHID_KFINSTANCE ){
      vector<char> instname;
      fail_if_err ( ReadASCIIZ(source,instname) );
      XXMessage((x3DSLOADER" instance name %s",&instname[0]));
      object->name_ = &instname[0];
    }else{
      XXMessage((x3DSLOADER" chunk 0x%hx, size %d",chunk.ID,chunk.length ));
      fail_if ( source->Skip(chunk.length) != chunk.length );
    }
  }
  GenKFNodeMatrices( model->frames_, object, pos_track, rot_track, scl_track);
  return SCE_OK;
}

/// загружет трек с трансформациями кадра
static SCERROR LoadKFTrack(DataSource* source,TRACK& track,x_3DS_CHUNK chunk)
{
  source->ReadShort(); // skip flag
  u16_t unknown4[4];
  { for ( int i=0;i <4; ++i ) unknown4[i] = source->ReadShort(); }
  u16_t keys_no = source->ReadShort();
  source->ReadShort(); // skip unknown
  fail_if_err ( source->PeekError() );
  track.resize(keys_no);
  for ( unsigned i=0; i < keys_no ; ++i ) {
    track[i].frame = source->ReadShort();
    source->Skip(4); // unknown
    if ( chunk.ID == CHID_KFROTTRACK ) // если это поворот то прочитать угол поворота
      track[i].angle = -source->ReadFloat();
    track[i].point.x = source->ReadFloat();
    track[i].point.y = source->ReadFloat();
    track[i].point.z = source->ReadFloat();
    fail_if_err ( source->PeekError() );
  }
  return SCE_OK;
}


/*

  Read simple Chunks & values

*/

static SCERROR Read_3DS_CHUNK(DataSource* source,x_3DS_CHUNK* chunk)
{
  chunk->ID     = source->ReadShort();
  chunk->length = source->ReadLong();
  fail_if ( source->PeekError() != SCE_OK || chunk->length < 2+4 );
  chunk->length -= (2+4);
  return SCE_OK;
}

static SCERROR ReadASCIIZ(InputStream* is,vector<char>& asciiz) {
  asciiz.clear();
  asciiz.reserve(128);
  char ch;
  do { 
    ch = is->ReadByte();
    fail_if_err ( is->PeekError() );
    asciiz.push_back(ch);
  }while ( ch != 0 );
  return SCE_OK;
}

static SCERROR ReadColorF(DataSource* source,frgba_t& color,x_3DS_CHUNK chunk)
{
  color = fRGBA_t(0,0,0,1.0f);
  u32_t endofdat = chunk.length + source->WasRead();
  while ( source->WasRead() < endofdat ) {
    fail_if_err ( Read_3DS_CHUNK(source,&chunk) );
    if ( chunk.ID == CHID_COLOR24 ) {
      color.R = float(source->ReadByte()) * (1.0f/255.0f);
      color.G = float(source->ReadByte()) * (1.0f/255.0f);
      color.B = float(source->ReadByte()) * (1.0f/255.0f);
      color.A = 1.0f;
      fail_if_err ( source->PeekError() );
    }else{
      XXMessage((x3DSLOADER"  chunk 0x%hx, size %d",chunk.ID,chunk.length ));
      fail_if ( source->Skip(chunk.length) != chunk.length );
    }
  }
  return SCE_OK;
}

static SCERROR ReadPercent(DataSource* source,float& percent,x_3DS_CHUNK chunk)
{
  percent = 0;
  u32_t endofdat = chunk.length + source->WasRead();
  while ( source->WasRead() < endofdat ) {
    fail_if_err ( Read_3DS_CHUNK(source,&chunk) );
    if ( chunk.ID == CHID_IPERCENT ) {
      u16_t ipercent = source->ReadShort();
      percent = float(ipercent)/float((u16_t)USHRT_MAX);
      fail_if_err ( source->PeekError() );
    }else if ( chunk.ID == CHID_FPERCENT ) {
      percent = source->ReadFloat();
      fail_if_err ( source->PeekError() );
    }else{
      XXMessage((x3DSLOADER"  chunk 0x%hx, size %d",chunk.ID,chunk.length ));
      fail_if ( source->Skip(chunk.length) != chunk.length );
    }
  }
  return SCE_OK;
}


/*

  Вычисление анимационных матриц

*/

static SCERROR 
GenKFNodeMatrices(
  u16_t frames_no,Object3D* object,
  const TRACK& pos_track,const TRACK& rot_track,const TRACK& scl_track)
{
  object->track_.resize(frames_no+1);
  u16_t fr_no = 0;
  TRACK::const_iterator posit  = pos_track.begin();
  TRACK::const_iterator positE = pos_track.end();
  TRACK::const_iterator rotit  = rot_track.begin();
  TRACK::const_iterator rotitE = rot_track.end();
  TRACK::const_iterator sclit  = scl_track.begin();
  TRACK::const_iterator sclitE = scl_track.end();
  mat4 mR ( mat4_id );  // матрица поворота (наследуется)
  mat4 mT ( mat4_id );  // матрияа сдвига   (не наследуется) 
  mat4 mS ( mat4_id);   // матрица масштабирования (наследуется)
  for(;fr_no<=frames_no;++fr_no)
  {
    mat4 mr ( mat4_id );
    // вычисляем матрицу смещения
    while ( posit != positE ) { 
      if ( (*posit).frame == fr_no ) mT.set_translation((*posit).point);
      else if ( (*posit).frame < fr_no ) { ++posit; continue; }
      else {
        TRACK::const_iterator p = posit-1;
        const float flr0 = (*posit).frame - (*p).frame;
        const float flr1 = fr_no - (*p).frame;
        const float flr = flr1/flr0;
        vec3  shift = (*posit).point-(*p).point;
        shift.x *= flr;
        shift.y *= flr;
        shift.z *= flr;
        mT.set_translation((*p).point+shift);
      }
      break;
    }

    // вычисляем матрицу масштабирования
    while ( sclit != sclitE ) {
      if ( (*sclit).frame == fr_no ) {
        mS.a00 = (*sclit).point.x;
        mS.a11 = (*sclit).point.y;
        mS.a22 = (*sclit).point.z;
      }
      else if ( (*sclit).frame < fr_no ) { ++sclit; continue; }
      break;
    }

    // вычисляем матрицу поворота
    while ( rotit != rotitE ) {
      if ( (*rotit).frame == fr_no ) {
        mr.set_rot((*rotit).angle,(*rotit).point);
        mR = mR  * mr;
        mr = mat4_id;
      }
      else if ( (*rotit).frame < fr_no ) { ++rotit; continue; }
      else {
        TRACK::const_iterator p = rotit-1;
        const float flr0 = (*rotit).frame - (*p).frame;
        const float flr1 = fr_no - (*p).frame;
        const float flr = flr1/flr0;
        mr.set_rot((*rotit).angle*flr,(*rotit).point);
      }
      break;
    }

    // вычисляем матрицу трансформации вертексов обьекта для фрейма fr_no
    mat4 mm =
      mat4_id
      * mT
      * mS
      * mR
      * mr
      ;
    if ( fr_no == 0 ) {
      object->relT_ = mm;
      if ( object->parent_ )
        object->absT_ = object->parent_->absT_ * mm;
      else
        object->absT_ = mm;
    }else{
      if ( object->parent_ )
        object->track_[fr_no-1] = object->parent_->track_[fr_no-1] * mm;
      else
        object->track_[fr_no-1] = mm;
    }
  }
  return SCE_OK;
}

static void FixOordByPivot(Model3D* model)
{
  for ( int i = 0, iE = model->objects_.size(); i < iE; ++i )
  {
    Object3D* obj = model->objects_[i];
    if ( !obj->refer_ ) {
      mat4 inv (mat4_id);
      float mirrored = obj->oord_.a00 * obj->oord_.a11 * obj->oord_.a22;
      const mat4& fr0 = model->objects_[i]->track_.front();
      mirrored = mirrored<0?mirrored:fr0.a00 * fr0.a11 * fr0.a22;
      // проверяем что обьект отзеркален, 
      // если это так то иверсим порядок обхода вершин треугольников
      if ( mirrored < 0 && !obj->refer_ ) {
        XXMessage(("object('%s') mirrored, fixing",obj->name_));
        for ( int face = 0, faceE = obj->faces_.size() ; face < faceE; ++face ) {
          int ix = obj->faces_[face].ixfirst_;
          int const ixE = ix+obj->faces_[face].ixcount_; 
          Model3D::SLICE* slice = &model->slice_[obj->faces_[face].slice_];
          for ( ; ix < ixE ; ix+=3 ) {
            swap(slice->indices_[ix],slice->indices_[ix+2]);
          }
        }
        inv.a00 = -1;
      }
      mat4 pT( mat4_id ) ; pT.set_translation(-obj->pivot_);
      obj->oord_  = pT * inv * obj->oord_;
    }else{
      mat4 pT( mat4_id ) ; pT.set_translation(-obj->pivot_);
      obj->oord_  = pT * obj->oord_;
    }
  }
}

NAMESPACE_MSKIN_END


