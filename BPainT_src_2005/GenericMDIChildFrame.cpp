// GenericMDIChildFrame.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited

#include "stdafx.h"
#include "bpaint.h"
#include "GenericMDIChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenericMDIChildFrame

IMPLEMENT_DYNCREATE(CGenericMDIChildFrame, CMDIChildWnd)

CGenericMDIChildFrame::CGenericMDIChildFrame() : m_pDocTemplate( 0 )
{
}

CGenericMDIChildFrame::~CGenericMDIChildFrame()
{
}


BEGIN_MESSAGE_MAP(CGenericMDIChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CGenericMDIChildFrame)
	ON_COMMAND(ID_VIEW_EXPAND, OnViewExpand)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenericMDIChildFrame message handlers

void CGenericMDIChildFrame::OnViewExpand() 
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

BOOL CGenericMDIChildFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class

	cs.style &= ~FWS_ADDTOTITLE;
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

BOOL CGenericMDIChildFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
#if 1 

	nIDResource = IDR_BPAINTTYPE;

#endif

	return CMDIChildWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
}

BOOL CGenericMDIChildFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	
	if ( !IsWindowEnabled() ) {

		return GLOBAL_StencilHackOnSetCursor( pWnd, nHitTest, message );

	}

	return CMDIChildWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CGenericMDIChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if ( pContext ) {

		m_pDocTemplate = pContext->m_pNewDocTemplate;

	}

	return CMDIChildWnd::OnCreateClient(lpcs, pContext);
}

// ----------------------------------------------------------------------------

bool CGenericMDIChildFrame::StoreInternalSettings( const char * sectionName, const char * baseEntryName )
{
	return true;
}

bool CGenericMDIChildFrame::RestoreInternalSettings( const char * sectionName, const char * baseEntryName )
{
	return true;
}

// ----------------------------------------------------------------------------

