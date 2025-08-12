// SpotAdjustmentPropDlg.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "SpotAdjustmentPropDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpotAdjustmentPropDlg dialog


CSpotAdjustmentPropDlg::CSpotAdjustmentPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpotAdjustmentPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpotAdjustmentPropDlg)
	m_nFrame = 0;
	m_xPos = 0;
	m_yPos = 0;
	m_Note = _T("");
	//}}AFX_DATA_INIT

	m_nMinFrame = 0;
	m_nMaxFrame = 0;
}


void CSpotAdjustmentPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpotAdjustmentPropDlg)
	DDX_Text(pDX, IDC_FRAME_NUMBER_EDIT, m_nFrame);
	DDX_Text(pDX, IDC_LINK_1_X_EDIT2, m_xPos);
	DDX_Text(pDX, IDC_LINK_1_Y_EDIT, m_yPos);
	DDX_Text(pDX, IDC_NOTE_EDIT, m_Note);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpotAdjustmentPropDlg, CDialog)
	//{{AFX_MSG_MAP(CSpotAdjustmentPropDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpotAdjustmentPropDlg message handlers

BOOL CSpotAdjustmentPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// setup the spinner controls
	// ------------------------------------------------------------------------

	CSpinButtonCtrl * pXSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_1_X_SPIN);

	if ( pXSpin ) {

		pXSpin->SetRange( -1024, 1024 );
		pXSpin->SetPos( m_xPos );

	}

	CSpinButtonCtrl * pYSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_1_Y_SPIN);

	if ( pYSpin ) {

		pYSpin->SetRange( -1024, 1024 );
		pYSpin->SetPos( m_yPos );

	}

	CSpinButtonCtrl * pFrameSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_FRAME_NUMBER_SPIN);

	if ( pFrameSpin ) {

		pFrameSpin->SetRange( m_nMinFrame, m_nMaxFrame );
		pFrameSpin->SetPos( m_nFrame );

	}

	// ------------------------------------------------------------------------

	if ( (m_nMinFrame == m_nFrame) && (m_nMaxFrame == m_nFrame) ) {

		CWnd * pFrameEdit = GetDlgItem( IDC_FRAME_NUMBER_EDIT );

		if ( pFrameEdit ) {

			pFrameEdit->EnableWindow( FALSE );

		}

		CWnd * pFrameSpin = GetDlgItem( IDC_FRAME_NUMBER_SPIN );

		if ( pFrameSpin ) {

			pFrameSpin->EnableWindow( FALSE );

		}

	}
	
	// ------------------------------------------------------------------------

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
