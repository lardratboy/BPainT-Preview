// QuickRGBDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "QuickRGBDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuickRGBDlg dialog


CQuickRGBDlg::CQuickRGBDlg(
	CWnd* pParent /*=NULL*/
	,CWnd * pCenterWnd /* = 0 */
	,CBpaintDoc * pDocument /* = 0 */
) : CDialog(CQuickRGBDlg::IDD, pParent)
	,m_pCenterWnd( pCenterWnd )
	,m_pDocument( pDocument )
	,m_bInit( true )
{
	//{{AFX_DATA_INIT(CQuickRGBDlg)
	m_rValue = 0;
	m_gValue = 0;
	m_bValue = 0;
	m_bLiveUpdates = TRUE;
	//}}AFX_DATA_INIT

	m_nSlot = 0;

}


void CQuickRGBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuickRGBDlg)
	DDX_Control(pDX, IDC_G_SCROLLBAR, m_GScrollBar);
	DDX_Control(pDX, IDC_R_SCROLLBAR, m_RScrollBar);
	DDX_Control(pDX, IDC_B_SCROLLBAR, m_BScrollBar);
	DDX_Text(pDX, IDC_R_EDIT, m_rValue);
	DDV_MinMaxInt(pDX, m_rValue, 0, 255);
	DDX_Text(pDX, IDC_G_EDIT, m_gValue);
	DDV_MinMaxInt(pDX, m_gValue, 0, 255);
	DDX_Text(pDX, IDC_B_EDIT, m_bValue);
	DDV_MinMaxInt(pDX, m_bValue, 0, 255);
	DDX_Check(pDX, IDC_LIVE_UPDATES, m_bLiveUpdates);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuickRGBDlg, CDialog)
	//{{AFX_MSG_MAP(CQuickRGBDlg)
	ON_EN_CHANGE(IDC_R_EDIT, OnChangeREdit)
	ON_EN_CHANGE(IDC_G_EDIT, OnChangeGEdit)
	ON_EN_CHANGE(IDC_B_EDIT, OnChangeBEdit)
	ON_BN_CLICKED(IDC_LIVE_UPDATES, OnLiveUpdates)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuickRGBDlg message handlers

BOOL CQuickRGBDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// setup the R scrollbar control

	CScrollBar * pRScroll = (CScrollBar *)GetDlgItem(IDC_R_SCROLLBAR);

	if ( pRScroll ) {

		pRScroll->SetScrollRange( 0, 255 );
		pRScroll->SetScrollPos( m_rValue );

	}

	// setup the G scrollbar control

	CScrollBar * pGScroll = (CScrollBar *)GetDlgItem(IDC_G_SCROLLBAR);

	if ( pGScroll ) {

		pGScroll->SetScrollRange( 0, 255 );
		pGScroll->SetScrollPos( m_gValue );

	}

	// setup the B scrollbar control

	CScrollBar * pBScroll = (CScrollBar *)GetDlgItem(IDC_B_SCROLLBAR);

	if ( pBScroll ) {

		pBScroll->SetScrollRange( 0, 255 );
		pBScroll->SetScrollPos( m_bValue );

	}

	// check to see if the window is supposed to center over a specific window
	// this window doesn't need to be the parent of the dialog.
	// ------------------------------------------------------------------------

	if ( m_pCenterWnd ) {

		CRect centerRect, currentRect;

		m_pCenterWnd->GetWindowRect( centerRect );

		GetWindowRect( &currentRect );

		int cx = currentRect.Width();
		int cy = currentRect.Height();

		int x = centerRect.left - ((cx - centerRect.Width())/2);
		int y = centerRect.top - ((cy - centerRect.Height())/2);

		int sx = GetSystemMetrics( SM_CXSCREEN );
		int sy = GetSystemMetrics( SM_CYSCREEN );

		if ( 0 > x ) x = 0;
		if ( (x + cx) > sx ) x += (sx - (x + cx));

		if ( 0 > y ) y = 0;
		if ( (y + cy) > sy ) y += (sy - (y + cy));

		SetWindowPos( 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE );

	}
	
	m_bInit = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ----------------------------------------------------------------------------

void CQuickRGBDlg::UpdateColorPreview()
{
	if ( m_pDocument && m_bLiveUpdates && (!m_bInit) ) {

		m_rValue = GetDlgItemInt( IDC_R_EDIT );
		m_gValue = GetDlgItemInt( IDC_G_EDIT );
		m_bValue = GetDlgItemInt( IDC_B_EDIT );

		m_pDocument->Request_ColorRGBChange(
			m_nSlot, m_rValue, m_gValue, m_bValue,
			true, false
		);

	}
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

void CQuickRGBDlg::OnChangeREdit() 
{
	CScrollBar *pScrollBar = (CScrollBar *)GetDlgItem(IDC_R_SCROLLBAR);

	pScrollBar->SetScrollPos(GetDlgItemInt(IDC_R_EDIT));

	UpdateColorPreview();
}

void CQuickRGBDlg::OnChangeGEdit() 
{
	CScrollBar *pScrollBar = (CScrollBar *)GetDlgItem(IDC_G_SCROLLBAR);

	pScrollBar->SetScrollPos(GetDlgItemInt(IDC_G_EDIT));

	UpdateColorPreview();
}

void CQuickRGBDlg::OnChangeBEdit() 
{
	CScrollBar *pScrollBar = (CScrollBar *)GetDlgItem(IDC_B_SCROLLBAR);

	pScrollBar->SetScrollPos(GetDlgItemInt(IDC_B_EDIT));

	UpdateColorPreview();
}


void CQuickRGBDlg::OnLiveUpdates() 
{
	UpdateData();
}

void CQuickRGBDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int pos = pScrollBar->GetScrollPos();

	switch (nSBCode) {

	case SB_LINELEFT:
		pos += -1;
		break;

	case SB_LINERIGHT:
		pos += 1;
		break;

	case SB_PAGELEFT:
		pos += -10;
		break;

	case SB_PAGERIGHT:
		pos += 10;
		break;

	case SB_THUMBTRACK:
		pos = nPos;

	}

	if (pScrollBar == GetDlgItem(IDC_R_SCROLLBAR)) {

		SetDlgItemInt(IDC_R_EDIT, pos);

	} else if (pScrollBar == GetDlgItem(IDC_G_SCROLLBAR)) {

		SetDlgItemInt(IDC_G_EDIT, pos);

	} else if (pScrollBar == GetDlgItem(IDC_B_SCROLLBAR)) {

		SetDlgItemInt(IDC_B_EDIT, pos);

	}

	pScrollBar->SetScrollPos(pos);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
