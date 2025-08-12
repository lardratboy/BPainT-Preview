// GotoFrameDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "GotoFrameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGotoFrameDlg dialog


CGotoFrameDlg::CGotoFrameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGotoFrameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGotoFrameDlg)
	m_nFrame = 0;
	m_strFrameRange = _T("");
	//}}AFX_DATA_INIT

	m_nFrameMin = 0;
	m_nFrameMax = 0;
}


void CGotoFrameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGotoFrameDlg)
	DDX_Text(pDX, IDC_FRAME_EDIT, m_nFrame);
	DDX_Text(pDX, IDC_FRAME_RANGE_STATIC, m_strFrameRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGotoFrameDlg, CDialog)
	//{{AFX_MSG_MAP(CGotoFrameDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGotoFrameDlg message handlers

BOOL CGotoFrameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CSpinButtonCtrl * pFrameSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_FRAME_SPIN);

	if ( pFrameSpin ) {

		pFrameSpin->SetRange( m_nFrameMin, m_nFrameMax );
		pFrameSpin->SetPos( m_nFrame );

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGotoFrameDlg::OnOK() 
{
	int nFrame = GetDlgItemInt( IDC_FRAME_EDIT );

	int limitedFrame = max( m_nFrameMin, min( nFrame, m_nFrameMax ) );

	if ( nFrame == limitedFrame ) {

		CDialog::OnOK();

	} else {

		SetDlgItemInt( IDC_FRAME_EDIT, limitedFrame );
		GetDlgItem( IDC_FRAME_EDIT )->SetFocus();
		MessageBeep( MB_ICONQUESTION );

	}
	
}
