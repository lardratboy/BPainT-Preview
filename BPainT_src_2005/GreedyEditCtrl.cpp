// GreedyEditCtrl.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "GreedyEditCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGreedyEditCtrl

CGreedyEditCtrl::CGreedyEditCtrl()
{
}

CGreedyEditCtrl::~CGreedyEditCtrl()
{
}


BEGIN_MESSAGE_MAP(CGreedyEditCtrl, CEdit)
	//{{AFX_MSG_MAP(CGreedyEditCtrl)
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGreedyEditCtrl message handlers

UINT CGreedyEditCtrl::OnGetDlgCode() 
{
	return DLGC_WANTALLKEYS;
//	return CEdit::OnGetDlgCode();
}
