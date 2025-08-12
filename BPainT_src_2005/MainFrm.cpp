// MainFrm.cpp : implementation of the CMainFrame class
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"

#include "PopFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_DRAWING_TOOLS, OnViewDrawingTools)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRAWING_TOOLS, OnUpdateViewDrawingTools)
	ON_COMMAND(ID_VIEW_QUICK_BRUSHES, OnViewQuickBrushes)
	ON_UPDATE_COMMAND_UI(ID_VIEW_QUICK_BRUSHES, OnUpdateViewQuickBrushes)
	ON_COMMAND(ID_VIEW_VIEW_SETTINGS, OnViewViewSettings)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VIEW_SETTINGS, OnUpdateViewViewSettings)
	ON_COMMAND(ID_VIEW_ZOOM_SETTINGS, OnViewZoomSettings)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_SETTINGS, OnUpdateViewZoomSettings)
	ON_WM_DESTROY()
	ON_COMMAND(ID_NEW_POPUP_VIEW, OnNewPopupView)
	ON_UPDATE_COMMAND_UI(ID_NEW_POPUP_VIEW, OnUpdateNewPopupView)
	ON_COMMAND(ID_VIEW_BRUSH_XFORM, OnViewBrushXform)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BRUSH_XFORM, OnUpdateViewBrushXform)
	ON_COMMAND(ID_VIEW_PLAYBACK_TOOLBAR, OnViewPlaybackToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PLAYBACK_TOOLBAR, OnUpdateViewPlaybackToolbar)
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_LAYOUT_TOOLBAR, OnViewLayoutToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LAYOUT_TOOLBAR, OnUpdateViewLayoutToolbar)
	ON_COMMAND(ID_VIEW_COMPOSITE_TOOLBAR, OnViewCompositeToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMPOSITE_TOOLBAR, OnUpdateViewCompositeToolbar)
	ON_COMMAND(ID_REQUEST_PENDING_MODE, OnRequestPendingMode)
	ON_WM_TIMER()
	ON_COMMAND(ID_SHOW_SPECIAL_TOOLBAR, OnShowSpecialToolbar)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SPECIAL_TOOLBAR, OnUpdateShowSpecialToolbar)
	ON_COMMAND(ID_VIEW_DRAW_SPECIAL_TOOLBAR, OnViewDrawSpecialToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRAW_SPECIAL_TOOLBAR, OnUpdateViewDrawSpecialToolbar)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_ANIM_PLAY, OnAnimPlay)
	ON_COMMAND(ID_ANIM_STOP, OnAnimStop)
	ON_UPDATE_COMMAND_UI(ID_ANIM_STOP, OnUpdateAnimStop)
	ON_UPDATE_COMMAND_UI(ID_ANIM_PLAY, OnUpdateAnimPlay)
	ON_COMMAND(ID_VIEW_ROP_TOOLBAR, OnViewRopToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ROP_TOOLBAR, OnUpdateViewRopToolbar)
	ON_COMMAND(ID_REMEMBER_TOOLBAR_SIZES, OnRememberToolbarSizes)
	ON_UPDATE_COMMAND_UI(ID_REMEMBER_TOOLBAR_SIZES, OnUpdateRememberToolbarSizes)
	//}}AFX_MSG_MAP
	ON_WM_ENABLE()
#if defined(FANCY_WINDOWS_MANAGE_DIALOG)
	ON_COMMAND(ID_WINDOW_MANAGE,OnWindowManage)
#endif
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() : 
	m_bFirstTime( true ), 
	m_PendingTimerID( 0 ),
	m_AnimationTimerID( 0 )
{
	// TODO: add member initialization code here

	strcpy( m_InternalName, "MainFrameWnd" );

	m_nPlayAnimFrameDelay = GLOBAL_GetSettingInt( "AnimationPlaybackDelay", 100 );

	m_bRememberToolbarSizes = (1 == GLOBAL_GetSettingInt( "RemberToolbarSizes", 0 ));

}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

#if defined(FANCY_WINDOWS_MANAGE_DIALOG)
	VERIFY(m_MDIClient.SubclassWindow(m_hWndMDIClient));
#endif

	// setup toolbar init related values
	// ------------------------------------------------------------------------

	DWORD dwTBFlags = TBSTYLE_FLAT;

	DWORD dwTBStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;

	CRect tbRect( 0, 0, 0, 0 );

	// main toolbar
	// ------------------------------------------------------------------------

	if (!m_wndToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_MAINFRAME ) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.SetWindowText( "toolbar" );

	// ------------------------------------------------------------------------

	if (!m_drawingToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_DRAWING_TOOLS ) ||
		!m_drawingToolBar.LoadToolBar(IDR_DRAWING_TOOLS))
	{
		TRACE0("Failed to create drawing toolbar\n");
		return -1;      // fail to create
	}

	m_drawingToolBar.SetWindowText( "tools" );

	// ------------------------------------------------------------------------

	if (!m_quickBrushToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_QUICK_BRUSHES ) ||
		!m_quickBrushToolBar.LoadToolBar(IDR_QUICK_BRUSHES))
	{
		TRACE0("Failed to create quick brushes toolbar\n");
		return -1;      // fail to create
	}

	m_quickBrushToolBar.SetWindowText( "quick brush" );

	// ------------------------------------------------------------------------

	if (!m_viewSettingsToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_VIEW_SETTINGS ) ||
		!m_viewSettingsToolBar.LoadToolBar(IDR_VIEW_SETTINGS))
	{
		TRACE0("Failed to create view settings toolbar\n");
		return -1;      // fail to create
	}

	m_viewSettingsToolBar.SetWindowText( "view settings" );

	// ------------------------------------------------------------------------

	if (!m_zoomSettingsToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_ZOOM_SETTINGS ) ||
		!m_zoomSettingsToolBar.LoadToolBar(IDR_ZOOM_SETTINGS))
	{
		TRACE0("Failed to create zoom settings toolbar\n");
		return -1;      // fail to create
	}

	m_zoomSettingsToolBar.SetWindowText( "zoom" );

	// ------------------------------------------------------------------------

	if (!m_brushTransfromToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_DEFORMATION_TOOLBAR ) ||
		!m_brushTransfromToolBar.LoadToolBar(IDR_DEFORMATION_TOOLBAR))
	{
		TRACE0("Failed to create brush transform toolbar\n");
		return -1;      // fail to create
	}

	m_brushTransfromToolBar.SetWindowText( "transform" );

	// ------------------------------------------------------------------------

	if (!m_playbackToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_ANIMATION_TOOLBAR ) ||
		!m_playbackToolBar.LoadToolBar(IDR_ANIMATION_TOOLBAR))
	{
		TRACE0("Failed to create playback toolbar\n");
		return -1;      // fail to create
	}

	m_playbackToolBar.SetWindowText( "playback" );

	// ------------------------------------------------------------------------

	if (!m_layoutToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_UI_LAYOUT_TOOLBAR ) ||
		!m_layoutToolBar.LoadToolBar(IDR_UI_LAYOUT_TOOLBAR))
	{
		TRACE0("Failed to create ui layout toolbar\n");
		return -1;      // fail to create
	}

	m_layoutToolBar.SetWindowText( "layout" );

	// ------------------------------------------------------------------------

	if (!m_compositeToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_COMPOSITE_MODE_TOOLBAR ) ||
		!m_compositeToolBar.LoadToolBar(IDR_COMPOSITE_MODE_TOOLBAR))
	{
		TRACE0("Failed to create composite toolbar\n");
		return -1;      // fail to create
	}

	m_compositeToolBar.SetWindowText( "composite" );

	// ------------------------------------------------------------------------

	if (!m_specialToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_SPECIAL_TOOLBAR ) ||
		!m_specialToolBar.LoadToolBar(IDR_SPECIAL_TOOLBAR))
	{
		TRACE0("Failed to create special toolbar\n");
		return -1;      // fail to create
	}

	m_specialToolBar.SetWindowText( "special" );

	// ------------------------------------------------------------------------

	if (!m_drawSpecialToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_DRAW_SPECIAL ) ||
		!m_drawSpecialToolBar.LoadToolBar(IDR_DRAW_SPECIAL))
	{
		TRACE0("Failed to create draw special toolbar\n");
		return -1;      // fail to create
	}

	m_drawSpecialToolBar.SetWindowText( "More drawing" );

	// ------------------------------------------------------------------------

	if (!m_ropToolBar.CreateEx( this, dwTBFlags, dwTBStyle, tbRect, IDR_DRAW_ROPS ) ||
		!m_ropToolBar.LoadToolBar(IDR_DRAW_ROPS))
	{
		TRACE0("Failed to create draw rop's toolbar\n");
		return -1;      // fail to create
	}

	m_ropToolBar.SetWindowText( "Paint Modes" );

	// Status bar
	// ------------------------------------------------------------------------

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Setup docking info
	// ------------------------------------------------------------------------

	SyncToolbarRememberStates();

	int dockSettings = CBRS_ALIGN_ANY;

	m_viewSettingsToolBar.EnableDocking(dockSettings);
	m_zoomSettingsToolBar.EnableDocking(dockSettings);
	m_quickBrushToolBar.EnableDocking(dockSettings);
	m_drawingToolBar.EnableDocking(dockSettings);
	m_wndToolBar.EnableDocking(dockSettings);
	m_brushTransfromToolBar.EnableDocking(dockSettings);
	m_playbackToolBar.EnableDocking(dockSettings);
	m_layoutToolBar.EnableDocking(dockSettings);
	m_compositeToolBar.EnableDocking(dockSettings);
	m_specialToolBar.EnableDocking(dockSettings);
	m_drawSpecialToolBar.EnableDocking(dockSettings);
	m_ropToolBar.EnableDocking(dockSettings);

	EnableDocking(dockSettings);

	// doc at the top...

	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_viewSettingsToolBar);
	DockControlBar(&m_playbackToolBar);
	DockControlBar(&m_layoutToolBar);
	DockControlBar(&m_compositeToolBar);
	DockControlBar(&m_specialToolBar);
	DockControlBar(&m_drawSpecialToolBar);
	DockControlBar(&m_ropToolBar);

#if 1

	ShowControlBar( &m_wndToolBar, false, FALSE );
	ShowControlBar( &m_viewSettingsToolBar, false, FALSE );
	ShowControlBar( &m_layoutToolBar, false, FALSE );
	ShowControlBar( &m_compositeToolBar, false, FALSE );
	ShowControlBar( &m_specialToolBar, false, FALSE );
	ShowControlBar( &m_drawSpecialToolBar, false, FALSE );
	ShowControlBar( &m_ropToolBar, false, FALSE );
	
#endif

	// determine initial positions for

	DockControlBar(&m_drawingToolBar,AFX_IDW_DOCKBAR_RIGHT);

	DockControlBar(&m_zoomSettingsToolBar,AFX_IDW_DOCKBAR_LEFT);
	DockControlBarLeftOf( &m_brushTransfromToolBar, &m_zoomSettingsToolBar );
	DockControlBarLeftOf( &m_quickBrushToolBar, &m_brushTransfromToolBar );
	
	ShowControlBar( &m_zoomSettingsToolBar, false, FALSE );

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

// ----------------------------------------------------------------------------

void
CMainFrame::OnToolbarX( toolbar_type * pToolbar )
{
	ShowControlBar( pToolbar, !pToolbar->IsWindowVisible(), FALSE );
	RecalcLayout();
}

void
CMainFrame::OnUpdateToolbarX( toolbar_type * pToolbar, CCmdUI* pCmdUI )
{
	if ( pToolbar->GetSafeHwnd() ) {

		pCmdUI->SetCheck( pToolbar->IsWindowVisible() ? 1 : 0 );
		pCmdUI->Enable( TRUE );

	} else {

		pCmdUI->Enable( FALSE );

	}
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnViewDrawingTools() 
{
	OnToolbarX( &m_drawingToolBar );
}

void CMainFrame::OnUpdateViewDrawingTools(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_drawingToolBar, pCmdUI );
}

void CMainFrame::OnViewQuickBrushes() 
{
	OnToolbarX( &m_quickBrushToolBar );
}

void CMainFrame::OnUpdateViewQuickBrushes(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_quickBrushToolBar, pCmdUI );
}

void CMainFrame::OnViewViewSettings() 
{
	OnToolbarX( &m_viewSettingsToolBar );
}

void CMainFrame::OnUpdateViewViewSettings(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_viewSettingsToolBar, pCmdUI );
}

void CMainFrame::OnViewZoomSettings() 
{
	OnToolbarX( &m_zoomSettingsToolBar );
}

void CMainFrame::OnUpdateViewZoomSettings(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_zoomSettingsToolBar, pCmdUI );
}

void CMainFrame::OnViewBrushXform() 
{
	OnToolbarX( &m_brushTransfromToolBar );
}

void CMainFrame::OnUpdateViewBrushXform(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_brushTransfromToolBar, pCmdUI );
}


void CMainFrame::OnViewPlaybackToolbar() 
{
	OnToolbarX( &m_playbackToolBar );
}

void CMainFrame::OnUpdateViewPlaybackToolbar(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_playbackToolBar, pCmdUI );
}

void CMainFrame::OnViewLayoutToolbar() 
{
	OnToolbarX( &m_layoutToolBar );
}

void CMainFrame::OnUpdateViewLayoutToolbar(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_layoutToolBar, pCmdUI );
}

void CMainFrame::OnViewCompositeToolbar() 
{
	OnToolbarX( &m_compositeToolBar );
}

void CMainFrame::OnUpdateViewCompositeToolbar(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_compositeToolBar, pCmdUI );
}

void CMainFrame::OnShowSpecialToolbar() 
{
	OnToolbarX( &m_specialToolBar );
	
}

void CMainFrame::OnUpdateShowSpecialToolbar(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_specialToolBar, pCmdUI );
}

void CMainFrame::OnViewDrawSpecialToolbar() 
{
	OnToolbarX( &m_drawSpecialToolBar );
}

void CMainFrame::OnUpdateViewDrawSpecialToolbar(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_drawSpecialToolBar, pCmdUI );
}

void CMainFrame::OnViewRopToolbar() 
{
	OnToolbarX( &m_ropToolBar );
}

void CMainFrame::OnUpdateViewRopToolbar(CCmdUI* pCmdUI) 
{
	OnUpdateToolbarX( &m_ropToolBar, pCmdUI );
}
// ----------------------------------------------------------------------------

void CMainFrame::OnDestroy() 
{
	GLOBAL_PutSettingInt( "RemberToolbarSizes", m_bRememberToolbarSizes ? 1 : 0 );

	if ( m_AnimationTimerID ) {

		KillTimer( m_AnimationTimerID );

	}

	if ( m_PendingTimerID ) {

		KillTimer( m_PendingTimerID );

	}

	CMDIFrameWnd::OnDestroy();
}

// ============================================================================
// Popup frame related methods
// ============================================================================

void CMainFrame::OnNewPopupView() 
{
	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );
	
	if ( pApp ) {

		// --------------------------------------------------------------------

		CMDIChildWnd * pChild = MDIGetActive();

		if ( NULL == pChild ) {

			TRACE( "Unable to find active MDI child.\n" );

			return;

		}

		CDocument * pDocument = pChild->GetActiveDocument();

		if ( NULL == pDocument ) {

			TRACE( "Unable to get active document.\n" );

			return;

		}

		// --------------------------------------------------------------------

		CDocTemplate * pTemplate = pApp->GetPopupEditDocTemplate();

		if ( pTemplate ) {

			CFrameWnd * pFrame = pTemplate->CreateNewFrame( pDocument, NULL );
	
			if ( 0 == pFrame ) {
	
				TRACE( "Failed to create new popup frame.\n" );
	
				return;
	
			}

			// ----------------------------------------------------------------

			pTemplate->InitialUpdateFrame( pFrame, pDocument );

		}

	}
}

void CMainFrame::OnUpdateNewPopupView(CCmdUI* pCmdUI) 
{
	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );
	
	if ( pApp ) {

		pCmdUI->Enable( 0 != pApp->GetPopupEditDocTemplate() );

	} else {

		pCmdUI->Enable( FALSE );

	}
}

void CMainFrame::OnEnable( BOOL bEnable )
{
	EnablePopupFrames( bEnable );
}

void CMainFrame::AddPopupFrame( CFrameWnd * pFrame )
{
	m_PopupFrames.push_back( pFrame );
}

void CMainFrame::RemovePopupFrame( CFrameWnd * pFrame )
{
	m_PopupFrames.remove( pFrame );
}

void CMainFrame::EnablePopupFrames( BOOL bEnable )
{
	popup_frames_collection_type::iterator it;

	for ( it = m_PopupFrames.begin(); it != m_PopupFrames.end(); it++ ) {

		(*it)->EnableWindow( bEnable );

	}
}

// ============================================================================

//
//	CMainFrame::DockControlBarLeftOf()
//

void CMainFrame::DockControlBarLeftOf(toolbar_type* Bar, toolbar_type* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;
	
	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate

	RecalcLayout(TRUE);
	
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();

	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
	
	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line. By calculating a rectangle, we
	// are simulating a Toolbar being dragged to that location and docked.

	DockControlBar(Bar,n,&rect);

}

// ============================================================================

#if defined(FANCY_WINDOWS_MANAGE_DIALOG)

void CMainFrame::OnWindowManage()
{
	m_MDIClient.ManageWindows(this);
}

#endif


void CMainFrame::ActivateFrame(int nCmdShow) 
{
	// restore the size and position of this frame window
	// ------------------------------------------------------------------------

	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );
	
	if ( pApp ) {

		if ( m_bFirstTime ) {

#if 0 // this call is inside the app logic otherwise the start up is ugly
			pApp->RestoreFrameWindowSettings( this, m_InternalName );
#endif

#if 0 // this is currently broken.
			CString sectionString;
			sectionString.Format( "Settings %s", m_InternalName );
			LoadBarState( sectionString );
#endif

			m_bFirstTime = false;

		}

	}

	
	CMDIFrameWnd::ActivateFrame( nCmdShow );
}

void CMainFrame::OnClose() 
{
	// Save off the size and position of this frame window
	// ------------------------------------------------------------------------

	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );
	
	if ( pApp ) {

		pApp->StoreFrameWindowSettings( this, m_InternalName );

	} 

	GLOBAL_PutSettingInt( "AnimationPlaybackDelay", m_nPlayAnimFrameDelay );

#if 1

	// destroy all the registered popup's
	// ------------------------------------------------------------------------

	popup_frames_collection_type::iterator it;

	while ( !m_PopupFrames.empty() ) {

		m_PopupFrames.back()->DestroyWindow();

		m_PopupFrames.pop_back();

	}

#endif

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnRequestPendingMode() 
{
	if ( m_PendingTimerID ) {

		KillTimer( m_PendingTimerID );

	}

	m_PendingTimerID = SetTimer( PENDING_TIMER, 2000, 0 );
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if ( PENDING_TIMER == nIDEvent ) {

		// Post the pending mode to be handled by the document (I hope)

		KillTimer( m_PendingTimerID );

		m_PendingTimerID = 0;

		PostMessage( WM_COMMAND, ID_PENDING_MODE );

	}  else if ( ANIMATION_TIMER == nIDEvent ) {

		if ( IsWindowEnabled() ) {

			SendMessage( WM_COMMAND, ID_PLAY_ANIM_NEXT_FRAME );

		} else {

			StopPlayingAnimation();

		}

	}

	CMDIFrameWnd::OnTimer(nIDEvent);
}


BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default

	if ( !IsWindowEnabled() ) {

		return GLOBAL_StencilHackOnSetCursor( pWnd, nHitTest, message );

	}

	return CMDIFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}

// ----------------------------------------------------------------------------

//
//	CMainFrame::StopPlayingAnimation()
//

void
CMainFrame::StopPlayingAnimation()
{
	if ( m_AnimationTimerID ) {

		KillTimer( m_AnimationTimerID );

		PostMessage( WM_COMMAND, ID_PLAY_ANIM_STOP );

		m_AnimationTimerID = 0;

	}
}

//
//	CMainFrame::StartPlayingAnimation()
//

void
CMainFrame::StartPlayingAnimation()
{
	if ( !m_AnimationTimerID ) {

		m_nPlayAnimFrameDelay = GLOBAL_GetSettingInt( "AnimationPlaybackDelay", 100 );

		m_AnimationTimerID = SetTimer( ANIMATION_TIMER, m_nPlayAnimFrameDelay, 0 );

		if ( m_AnimationTimerID ) {

			PostMessage( WM_COMMAND, ID_PLAY_ANIM_START );

		}

	}
}

// ----------------------------------------------------------------------------

void CMainFrame::OnAnimPlay() 
{
	if ( m_AnimationTimerID ) {

		StopPlayingAnimation();

	} else {

		StartPlayingAnimation();

	}
}

void CMainFrame::OnUpdateAnimPlay(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );
	pCmdUI->SetCheck( (0 != m_AnimationTimerID) ? 1 : 0 );
	
}

void CMainFrame::OnAnimStop() 
{
	StopPlayingAnimation();
}

void CMainFrame::OnUpdateAnimStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (0 != m_AnimationTimerID) ? TRUE : FALSE );
}

void CMainFrame::OnRememberToolbarSizes() 
{
	m_bRememberToolbarSizes = !m_bRememberToolbarSizes;

	SyncToolbarRememberStates();
}

void CMainFrame::OnUpdateRememberToolbarSizes(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();

	pCmdUI->SetCheck( m_bRememberToolbarSizes ? 1 : 0 );
}
