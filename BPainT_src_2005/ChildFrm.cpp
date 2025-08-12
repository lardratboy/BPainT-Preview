// ChildFrm.cpp : implementation of the CChildFrame class
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VFRAME_SHOW_ZOOM_TOOLBAR, OnVframeShowZoomToolbar)
	ON_UPDATE_COMMAND_UI(ID_VFRAME_SHOW_ZOOM_TOOLBAR, OnUpdateVframeShowZoomToolbar)
	ON_COMMAND(ID_VFRAME_SHY_ZOOM_TOOLBAR, OnVframeShyZoomToolbar)
	ON_UPDATE_COMMAND_UI(ID_VFRAME_SHY_ZOOM_TOOLBAR, OnUpdateVframeShyZoomToolbar)
	ON_WM_MDIACTIVATE()
	ON_COMMAND(ID_VIEW_EXPAND, OnViewExpand)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame() : 
	m_bFirstTime( true ), m_bShyZoomToolbar( false ), m_pDocTemplate( 0 )
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style &= ~FWS_ADDTOTITLE;

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::ActivateFrame(int nCmdShow) 
{
	if ( m_bFirstTime ) {

		m_bFirstTime = false;

#if 0 // this should be read from a 'behaviours' setting

		nCmdShow = SW_SHOWMAXIMIZED;

#endif

	}

	// TODO: Add your specialized code here and/or call the base class
	
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// setup toolbar init related values
	// ------------------------------------------------------------------------

	DWORD dwTBFlags = TBSTYLE_FLAT;

	DWORD dwTBStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;

	CRect tbRect( 0, 0, 0, 0 );

	// mdi child docking zoom toolbar
	// ------------------------------------------------------------------------

	if (!m_zoomSettingsToolBar.CreateEx(this, dwTBFlags, dwTBStyle, tbRect, IDR_ZOOM_SETTINGS ) ||
		!m_zoomSettingsToolBar.LoadToolBar(IDR_ZOOM_SETTINGS))
	{
		TRACE0("Failed to create zoom settings toolbar\n");
		return -1;      // fail to create
	}

	// ------------------------------------------------------------------------

	int dockSettings = CBRS_ALIGN_ANY;

	EnableDocking(dockSettings);

	m_zoomSettingsToolBar.EnableDocking(dockSettings);

	DockControlBar(&m_zoomSettingsToolBar,AFX_IDW_DOCKBAR_RIGHT);

	ShowControlBar( &m_zoomSettingsToolBar, FALSE, FALSE );

	return 0;
}

// ----------------------------------------------------------------------------

void
CChildFrame::OnToolbarX( CToolBar * pToolbar )
{
	ShowControlBar( pToolbar, !pToolbar->IsWindowVisible(), FALSE );
	RecalcLayout();
}

void
CChildFrame::OnUpdateToolbarX( CToolBar * pToolbar, CCmdUI* pCmdUI )
{
	if ( pToolbar->GetSafeHwnd() ) {

		pCmdUI->SetCheck( pToolbar->IsWindowVisible() ? 1 : 0 );
		pCmdUI->Enable( TRUE );

	} else {

		pCmdUI->Enable( FALSE );

	}
}

void
CChildFrame::HandleShyToolbar( CToolBar * pToolbar, const int nActivateState )
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

void CChildFrame::OnVframeShowZoomToolbar() 
{
	OnToolbarX( &m_zoomSettingsToolBar );
}

void CChildFrame::OnUpdateVframeShowZoomToolbar(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_zoomSettingsToolBar, pCmdUI );
}

// ----------------------------------------------------------------------------

void CChildFrame::OnVframeShyZoomToolbar() 
{
	m_bShyZoomToolbar = !m_bShyZoomToolbar;
}

void CChildFrame::OnUpdateVframeShyZoomToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bShyZoomToolbar ? 1 : 0 );

	pCmdUI->Enable( TRUE );
}

// ----------------------------------------------------------------------------

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	
	// handle shy toolbars

	if ( m_bShyZoomToolbar ) {

		HandleShyToolbar(
			&m_zoomSettingsToolBar, (bActivate) ? WA_ACTIVE : WA_INACTIVE
		);

	}
	
}

void CChildFrame::OnViewExpand() 
{
	DWORD style = GetStyle();

	if ( WS_MAXIMIZEBOX & style ) {

		if ( IsZoomed() ) {

			PostMessage( WM_SYSCOMMAND, SC_RESTORE );

		} else {

			PostMessage( WM_SYSCOMMAND, SC_MAXIMIZE );

		}

	}

}

BOOL CChildFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class

#if 1 

	nIDResource = IDR_BPAINTTYPE;

#endif
	
	return CMDIChildWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
}

BOOL CChildFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	
	if ( !IsWindowEnabled() ) {

		return GLOBAL_StencilHackOnSetCursor( pWnd, nHitTest, message );

	}

	return CMDIChildWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class

	if ( pContext ) {

		m_pDocTemplate = pContext->m_pNewDocTemplate;

	}
	
	return CMDIChildWnd::OnCreateClient(lpcs, pContext);
}

// ----------------------------------------------------------------------------

bool CChildFrame::StoreInternalSettings( const char * sectionName, const char * baseEntryName )
{
	// ------------------------------------------------------------------------

	PutInternalSetting( sectionName, baseEntryName, "bShyZoomToolbar", m_bShyZoomToolbar ? 1 : 0 );

	// ------------------------------------------------------------------------

	bool bInitialZoomToolbarVisible = (TRUE == m_zoomSettingsToolBar.IsWindowVisible());

	PutInternalSetting( sectionName, baseEntryName, "bZoomToolbarVisible", bInitialZoomToolbarVisible ? 1 : 0 );

	// ------------------------------------------------------------------------

	return true;
}

bool CChildFrame::RestoreInternalSettings( const char * sectionName, const char * baseEntryName )
{
	// ------------------------------------------------------------------------

	m_bShyZoomToolbar = (1 == GetInternalSetting( sectionName, baseEntryName, "bShyZoomToolbar", m_bShyZoomToolbar ? 1 : 0 ));

	// ------------------------------------------------------------------------

	bool bInitialZoomToolbarVisible = (TRUE == m_zoomSettingsToolBar.IsWindowVisible());

	bool bZoomToolbarVisible = (1 == GetInternalSetting( sectionName, baseEntryName, "bZoomToolbarVisible", bInitialZoomToolbarVisible ? 1 : 0 ));

	if ( bZoomToolbarVisible != bInitialZoomToolbarVisible ) {

		ShowControlBar( &m_zoomSettingsToolBar, !bZoomToolbarVisible, FALSE );

	}

	// ------------------------------------------------------------------------

	RecalcLayout();
	
	return true;
}

// ----------------------------------------------------------------------------

