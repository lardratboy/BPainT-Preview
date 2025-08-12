// bpaint.cpp : Defines the class behaviors for the application.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited

#include "stdafx.h"
#include "bpaint.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "bpaintDoc.h"
#include "bpaintView.h"
#include "PopFrameWnd.h"
#include "SimplePaletteView.h"
#include "GenericMDIChildFrame.h"
#include "AnimGridView.h"
#include "SimpleSplitFrame.h"
#include "WindowClipFrmWnd.h"
#include "NullView.h"
#include "AnimationSpotView.h"
#include "ShowcaseAnimationsView.h"
#include "VisualElementInfoView.h"
#include "LayersStripView.h"
#include "FramesStripView.h"
#include "BrushView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CBpaintApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CBpaintApp

BEGIN_MESSAGE_MAP(CBpaintApp, CWinApp)
	//{{AFX_MSG_MAP(CBpaintApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_TPB_XML_IMPORT, OnTpbXmlImport)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBpaintApp construction

CBpaintApp::CBpaintApp()
{

	DOC_TEMP_HELP_BEGIN_CONSTRUCT
		DOC_TEMP_HELP_CONSTRUCT( DocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( PopupEditDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( MDIChildEditDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( MDISimplePaletteDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( PopupSimplePaletteDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( MDIChildGridDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( PopupGridDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( MDIChildMultiView1DocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( MDIChildMultiView2DocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( WindowClipDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( AnimationSpotsDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( ShowcaseAnimsDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( PopupAnimSpotsDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( VisualElementInfoViewDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( PopupVisualElementInfoViewDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( LayerStripViewDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( FrameStripViewDocTemplate )
		DOC_TEMP_HELP_CONSTRUCT( BrushViewDocTemplate )
	DOC_TEMP_HELP_END_CONSTRUCT

	m_hMenuShared = 0;
	m_hAccelTable = 0;

}

CBpaintApp::~CBpaintApp()
{

#if 1 // BPT 10/3/02

	// destroy any toolbars
	// ------------------------------------------------------------------------

#ifdef _DEBUG

	TRACE( "Bpaint is deleting toolbars list elements\n" );

#endif

	while ( !m_DestroyToolbarsCollection.empty() ) {

		delete m_DestroyToolbarsCollection.back();

		m_DestroyToolbarsCollection.pop_back();

	}

	// clean up the clipboard animation
	// ------------------------------------------------------------------------

	if ( CBpaintDoc::s_clipboardShowcase ) {

#ifdef _DEBUG

		TRACE( "BPaint is exiting deleting the clipboard animation!\n" );

#endif

		delete CBpaintDoc::s_clipboardShowcase;

		CBpaintDoc::s_clipboardShowcase = 0;

	}

#endif

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBpaintApp object

CBpaintApp theApp;

/////////////////////////////////////////////////////////////////////////////

//
//	CBpaintApp::SharedResourceAddDocTemplate()
//

void CBpaintApp::SharedResourceAddDocTemplate( CMultiDocTemplate * pTemplate )
{

	// Replace the menu with the shared one.
	// ------------------------------------------------------------------------

	if ( m_hMenuShared ) {

		HMENU hOldMenu = pTemplate->m_hMenuShared;

		pTemplate->m_hMenuShared = m_hMenuShared;

		DestroyMenu( hOldMenu );

	}

	// Replace the 'accelerator' with the shared one
	// ------------------------------------------------------------------------

	if ( m_hAccelTable ) {

		HACCEL hOldAccelTable = pTemplate->m_hAccelTable;

		pTemplate->m_hAccelTable = m_hAccelTable;

		DestroyAcceleratorTable( hOldAccelTable );

	}

	// ------------------------------------------------------------------------

	AddDocTemplate( pTemplate );

}

/////////////////////////////////////////////////////////////////////////////
// CBpaintApp initialization

BOOL CBpaintApp::InitInstance()
{

#if 1 // BPT 6/9/03 -- added seemingly missing settings

	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit()) {

		AfxMessageBox( "AfxOleInit() failed?", MB_OK | MB_ICONERROR);
		return FALSE;

	}

#endif

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if 0 // depreciated API
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("www.thinkbpt.com"));

	LoadStdProfileSettings(12);  // Load standard INI file options (including MRU)

	// ========================================================================

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_pDocTemplate = new CMultiDocTemplate(
		IDR_BPAINTTYPE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CBpaintView));
	AddDocTemplate( m_pDocTemplate );

	// ========================================================================

	m_hMenuShared = m_pDocTemplate->m_hMenuShared;
	m_hAccelTable = m_pDocTemplate->m_hAccelTable;

	// ========================================================================

	// Create the popup paint view document template.

	m_pPopupEditDocTemplate = new CMultiDocTemplate(
		IDR_POPUP_FRAME,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CPopFrameWnd), // popup frame
		RUNTIME_CLASS(CBpaintView));
	SharedResourceAddDocTemplate( m_pPopupEditDocTemplate );

	// Create the mdi child view document template.

	m_pMDIChildEditDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CChildFrame),  // this is the edit child frame
		RUNTIME_CLASS(CBpaintView));
	SharedResourceAddDocTemplate( m_pMDIChildEditDocTemplate );

	// Create the mdi child palette view document template.

	m_pMDISimplePaletteDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CGenericMDIChildFrame), // generic mdi child frame
		RUNTIME_CLASS(CSimplePaletteView));
	SharedResourceAddDocTemplate( m_pMDISimplePaletteDocTemplate );

	// Create the popup child palette view document template.

	m_pPopupSimplePaletteDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CPopFrameWnd), // generic mdi child frame
		RUNTIME_CLASS(CSimplePaletteView));
	SharedResourceAddDocTemplate( m_pPopupSimplePaletteDocTemplate );

	// Create the mdi child grid view document template.

	m_pMDIChildGridDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CGenericMDIChildFrame),  // generic mdi child frame
		RUNTIME_CLASS(CAnimGridView));
	SharedResourceAddDocTemplate( m_pMDIChildGridDocTemplate );

	// popup grid window.

	m_pPopupGridDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CPopFrameWnd), // popup frame
		RUNTIME_CLASS(CAnimGridView));
	SharedResourceAddDocTemplate( m_pPopupGridDocTemplate );
	
	// Create the mdi multi view document template 1.

	m_pMDIChildMultiView1DocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CSimpleSplitFrame),  // splitter frame
		NULL);
	SharedResourceAddDocTemplate( m_pMDIChildMultiView1DocTemplate );

	// Create the mdi multi view document template 2.

	m_pMDIChildMultiView2DocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CSimpleSplitFrame),  // splitter frame
		NULL);
	SharedResourceAddDocTemplate( m_pMDIChildMultiView2DocTemplate );

	// Create the window clip document template.

	m_pWindowClipDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CWindowClipFrmWnd),  // window clip
		RUNTIME_CLASS(CNullView));
	SharedResourceAddDocTemplate( m_pWindowClipDocTemplate );

	// ------------

	// Create the mdi child animation spot view document template.

	m_pAnimationSpotsDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CGenericMDIChildFrame),  // generic mdi child frame
		RUNTIME_CLASS(CAnimationSpotView));
	SharedResourceAddDocTemplate( m_pAnimationSpotsDocTemplate );

	// Create the mdi child showcase animations view document template.

	m_pShowcaseAnimsDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CGenericMDIChildFrame),  // generic mdi child frame
		RUNTIME_CLASS(CShowcaseAnimationsView));
	SharedResourceAddDocTemplate( m_pShowcaseAnimsDocTemplate );

	// -----------------------------

	m_pPopupAnimSpotsDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CPopFrameWnd), // popup frame
		RUNTIME_CLASS(CAnimationSpotView));
	SharedResourceAddDocTemplate( m_pPopupAnimSpotsDocTemplate );

	// Create mdi child visual element info view

	m_pVisualElementInfoViewDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CGenericMDIChildFrame),  // generic mdi child frame
		RUNTIME_CLASS(CVisualElementInfoView));
	SharedResourceAddDocTemplate( m_pVisualElementInfoViewDocTemplate );

	// Create popup visual element info view

	m_pPopupVisualElementInfoViewDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CPopFrameWnd), // popup frame
		RUNTIME_CLASS(CVisualElementInfoView));
	SharedResourceAddDocTemplate( m_pPopupVisualElementInfoViewDocTemplate );

	// create mdi child for the layer strip view

	m_pLayerStripViewDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CGenericMDIChildFrame),  // generic mdi child frame
		RUNTIME_CLASS(CLayersStripView));
	SharedResourceAddDocTemplate( m_pLayerStripViewDocTemplate );

	// frame strip mdi child

	m_pFrameStripViewDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CGenericMDIChildFrame),  // generic mdi child frame
		RUNTIME_CLASS(CFramesStripView));
	SharedResourceAddDocTemplate( m_pFrameStripViewDocTemplate );

	// Brush view mdi child

	m_pBrushViewDocTemplate = new CMultiDocTemplate(
		IDR_SECONDARY_DOC_TEMPLATE,
		RUNTIME_CLASS(CBpaintDoc),
		RUNTIME_CLASS(CGenericMDIChildFrame),  // generic mdi child frame
		RUNTIME_CLASS(CBrushView));
	SharedResourceAddDocTemplate( m_pBrushViewDocTemplate );

	// ========================================================================

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// set the frame's internal name

	pMainFrame->SetInternalName( "MainFrameWnd" );

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(); // TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;

// turn off initial empty document 
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	ParseCommandLine( cmdInfo );

	if ( (CCommandLineInfo::FileNothing == cmdInfo.m_nShellCommand) 
		&& (!cmdInfo.m_strFileName.IsEmpty()) ) {

		cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;

		if ( !OpenDocumentFile( cmdInfo.m_strFileName ) ) {

			return FALSE;

		}

	} else {

		// Dispatch commands specified on the command line
		if (!ProcessShellCommand( cmdInfo ))
			return FALSE;

	}

	// restore the frame settings, this is here so that the position of the
	// window is determined before the window is shown.  Otherwise it just
	// looked odd that the window showed up in some random default position
	// then after the initial document is created then it snaps to it's
	// stored position.  

#if 1

	RestoreFrameWindowSettings( pMainFrame, pMainFrame->m_InternalName );

#endif

	m_nCmdShow = SW_SHOW;

	// The main window has been initialized, so show and update it.

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	// Now that the window is active create a new document
	// NOTE: Probably should allow for an option to load last document?

	if ( CCommandLineInfo::FileNothing == cmdInfo.m_nShellCommand ) {

		pMainFrame->SendMessage( WM_COMMAND, ID_FILE_NEW );

	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CBpaintApp message handlers

/////////////////////////////////////////////////////////////////////////////

void
CBpaintApp::StoreFrameWindowSettings( CFrameWnd * pWnd, const char * name )
{
	// Retrieve the current settings
	// ------------------------------------------------------------------------

	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	pWnd->GetWindowPlacement( &wp );

	// Store the settings under the 'Settings name' section
	// ------------------------------------------------------------------------

	CString sectionString;
	sectionString.Format( "Settings %s", name );

	// Store off the useful parts of the WINDOWPLACEMENT structure.
	// ------------------------------------------------------------------------

	WriteProfileInt( sectionString, _T("flags"), wp.flags );
	WriteProfileInt( sectionString, _T("showCmd"), wp.showCmd );
	WriteProfileInt( sectionString, _T("left"), wp.rcNormalPosition.left );
	WriteProfileInt( sectionString, _T("top"), wp.rcNormalPosition.top );
	WriteProfileInt( sectionString, _T("right"), wp.rcNormalPosition.right );
	WriteProfileInt( sectionString, _T("bottom"), wp.rcNormalPosition.bottom );

	// Ask to save off the toolbar settings for this frame.
	// ------------------------------------------------------------------------

	pWnd->SaveBarState( sectionString );

}

void 
CBpaintApp::RestoreFrameWindowSettings( CFrameWnd * pWnd, const char * name )
{
	// Retrieve the current settings
	// ------------------------------------------------------------------------

	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	pWnd->GetWindowPlacement( &wp );

	// Read the settings from the 'Settings name' section
	// ------------------------------------------------------------------------

	CString sectionString;
	sectionString.Format( "Settings %s", name );

	// Read the values using the current settings as the default.
	// ------------------------------------------------------------------------

	wp.flags = GetProfileInt( sectionString, _T("flags"), wp.flags );
	wp.showCmd = GetProfileInt( sectionString, _T("showCmd"), wp.showCmd );
	wp.rcNormalPosition.left = GetProfileInt( sectionString, _T("left"), wp.rcNormalPosition.left );
	wp.rcNormalPosition.top = GetProfileInt( sectionString, _T("top"), wp.rcNormalPosition.top );
	wp.rcNormalPosition.right = GetProfileInt( sectionString, _T("right"), wp.rcNormalPosition.right );
	wp.rcNormalPosition.bottom = GetProfileInt( sectionString, _T("bottom"), wp.rcNormalPosition.bottom );

	// Update the window placement.
	// ------------------------------------------------------------------------

	pWnd->SetWindowPlacement( &wp );

	// Ask to save off the toolbar settings for this frame.
	// ------------------------------------------------------------------------

	pWnd->LoadBarState( sectionString );

}

int CBpaintApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinApp::ExitInstance();
}

// ----------------------------------------------------------------------------

bool CBpaintApp::PrepareForNewDocument( CDocument * pExcludeDocmentFromClose ) {

	if ( pExcludeDocmentFromClose ) {

		typedef std::list< CDocument * > close_collection_type;

		close_collection_type closeDocumentsList;

		// Build the list of documents to close
		// -------------------------------------------------------------------

		POSITION pos = GetFirstDocTemplatePosition();

		while ( NULL != pos ) {

			CDocTemplate * pDocTemplate = GetNextDocTemplate( pos );

			if ( pDocTemplate ) {

				POSITION docPos = pDocTemplate->GetFirstDocPosition();

				while ( NULL != docPos ) {

					CDocument * pDoc = pDocTemplate->GetNextDoc( docPos );

					if ( pDoc && (pDoc != pExcludeDocmentFromClose) ) {

						closeDocumentsList.push_back( pDoc );

					}

				}

			}
			
		}

		// Okay now iterate through the documents to close collection
		// ----------------------------------------------------------------

		close_collection_type::iterator it;

		for ( it = closeDocumentsList.begin(); it != closeDocumentsList.end(); it++ ) {

			if ( (*it)->SaveModified() ) {

				(*it)->OnCloseDocument();

			} else {

				return false;

			}

		}

	} else {

		if ( SaveAllModified() ) {

			TRACE( "Closing all documents()\n" );

			CloseAllDocuments( FALSE );

		} else {

			TRACE( "Save all modifed returned FALSE\n" );

		}

	}

	return true;
}

// ----------------------------------------------------------------------------

void CBpaintApp::OnFileNew()
{
	CWinApp::OnFileNew();
}

void CBpaintApp::OnFileOpen()
{
	PrepareForNewDocument( 0 );
	CWinApp::OnFileOpen();
}

/////////////////////////////////////////////////////////////////////////////

//
//	GLOBAL_GetSettingInt()
//

int
GLOBAL_GetSettingInt( const char * pEntry, const int defaultValue, const char * pSection )
{
	// Get the section to use
	// ------------------------------------------------------------------------

	CString section;

	if ( pSection ) {

		section = pSection;

	} else {

		section = "General Settings";

	}

	CString entry = pEntry;

	// Use the global 
	// ------------------------------------------------------------------------

	CWinApp * pApp = AfxGetApp();

	if ( pApp ) {

		return pApp->GetProfileInt( section, entry, defaultValue );

	}

	return defaultValue;
}

//
//	GLOBAL_PutSettingInt()
//

void
GLOBAL_PutSettingInt( const char * pEntry, const int value, const char * pSection )
{
	// Get the section to use
	// ------------------------------------------------------------------------

	CString section;

	if ( pSection ) {

		section = pSection;

	} else {

		section = "General Settings";

	}

	CString entry = pEntry;

	// Use the global 
	// ------------------------------------------------------------------------

	CWinApp * pApp = AfxGetApp();

	if ( pApp ) {

		pApp->WriteProfileInt( section, entry, value );

	}
	
}

//
//	GLOBAL_GetSettingIntHelper()
//

int 
GLOBAL_GetSettingIntHelper( const char * pBase, const char * pPostfix, const int value )
{
	if ( pBase ) {

		CString entry;

		if ( pPostfix ) {

			entry.Format( "%s.%s", pBase, pPostfix );

		} else {

			entry = pBase;

		}

		return GLOBAL_GetSettingInt( entry, value );

	}

	return value;
}

//
//	GLOBAL_GetSettingIntHelper()
//

void 
GLOBAL_PutSettingIntHelper( const char * pBase, const char * pPostfix, const int value )
{
	if ( pBase ) {

		CString entry;

		if ( pPostfix ) {

			entry.Format( "%s.%s", pBase, pPostfix );

		} else {

			entry = pBase;

		}

		GLOBAL_PutSettingInt( entry, value );

	}
}

// ----------------------------------------------------------------------------

//
//	GLOBAL_CreateNewWindow()
//

CFrameWnd *
GLOBAL_CreateNewWindow(
	CDocTemplate * pTemplate, CDocument * pDocument, const char * pSettingsName
)
{
	ASSERT_VALID( pTemplate );
	ASSERT_VALID( pDocument );

	CFrameWnd * pFrame = pTemplate->CreateNewFrame( pDocument, NULL );

	if ( 0 == pFrame ) {

		TRACE0( "Warning: failed to create new frame.\n" );
		return 0;

	}

	ASSERT_KINDOF( CFrameWnd, pFrame );

	// Read the window positions

	if ( pSettingsName ) {

		CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );
		
		if ( pApp ) {

			pApp->RestoreFrameWindowSettings( pFrame, pSettingsName );

		}

	}

	// Okay ask the template to perform it's initialization of the frame window.

	pTemplate->InitialUpdateFrame( pFrame, pDocument );

	return pFrame;
}

// ----------------------------------------------------------------------------

//
//	GLOBAL_StencilHackOnSetCursor()
//

BOOL 
GLOBAL_StencilHackOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CBpaintView)) ) {

		CBpaintView * pView = (CBpaintView *)pWnd;

		CBpaintDoc * pDoc;

		if ( pDoc = pView->GetDocument() ) {

			if ( pDoc->m_pPickupColorHackDlg ) {

				pDoc->m_nPickupColorHackMsg = message;

				if ( pView->PUBLIC_OnSetCursor( pWnd, nHitTest, message ) ) {
	
					return TRUE;
	
				}

			}

		}

	}

	// Find the damn view if possible as quickly 
	
	if ( pWnd ) {
	
		CPoint pt;
	
		GetCursorPos( &pt );
	
		CWnd * pFound = pWnd;
	
		for ( int i = 0; 0 != pFound; i++ ) {
	
			if ( pFound->IsKindOf(RUNTIME_CLASS(CBpaintView)) ) {

				CBpaintView * pView = (CBpaintView *)pFound;

				CBpaintDoc * pDoc;
		
				if ( pDoc = pView->GetDocument() ) {
		
					if ( pDoc->m_pPickupColorHackDlg ) {
		
						pDoc->m_nPickupColorHackMsg = message;

						if ( pView->PUBLIC_OnSetCursor( pWnd, HTCLIENT, message ) ) {
			
							return TRUE;
			
						}
		
					}
		
				}

				break;

			}

			CPoint clientPt = pt;

			pFound->ScreenToClient( &clientPt );
	
			pFound = pFound->ChildWindowFromPoint( 
				clientPt, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT
			);
	
			if ( 10 < i ) {
	
				break;
	
			}
	
		}

	}

	if ( (WM_LBUTTONDOWN == message) || (WM_RBUTTONDOWN == message) ) {

		MessageBeep( MB_ICONQUESTION  );

	}

	SetCursor( LoadCursor( NULL, IDC_NO ) );

	return TRUE;
}

//
//	CBpaintApp::OnTpbXmlImport()
//

void CBpaintApp::OnTpbXmlImport() 
{
	PrepareForNewDocument( 0 );

	// ------------------------------------------------------------------------

	CFileDialog opf( 
		TRUE, _T("*.tpb"), NULL, OFN_FILEMUSTEXIST | OFN_LONGNAMES, 
		_T("TPB Files (*.tpb)|*.tpb||"),
		AfxGetMainWnd()
	);

	if ( IDOK != opf.DoModal() ) {

		return;

	}

	// ------------------------------------------------------------------------

	CDocument * pDoc = OpenDocumentFile( opf.GetFileName() );

	if ( pDoc ) {

		// Get the loaded filename and change the extension
		// --------------------------------------------------------------------

		char newFilename[_MAX_PATH];
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath( opf.GetFileName(), drive, dir, fname, ext );

		_makepath( newFilename, drive, dir, fname, ".bpt" );

		// Set the name and mark the file as modified.
		// --------------------------------------------------------------------

		pDoc->SetModifiedFlag( TRUE );
		pDoc->SetPathName( newFilename );
		pDoc->SetTitle( newFilename );

	} else {

		OnFileNew();

	}

}
