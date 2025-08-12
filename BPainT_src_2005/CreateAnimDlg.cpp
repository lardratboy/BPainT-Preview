// CreateAnimDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "CreateAnimDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreateAnimDlg dialog


CCreateAnimDlg::CCreateAnimDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateAnimDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateAnimDlg)
	m_AnimationName = _T("Untitled animation");
	m_nFrameCount = 1;
	m_nLayerCount = 1;
	m_nFrameRate = 0;
	m_nLinkX = 0;
	m_nLinkY = 0;
	//}}AFX_DATA_INIT
}


void CCreateAnimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateAnimDlg)
	DDX_Text(pDX, IDC_ANIMATION_NAME, m_AnimationName);
	DDX_Text(pDX, IDC_FRAMES_EDIT, m_nFrameCount);
	DDX_Text(pDX, IDC_LAYERS_EDIT, m_nLayerCount);
	DDX_Text(pDX, IDC_FRAME_RATE_EDIT, m_nFrameRate);
	DDX_Text(pDX, IDC_LINK_1_X_EDIT, m_nLinkX);
	DDX_Text(pDX, IDC_LINK_1_Y_EDIT, m_nLinkY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateAnimDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateAnimDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateAnimDlg message handlers

BOOL CCreateAnimDlg::OnInitDialog() 
{
	
	// Set animation name ( before call to OnInitDialog() )

#if defined(MYISLAND)

	m_AnimationName = _T("1 Untitled animation");
	
#endif // defined(MYISLAND)

	CDialog::OnInitDialog();

	// setup the frames spin control

	CSpinButtonCtrl * pFramesSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_FRAMES_SPIN);

	if ( pFramesSpin ) {

		pFramesSpin->SetRange( 1, 32767 );
		pFramesSpin->SetPos( m_nFrameCount );

	}

	// setup the layers spin control

	CSpinButtonCtrl * pLayersSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LAYERS_SPIN);

	if ( pLayersSpin ) {

		pLayersSpin->SetRange( 1, 32767 );
		pLayersSpin->SetPos( m_nLayerCount );

	}

	int upperFrameRateLimit = GLOBAL_GetSettingInt( "nUpperFrameRateDelay", 1000 );
	
	// Setup the framerate spin

	CSpinButtonCtrl * pFrameRateSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_FRAME_RATE_SPIN);

	if ( pFrameRateSpin ) {

		pFrameRateSpin->SetRange( 0, upperFrameRateLimit );
		pFrameRateSpin->SetPos( m_nFrameRate );

	}

	// Setup link X spinner

	CSpinButtonCtrl * pLinkXSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_1_X_SPIN);

	if ( pLinkXSpin ) {

		pLinkXSpin->SetRange( -16384, 16384 );
		pLinkXSpin->SetPos( m_nLinkX );

	}

	// Setup link Y spinner

	CSpinButtonCtrl * pLinkYSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_LINK_1_Y_SPIN);

	if ( pLinkYSpin ) {

		pLinkYSpin->SetRange( -16384, 16384 );
		pLinkYSpin->SetPos( m_nLinkY );

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ----------------------------------------------------------------------------

bool CCreateAnimDlg::LoadSettings( CString & section )
{
	m_SettingsSection = section;
	m_nFrameCount = GLOBAL_GetSettingInt( "nFrameCount", m_nFrameCount, m_SettingsSection );
	m_nLayerCount = GLOBAL_GetSettingInt( "nLayerCount", m_nLayerCount, m_SettingsSection );
	m_nFrameRate = GLOBAL_GetSettingInt( "nFrameRate", m_nFrameRate, m_SettingsSection );
	m_nLinkX = GLOBAL_GetSettingInt( "nLinkX", m_nLinkX, m_SettingsSection );
	m_nLinkY = GLOBAL_GetSettingInt( "nLinkY", m_nLinkY, m_SettingsSection );

	return true;
}

bool CCreateAnimDlg::SaveSettings()
{
	if ( m_SettingsSection.IsEmpty() ) {

		return false;

	}

	GLOBAL_PutSettingInt( "nFrameCount", m_nFrameCount, m_SettingsSection );
	GLOBAL_PutSettingInt( "nLayerCount", m_nLayerCount, m_SettingsSection );
	GLOBAL_PutSettingInt( "nFrameRate", m_nFrameRate, m_SettingsSection );
	GLOBAL_PutSettingInt( "nLinkX", m_nLinkX, m_SettingsSection );
	GLOBAL_PutSettingInt( "nLinkY", m_nLinkY, m_SettingsSection );

	return true;
}


