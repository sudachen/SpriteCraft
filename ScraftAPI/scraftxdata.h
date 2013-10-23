/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Oct 20 23:11:37 2005
 */
/* Compiler settings for ..\scraftxdata.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __scraftxdata_h__
#define __scraftxdata_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IXdataIterator_FWD_DEFINED__
#define __IXdataIterator_FWD_DEFINED__
typedef interface IXdataIterator IXdataIterator;
#endif 	/* __IXdataIterator_FWD_DEFINED__ */


#ifndef __IXdata_FWD_DEFINED__
#define __IXdata_FWD_DEFINED__
typedef interface IXdata IXdata;
#endif 	/* __IXdata_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_scraftxdata_0000 */
/* [local] */ 





extern RPC_IF_HANDLE __MIDL_itf_scraftxdata_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_scraftxdata_0000_v0_0_s_ifspec;


#ifndef __ScraftXdata_LIBRARY_DEFINED__
#define __ScraftXdata_LIBRARY_DEFINED__

/* library ScraftXdata */
/* [helpstring][uuid][version] */ 




EXTERN_C const IID LIBID_ScraftXdata;

#ifndef __IXdataIterator_INTERFACE_DEFINED__
#define __IXdataIterator_INTERFACE_DEFINED__

/* interface IXdataIterator */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IXdataIterator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7A914D61-5AEF-4fda-B797-11CAA4024E22")
    IXdataIterator : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Reset( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Get( 
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXdataIteratorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IXdataIterator __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IXdataIterator __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IXdataIterator __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IXdataIterator __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IXdataIterator __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IXdataIterator __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IXdataIterator __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IXdataIterator __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IXdataIterator __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            IXdataIterator __RPC_FAR * This,
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);
        
        END_INTERFACE
    } IXdataIteratorVtbl;

    interface IXdataIterator
    {
        CONST_VTBL struct IXdataIteratorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXdataIterator_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXdataIterator_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXdataIterator_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXdataIterator_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXdataIterator_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXdataIterator_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXdataIterator_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXdataIterator_Reset(This,pVal)	\
    (This)->lpVtbl -> Reset(This,pVal)

#define IXdataIterator_Next(This,pVal)	\
    (This)->lpVtbl -> Next(This,pVal)

#define IXdataIterator_Get(This,pI)	\
    (This)->lpVtbl -> Get(This,pI)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IXdataIterator_Reset_Proxy( 
    IXdataIterator __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IXdataIterator_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdataIterator_Next_Proxy( 
    IXdataIterator __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IXdataIterator_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdataIterator_Get_Proxy( 
    IXdataIterator __RPC_FAR * This,
    /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);


void __RPC_STUB IXdataIterator_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXdataIterator_INTERFACE_DEFINED__ */


#ifndef __IXdata_INTERFACE_DEFINED__
#define __IXdata_INTERFACE_DEFINED__

/* interface IXdata */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IXdata;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("71B11E33-FE97-43c4-AAE9-663B218D8369")
    IXdata : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetTag( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRoot( 
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSrcName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSrcNameAsPWIDE( 
            /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContent( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStrAttr( 
            /* [in] */ BSTR name,
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIntAttr( 
            /* [in] */ BSTR name,
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFltAttr( 
            /* [in] */ BSTR name,
            /* [retval][out] */ float __RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBoolAttr( 
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Iterate( 
            /* [retval][out] */ IXdataIterator __RPC_FAR *__RPC_FAR *pI) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IterateTag( 
            /* [in] */ BSTR path,
            /* [retval][out] */ IXdataIterator __RPC_FAR *__RPC_FAR *pI) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCountTag( 
            /* [in] */ BSTR tag,
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Insert( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Erase( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetContent( 
            /* [in] */ BSTR pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetStrAttr( 
            /* [in] */ BSTR name,
            /* [in] */ BSTR val) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIntAttr( 
            /* [in] */ BSTR name,
            /* [in] */ long val) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFltAttr( 
            /* [in] */ BSTR name,
            /* [in] */ float val) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBoolAttr( 
            /* [in] */ BSTR name,
            /* [in] */ VARIANT_BOOL val) = 0;
        
        virtual /* [restricted][hidden] */ HRESULT STDMETHODCALLTYPE StoreToXML( 
            /* [in] */ BSTR filename,
            /* [defaultvalue][in] */ long encoding = 0) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StoreToDEF( 
            /* [in] */ BSTR filename,
            /* [defaultvalue][in] */ long encoding = 0) = 0;
        
        virtual /* [restricted][hidden] */ HRESULT STDMETHODCALLTYPE GetNameAsPWIDE( 
            /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [restricted][hidden] */ HRESULT STDMETHODCALLTYPE GetContentAsPWIDE( 
            /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [restricted][hidden] */ HRESULT STDMETHODCALLTYPE GetStrAttrAsPWIDE( 
            /* [in] */ BSTR name,
            /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Freeze( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InsertCopyOf( 
            /* [in] */ IXdata __RPC_FAR *node,
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXdataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IXdata __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IXdata __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IXdata __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IXdata __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IXdata __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IXdata __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IXdata __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTag )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRoot )( 
            IXdata __RPC_FAR * This,
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSrcName )( 
            IXdata __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSrcNameAsPWIDE )( 
            IXdata __RPC_FAR * This,
            /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetName )( 
            IXdata __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetContent )( 
            IXdata __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetStrAttr )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIntAttr )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFltAttr )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ float __RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetBoolAttr )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Iterate )( 
            IXdata __RPC_FAR * This,
            /* [retval][out] */ IXdataIterator __RPC_FAR *__RPC_FAR *pI);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IterateTag )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR path,
            /* [retval][out] */ IXdataIterator __RPC_FAR *__RPC_FAR *pI);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCountTag )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR tag,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCount )( 
            IXdata __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Insert )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Erase )( 
            IXdata __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetContent )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetStrAttr )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [in] */ BSTR val);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetIntAttr )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [in] */ long val);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFltAttr )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [in] */ float val);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetBoolAttr )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [in] */ VARIANT_BOOL val);
        
        /* [restricted][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StoreToXML )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR filename,
            /* [defaultvalue][in] */ long encoding);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StoreToDEF )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR filename,
            /* [defaultvalue][in] */ long encoding);
        
        /* [restricted][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNameAsPWIDE )( 
            IXdata __RPC_FAR * This,
            /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal);
        
        /* [restricted][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetContentAsPWIDE )( 
            IXdata __RPC_FAR * This,
            /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal);
        
        /* [restricted][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetStrAttrAsPWIDE )( 
            IXdata __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Freeze )( 
            IXdata __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IXdata __RPC_FAR * This,
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InsertCopyOf )( 
            IXdata __RPC_FAR * This,
            /* [in] */ IXdata __RPC_FAR *node,
            /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);
        
        END_INTERFACE
    } IXdataVtbl;

    interface IXdata
    {
        CONST_VTBL struct IXdataVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXdata_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXdata_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXdata_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXdata_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXdata_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXdata_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXdata_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXdata_GetTag(This,name,pI)	\
    (This)->lpVtbl -> GetTag(This,name,pI)

#define IXdata_GetRoot(This,pI)	\
    (This)->lpVtbl -> GetRoot(This,pI)

#define IXdata_GetSrcName(This,pVal)	\
    (This)->lpVtbl -> GetSrcName(This,pVal)

#define IXdata_GetSrcNameAsPWIDE(This,pVal)	\
    (This)->lpVtbl -> GetSrcNameAsPWIDE(This,pVal)

#define IXdata_GetName(This,pVal)	\
    (This)->lpVtbl -> GetName(This,pVal)

#define IXdata_GetContent(This,pVal)	\
    (This)->lpVtbl -> GetContent(This,pVal)

#define IXdata_GetStrAttr(This,name,pVal)	\
    (This)->lpVtbl -> GetStrAttr(This,name,pVal)

#define IXdata_GetIntAttr(This,name,pVal)	\
    (This)->lpVtbl -> GetIntAttr(This,name,pVal)

#define IXdata_GetFltAttr(This,name,pVal)	\
    (This)->lpVtbl -> GetFltAttr(This,name,pVal)

#define IXdata_GetBoolAttr(This,name,pVal)	\
    (This)->lpVtbl -> GetBoolAttr(This,name,pVal)

#define IXdata_Iterate(This,pI)	\
    (This)->lpVtbl -> Iterate(This,pI)

#define IXdata_IterateTag(This,path,pI)	\
    (This)->lpVtbl -> IterateTag(This,path,pI)

#define IXdata_GetCountTag(This,tag,pVal)	\
    (This)->lpVtbl -> GetCountTag(This,tag,pVal)

#define IXdata_GetCount(This,pVal)	\
    (This)->lpVtbl -> GetCount(This,pVal)

#define IXdata_Insert(This,name,pI)	\
    (This)->lpVtbl -> Insert(This,name,pI)

#define IXdata_Erase(This)	\
    (This)->lpVtbl -> Erase(This)

#define IXdata_SetContent(This,pVal)	\
    (This)->lpVtbl -> SetContent(This,pVal)

#define IXdata_SetStrAttr(This,name,val)	\
    (This)->lpVtbl -> SetStrAttr(This,name,val)

#define IXdata_SetIntAttr(This,name,val)	\
    (This)->lpVtbl -> SetIntAttr(This,name,val)

#define IXdata_SetFltAttr(This,name,val)	\
    (This)->lpVtbl -> SetFltAttr(This,name,val)

#define IXdata_SetBoolAttr(This,name,val)	\
    (This)->lpVtbl -> SetBoolAttr(This,name,val)

#define IXdata_StoreToXML(This,filename,encoding)	\
    (This)->lpVtbl -> StoreToXML(This,filename,encoding)

#define IXdata_StoreToDEF(This,filename,encoding)	\
    (This)->lpVtbl -> StoreToDEF(This,filename,encoding)

#define IXdata_GetNameAsPWIDE(This,pVal)	\
    (This)->lpVtbl -> GetNameAsPWIDE(This,pVal)

#define IXdata_GetContentAsPWIDE(This,pVal)	\
    (This)->lpVtbl -> GetContentAsPWIDE(This,pVal)

#define IXdata_GetStrAttrAsPWIDE(This,name,pVal)	\
    (This)->lpVtbl -> GetStrAttrAsPWIDE(This,name,pVal)

#define IXdata_Freeze(This)	\
    (This)->lpVtbl -> Freeze(This)

#define IXdata_Clone(This,pI)	\
    (This)->lpVtbl -> Clone(This,pI)

#define IXdata_InsertCopyOf(This,node,pI)	\
    (This)->lpVtbl -> InsertCopyOf(This,node,pI)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IXdata_GetTag_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);


void __RPC_STUB IXdata_GetTag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_GetRoot_Proxy( 
    IXdata __RPC_FAR * This,
    /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);


void __RPC_STUB IXdata_GetRoot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_GetSrcName_Proxy( 
    IXdata __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IXdata_GetSrcName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_GetSrcNameAsPWIDE_Proxy( 
    IXdata __RPC_FAR * This,
    /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IXdata_GetSrcNameAsPWIDE_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_GetName_Proxy( 
    IXdata __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IXdata_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_GetContent_Proxy( 
    IXdata __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IXdata_GetContent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_GetStrAttr_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IXdata_GetStrAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_GetIntAttr_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IXdata_GetIntAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_GetFltAttr_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ float __RPC_FAR *pVal);


void __RPC_STUB IXdata_GetFltAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_GetBoolAttr_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IXdata_GetBoolAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_Iterate_Proxy( 
    IXdata __RPC_FAR * This,
    /* [retval][out] */ IXdataIterator __RPC_FAR *__RPC_FAR *pI);


void __RPC_STUB IXdata_Iterate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_IterateTag_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR path,
    /* [retval][out] */ IXdataIterator __RPC_FAR *__RPC_FAR *pI);


void __RPC_STUB IXdata_IterateTag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_GetCountTag_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR tag,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IXdata_GetCountTag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_GetCount_Proxy( 
    IXdata __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IXdata_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_Insert_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);


void __RPC_STUB IXdata_Insert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_Erase_Proxy( 
    IXdata __RPC_FAR * This);


void __RPC_STUB IXdata_Erase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_SetContent_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR pVal);


void __RPC_STUB IXdata_SetContent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_SetStrAttr_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [in] */ BSTR val);


void __RPC_STUB IXdata_SetStrAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_SetIntAttr_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [in] */ long val);


void __RPC_STUB IXdata_SetIntAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_SetFltAttr_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [in] */ float val);


void __RPC_STUB IXdata_SetFltAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_SetBoolAttr_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [in] */ VARIANT_BOOL val);


void __RPC_STUB IXdata_SetBoolAttr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden] */ HRESULT STDMETHODCALLTYPE IXdata_StoreToXML_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR filename,
    /* [defaultvalue][in] */ long encoding);


void __RPC_STUB IXdata_StoreToXML_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_StoreToDEF_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR filename,
    /* [defaultvalue][in] */ long encoding);


void __RPC_STUB IXdata_StoreToDEF_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden] */ HRESULT STDMETHODCALLTYPE IXdata_GetNameAsPWIDE_Proxy( 
    IXdata __RPC_FAR * This,
    /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IXdata_GetNameAsPWIDE_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden] */ HRESULT STDMETHODCALLTYPE IXdata_GetContentAsPWIDE_Proxy( 
    IXdata __RPC_FAR * This,
    /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IXdata_GetContentAsPWIDE_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden] */ HRESULT STDMETHODCALLTYPE IXdata_GetStrAttrAsPWIDE_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ wchar_t __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IXdata_GetStrAttrAsPWIDE_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_Freeze_Proxy( 
    IXdata __RPC_FAR * This);


void __RPC_STUB IXdata_Freeze_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_Clone_Proxy( 
    IXdata __RPC_FAR * This,
    /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);


void __RPC_STUB IXdata_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IXdata_InsertCopyOf_Proxy( 
    IXdata __RPC_FAR * This,
    /* [in] */ IXdata __RPC_FAR *node,
    /* [retval][out] */ IXdata __RPC_FAR *__RPC_FAR *pI);


void __RPC_STUB IXdata_InsertCopyOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXdata_INTERFACE_DEFINED__ */

#endif /* __ScraftXdata_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
