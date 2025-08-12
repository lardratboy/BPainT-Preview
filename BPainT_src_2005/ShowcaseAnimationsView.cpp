// ShowcaseAnimationsView.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "ShowcaseAnimationsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowcaseAnimationsView

IMPLEMENT_DYNCREATE(CShowcaseAnimationsView, CListView)

CShowcaseAnimationsView::CShowcaseAnimationsView()
{
}

CShowcaseAnimationsView::~CShowcaseAnimationsView()
{
}


BEGIN_MESSAGE_MAP(CShowcaseAnimationsView, CListView)
	//{{AFX_MSG_MAP(CShowcaseAnimationsView)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowcaseAnimationsView drawing

void CShowcaseAnimationsView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CShowcaseAnimationsView diagnostics

#ifdef _DEBUG
void CShowcaseAnimationsView::AssertValid() const
{
	CListView::AssertValid();
}

void CShowcaseAnimationsView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CBpaintDoc* CShowcaseAnimationsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBpaintDoc)));
	return (CBpaintDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShowcaseAnimationsView message handlers

void CShowcaseAnimationsView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Ask the document to potentially give focus to us :)
	// ------------------------------------------------------------------------

	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc ) {

		pDoc->HandleAutoFocus( this );

	}

	// TODO: Add your message handler code here and/or call default

	CListView::OnMouseMove(nFlags, point);
}

void CShowcaseAnimationsView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();
	
	// ------------------------------------------------------------------------

	CString title = "Showcase Animations ( " + GetDocument()->GetTitle() + " )";

	SetWindowText( title );

	GetParentFrame()->SetWindowText( title );

	// Try to force the menu to be what we really want.
	// ------------------------------------------------------------------------

	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc ) {

		pDoc->EnsureProperMenu( this );

	}
	
	// ------------------------------------------------------------------------

	BPT::CAnimationShowcase * pShowcase = pDoc->GetShowcaseObject();

	if ( pShowcase ) {

		FillListWithAnimationInfo( pShowcase );

	}

}

void CShowcaseAnimationsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if ( pHint ) {

		if ( pHint->IsKindOf( RUNTIME_CLASS( CActiveAnimationChange ) ) ) {

			CActiveAnimationChange * pChange = (CActiveAnimationChange *)pHint;

		}

	}

}

// ----------------------------------------------------------------------------

//
//	FillListWithAnimationInfoCore()
//

bool
CShowcaseAnimationsView::FillListWithAnimationInfoCore(
	BPT::CAnimationShowcase * pShowcase
)
{

	// Start by getting the control & deleting all the items
	// ------------------------------------------------------------------------

	CListCtrl & listCtrl = GetListCtrl();

	listCtrl.DeleteAllItems();

	// Prepare the column header's
	// ------------------------------------------------------------------------

	int nameColumnIndex = listCtrl.InsertColumn( 0, "Name" );
	int framesColumnIndex = listCtrl.InsertColumn( 1, "Frames" );
	int layersColumnIndex = listCtrl.InsertColumn( 1, "Layers" );
	int linkXColumnIndex = listCtrl.InsertColumn( 1, "Link.x" );
	int linkYColumnIndex = listCtrl.InsertColumn( 1, "Link.y" );

	 if (
		(-1 == nameColumnIndex) ||
		(-1 == framesColumnIndex) ||
		(-1 == layersColumnIndex) ||
		(-1 == linkXColumnIndex) ||
		(-1 == linkYColumnIndex) ) {

		return false;

	 }

	// ------------------------------------------------------------------------

	int animCount = pShowcase->AnimationCount();

	BPT::CAnimation * pFirst = pShowcase->FirstAnimation();

	BPT::CAnimation * pAnim = pFirst;

	for ( int i = 0; i < animCount; i++ ) {

		// --------------------------------------------------------------------

		int index = listCtrl.InsertItem( i, pAnim->GetName() );

		if ( -1 == index ) {

			return false;

		}

		listCtrl.SetItemData( index, (DWORD) pAnim );

		// Add each column information
		// --------------------------------------------------------------------

		// --------------------------------------------------------------------

		pAnim = pShowcase->NextAnimation( pAnim );

		if ( pAnim == pFirst ) {

			break;

		}

	}

	return true;

}

//
//	FillListWithAnimationInfo()
//

void 
CShowcaseAnimationsView::FillListWithAnimationInfo(
	BPT::CAnimationShowcase * pShowcase
)
{
	CListCtrl & listCtrl = GetListCtrl();

	listCtrl.SetRedraw( FALSE );

	FillListWithAnimationInfoCore( pShowcase );

	listCtrl.SetRedraw( TRUE );
}

// ----------------------------------------------------------------------------


