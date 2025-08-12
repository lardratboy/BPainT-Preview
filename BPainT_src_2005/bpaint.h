// bpaint.h : main header file for the BPAINT application
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//

#if !defined(AFX_BPAINT_H__388E8CBE_0339_42F3_9465_49BA3823429C__INCLUDED_)
#define AFX_BPAINT_H__388E8CBE_0339_42F3_9465_49BA3823429C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include <list>

// (REGISTERED-MEMORY-LEAK-SYSTEM) ------------------------------------------

//
//	TMemoryLeakCollection<>
//

template< class T, const bool bIsArray > class TMemoryLeakCollection {

	typedef std::list< T > collection_type;

	collection_type m_Collection;

public:

	~TMemoryLeakCollection() {

#ifdef _DEBUG
		TRACE( "Registered Memory Leak destroying collection\n" );
#endif

		while ( !m_Collection.empty() ) {

#ifdef _DEBUG
			TRACE( "\t%p destroyed\n", (void *)m_Collection.back() );
#endif

			if ( bIsArray ) {

				delete [] m_Collection.back();

			} else {

				delete m_Collection.back();

			}

			m_Collection.pop_back();

		}

#ifdef _DEBUG
		TRACE( "done\n" );
#endif

	}

	void push_back( T p ) {

		m_Collection.push_back( p );

	}

	void remove( T p ) {

		collection_type::iterator it = std::find( m_Collection.begin(), m_Collection.end(), p );

		if ( it != m_Collection.end() ) {

			m_Collection.erase( it );

		}

	}

};

// ----------------------------------------------------------------------------

//
//	AtExitDeleteArray<>()
//

template< class T > void AtExitDeleteArray( T p, const bool bRemove = false ) {

	static TMemoryLeakCollection<T,true> m_Leaks;

	if ( bRemove ) {

		m_Leaks.remove( p );

	} else {

		m_Leaks.push_back( p );

	}

}

// ----------------------------------------------------------------------------

//
//	AtExitDeleteObject<>()
//

template< class T > void AtExitDeleteObject( T p, const bool bRemove = false ) {

	static TMemoryLeakCollection<T,false> m_Leaks;

	if ( bRemove ) {

		m_Leaks.remove( p );

	} else {

		m_Leaks.push_back( p );

	}

}

// (DOCUMENT-TEMPLATE-HELPER-MACROS) ----------------------------------------

#define DOC_TEMP_HELP_BEGIN_CONSTRUCT
#define DOC_TEMP_HELP_END_CONSTRUCT

#define DOC_TEMP_HELP_CONSTRUCT( THE_NAME ) \
	m_p##THE_NAME = 0;

// ------------------

#define DOC_TEMP_HELP_BEGIN_DECLARE
#define DOC_TEMP_HELP_END_DECLARE

#define DOC_TEMP_HELP_DECLARE( THE_NAME ) \
	private: CMultiDocTemplate * m_p##THE_NAME; \
	public: CMultiDocTemplate * Get##THE_NAME() { return m_p##THE_NAME; }

// ------------------

#define DOC_TEMP_HELP_BEGIN_DEFINE_ID \
	public: enum DOCTEMPLATEID { \
		UNKNOWN = 0

#define DOC_TEMP_HELP_END_DEFINE_ID \
	};

#define DOC_TEMP_HELP_DEFINE_ID( THE_NAME, THE_ID ) \
		, ##THE_NAME = (THE_ID)

// ------------------

#define DOC_TEMP_HELP_BEGIN_PTR2ID \
	int GetDocTemplateStorageID( CDocTemplate * pDocTemplate ) {

#define DOC_TEMP_HELP_END_PTR2ID \
		return 0; \
	}

#define DOC_TEMP_HELP_PTR2ID( THE_NAME ) \
		if ( pDocTemplate == (CDocTemplate *)m_p##THE_NAME ) { \
			return DOCTEMPLATEID::##THE_NAME; \
		}

// ------------------

#define DOC_TEMP_HELP_BEGIN_ID2PTR \
	CDocTemplate * GetDocTemplateFromStorageID( const int storageID ) {

#define DOC_TEMP_HELP_END_ID2PTR \
		return 0; \
	}

#define DOC_TEMP_HELP_ID2PTR( THE_NAME ) \
	if ( storageID == (DOCTEMPLATEID::##THE_NAME) ) { \
		return Get##THE_NAME(); \
	}

/////////////////////////////////////////////////////////////////////////////
// CBpaintApp:
// See bpaint.cpp for the implementation of this class
//

class CBpaintApp : public CWinApp
{

	// For each document template the following sections need
	// an element.  The constructor also needs to be filled out.

	// ----------

	DOC_TEMP_HELP_BEGIN_DEFINE_ID
		DOC_TEMP_HELP_DEFINE_ID( DocTemplate, 0x12340001 )
		DOC_TEMP_HELP_DEFINE_ID( PopupEditDocTemplate, 0x12340002 )
		DOC_TEMP_HELP_DEFINE_ID( MDIChildEditDocTemplate, 0x12340003 )
		DOC_TEMP_HELP_DEFINE_ID( MDISimplePaletteDocTemplate, 0x12340004 )
		DOC_TEMP_HELP_DEFINE_ID( PopupSimplePaletteDocTemplate, 0x12340005 )
		DOC_TEMP_HELP_DEFINE_ID( MDIChildGridDocTemplate, 0x12340006 )
		DOC_TEMP_HELP_DEFINE_ID( PopupGridDocTemplate, 0x12340007 )
		DOC_TEMP_HELP_DEFINE_ID( MDIChildMultiView1DocTemplate, 0x12340008 )
		DOC_TEMP_HELP_DEFINE_ID( MDIChildMultiView2DocTemplate, 0x12340009 )
		DOC_TEMP_HELP_DEFINE_ID( WindowClipDocTemplate, 0x1234000a )
		DOC_TEMP_HELP_DEFINE_ID( AnimationSpotsDocTemplate, 0x1234000b )
		DOC_TEMP_HELP_DEFINE_ID( ShowcaseAnimsDocTemplate, 0x1234000c )
		DOC_TEMP_HELP_DEFINE_ID( PopupAnimSpotsDocTemplate, 0x1234000d )
		DOC_TEMP_HELP_DEFINE_ID( VisualElementInfoViewDocTemplate, 0x1234000e )
		DOC_TEMP_HELP_DEFINE_ID( PopupVisualElementInfoViewDocTemplate, 0x1234000f )
		DOC_TEMP_HELP_DEFINE_ID( LayerStripViewDocTemplate, 0x12340010 )
		DOC_TEMP_HELP_DEFINE_ID( FrameStripViewDocTemplate, 0x12340011 )
		DOC_TEMP_HELP_DEFINE_ID( BrushViewDocTemplate, 0x12340012 )
	DOC_TEMP_HELP_END_DEFINE_ID

	// ----------

	DOC_TEMP_HELP_BEGIN_DECLARE
		DOC_TEMP_HELP_DECLARE( DocTemplate )
		DOC_TEMP_HELP_DECLARE( PopupEditDocTemplate )
		DOC_TEMP_HELP_DECLARE( MDIChildEditDocTemplate )
		DOC_TEMP_HELP_DECLARE( MDISimplePaletteDocTemplate )
		DOC_TEMP_HELP_DECLARE( PopupSimplePaletteDocTemplate )
		DOC_TEMP_HELP_DECLARE( MDIChildGridDocTemplate )
		DOC_TEMP_HELP_DECLARE( PopupGridDocTemplate )
		DOC_TEMP_HELP_DECLARE( MDIChildMultiView1DocTemplate )
		DOC_TEMP_HELP_DECLARE( MDIChildMultiView2DocTemplate )
		DOC_TEMP_HELP_DECLARE( WindowClipDocTemplate )
		DOC_TEMP_HELP_DECLARE( AnimationSpotsDocTemplate )
		DOC_TEMP_HELP_DECLARE( ShowcaseAnimsDocTemplate )
		DOC_TEMP_HELP_DECLARE( PopupAnimSpotsDocTemplate )
		DOC_TEMP_HELP_DECLARE( VisualElementInfoViewDocTemplate )
		DOC_TEMP_HELP_DECLARE( PopupVisualElementInfoViewDocTemplate )
		DOC_TEMP_HELP_DECLARE( LayerStripViewDocTemplate )
		DOC_TEMP_HELP_DECLARE( FrameStripViewDocTemplate )
		DOC_TEMP_HELP_DECLARE( BrushViewDocTemplate )
	DOC_TEMP_HELP_END_DECLARE

	// ----------

	DOC_TEMP_HELP_BEGIN_PTR2ID
		DOC_TEMP_HELP_PTR2ID( DocTemplate )
		DOC_TEMP_HELP_PTR2ID( PopupEditDocTemplate )
		DOC_TEMP_HELP_PTR2ID( MDIChildEditDocTemplate )
		DOC_TEMP_HELP_PTR2ID( MDISimplePaletteDocTemplate )
		DOC_TEMP_HELP_PTR2ID( PopupSimplePaletteDocTemplate )
		DOC_TEMP_HELP_PTR2ID( MDIChildGridDocTemplate )
		DOC_TEMP_HELP_PTR2ID( PopupGridDocTemplate )
		DOC_TEMP_HELP_PTR2ID( MDIChildMultiView1DocTemplate )
		DOC_TEMP_HELP_PTR2ID( MDIChildMultiView2DocTemplate )
		DOC_TEMP_HELP_PTR2ID( WindowClipDocTemplate )
		DOC_TEMP_HELP_PTR2ID( AnimationSpotsDocTemplate )
		DOC_TEMP_HELP_PTR2ID( ShowcaseAnimsDocTemplate )
		DOC_TEMP_HELP_PTR2ID( PopupAnimSpotsDocTemplate )
		DOC_TEMP_HELP_PTR2ID( VisualElementInfoViewDocTemplate )
		DOC_TEMP_HELP_PTR2ID( PopupVisualElementInfoViewDocTemplate )
		DOC_TEMP_HELP_PTR2ID( LayerStripViewDocTemplate )
		DOC_TEMP_HELP_PTR2ID( FrameStripViewDocTemplate )
		DOC_TEMP_HELP_PTR2ID( BrushViewDocTemplate )
	DOC_TEMP_HELP_END_PTR2ID

	// ----------

	DOC_TEMP_HELP_BEGIN_ID2PTR
		DOC_TEMP_HELP_ID2PTR( DocTemplate )
		DOC_TEMP_HELP_ID2PTR( PopupEditDocTemplate )
		DOC_TEMP_HELP_ID2PTR( MDIChildEditDocTemplate )
		DOC_TEMP_HELP_ID2PTR( MDISimplePaletteDocTemplate )
		DOC_TEMP_HELP_ID2PTR( PopupSimplePaletteDocTemplate )
		DOC_TEMP_HELP_ID2PTR( MDIChildGridDocTemplate )
		DOC_TEMP_HELP_ID2PTR( PopupGridDocTemplate )
		DOC_TEMP_HELP_ID2PTR( MDIChildMultiView1DocTemplate )
		DOC_TEMP_HELP_ID2PTR( MDIChildMultiView2DocTemplate )
		DOC_TEMP_HELP_ID2PTR( WindowClipDocTemplate )
		DOC_TEMP_HELP_ID2PTR( AnimationSpotsDocTemplate )
		DOC_TEMP_HELP_ID2PTR( ShowcaseAnimsDocTemplate )
		DOC_TEMP_HELP_ID2PTR( PopupAnimSpotsDocTemplate )
		DOC_TEMP_HELP_ID2PTR( VisualElementInfoViewDocTemplate )
		DOC_TEMP_HELP_ID2PTR( PopupVisualElementInfoViewDocTemplate )
		DOC_TEMP_HELP_ID2PTR( LayerStripViewDocTemplate )
		DOC_TEMP_HELP_ID2PTR( FrameStripViewDocTemplate )
		DOC_TEMP_HELP_ID2PTR( BrushViewDocTemplate )
	DOC_TEMP_HELP_END_ID2PTR

private:

	HMENU m_hMenuShared;
	HACCEL m_hAccelTable;

#if 1 // BPT 10/3/02

	std::list< CToolBar * > m_DestroyToolbarsCollection;

#endif

private:

	void SharedResourceAddDocTemplate( CMultiDocTemplate * pTemplate );

public:

	void StoreFrameWindowSettings( CFrameWnd * pWnd, const char * name );
	void RestoreFrameWindowSettings( CFrameWnd * pWnd, const char * name );
	bool PrepareForNewDocument( CDocument * pExcludeDocmentFromClose = 0 );

	CRecentFileList * GetRecentFileList() {

		return m_pRecentFileList;

	}

#if 1 // BPT 10/3/02

	void AddToDestroyCToolbarList( CToolBar * pToolbar ) {

		m_DestroyToolbarsCollection.push_back( pToolbar );

	}

#endif

public:

	CBpaintApp();
	~CBpaintApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBpaintApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CBpaintApp)
	afx_msg void OnAppAbout();
	afx_msg void OnTpbXmlImport();
	//}}AFX_MSG
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// Global functions
// ----------------------------------------------------------------------------

int GLOBAL_GetSettingInt( const char * pEntry, const int defaultValue = 0, const char * pSection = 0 );
void GLOBAL_PutSettingInt( const char * pEntry, const int value = 0, const char * pSection = 0 );
int GLOBAL_GetSettingIntHelper( const char * pBase, const char * pPostfix, const int value );
void GLOBAL_PutSettingIntHelper( const char * pBase, const char * pPostfix, const int value );

CFrameWnd *
GLOBAL_CreateNewWindow(
	CDocTemplate * pTemplate, CDocument * pDocument, const char * pSettingsName = 0
);

#endif // !defined(AFX_BPAINT_H__388E8CBE_0339_42F3_9465_49BA3823429C__INCLUDED_)
