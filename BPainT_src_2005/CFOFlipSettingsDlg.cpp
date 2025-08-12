// CFOFlipSettingsDlg.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "CFOFlipSettingsDlg.h"
#include "TCompositeOpBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCFOFlipSettingsDlg dialog


CCFOFlipSettingsDlg::CCFOFlipSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCFOFlipSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCFOFlipSettingsDlg)
	m_bHFlip = FALSE;
	m_bVFlip = FALSE;
	//}}AFX_DATA_INIT
}


void CCFOFlipSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCFOFlipSettingsDlg)
	DDX_Check(pDX, IDC_HFLIP_CHECK, m_bHFlip);
	DDX_Check(pDX, IDC_VFLIP_CHECK, m_bVFlip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCFOFlipSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CCFOFlipSettingsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCFOFlipSettingsDlg message handlers
