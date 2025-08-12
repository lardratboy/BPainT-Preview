// EnterStringDlg.cpp : header file
//
//	Copyright (c) 2002, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "EnterStringDlg.h"


// CEnterStringDlg dialog

IMPLEMENT_DYNAMIC(CEnterStringDlg, CDialog)
CEnterStringDlg::CEnterStringDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnterStringDlg::IDD, pParent)
	, m_DescriptionString(_T(""))
	, m_EditString(_T(""))
{
}

CEnterStringDlg::~CEnterStringDlg()
{
}

void CEnterStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DESCRIPTION, m_DescriptionString);
	DDX_Text(pDX, IDC_EDIT_STRING, m_EditString);
}


BEGIN_MESSAGE_MAP(CEnterStringDlg, CDialog)
END_MESSAGE_MAP()


// CEnterStringDlg message handlers

BOOL CEnterStringDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText( m_Title );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
