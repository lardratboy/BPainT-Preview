// NewAnimationShowcase.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "NewAnimationShowcase.h"
#include "ChooseSizeDlg.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewAnimationShowcase dialog

CNewAnimationShowcaseDlg::CNewAnimationShowcaseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewAnimationShowcaseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewAnimationShowcaseDlg)
	m_AnimationName = _T("Untitled animation");
	m_nFrameCount = 1;
	m_nLayerCount = 1;
	m_ShowcaseName = _T("Untitled showcase");
	//}}AFX_DATA_INIT

	m_bAnimCheck = TRUE;
	m_CanvasSize.cx = 0;
	m_CanvasSize.cy = 0;
	m_PixelAspect.cx = 1;
	m_PixelAspect.cy = 1;
	m_pRecentFileList = 0;
	m_pRecentFilesPopupMenu = 0;
	m_nMRUElement = 0;

}

CNewAnimationShowcaseDlg::~CNewAnimationShowcaseDlg()
{
	if ( m_pRecentFilesPopupMenu ) {

		m_pRecentFilesPopupMenu->DestroyMenu();

		delete m_pRecentFilesPopupMenu;

	}
}

void CNewAnimationShowcaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewAnimationShowcaseDlg)
	DDX_Control(pDX, IDC_RECENT_FILES, m_RecentFilesButton);
	DDX_Text(pDX, IDC_ANIMATION_NAME, m_AnimationName);
	DDX_Text(pDX, IDC_FRAMES_EDIT, m_nFrameCount);
	DDX_Text(pDX, IDC_LAYERS_EDIT, m_nLayerCount);
	DDX_Text(pDX, IDC_SHOWCASE_NAME, m_ShowcaseName);
	//}}AFX_DATA_MAP
}
//	DDX_Check(pDX, IDC_CREATE_ANIM_CHECK, m_bAnimCheck);

BEGIN_MESSAGE_MAP(CNewAnimationShowcaseDlg, CDialog)
	//{{AFX_MSG_MAP(CNewAnimationShowcaseDlg)
	ON_BN_CLICKED(IDC_CANVAS_SIZE, OnCanvasSize)
	ON_BN_CLICKED(IDC_RECENT_FILES, OnRecentFiles)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE( ID_FILE_MRU_FILE1, (ID_FILE_MRU_FILE1 + 31), OnMRUFile )
	ON_BN_CLICKED(IDC_SET_PIXEL_ASPECT_RATIO, OnBnClickedSetPixelAspectRatio)
END_MESSAGE_MAP()
//	ON_BN_CLICKED(IDC_CREATE_ANIM_CHECK, OnCreateAnimCheck)

/////////////////////////////////////////////////////////////////////////////
// Helpers

void CNewAnimationShowcaseDlg::EnableAnimationInfo( const BOOL bEnable )
{
	CWnd * p;
	
	if ( p = GetDlgItem( IDC_ANIMATION_NAME ) ) {

		p->EnableWindow( bEnable );

	}

	if ( p = GetDlgItem( IDC_FRAMES_EDIT ) ) {

		p->EnableWindow( bEnable );

	}

	if ( p = GetDlgItem( IDC_FRAMES_SPIN ) ) {

		p->EnableWindow( bEnable );

	}

	if ( p = GetDlgItem( IDC_LAYERS_EDIT ) ) {

		p->EnableWindow( bEnable );

	}

	if ( p = GetDlgItem( IDC_LAYERS_SPIN ) ) {

		p->EnableWindow( bEnable );

	}

}

void CNewAnimationShowcaseDlg::SyncCanvasSizeText()
{
	CString text;

	text.Format( "Canvas Size %d x %d", m_CanvasSize.cx, m_CanvasSize.cy );

	SetDlgItemText( IDC_CANVAS_SIZE_TEXT, text );
}

void CNewAnimationShowcaseDlg::SyncPixelScaleText()
{
	CString text;

	text.Format( "Pixel Scale X=%d, Y=%d", m_PixelAspect.cx, m_PixelAspect.cy );

	SetDlgItemText( IDC_PIXEL_ASPECT_TEXT, text );
}

/////////////////////////////////////////////////////////////////////////////
// CNewAnimationShowcase message handlers

#if 0
void CNewAnimationShowcaseDlg::OnCreateAnimCheck() 
{
	m_bAnimCheck = (0 != IsDlgButtonChecked( IDC_CREATE_ANIM_CHECK ) );

	EnableAnimationInfo( m_bAnimCheck );
}
#endif

BOOL CNewAnimationShowcaseDlg::OnInitDialog() 
{
	// Set animation name ( before call to OnInitDialog() )
	
	m_AnimationName = _T("1 Untitled animation");
	
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	EnableAnimationInfo( m_bAnimCheck );

	SyncCanvasSizeText();
	SyncPixelScaleText();
	
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

	// Setup the recent files button etc.

	if ( m_pRecentFileList ) {

		char currentDirectory[ _MAX_PATH ];

		_getcwd( currentDirectory, sizeof( currentDirectory ) - 1 );

		m_pRecentFilesPopupMenu = new CMenu();

		if ( m_pRecentFilesPopupMenu ) {

			if ( m_pRecentFilesPopupMenu->CreatePopupMenu() ) {

				int nSize = m_pRecentFileList->GetSize();
	
				for (int i = 0; i < nSize; i++) {
	
					if ( 0 != (*m_pRecentFileList)[i].GetLength() ) {
	
						CString displayName;
	
						m_pRecentFileList->GetDisplayName(
							displayName, i, 
							currentDirectory, 
							strlen( currentDirectory )
						);
	
						m_pRecentFilesPopupMenu->AppendMenu(
							MF_STRING, (ID_FILE_MRU_FILE1 + i), displayName
						);
	
						TRACE( "adding \"%s\"\n", displayName );
	
					}
	
				}

			} else {

				delete m_pRecentFilesPopupMenu;

				m_pRecentFilesPopupMenu = 0;

			}

		}
	
	}

	m_RecentFilesButton.EnableWindow( (0 != m_pRecentFilesPopupMenu) ? TRUE : FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewAnimationShowcaseDlg::OnCanvasSize() 
{
	if ( !GetSizeDialog(
		this, "Canvas Size", m_CanvasSize, 
		&CSize( 32767, 32767 ), "SizeCanvasDlg", true ) ) {

		return;

	}

	SyncCanvasSizeText();
}

// ----------------------------------------------------------------------------

bool CNewAnimationShowcaseDlg::LoadSettings( CString & section )
{
	m_SettingsSection = section;
	m_bAnimCheck = GLOBAL_GetSettingInt( "bAnimCheck", m_bAnimCheck, m_SettingsSection );
	m_CanvasSize.cx = GLOBAL_GetSettingInt( "m_CanvasSize.cx", m_CanvasSize.cx, m_SettingsSection );
	m_CanvasSize.cy = GLOBAL_GetSettingInt( "m_CanvasSize.cy", m_CanvasSize.cy, m_SettingsSection );
	m_nFrameCount = GLOBAL_GetSettingInt( "nFrameCount", m_nFrameCount, m_SettingsSection );
	m_nLayerCount = GLOBAL_GetSettingInt( "nLayerCount", m_nLayerCount, m_SettingsSection );
	m_PixelAspect.cx = GLOBAL_GetSettingInt( "m_PixelAspect.cx", m_PixelAspect.cx, m_SettingsSection );
	m_PixelAspect.cy = GLOBAL_GetSettingInt( "m_PixelAspect.cy", m_PixelAspect.cy, m_SettingsSection );

	return true;
}

bool CNewAnimationShowcaseDlg::SaveSettings()
{
	if ( m_SettingsSection.IsEmpty() ) {

		return false;

	}

	GLOBAL_PutSettingInt( "bAnimCheck", m_bAnimCheck, m_SettingsSection );
	GLOBAL_PutSettingInt( "m_CanvasSize.cx", m_CanvasSize.cx, m_SettingsSection );
	GLOBAL_PutSettingInt( "m_CanvasSize.cy", m_CanvasSize.cy, m_SettingsSection );
	GLOBAL_PutSettingInt( "nFrameCount", m_nFrameCount, m_SettingsSection );
	GLOBAL_PutSettingInt( "nLayerCount", m_nLayerCount, m_SettingsSection );
	GLOBAL_PutSettingInt( "m_PixelAspect.cx", m_PixelAspect.cx, m_SettingsSection );
	GLOBAL_PutSettingInt( "m_PixelAspect.cy", m_PixelAspect.cy, m_SettingsSection );

	return true;
}

void CNewAnimationShowcaseDlg::OnRecentFiles() 
{

	if ( m_pRecentFilesPopupMenu ) {

		CRect rc;
	
		m_RecentFilesButton.GetWindowRect( &rc );

		m_pRecentFilesPopupMenu->TrackPopupMenu(
			TPM_LEFTALIGN | TPM_LEFTBUTTON,
			rc.right, rc.top, this, NULL
		);

	}

}

void CNewAnimationShowcaseDlg::OnMRUFile( UINT nID )
{
	m_nMRUElement = nID;

	EndDialog( IDOK );
}


void CNewAnimationShowcaseDlg::OnBnClickedSetPixelAspectRatio()
{
	if ( !GetSizeDialog(
		this, "Pixel Aspect Ratio", m_PixelAspect, 
		&CSize( 32, 32 ), "PixelAspectRatioDlg", true ) ) {

		return;

	}

	SyncPixelScaleText();
}
