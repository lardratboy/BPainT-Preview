// DeleteFrameRangeDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "DeleteFrameRangeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteFrameRangeDlg dialog


CDeleteFrameRangeDlg::CDeleteFrameRangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteFrameRangeDlg::IDD, pParent)
{
	m_nFromMin = 0;
	m_nFromMax = 0;
	m_nToMin = 0;
	m_nToMax = 0;

	//{{AFX_DATA_INIT(CDeleteFrameRangeDlg)
	m_nFrom = 0;
	m_nTo = 0;
	//}}AFX_DATA_INIT
}


void CDeleteFrameRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteFrameRangeDlg)
	DDX_Text(pDX, IDC_FROM_EDIT, m_nFrom);
	DDX_Text(pDX, IDC_TO_EDIT, m_nTo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteFrameRangeDlg, CDialog)
	//{{AFX_MSG_MAP(CDeleteFrameRangeDlg)
	ON_EN_CHANGE(IDC_FROM_EDIT, OnChangeFromEdit)
	ON_EN_CHANGE(IDC_TO_EDIT, OnChangeToEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteFrameRangeDlg message handlers

BOOL CDeleteFrameRangeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// setup the spin control

	CSpinButtonCtrl * pFromSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_FROM_SPIN);

	if ( pFromSpin ) {

		pFromSpin->SetRange( m_nFromMin, m_nFromMax );
		pFromSpin->SetPos( m_nFrom );

	}

	CSpinButtonCtrl * pToSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_TO_SPIN);

	if ( pToSpin ) {

		pToSpin->SetRange( m_nToMin, m_nToMax );
		pToSpin->SetPos( m_nTo );

	}

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeleteFrameRangeDlg::OnChangeFromEdit() 
{
}

void CDeleteFrameRangeDlg::OnChangeToEdit() 
{
}

void CDeleteFrameRangeDlg::OnOK() 
{

	// ------------------------------------------------------------------------

	int from = GetDlgItemInt( IDC_FROM_EDIT );

	int limitedFrom = max( m_nFromMin, min( m_nFromMax, from ) );

	if ( from != limitedFrom ) {

		MessageBeep( MB_ICONHAND );
		SetDlgItemInt( IDC_FROM_EDIT, limitedFrom );
		GetDlgItem( IDC_FROM_EDIT )->SetFocus();
		return;

	}

	// ------------------------------------------------------------------------

	int to = GetDlgItemInt( IDC_TO_EDIT );

	int limitedTo = max( m_nToMin, min( m_nToMax, to ) );

	if ( to != limitedTo ) {

		MessageBeep( MB_ICONHAND );
		SetDlgItemInt( IDC_TO_EDIT, limitedTo );
		GetDlgItem( IDC_TO_EDIT )->SetFocus();
		return;

	}

	// ------------------------------------------------------------------------

	if ( limitedFrom > limitedTo ) {

		MessageBeep( MB_ICONHAND );
		SetDlgItemInt( IDC_FROM_EDIT, limitedTo );
		SetDlgItemInt( IDC_TO_EDIT, limitedFrom );
		GetDlgItem( IDC_TO_EDIT )->SetFocus();
		return;

	}

	// ------------------------------------------------------------------------

	CDialog::OnOK();
}
