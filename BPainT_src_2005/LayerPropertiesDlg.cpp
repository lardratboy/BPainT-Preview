// LayerPropertiesDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "LayerPropertiesDlg.h"
#include "Bpaintdoc.h"
#include "PaletteManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayerPropertiesDlg dialog


CLayerPropertiesDlg::CLayerPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLayerPropertiesDlg::IDD, pParent)
{
	m_pRequestedPalette = 0;
	m_pShowcase = 0;
	m_pDoc = 0;

	//{{AFX_DATA_INIT(CLayerPropertiesDlg)
	m_strNote = _T("");
	m_bVisible = FALSE;
	m_strName = _T("");
	m_nOutlineOption = -1;
	//}}AFX_DATA_INIT
}


void CLayerPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayerPropertiesDlg)
	DDX_Control(pDX, IDC_NOTE_EDIT, m_GreedyEditCtrl);
	DDX_Check(pDX, IDC_VISIBLE_CHECK, m_bVisible);
	DDX_Text(pDX, IDC_NAME_EDIT, m_strName);
	DDX_Radio(pDX, IDC_NO_OUTLINE, m_nOutlineOption);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_NOTE_EDIT, m_strNote);
	DDX_Control(pDX, IDC_PALETTE_COMBO, m_PaletteCombo);
}


BEGIN_MESSAGE_MAP(CLayerPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CLayerPropertiesDlg)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_PALETTE_COMBO, OnCbnSelchangePaletteCombo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerPropertiesDlg message handlers

// ----------------------------------------------------------------------------

void CBpaintDoc::OnLayerProperties() 
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

	BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( 
		m_pAnimation->CurrentLayer()
	);

	if ( !pLayerInfo ) {

		return;

	}

	// Prepare the dialog info
	// ------------------------------------------------------------------------

	CLayerPropertiesDlg dlg( AfxGetMainWnd() );

	dlg.m_strName = CString( pLayerInfo->GetName() );
	dlg.m_strNote = pLayerInfo->GetNote();
	dlg.m_bVisible = (0 == (BPT::CLayerInfo::INVISIBLE & pLayerInfo->GetFlags()) );

	// ------------------------------------------------------------------------

	if ( pLayerInfo->CheckFlags( BPT::CLayerInfo::OUTLINE_COLOR_A ) ) {

		dlg.m_nOutlineOption = 1;

	} else if ( pLayerInfo->CheckFlags( BPT::CLayerInfo::OUTLINE_COLOR_B ) ) {

		dlg.m_nOutlineOption = 2;

	} else {

		dlg.m_nOutlineOption = 0;

	}

	// ------------------------------------------------------------------------

	dlg.m_pDoc = this;

	dlg.m_pShowcase = m_pShowcase;

	dlg.m_pRequestedPalette = pLayerInfo->GetPalette();

	if ( !dlg.m_pRequestedPalette ) {

		// this isn't quite right... Since the layer will
		// default to the animation palette override...

		dlg.m_pRequestedPalette = m_pShowcase->GetDefaultPalette();

	}

	// ------------------------------------------------------------------------

	if ( IDOK == dlg.DoModal() ) {

		// Deal with palette change request (BPT 6/7/02)
		// --------------------------------------------------------------------

		if ( m_pShowcase->GetDefaultPalette() == dlg.m_pRequestedPalette ) {

			dlg.m_pRequestedPalette = 0;

		}

		if ( dlg.m_pRequestedPalette != pLayerInfo->GetPalette() ) {

			pLayerInfo->AdoptPalette( dlg.m_pRequestedPalette );

		}

		// Set the layer properties
		// --------------------------------------------------------------------

		pLayerInfo->SetName( dlg.m_strName );
		pLayerInfo->SetNote( dlg.m_strNote );

		if ( dlg.m_bVisible ) {

			pLayerInfo->ModifyFlags( BPT::CLayerInfo::INVISIBLE, 0 );

		} else {

			pLayerInfo->ModifyFlags( 0, BPT::CLayerInfo::INVISIBLE );

		}
	
		// Do the outline option
		// --------------------------------------------------------------------

		pLayerInfo->ModifyFlags( 
			BPT::CLayerInfo::OUTLINE_COLOR_A | BPT::CLayerInfo::OUTLINE_COLOR_B, 0
		);

		if ( 1 == dlg.m_nOutlineOption ) {

			pLayerInfo->ModifyFlags( 0, BPT::CLayerInfo::OUTLINE_COLOR_A );

		} else if ( 2 == dlg.m_nOutlineOption ) {

			pLayerInfo->ModifyFlags( 0, BPT::CLayerInfo::OUTLINE_COLOR_B );

		}

		// Make sure that the palettes are managed (catch all implmentation...)
		// --------------------------------------------------------------------

		m_pShowcase->ManageSharedPaletteEntries(); // 6/18/01
	
		// Update the user interface elements
		// --------------------------------------------------------------------

		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
	
		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

		ForceFullRedraw();

		SetModifiedFlag();

	}

}

void CBpaintDoc::OnUpdateLayerProperties(CCmdUI* pCmdUI) 
{

	if ( m_pAnimation ) {

		pCmdUI->Enable( (0 != m_pAnimation->ActiveLayer()) );

	} else {

		pCmdUI->Enable( FALSE );

	}

}

// ============================================================================

BOOL CLayerPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillInPaletteComboBox();

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ============================================================================

//
//	CLayerPropertiesDlg::AddPaletteToList()
//

bool 
CLayerPropertiesDlg::AddPaletteToList( BPT::CAnnotatedPalette * pPalette )
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
//	CLayerPropertiesDlg::FillInPaletteComboBox()
//

bool 
CLayerPropertiesDlg::FillInPaletteComboBox()
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

//
//	CLayerPropertiesDlg::OnCbnSelchangePaletteCombo()
//

void 
CLayerPropertiesDlg::OnCbnSelchangePaletteCombo()
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


