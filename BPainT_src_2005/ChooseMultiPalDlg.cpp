// ChooseMultiPalDlg.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "ChooseMultiPalDlg.h"
#include "BpaintDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseMultiPalDlg dialog


CChooseMultiPalDlg::CChooseMultiPalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseMultiPalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseMultiPalDlg)
	m_nSelection = -1;
	//}}AFX_DATA_INIT

	m_bUserCanCancel = true;
}


void CChooseMultiPalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseMultiPalDlg)
	DDX_Radio(pDX, IDC_RADIO1, m_nSelection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseMultiPalDlg, CDialog)
	//{{AFX_MSG_MAP(CChooseMultiPalDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ============================================================================

//
//	CBpaintDoc::ConfirmMultiplePaletteBehavior()
//

bool CBpaintDoc::ConfirmMultiplePaletteBehavior(
	const char * pMsgTitle, 
	const bool bUserCanCancel,
	const bool bBroadCastPaletteChange
)
{

#if !defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

	BPT::CAnnotatedPalette * pCurrentPalette = GetCurrentPalette();

	CChooseMultiPalDlg dlg( AfxGetMainWnd() );

	dlg.m_bUserCanCancel = bUserCanCancel;

	// Determine the level of the existing palette
	// ------------------------------------------------------------------------

	dlg.m_nSelection = 0;

	if ( !m_pAnimation ) { // 6/21/01

		return false; // no palette assume bad state and bail 

	} else {

		// Does the layer have a palette?
		// --------------------------------------------------------------------

		BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( 
			m_pAnimation->CurrentLayer()
		);
	
		if ( pLayerInfo ) {

			BPT::CAnnotatedPalette * pPal = pLayerInfo->GetPalette();
	
			if ( pPal ) {

				dlg.m_nSelection = 2;
	
			}

		}

		// Does the animation have a palette?
		// --------------------------------------------------------------------

		if ( 0 == dlg.m_nSelection ) {

			BPT::CAnnotatedPalette * pPal = m_pAnimation->GetPalette();
	
			if ( pPal ) {

				dlg.m_nSelection = 1;
	
			}

		}

	}

	// Handle 'policy' issues
	// ------------------------------------------------------------------------

	if ( 2 == dlg.m_nSelection ) {

		// automatically edit the layer palette if there is one

		return true; 

	}

	if ( 1 == dlg.m_nSelection ) {

		if ( !m_pAnimation->LayerPalettesEnabled() ) {

			// If there is an animation palette and if layer palettes
			// aren't enabled then automatically edit the animation palette

			return true;

		}

	}

	if ( 0 == dlg.m_nSelection ) {

		if ( !m_pAnimation->MultiplePaletteSupport() ) {

			// If the animation doesn't support multiple palettes then
			// bail out here and don't ask a question.

			return true;

		}

	}

	// Need to pass on policy to the radio controls so it can disable
	// some choices to avoid user confusion.
	// ------------------------------------------------------------------------

// NEED TO FINISH LOGIC
// NEED TO FINISH LOGIC
// NEED TO FINISH LOGIC

	// ------------------------------------------------------------------------

	if ( IDCANCEL == dlg.DoModal() ) {

		return false;

	}

	// Now make sure that the palette exists for the selection
	// ------------------------------------------------------------------------

	bool bError = false;

	switch ( dlg.m_nSelection ) {

	case 1: // Animation
		if ( m_pAnimation ) {

			if ( pCurrentPalette != m_pAnimation->GetPalette() ) {

				bError = !m_pAnimation->CreatePaletteFrom( pCurrentPalette );

				if ( (!bError) && bBroadCastPaletteChange ) {
	
					ForceFullRedraw();

					UpdateAllViews(
						NULL, CColorChange::WHOLE_PALETTE_CHANGE,
						(CObject *)&CColorChange( 0 )
					);

				}

				SetModifiedFlag();

			}

		}
		break;

	case 2: // Layer
		if ( m_pAnimation ) {

			BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( 
				m_pAnimation->CurrentLayer()
			);
		
			if ( pLayerInfo ) {

				if ( pCurrentPalette != pLayerInfo->GetPalette() ) {

					bError = !pLayerInfo->CreatePaletteFrom( pCurrentPalette );

					if ( (!bError) && bBroadCastPaletteChange ) {
	
						ForceFullRedraw();

						UpdateAllViews(
							NULL, CColorChange::WHOLE_PALETTE_CHANGE,
							(CObject *)&CColorChange( 0 )
						);

					}

					SetModifiedFlag();

				}

			}

		}
		break;

	}

	if ( bError ) {

		// Deal with it as if the user hit cancel

		TRACE( "There was an error of some sort\n" );

		return false;

	}

#endif // !defined(BPT_SHOWCASE_MANAGED_PALETTES) BPT 6/5/02

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CChooseMultiPalDlg message handlers


BOOL CChooseMultiPalDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Does the user have a choice to cancel?
	// ------------------------------------------------------------------------

	CWnd * pWnd = GetDlgItem( IDCANCEL );

	if ( pWnd ) {

		pWnd->EnableWindow( (m_bUserCanCancel) ? TRUE : FALSE );

		if ( !m_bUserCanCancel ) {

			pWnd->ShowWindow( SW_HIDE );

		}

	}

	// ------------------------------------------------------------------------

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
