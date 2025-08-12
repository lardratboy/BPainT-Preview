// PopFrameWnd.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited

#include "stdafx.h"
#include "bpaint.h"
#include "MainFrm.h"
#include "PopFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPopFrameWnd

#if defined(POPUP_USE_MINI_FRAME)
IMPLEMENT_DYNCREATE(CPopFrameWnd, CMiniFrameWnd)
#else
IMPLEMENT_DYNCREATE(CPopFrameWnd, CFrameWnd)
#endif



CPopFrameWnd::CPopFrameWnd(
	const int tbResID /* = 0 */
) : m_pMainFrame( 0 ), m_bShyToolbar( true ), m_ToolbarResID( tbResID ), m_pDocTemplate( 0 )
{
}

CPopFrameWnd::~CPopFrameWnd()
{
}

#if defined(POPUP_USE_MINI_FRAME)
BEGIN_MESSAGE_MAP(CPopFrameWnd, CMiniFrameWnd)
#else
BEGIN_MESSAGE_MAP(CPopFrameWnd, CFrameWnd)
#endif
	//{{AFX_MSG_MAP(CPopFrameWnd)
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_VFRAME_SHOW_ZOOM_TOOLBAR, OnVframeShowZoomToolbar)
	ON_UPDATE_COMMAND_UI(ID_VFRAME_SHOW_ZOOM_TOOLBAR, OnUpdateVframeShowZoomToolbar)
	ON_COMMAND(ID_VFRAME_SHY_ZOOM_TOOLBAR, OnVframeShyZoomToolbar)
	ON_UPDATE_COMMAND_UI(ID_VFRAME_SHY_ZOOM_TOOLBAR, OnUpdateVframeShyZoomToolbar)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPopFrameWnd message handlers

BOOL CPopFrameWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	// Set this window up as a simple overlapped window

	cs.style &= ~FWS_ADDTOTITLE;

	cs.style = WS_OVERLAPPEDWINDOW;

	cs.dwExStyle = 0;

	// Make the app a popup for the main frame (if one)

	CWnd * pWnd = AfxGetApp()->m_pMainWnd;

	if ( pWnd ) {

		cs.hwndParent = pWnd->GetSafeHwnd();

		// This should probably tell the main frame
		// about ownership so it can disable this on the WM_ACTIVATE...

		if ( pWnd->IsKindOf( RUNTIME_CLASS(CMainFrame) ) ) {

			m_pMainFrame = (CMainFrame *)pWnd;

			m_pMainFrame->AddPopupFrame( this );

		}

	}

	return base_class::PreCreateWindow(cs);
}

void CPopFrameWnd::OnDestroy() 
{
	base_class::OnDestroy();

	if ( m_pMainFrame ) {

		m_pMainFrame->RemovePopupFrame( this );

	}
	
}

void CPopFrameWnd::ActivateFrame(int nCmdShow) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	base_class::ActivateFrame(nCmdShow);
}

void CPopFrameWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	base_class::OnActivate(nState, pWndOther, bMinimized);
	
	// handle shy toolbars

	if ( m_bShyToolbar && m_ToolbarResID ) {

		HandleShyToolbar( &m_ToolBar, nState );

	}

}

// ----------------------------------------------------------------------------

void
CPopFrameWnd::OnToolbarX( CToolBar * pToolbar )
{
	ShowControlBar( pToolbar, !pToolbar->IsWindowVisible(), FALSE );
	RecalcLayout();
}

void
CPopFrameWnd::OnUpdateToolbarX( CToolBar * pToolbar, CCmdUI* pCmdUI )
{
	if ( pToolbar->GetSafeHwnd() ) {

		pCmdUI->SetCheck( pToolbar->IsWindowVisible() ? 1 : 0 );
		pCmdUI->Enable( TRUE );

	} else {

		pCmdUI->Enable( FALSE );

	}
}

void
CPopFrameWnd::HandleShyToolbar( CToolBar * pToolbar, const int nActivateState )
{
	if ( WA_INACTIVE == nActivateState ) {

		if ( pToolbar->IsWindowVisible() ) {

			OnToolbarX( pToolbar );

		}

	} else {

		if ( !pToolbar->IsWindowVisible() ) {

			OnToolbarX( pToolbar );

		}

	}

}

// ----------------------------------------------------------------------------

void CPopFrameWnd::OnVframeShowZoomToolbar() 
{
	OnToolbarX( &m_ToolBar );
}

void CPopFrameWnd::OnUpdateVframeShowZoomToolbar(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_ToolBar, pCmdUI );
}

// ----------------------------------------------------------------------------

void CPopFrameWnd::OnVframeShyZoomToolbar() 
{
	m_bShyToolbar = !m_bShyToolbar;
}

void CPopFrameWnd::OnUpdateVframeShyZoomToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bShyToolbar ? 1 : 0 );

	pCmdUI->Enable( TRUE );
}


// ----------------------------------------------------------------------------

int CPopFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (base_class::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( m_ToolbarResID ) {
	
		// mdi child docking zoom toolbar
		// --------------------------------------------------------------------
	
		if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
			| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
			!m_ToolBar.LoadToolBar(m_ToolbarResID))
		{
			TRACE0("Failed to create toolbar\n");
			return -1;      // fail to create
		}
	
		// --------------------------------------------------------------------
	
		int dockSettings = CBRS_ALIGN_ANY;
	
		EnableDocking(dockSettings);
	
		m_ToolBar.EnableDocking(dockSettings);
	
		DockControlBar(&m_ToolBar,AFX_IDW_DOCKBAR_RIGHT);

	}

	return 0;
}

BOOL CPopFrameWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	
	if ( !IsWindowEnabled() ) {

		return GLOBAL_StencilHackOnSetCursor( pWnd, nHitTest, message );

	}

	return CMiniFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CPopFrameWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if ( pContext ) {

		m_pDocTemplate = pContext->m_pNewDocTemplate;

	}

	return base_class::OnCreateClient(lpcs, pContext);
}

// ----------------------------------------------------------------------------

bool CPopFrameWnd::StoreInternalSettings( const char * sectionName, const char * baseEntryName )
{
	return true;
}

bool CPopFrameWnd::RestoreInternalSettings( const char * sectionName, const char * baseEntryName )
{
	return true;
}

// ----------------------------------------------------------------------------

