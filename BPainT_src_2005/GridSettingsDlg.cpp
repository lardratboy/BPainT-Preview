// GridSettingsDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "GridSettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridSettingsDlg dialog


CGridSettingsDlg::CGridSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridSettingsDlg)
	m_grid_cy = 0;
	m_grid_cx = 0;
	m_offset_x = 0;
	m_offset_y = 0;
	//}}AFX_DATA_INIT
}


void CGridSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridSettingsDlg)
	DDX_Text(pDX, IDC_HEIGHT, m_grid_cy);
	DDX_Text(pDX, IDC_WIDTH, m_grid_cx);
	DDX_Text(pDX, IDC_X_DELTA_EDIT, m_offset_x);
	DDX_Text(pDX, IDC_Y_DELTA_EDIT, m_offset_y);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGridSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CGridSettingsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridSettingsDlg message handlers

BOOL CGridSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CSpinButtonCtrl * pCXSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_WIDTH_SPIN);

	if ( pCXSpin ) {

		pCXSpin->SetRange( 0, 1024 );
		pCXSpin->SetPos( m_grid_cx );

	}

	CSpinButtonCtrl * pCYSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_HEIGHT_SPIN);

	if ( pCYSpin ) {

		pCYSpin->SetRange( 0, 1024 );
		pCYSpin->SetPos( m_grid_cy );

	}

	CSpinButtonCtrl * pXSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_X_DELTA_SPIN);

	if ( pXSpin ) {

		pXSpin->SetRange( -1024, 1024 );
		pXSpin->SetPos( m_offset_x );

	}

	CSpinButtonCtrl * pYSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_Y_DELTA_SPIN);

	if ( pYSpin ) {

		pYSpin->SetRange( -1024, 1024 );
		pYSpin->SetPos( m_offset_y );

	}

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
