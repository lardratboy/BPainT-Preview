// LayersStripView.cpp : implementation file
//
//	Copyright (c) 2002, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//

// LayersStripView.cpp : implementation file
//

#include "stdafx.h"
#include "bpaint.h"
#include "LayersStripView.h"


// CLayersStripView

IMPLEMENT_DYNCREATE(CLayersStripView, CFormView)

CLayersStripView::CLayersStripView()
	: CFormView(CLayersStripView::IDD)
{
}

CLayersStripView::~CLayersStripView()
{
}

void CLayersStripView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LAYERS_LISTBOX, m_LayersListBox);
}

BEGIN_MESSAGE_MAP(CLayersStripView, CFormView)
	ON_BN_CLICKED(IDC_PREV_FRAME, OnBnClickedPrevFrame)
	ON_BN_CLICKED(IDC_NEXT_FRAME, OnBnClickedNextFrame)
	ON_BN_CLICKED(IDC_INSERT_LAYER, OnBnClickedInsertLayer)
	ON_BN_CLICKED(IDC_DELETE_LAYER, OnBnClickedDeleteLayer)
	ON_BN_CLICKED(IDC_MOVE_LAYER_UP, OnBnClickedMoveLayerUp)
	ON_BN_CLICKED(IDC_MOVE_LAYER_DOWN, OnBnClickedMoveLayerDown)
	ON_BN_CLICKED(IDC_LAYER_PROPERTIES, OnBnClickedLayerProperties)
	ON_CBN_SELCHANGE(IDC_ANIMATION_COMBO, OnCbnSelchangeAnimationCombo)
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_LBN_SELCHANGE(IDC_LAYERS_LISTBOX, OnLbnSelchangeLayersListbox)
END_MESSAGE_MAP()


// CLayersStripView diagnostics

#ifdef _DEBUG
void CLayersStripView::AssertValid() const
{
	CFormView::AssertValid();
}

void CLayersStripView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CLayersStripView message handlers

void CLayersStripView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
}

void CLayersStripView::OnBnClickedPrevFrame()
{
	// TODO: Add your control notification handler code here
}

void CLayersStripView::OnBnClickedNextFrame()
{
	// TODO: Add your control notification handler code here
}

void CLayersStripView::OnBnClickedInsertLayer()
{
	// TODO: Add your control notification handler code here
}

void CLayersStripView::OnBnClickedDeleteLayer()
{
	// TODO: Add your control notification handler code here
}

void CLayersStripView::OnBnClickedMoveLayerUp()
{
	// TODO: Add your control notification handler code here
}

void CLayersStripView::OnBnClickedMoveLayerDown()
{
	// TODO: Add your control notification handler code here
}

void CLayersStripView::OnBnClickedLayerProperties()
{
	// TODO: Add your control notification handler code here
}

void CLayersStripView::OnCbnSelchangeAnimationCombo()
{
	// TODO: Add your control notification handler code here
}

// ----------------------------------------------------------------------------

void CLayersStripView::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if ( nIDCtl != IDC_LAYERS_LISTBOX ) {

		CFormView::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
		return;

	}

	lpMeasureItemStruct->itemHeight = 96;
}

void CLayersStripView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ( nIDCtl != IDC_LAYERS_LISTBOX ) {

		CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);
		return;

	}

	CDC dc;

	if ( dc.Attach( lpDrawItemStruct->hDC ) ) {

		CRect insideRect = lpDrawItemStruct->rcItem;

		--insideRect.bottom;

		// Save these value to restore them when done drawing.
		COLORREF crOldTextColor = dc.GetTextColor();
		COLORREF crOldBkColor = dc.GetBkColor();

		// If this item is selected, set the background color 
		// and the text color to appropriate values. Also, erase
		// rect by filling it with the background color.
		if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
			(lpDrawItemStruct->itemState & ODS_SELECTED))
		{
			dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
			dc.FillSolidRect(&insideRect,::GetSysColor(COLOR_HIGHLIGHT));
		}
		else {

			dc.FillSolidRect(&insideRect, crOldBkColor);

		}

		// If this item has the focus, draw a red frame around the
		// item's rect.
		if ((lpDrawItemStruct->itemAction | ODA_FOCUS) &&
			(lpDrawItemStruct->itemState & ODS_FOCUS))
		{
			dc.DrawFocusRect( &insideRect );

			CRect selectedRect = insideRect;

			selectedRect.InflateRect( -2, -2 );

			CBrush redBrush( RGB( 255, 255, 0 ) );

			if ( NULL != redBrush.GetSafeHandle() ) {

				dc.FrameRect( selectedRect, &redBrush );

			}

		}


		CString itemText;

		itemText.Format( "Item %d (data 0x%08x)", lpDrawItemStruct->itemID, m_LayersListBox.GetItemData( lpDrawItemStruct->itemID ) );

		// Draw the text.
		dc.DrawText(
			itemText,
			&insideRect,
			DT_CENTER|DT_SINGLELINE|DT_VCENTER);

		dc.PatBlt(
			lpDrawItemStruct->rcItem.left
			,lpDrawItemStruct->rcItem.bottom - 1
			,lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left
			,1
			,BLACKNESS
		);

		// Reset the background color and the text color back to their
		// original values.

		dc.SetTextColor(crOldTextColor);
		dc.SetBkColor(crOldBkColor);

		dc.Detach();

	}

}

void CLayersStripView::OnLbnSelchangeLayersListbox()
{
	// TODO: Add your control notification handler code here
}

// ----------------------------------------------------------------------------


void CLayersStripView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	if ( NULL != m_LayersListBox.GetSafeHwnd() ) {

		for ( int counter = 0; counter < 16; counter++ ) {

			int item = m_LayersListBox.AddString( "Howdy" );

			if ( LB_ERR != item ) {

				m_LayersListBox.SetItemData( item, counter + 0x12340000 );

			}

		}

	}

}
