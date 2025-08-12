// SimpleSplitFrame.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited

#include "stdafx.h"
#include "bpaint.h"
#include "SimpleSplitFrame.h"

#include "bpaintView.h"
#include "AnimGridView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimpleSplitFrame

IMPLEMENT_DYNCREATE(CSimpleSplitFrame, CMDIChildWnd)

CSimpleSplitFrame::CSimpleSplitFrame() : m_pDocTemplate( 0 ), m_Layout( 0 )
{
}

CSimpleSplitFrame::~CSimpleSplitFrame()
{
}

// ----------------------------------------------------------------------------

BOOL CSimpleSplitFrame::OnCreateClient1(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// determine the size of the 'client' rect
	// ------------------------------------------------------------------------

	CRect rect;

	CWnd * pParent = GetParent();

	if ( pParent ) {

		pParent->GetClientRect( &rect );

	} else {

		GetClientRect( &rect );

	}

	CSize clientSize = rect.Size();

	CSize size( clientSize.cx/2, clientSize.cy/2 );

	// create the 1 row 2 col splitter
	// ------------------------------------------------------------------------

	if ( !m_wndSplitter1.CreateStatic( this, 1, 2 ) ) {

		TRACE0( "Failed to create first splitter\n" );

		return FALSE;

	}

	// create nested splitter
	// ------------------------------------------------------------------------

	if ( !m_wndSplitter2.CreateStatic(
		&m_wndSplitter1, 2 , 1, WS_CHILD | WS_VISIBLE,
		m_wndSplitter1.IdFromRowCol( 0, 0 )
		) ) {

		TRACE0( "Failed to create nested splitter\n" );

		return FALSE;

	}

	// create the nested top view
	// ------------------------------------------------------------------------

	if ( !m_wndSplitter2.CreateView(
		0, 0, RUNTIME_CLASS( CBpaintView ), size, pContext
		) ) {

		TRACE0( "Failed to create nested view 1\n" );

		return FALSE;

	}

	// create the nested top view
	// ------------------------------------------------------------------------

	if ( !m_wndSplitter2.CreateView(
		1, 0, RUNTIME_CLASS( CAnimGridView ), size, pContext
		) ) {

		TRACE0( "Failed to create nested view 2\n" );

		return FALSE;

	}

	// create the grid view
	// ------------------------------------------------------------------------

	if ( !m_wndSplitter1.CreateView(
		0, 1, RUNTIME_CLASS( CBpaintView ), CSize( clientSize.cx/2, clientSize.cy ) , pContext
		) ) {

		TRACE0( "Failed to create grid view\n" );

		return FALSE;

	}

	// ------------------------------------------------------------------------

	m_wndSplitter1.SetColumnInfo( 0, (clientSize.cx / 2), 0 );
	m_wndSplitter1.RecalcLayout();

	m_wndSplitter2.SetRowInfo( 0, (clientSize.cy / 2), 0 );
	m_wndSplitter2.RecalcLayout();

	// ------------------------------------------------------------------------

	CWnd * pZoomWnd = m_wndSplitter1.GetPane( 0, 1 );

	if ( pZoomWnd ) {

		pZoomWnd->PostMessage( WM_COMMAND, ID_VZOOM_3X );

	}


	return TRUE;
}

// ----------------------------------------------------------------------------

BOOL CSimpleSplitFrame::OnCreateClient2(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	// determine the size of the 'client' rect
	// ------------------------------------------------------------------------

	CRect rect;

	CWnd * pParent = GetParent();

	if ( pParent ) {

		pParent->GetClientRect( &rect );

	} else {

		GetClientRect( &rect );

	}

	CSize clientSize = rect.Size();

	CSize size( clientSize.cx/2, clientSize.cy - (clientSize.cy/4) );

	// create the 1 row 2 col splitter
	// ------------------------------------------------------------------------

	if ( !m_wndSplitter1.CreateStatic( this, 2, 1 ) ) {

		TRACE0( "Failed to create first splitter\n" );

		return FALSE;

	}

	// create nested splitter
	// ------------------------------------------------------------------------

	if ( !m_wndSplitter2.CreateStatic(
		&m_wndSplitter1, 1 , 2, WS_CHILD | WS_VISIBLE,
		m_wndSplitter1.IdFromRowCol( 0, 0 )
		) ) {

		TRACE0( "Failed to create nested splitter\n" );

		return FALSE;

	}

	// create the nested top view
	// ------------------------------------------------------------------------

	if ( !m_wndSplitter2.CreateView(
		0, 0, RUNTIME_CLASS( CBpaintView ), size, pContext
		) ) {

		TRACE0( "Failed to create nested view 1\n" );

		return FALSE;

	}

	// create the nested top view
	// ------------------------------------------------------------------------

	if ( !m_wndSplitter2.CreateView(
		0, 1, RUNTIME_CLASS( CBpaintView ), size, pContext
		) ) {

		TRACE0( "Failed to create nested view 2\n" );

		return FALSE;

	}

	// create the grid view
	// ------------------------------------------------------------------------

	if ( !m_wndSplitter1.CreateView(
		1, 0, RUNTIME_CLASS( CAnimGridView ), CSize( clientSize.cx, clientSize.cy / 4 ) , pContext
		) ) {

		TRACE0( "Failed to create grid view\n" );

		return FALSE;

	}

	// ------------------------------------------------------------------------

	m_wndSplitter2.SetColumnInfo( 0, (clientSize.cx / 2), 0 );
	m_wndSplitter2.RecalcLayout();

	m_wndSplitter1.SetRowInfo( 0, (clientSize.cy * 3 / 4), 0 );
	m_wndSplitter1.RecalcLayout();

	// ------------------------------------------------------------------------

	CWnd * pZoomWnd = m_wndSplitter2.GetPane( 0, 1 );

	if ( pZoomWnd ) {

		pZoomWnd->PostMessage( WM_COMMAND, ID_VZOOM_3X );

	}

	return TRUE;
}

// ----------------------------------------------------------------------------

BOOL CSimpleSplitFrame::OnCreateClient3(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	return OnCreateClient2( lpcs, pContext );
}

// ----------------------------------------------------------------------------

BOOL CSimpleSplitFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// ------------------------------------------------------------------------

	if ( pContext ) {

		m_pDocTemplate = pContext->m_pNewDocTemplate;

	}

	// Figure out which layout the frame should have.
	// ------------------------------------------------------------------------

	int layout = 0;

	// Try to use the doc template to determine layout
	// ------------------------------------------------------------------------

	if ( pContext ) {

		CDocTemplate * pDocTemplate = pContext->m_pNewDocTemplate;

		if ( pDocTemplate ) {

			CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );

			if ( pApp ) {

				if ( pDocTemplate == pApp->GetMDIChildMultiView1DocTemplate() ) {
	
					layout = CSimpleSplitFrame::LAYOUT_1;
	
				} else if ( pDocTemplate == pApp->GetMDIChildMultiView2DocTemplate() ) {
	
					layout = CSimpleSplitFrame::LAYOUT_2;
	
				}

			}

		}

	}

	// If the document template didn't give us a clue about the layout then
	// ask the document.
	// ------------------------------------------------------------------------

	if ( 0 == layout ) {

		if ( pContext && pContext->m_pCurrentDoc && 
			pContext->m_pCurrentDoc->IsKindOf(RUNTIME_CLASS(CBpaintDoc)) ) {
			  
			layout = ((CBpaintDoc*)pContext->m_pCurrentDoc)->GetPendingMultiViewData();
	
		}

	}

	// ------------------------------------------------------------------------

	switch ( layout ) {

	default:
	case CSimpleSplitFrame::LAYOUT_1:
		m_Layout = CSimpleSplitFrame::LAYOUT_1;
		return OnCreateClient1( lpcs, pContext );
		break;

	case CSimpleSplitFrame::LAYOUT_2:
		m_Layout = CSimpleSplitFrame::LAYOUT_2;
		return OnCreateClient2( lpcs, pContext );
		break;

	case CSimpleSplitFrame::LAYOUT_3:
		m_Layout = CSimpleSplitFrame::LAYOUT_3;
		return OnCreateClient3( lpcs, pContext );
		break;

	}

	return FALSE;

}

// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSimpleSplitFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CSimpleSplitFrame)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleSplitFrame message handlers

BOOL CSimpleSplitFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class

	cs.style &= ~FWS_ADDTOTITLE;
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

BOOL CSimpleSplitFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class

#if 1 

	nIDResource = IDR_BPAINTTYPE;

#endif
	
	return CMDIChildWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
}

BOOL CSimpleSplitFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	
	if ( !IsWindowEnabled() ) {

		return GLOBAL_StencilHackOnSetCursor( pWnd, nHitTest, message );

	}

	return CMDIChildWnd::OnSetCursor(pWnd, nHitTest, message);
}

// ----------------------------------------------------------------------------

//
//	CSimpleSplitFrame::StoreSplitterSettings()
//

bool CSimpleSplitFrame::StoreSplitterSettings(
	CSplitterWnd * pSplitter, const int number, 
	const char * sectionName, const char * baseEntryName
)
{
	CString splitBaseEntry;

	splitBaseEntry.Format( "Split%d", number );

	// Save off the splitter 1 information
	// ------------------------------------------------------------------------

	int rowCount1 = pSplitter->GetRowCount();

	PutInternalSetting( sectionName, baseEntryName, splitBaseEntry + "RowCount", rowCount1 );

	for ( int rc1 = 0; rc1 < rowCount1; rc1++ ) {

		int cyCur, cyMin;

		pSplitter->GetRowInfo( rc1, cyCur, cyMin );

		CString entry;

		entry.Format( "%sRow%dSize", splitBaseEntry, rc1 );

		PutInternalSetting( sectionName, baseEntryName, entry, cyCur );
	
	}

	int colCount1 = pSplitter->GetColumnCount();

	PutInternalSetting( sectionName, baseEntryName, splitBaseEntry + "ColCount", colCount1 );

	for ( int cc1 = 0; cc1 < colCount1; cc1++ ) {

		int cxCur, cxMin;

		pSplitter->GetColumnInfo( cc1, cxCur, cxMin );

		CString entry;

		entry.Format( "%sCol%dSize", splitBaseEntry, cc1 );

		PutInternalSetting( sectionName, baseEntryName, entry, cxCur );
	
	}

	// Save off the splitter's view information
	// ------------------------------------------------------------------------

	int split1View = 0;

	for ( rc1 = 0; rc1 < rowCount1; rc1++ ) {

		for ( cc1 = 0; cc1 < colCount1; cc1++ ) {

			CWnd * pWnd = pSplitter->GetPane( rc1, cc1 );

			if ( pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CBpaintView)) ) {

				CString baseEntry;

				baseEntry.Format( "%s.%sView%d", baseEntryName, splitBaseEntry, split1View );

				((CBpaintView *)pWnd)->StoreInternalSettings( sectionName, baseEntry );

				++split1View;

			} else {

				// Other types will go here

			}

		}

	}

	return true;
}

// ----------------------------------------------------------------------------

//
//	CSimpleSplitFrame::RestoreSplitterSettings()
//

bool CSimpleSplitFrame::RestoreSplitterSettings(
	CSplitterWnd * pSplitter, const int number, 
	const char * sectionName, const char * baseEntryName
)
{
	CString splitBaseEntry;

	splitBaseEntry.Format( "Split%d", number );

	// Read in the splitter 1 information
	// ------------------------------------------------------------------------

	int rowCount1 = pSplitter->GetRowCount();

	if ( rowCount1 != GetInternalSetting( sectionName, baseEntryName, splitBaseEntry + "RowCount", rowCount1 ) ) {

		return false;

	}

	for ( int rc1 = 0; rc1 < rowCount1; rc1++ ) {

		int cyCur, cyMin;

		pSplitter->GetRowInfo( rc1, cyCur, cyMin );

		CString entry;

		entry.Format( "%sRow%dSize", splitBaseEntry, rc1 );

		cyCur = GetInternalSetting( sectionName, baseEntryName, entry, cyCur );

		// Need to set the info

		pSplitter->SetRowInfo( rc1, cyCur, cyMin );
	
	}

	int colCount1 = pSplitter->GetColumnCount();

	if ( colCount1 != GetInternalSetting( sectionName, baseEntryName, splitBaseEntry + "ColCount", colCount1 ) ) {

		return false;

	}

	for ( int cc1 = 0; cc1 < colCount1; cc1++ ) {

		int cxCur, cxMin;

		pSplitter->GetColumnInfo( cc1, cxCur, cxMin );

		CString entry;

		entry.Format( "%sCol%dSize", splitBaseEntry, cc1 );

		cxCur = GetInternalSetting( sectionName, baseEntryName, entry, cxCur );
	
		// Need to set the info

		pSplitter->SetColumnInfo( cc1, cxCur, cxMin );

	}

	pSplitter->RecalcLayout();

	// Save off the splitter's view information
	// ------------------------------------------------------------------------

	int split1View = 0;

	for ( rc1 = 0; rc1 < rowCount1; rc1++ ) {

		for ( cc1 = 0; cc1 < colCount1; cc1++ ) {

			CWnd * pWnd = pSplitter->GetPane( rc1, cc1 );

			if ( pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CBpaintView)) ) {

				CString baseEntry;

				baseEntry.Format( "%s.%sView%d", baseEntryName, splitBaseEntry, split1View );

				((CBpaintView *)pWnd)->RestoreInternalSettings( sectionName, baseEntry );

				++split1View;

			} else {

				// Other types will go here

			}

		}

	}

	return true;
}

// ----------------------------------------------------------------------------

//
//	CSimpleSplitFrame::StoreInternalSettings()
//	

bool CSimpleSplitFrame::StoreInternalSettings( const char * sectionName, const char * baseEntryName )
{

	PutInternalSetting( sectionName, baseEntryName, "Layout", m_Layout );

	// ------------------------------------------------------------------------

	if ( !StoreSplitterSettings( &m_wndSplitter1, 0, sectionName, baseEntryName ) ) {

		return false;

	}

	// ------------------------------------------------------------------------

	if ( !StoreSplitterSettings( &m_wndSplitter2, 1, sectionName, baseEntryName ) ) {

		return false;

	}

	// ------------------------------------------------------------------------

	return true;
}

//
//	CSimpleSplitFrame::RestoreInternalSettings()
//	

bool CSimpleSplitFrame::RestoreInternalSettings( const char * sectionName, const char * baseEntryName )
{

	if ( m_Layout == GetInternalSetting( sectionName, baseEntryName, "Layout", m_Layout ) ) {

		// --------------------------------------------------------------------
	
		if ( !RestoreSplitterSettings( &m_wndSplitter1, 0, sectionName, baseEntryName ) ) {
	
			return false;
	
		}
	
		// --------------------------------------------------------------------
	
		if ( !RestoreSplitterSettings( &m_wndSplitter2, 1, sectionName, baseEntryName ) ) {
	
			return false;
	
		}

	}

	return true;
}

// ----------------------------------------------------------------------------

