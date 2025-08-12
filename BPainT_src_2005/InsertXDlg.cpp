// InsertXDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "InsertXDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInsertXDlg dialog


CInsertXDlg::CInsertXDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInsertXDlg::IDD, pParent)
{
	m_pTitle = 0;
	m_nMin = 0;
	m_nMax = 0;
	
	//{{AFX_DATA_INIT(CInsertXDlg)
	m_bInsertAfter = FALSE;
	m_bDuplicate = FALSE;
	m_count = 0;
	//}}AFX_DATA_INIT
}


void CInsertXDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertXDlg)
	DDX_Check(pDX, IDC_INSERT_AFTER_CHECK, m_bInsertAfter);
	DDX_Check(pDX, IDC_DUPLICATE, m_bDuplicate);
	DDX_Text(pDX, IDC_INSERT_COUNT, m_count);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInsertXDlg, CDialog)
	//{{AFX_MSG_MAP(CInsertXDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertXDlg message handlers

BOOL CInsertXDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// setup the width spin control

	CSpinButtonCtrl * pInsertSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_INSERT_SPIN);

	if ( pInsertSpin ) {

		pInsertSpin->SetRange( m_nMin, m_nMax );
		pInsertSpin->SetPos( m_count );

	}

	if ( m_pTitle ) {

		SetWindowText( m_pTitle );

	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ----------------------------------------------------------------------------

//
//	CInsertXDlg::InsertXDialog()
//

bool 
InsertXDialog(
	CWnd * pWnd, const char * title, 
	int & outCount, DWORD & outFlags,
	const int minCount, const int maxCount,
	const char * pSettingsStorage, 
	const bool bLimitStorageWithIncomingData
)
{
	// ------------------------------------------------------------------------

	CInsertXDlg insertDlg( (pWnd) ? pWnd : AfxGetMainWnd() );

	insertDlg.m_pTitle = title;

	insertDlg.m_nMin = minCount;
	insertDlg.m_nMax = maxCount;

	insertDlg.m_count = GLOBAL_GetSettingIntHelper( pSettingsStorage, "count", outCount );
	
	DWORD dwFlags = GLOBAL_GetSettingIntHelper( pSettingsStorage, "flags", outFlags );

	if ( bLimitStorageWithIncomingData ) {

		insertDlg.m_count = max( minCount, min( maxCount, insertDlg.m_count ) );

	}

	// ------------------------------------------------------------------------

	insertDlg.m_bInsertAfter = (0 != (dwFlags & CInsertXDlg::INSERT_AFTER));
	insertDlg.m_bDuplicate = (0 != (dwFlags & CInsertXDlg::DUPLICATE_CURRENT));

	// ------------------------------------------------------------------------

	if ( IDOK == insertDlg.DoModal() ) {

		// convert dlg data to output
		// -------------------------------------------------------------------

		outCount = insertDlg.m_count;

		outFlags = 0;

		if ( insertDlg.m_bInsertAfter ) outFlags |= CInsertXDlg::INSERT_AFTER;
		if ( insertDlg.m_bDuplicate ) outFlags |= CInsertXDlg::DUPLICATE_CURRENT;

		// Store settings
		// -------------------------------------------------------------------

		GLOBAL_PutSettingIntHelper( pSettingsStorage, "count", outCount );
		GLOBAL_PutSettingIntHelper( pSettingsStorage, "flags", outFlags );

		return true;

	}

	return false;

}



