// NamedSpotPropDlg.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "NamedSpotPropDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNamedSpotPropDlg dialog


CNamedSpotPropDlg::CNamedSpotPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNamedSpotPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNamedSpotPropDlg)
	m_Name = _T("");
	m_Note = _T("");
	m_xPos = 0;
	m_yPos = 0;
	//}}AFX_DATA_INIT
}


void CNamedSpotPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNamedSpotPropDlg)
	DDX_Text(pDX, IDC_NAME_EDIT, m_Name);
	DDX_Text(pDX, IDC_NOTE_EDIT, m_Note);
	DDX_Text(pDX, IDC_LINK_1_X_EDIT2, m_xPos);
	DDX_Text(pDX, IDC_LINK_1_Y_EDIT, m_yPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNamedSpotPropDlg, CDialog)
	//{{AFX_MSG_MAP(CNamedSpotPropDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNamedSpotPropDlg message handlers

BOOL CNamedSpotPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// setup the spinner controls

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

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
