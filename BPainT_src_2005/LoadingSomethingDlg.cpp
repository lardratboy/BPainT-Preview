// LoadingSomethingDlg.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "LoadingSomethingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoadingSomethingDlg dialog


CLoadingSomethingDlg::CLoadingSomethingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadingSomethingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadingSomethingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nRangeMin = 0;
	m_nRangeMax = 0;

}


void CLoadingSomethingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadingSomethingDlg)
	DDX_Control(pDX, IDC_FILE_OFFSET_INDICATOR, m_ProgressCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadingSomethingDlg, CDialog)
	//{{AFX_MSG_MAP(CLoadingSomethingDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadingSomethingDlg message handlers

void 
CLoadingSomethingDlg::ChangeMessage( const char * pText )
{
	if ( pText ) {

		SetWindowText( pText );

	}
}

void CLoadingSomethingDlg::UpdateStatus( 
	const int nCurrent, 
	const int nRangeMin, 
	const int nRangeMax
)
{
	if ( (nRangeMin != m_nRangeMin) || (nRangeMax != m_nRangeMax) ) {

		m_ProgressCtrl.SetRange32( nRangeMin, nRangeMax );

		m_nRangeMin = nRangeMin;
		m_nRangeMax = nRangeMax;

	}

	m_ProgressCtrl.SetPos( nCurrent );
}



