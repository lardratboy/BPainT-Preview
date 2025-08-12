// ChooseSizeDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "ChooseSizeDlg.h"
#include "limits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseSizeDlg dialog


CChooseSizeDlg::CChooseSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseSizeDlg::IDD, pParent)
	,m_min( 0, 0 )
	,m_max( 1, 1 )
	,m_LastChanged( 0 )
	,m_bSameSettings( false )
{
	//{{AFX_DATA_INIT(CChooseSizeDlg)
	m_height = 0;
	m_width = 0;
	m_bSameSettings = FALSE;
	//}}AFX_DATA_INIT
}


void CChooseSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseSizeDlg)
	DDX_Text(pDX, IDC_HEIGHT, m_height);
	DDX_Text(pDX, IDC_WIDTH, m_width);
	DDX_Check(pDX, IDC_SAME_SETTINGS, m_bSameSettings);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseSizeDlg, CDialog)
	//{{AFX_MSG_MAP(CChooseSizeDlg)
	ON_BN_CLICKED(IDC_SAME_SETTINGS, OnSameSettings)
	ON_EN_CHANGE(IDC_HEIGHT, OnChangeHeight)
	ON_EN_CHANGE(IDC_WIDTH, OnChangeWidth)
	ON_EN_CHANGE(IDC_HEIGHT_SPIN, OnChangeHeight)
	ON_EN_CHANGE(IDC_WIDTH_SPIN, OnChangeWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseSizeDlg message handlers

BOOL CChooseSizeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// setup the width spin control

	CSpinButtonCtrl * pWidthSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_WIDTH_SPIN);

	if ( pWidthSpin ) {

		pWidthSpin->SetRange( (short)m_min.cx, (short)m_max.cx );
		pWidthSpin->SetPos( m_width );

	}

	// setup the height spin control
	
	CSpinButtonCtrl * pHeightSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_HEIGHT_SPIN);

	if ( pHeightSpin ) {

		pHeightSpin->SetRange( (short)m_min.cy, (short)m_max.cy );
		pHeightSpin->SetPos( m_height );

	}

	if ( m_pTitle ) {

		SetWindowText( m_pTitle );

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseSizeDlg::OnSameSettings() 
{
	m_bSameSettings = (0 != IsDlgButtonChecked(IDC_SAME_SETTINGS));

	int realLast = m_LastChanged;

	EnsureSameSettings( 0 );

	if ( realLast ) m_LastChanged = realLast;

	if ( m_LastChanged ) {

		if ( GetDlgItem(m_LastChanged) ) {

			GetDlgItem(m_LastChanged)->SetFocus();

		}

	}

}

void CChooseSizeDlg::EnsureSameSettings( const int id )
{
	static int insideCount = 0;

	if ( m_bSameSettings & (!insideCount) ) {

		++insideCount;

		int value;
	
		if ( id ) {
	
			value = GetDlgItemInt( id );
	
		} else {

			if ( m_LastChanged ) {

				value = GetDlgItemInt( m_LastChanged );

			} else {

				value = max( GetDlgItemInt( IDC_WIDTH ), GetDlgItemInt( IDC_HEIGHT ) );

			}
	
		}
	
		SetDlgItemInt( (IDC_WIDTH == id) ? IDC_HEIGHT : IDC_WIDTH, value );

		--insideCount;

	}
}

void CChooseSizeDlg::OnChangeHeight() 
{
	m_LastChanged = IDC_HEIGHT;
	EnsureSameSettings( IDC_HEIGHT );
}

void CChooseSizeDlg::OnChangeWidth() 
{
	m_LastChanged = IDC_WIDTH;
	EnsureSameSettings( IDC_WIDTH );
}

// ----------------------------------------------------------------------------

//
//	GetSizeDialog()
//

bool 
GetSizeDialog(
	CWnd * pWnd, const char * title, SIZE & size, 
	const SIZE * pLimits, const char * pSettingsStorage /* = 0 */,
	const bool bInitFromIncomingSize /* = false */
)
{
	CChooseSizeDlg sizeDlg( (pWnd) ? pWnd : AfxGetMainWnd() );

	sizeDlg.m_pTitle = title;
	sizeDlg.m_width = GLOBAL_GetSettingIntHelper( pSettingsStorage, "cx", size.cx );
	sizeDlg.m_height = GLOBAL_GetSettingIntHelper( pSettingsStorage, "cy", size.cy );
	sizeDlg.m_bSameSettings = GLOBAL_GetSettingIntHelper( pSettingsStorage, "equal", sizeDlg.m_bSameSettings );

	sizeDlg.m_min = CSize( 0, 0 );

	sizeDlg.m_max = CSize(
		(pLimits) ? pLimits->cx : INT_MAX, 
		(pLimits) ? pLimits->cy : INT_MAX
	);

	if ( bInitFromIncomingSize ) {

		sizeDlg.m_width = max( sizeDlg.m_min.cx, min( size.cx, sizeDlg.m_max.cx ) );
		sizeDlg.m_height = max( sizeDlg.m_min.cy, min( size.cy, sizeDlg.m_max.cy ) );

	}

	if ( IDOK == sizeDlg.DoModal() ) {

		size.cx = sizeDlg.m_width;
		size.cy = sizeDlg.m_height;

		GLOBAL_PutSettingIntHelper( pSettingsStorage, "cx", size.cx );
		GLOBAL_PutSettingIntHelper( pSettingsStorage, "cy", size.cy );
		GLOBAL_PutSettingIntHelper( pSettingsStorage, "equal", sizeDlg.m_bSameSettings );

		return true;

	}

	return false;

}



