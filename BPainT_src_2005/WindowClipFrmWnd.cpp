// WindowClipFrmWnd.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited

#include "stdafx.h"
#include "bpaint.h"
#include "MainFrm.h"
#include "WindowClipFrmWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindowClipFrmWnd

IMPLEMENT_DYNCREATE(CWindowClipFrmWnd, CMiniFrameWnd)

CWindowClipFrmWnd::CWindowClipFrmWnd()
{
	m_pMainFrame = 0;
	m_pDocTemplate = 0;
}

CWindowClipFrmWnd::~CWindowClipFrmWnd()
{
}


BEGIN_MESSAGE_MAP(CWindowClipFrmWnd, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CWindowClipFrmWnd)
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindowClipFrmWnd message handlers

BOOL CWindowClipFrmWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if ( pContext ) {

		m_pDocTemplate = pContext->m_pNewDocTemplate;

	}

	if ( base_class::OnCreateClient(lpcs, pContext) ) {

		SetWindowText( "" );

#if 0

	// Make the app a popup for the main frame (if one)

	CWnd * pMainWnd = AfxGetApp()->m_pMainWnd;

	HWND hwndMain = pMainWnd->GetSafeHwnd();

	CWnd * pSib = GetWindow(GW_HWNDFIRST);

	for ( int count = 0; NULL != pSib; count++ ) {

		if ( (CWnd *)this != pSib ) {

			CWnd * pOwner = pSib->GetWindow( GW_OWNER );

			if ( pOwner ) {

				if ( pOwner->GetSafeHwnd() == hwndMain ) {

					CRect rect;
			
					pSib->GetWindowRect( &rect );
		
					TRACE( "%d) %d,%d,%d,%d (owner %p)\n", count, rect.left, rect.top, rect.right, rect.bottom, (void *)pOwner );

				}

			}

		}

		pSib = pSib->GetWindow( GW_HWNDNEXT );

	}

#endif


		
		return TRUE;

	}


	return FALSE;
}

void CWindowClipFrmWnd::OnDestroy() 
{
	base_class::OnDestroy();
	
	if ( m_pMainFrame ) {

		m_pMainFrame->RemovePopupFrame( this );

	}
}

BOOL CWindowClipFrmWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_THICKFRAME | WS_SYSMENU;

	cs.dwExStyle = WS_EX_TOOLWINDOW;

	cs.hMenu = NULL;

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

// ----------------------------------------------------------------------------

bool CWindowClipFrmWnd::StoreInternalSettings( const char * sectionName, const char * baseEntryName )
{
	return true;
}

bool CWindowClipFrmWnd::RestoreInternalSettings( const char * sectionName, const char * baseEntryName )
{
	return true;
}

// ----------------------------------------------------------------------------


BOOL CWindowClipFrmWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if ( !IsWindowEnabled() ) {

TRACE( "CWindowClipFrmWnd::OnSetCursor()" );

		return GLOBAL_StencilHackOnSetCursor( pWnd, nHitTest, message );

	}

	return base_class::OnSetCursor(pWnd, nHitTest, message);
}
