// AnimationPropertiesDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "AnimationPropertiesDlg.h"
#include "PaletteManager.h"
#include "bpaintdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnimationPropertiesDlg dialog


CAnimationPropertiesDlg::CAnimationPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimationPropertiesDlg::IDD, pParent)
{

	m_pRequestedPalette = 0;
	m_pShowcase = 0;
	m_pDoc = 0;

	//{{AFX_DATA_INIT(CAnimationPropertiesDlg)
	m_nFrameRate = 0;
	m_strName = _T("");
	m_nLinkX = 0;
	m_nLinkY = 0;
	m_strNote = _T("");
	m_nOutlineA = 0;
	m_nOutlineB = 0;
	//}}AFX_DATA_INIT
}


void CAnimationPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnimationPropertiesDlg)
	DDX_Control(pDX, IDC_NOTE_EDIT, m_GreedyEditCtrl);
	DDX_Text(pDX, IDC_FRAME_RATE_EDIT, m_nFrameRate);
	DDX_Text(pDX, IDC_NAME_EDIT, m_strName);
	DDX_Text(pDX, IDC_LINK_1_X_EDIT2, m_nLinkX);
	DDX_Text(pDX, IDC_LINK_1_Y_EDIT, m_nLinkY);
	DDX_Text(pDX, IDC_OUTLINE_A, m_nOutlineA);
	DDX_Text(pDX, IDC_OUTLINE_B, m_nOutlineB);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_NOTE_EDIT, m_strNote);
	DDX_Control(pDX, IDC_PALETTE_COMBO, m_PaletteCombo);
}


BEGIN_MESSAGE_MAP(CAnimationPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CAnimationPropertiesDlg)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_PALETTE_COMBO, OnCbnSelchangePaletteCombo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimationPropertiesDlg message handlers

BOOL CAnimationPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

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

	// Setup color A spinner

	CSpinButtonCtrl * pOutlineASpin = (CSpinButtonCtrl *)GetDlgItem(IDC_OUTLINE_A_SPIN);

	if ( pOutlineASpin ) {

		pOutlineASpin->SetRange( 0, 255 );
		pOutlineASpin->SetPos( m_nOutlineA );

	}

	// Setup color B spinner

	CSpinButtonCtrl * pOutlineBSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_OUTLINE_B_SPIN);

	if ( pOutlineBSpin ) {

		pOutlineBSpin->SetRange( 0, 255 );
		pOutlineBSpin->SetPos( m_nOutlineB );

	}

	// Fill in the palette combo

	FillInPaletteComboBox();

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ============================================================================

void CBpaintDoc::OnAnimProperties() 
{
	OnAnimProperties2( AfxGetMainWnd() ); // BPT 6/5/02
}

void CBpaintDoc::OnAnimProperties2( CWnd * pOwner ) // BPT 6/5/02
{
	// error check
	// ------------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return;

	}

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	// Deal with possible sub dialog use BPT 6/5/02
	// ------------------------------------------------------------------------

	CWnd * pParent = pOwner;

	if ( !pOwner ) {

		pParent = AfxGetMainWnd();

	}

	// Prepare the dialog info
	// ------------------------------------------------------------------------

	CAnimationPropertiesDlg dlg( pParent  );

	dlg.m_strName = m_pAnimation->GetName();
	dlg.m_strNote = m_pAnimation->GetNote();
	dlg.m_nFrameRate = m_pAnimation->GetPlaybackDelay();
	dlg.m_nLinkX = m_pAnimation->m_Link1.x;
	dlg.m_nLinkY = m_pAnimation->m_Link1.y;
	dlg.m_nOutlineA = m_pAnimation->GetOutlineColorA();
	dlg.m_nOutlineB = m_pAnimation->GetOutlineColorB();

	// ------------------------------------------------------------------------

	dlg.m_pDoc = this;

	dlg.m_pShowcase = m_pShowcase;

	dlg.m_pRequestedPalette = m_pAnimation->GetPalette();

	if ( !dlg.m_pRequestedPalette ) {

		dlg.m_pRequestedPalette = m_pShowcase->GetDefaultPalette();

	}

	// ------------------------------------------------------------------------

	if ( IDOK == dlg.DoModal() ) {

		// Deal with palette change request (BPT 6/7/02)
		// --------------------------------------------------------------------

		if ( m_pShowcase->GetDefaultPalette() == dlg.m_pRequestedPalette ) {

			dlg.m_pRequestedPalette = 0;

		}

		if ( dlg.m_pRequestedPalette != m_pAnimation->GetPalette() ) {

			m_pAnimation->AdoptPalette( dlg.m_pRequestedPalette );

		}

		// Set the animation properties
		// --------------------------------------------------------------------

		m_pAnimation->SetName( dlg.m_strName );
		m_pAnimation->SetNote( dlg.m_strNote );
		m_pAnimation->SetPlaybackDelay( dlg.m_nFrameRate );
		m_pAnimation->m_Link1.x = dlg.m_nLinkX;
		m_pAnimation->m_Link1.y = dlg.m_nLinkY;

		m_pAnimation->SetOutlineColorA( dlg.m_nOutlineA ); // BPT 6/27/01
		m_pAnimation->SetOutlineColorB( dlg.m_nOutlineB );

		// Make sure that the palettes are managed (catch all implmentation...)
		// --------------------------------------------------------------------

		m_pShowcase->ManageSharedPaletteEntries(); // 6/17/01
	
		// Update the user interface elements
		// --------------------------------------------------------------------

		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;

		ForceFullRedraw();
	
		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

		SetModifiedFlag();

		// Hookup the animation frame rate (lame)
		// --------------------------------------------------------------------

		SyncAnimationPlaybackRate();

		// --------------------------------------------------------------------

	}
}

void CBpaintDoc::OnUpdateAnimProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

// ============================================================================

//
//	CAnimationPropertiesDlg::AddPaletteToList()
//

bool 
CAnimationPropertiesDlg::AddPaletteToList( BPT::CAnnotatedPalette * pPalette )
{
	// get or build the palette name
	// ------------------------------------------------------------------------

	char errorName[ _MAX_PATH ];

	char * pszName = pPalette->GetName();

	if ( !pszName ) {

		pszName = errorName;

		sprintf( errorName, "Untitled palette %p", pPalette );

	}

	// ----------------------------------------------------------------

	int item = m_PaletteCombo.AddString( pszName );

	if ( LB_ERR == item ) {

		return false;

	}

	m_PaletteCombo.SetItemData( item, (DWORD)pPalette );

	// ----------------------------------------------------------------

	if ( pPalette == m_pRequestedPalette ) {

		m_PaletteCombo.SetCurSel( item );

	}

	return true;

}

//
//	CAnimationPropertiesDlg::FillInPaletteComboBox()
//

bool 
CAnimationPropertiesDlg::FillInPaletteComboBox()
{
	m_PaletteCombo.ResetContent();

	if ( !m_pShowcase ) {

		return true;

	}

	// Add the default palette
	// ------------------------------------------------------------------------

	BPT::CAnnotatedPalette * pDefaultPalette = m_pShowcase->GetDefaultPalette();

	if ( pDefaultPalette ) {

		if ( !AddPaletteToList( pDefaultPalette ) ) {

			return false;

		}

	}

	// Add all the other showcase managed palettes
	// ------------------------------------------------------------------------

	if ( m_pShowcase ) {

		palette_iterator it = m_pShowcase->BeginPaletteIterator();

		palette_iterator end = m_pShowcase->EndPaletteIterator();

		while ( it != end ) {

			if ( !AddPaletteToList( *it ) ) {

				return false;

			}

			++it;

		}

	}

	// Add the palette manager request entry
	// ------------------------------------------------------------------------

	int item = m_PaletteCombo.AddString( "...PALETTE MANAGER..." );

	if ( LB_ERR == item ) {

		return false;

	}

	return true;
}



void CAnimationPropertiesDlg::OnCbnSelchangePaletteCombo()
{
	int item = m_PaletteCombo.GetCurSel();

	if ( (CB_ERR != item) && m_pShowcase ) {

		BPT::CAnnotatedPalette * pPal = (BPT::CAnnotatedPalette *)m_PaletteCombo.GetItemDataPtr( item );

		if ( ((BPT::CAnnotatedPalette *)-1) != pPal ) {

			if ( pPal ) {

				m_pRequestedPalette = pPal;

			} else {

				CPaletteManager dlg( this );

				dlg.m_pDoc = m_pDoc;
				dlg.m_pShowcase = m_pShowcase;
				dlg.m_pSelectedPalette = m_pRequestedPalette;

				if ( IDOK == dlg.DoModal() ) {

					m_pRequestedPalette = dlg.m_pSelectedPalette;

				}

				FillInPaletteComboBox();

			}

		}

	}
}
