// FramePropertiesDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "FramePropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFramePropertiesDlg dialog


CFramePropertiesDlg::CFramePropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFramePropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFramePropertiesDlg)
	m_strNote = _T("");
	m_nLink1X = 0;
	m_nLink1Y = 0;
	m_nLink2X = 0;
	m_nLink2Y = 0;
	m_nLink3X = 0;
	m_nLink4X = 0;
	m_nLink3Y = 0;
	m_nLink4Y = 0;
	//}}AFX_DATA_INIT
}


void CFramePropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFramePropertiesDlg)
	DDX_Text(pDX, IDC_LINK_1_X_EDIT, m_nLink1X);
	DDX_Text(pDX, IDC_LINK_1_Y_EDIT, m_nLink1Y);
	DDX_Text(pDX, IDC_LINK_2_X_EDIT, m_nLink2X);
	DDX_Text(pDX, IDC_LINK_2_Y_EDIT, m_nLink2Y);
	DDX_Text(pDX, IDC_LINK_3_X_EDIT, m_nLink3X);
	DDX_Text(pDX, IDC_LINK_4_X_EDIT, m_nLink4X);
	DDX_Text(pDX, IDC_LINK_3_Y_EDIT, m_nLink3Y);
	DDX_Text(pDX, IDC_LINK_4_Y_EDIT, m_nLink4Y);
	DDX_Control(pDX, IDC_NOTE_EDIT, m_GreedyEditCtrl);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_NOTE_EDIT, m_strNote);
}


BEGIN_MESSAGE_MAP(CFramePropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CFramePropertiesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFramePropertiesDlg message handlers

BOOL CFramePropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int spinMin = -32767;
	int spinMax = 32767;

	// link point spin controls
	
	CSpinButtonCtrl * p1XSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_1_X_SPIN);

	if ( p1XSpin ) {

		p1XSpin->SetRange( spinMin, spinMax );
		p1XSpin->SetPos( m_nLink1X );

	}

	CSpinButtonCtrl * p1YSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_1_Y_SPIN);

	if ( p1YSpin ) {

		p1YSpin->SetRange( spinMin, spinMax );
		p1YSpin->SetPos( m_nLink1Y );

	}

	// link point spin controls
	
	CSpinButtonCtrl * p2XSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_2_X_SPIN);

	if ( p2XSpin ) {

		p2XSpin->SetRange( spinMin, spinMax );
		p2XSpin->SetPos( m_nLink2X );

	}

	CSpinButtonCtrl * p2YSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_2_Y_SPIN);

	if ( p2YSpin ) {

		p2YSpin->SetRange( spinMin, spinMax );
		p2YSpin->SetPos( m_nLink2Y );

	}

	// link point spin controls
	
	CSpinButtonCtrl * p3XSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_3_X_SPIN);

	if ( p3XSpin ) {

		p3XSpin->SetRange( spinMin, spinMax );
		p3XSpin->SetPos( m_nLink3X );

	}

	CSpinButtonCtrl * p3YSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_3_Y_SPIN);

	if ( p3YSpin ) {

		p3YSpin->SetRange( spinMin, spinMax );
		p3YSpin->SetPos( m_nLink3Y );

	}

	// link point spin controls
	
	CSpinButtonCtrl * p4XSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_4_X_SPIN);

	if ( p4XSpin ) {

		p4XSpin->SetRange( spinMin, spinMax );
		p4XSpin->SetPos( m_nLink4X );

	}

	CSpinButtonCtrl * p4YSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_4_Y_SPIN);

	if ( p4YSpin ) {

		p4YSpin->SetRange( spinMin, spinMax );
		p4YSpin->SetPos( m_nLink4Y );

	}

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
