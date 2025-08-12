// ClearFrameLayersDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "ClearFrameLayersDlg.h"
#include "BpaintDoc.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClearFrameLayersDlg dialog


CClearFrameLayersDlg::CClearFrameLayersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClearFrameLayersDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClearFrameLayersDlg)
	m_nFrom = 0;
	m_nTo = 0;
	m_bImage = FALSE;
	m_bFrameNotes = FALSE;
	m_bLayerNotes = FALSE;
	m_bLink1 = FALSE;
	m_bLink2 = FALSE;
	m_bLink3 = FALSE;
	m_bLink4 = FALSE;
	//}}AFX_DATA_INIT

	m_nMin = 0;
	m_nMax = 0;
}


void CClearFrameLayersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClearFrameLayersDlg)
	DDX_Control(pDX, IDC_LAYERS_LIST, m_SelectedLayersListBox);
	DDX_Text(pDX, IDC_FROM_EDIT, m_nFrom);
	DDX_Text(pDX, IDC_TO_EDIT, m_nTo);
	DDX_Check(pDX, IDC_IMAGE_CHECK, m_bImage);
	DDX_Check(pDX, IDC_FRAME_NOTES_CHECK, m_bFrameNotes);
	DDX_Check(pDX, IDC_LAYER_NOTES_CHECK, m_bLayerNotes);
	DDX_Check(pDX, IDC_LINK_1_CHECK, m_bLink1);
	DDX_Check(pDX, IDC_LINK_2_CHECK, m_bLink2);
	DDX_Check(pDX, IDC_LINK_3_CHECK, m_bLink3);
	DDX_Check(pDX, IDC_LINK_4_CHECK, m_bLink4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClearFrameLayersDlg, CDialog)
	//{{AFX_MSG_MAP(CClearFrameLayersDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClearFrameLayersDlg message handlers

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::CompositeFramesOperationDialogHandler()
//

bool 
CBpaintDoc::ClearLayersDialogHandler( CWnd * pParent )
{
	CClearFrameLayersDlg dlg( pParent );

	// Setup the dlg members
	// ------------------------------------------------------------------------

	dlg.m_bImage = GLOBAL_GetSettingInt( "ClearLayers.m_bImage", TRUE );
	dlg.m_bFrameNotes = GLOBAL_GetSettingInt( "ClearLayers.m_bFrameNotes", TRUE );
	dlg.m_bLayerNotes = GLOBAL_GetSettingInt( "ClearLayers.m_bLayerNotes", TRUE );
	dlg.m_nMin = 1;
	dlg.m_nMax = m_pAnimation->FrameCount();
	dlg.m_nFrom = m_pAnimation->CurrentFrame() + 1;
	dlg.m_nTo = dlg.m_nFrom;
	dlg.m_pAnimation = m_pAnimation;
	dlg.m_SelectedLayers.push_back( m_pAnimation->CurrentLayer() );
	dlg.m_bLink1 = GLOBAL_GetSettingInt( "ClearLayers.m_bLink1", TRUE );
	dlg.m_bLink2 = GLOBAL_GetSettingInt( "ClearLayers.m_bLink2", TRUE );
	dlg.m_bLink3 = GLOBAL_GetSettingInt( "ClearLayers.m_bLink3", TRUE );
	dlg.m_bLink4 = GLOBAL_GetSettingInt( "ClearLayers.m_bLink4", TRUE );
	
	// Do the modal dialog 
	// ------------------------------------------------------------------------

	if ( IDOK != dlg.DoModal() ) {

		return false;

	}

	GLOBAL_PutSettingInt( "ClearLayers.m_bImage", dlg.m_bImage );
	GLOBAL_PutSettingInt( "ClearLayers.m_bFrameNotes", dlg.m_bFrameNotes );
	GLOBAL_PutSettingInt( "ClearLayers.m_bLayerNotes", dlg.m_bLayerNotes );
	GLOBAL_GetSettingInt( "ClearLayers.m_bLink1", dlg.m_bLink1 );
	GLOBAL_GetSettingInt( "ClearLayers.m_bLink2", dlg.m_bLink2 );
	GLOBAL_GetSettingInt( "ClearLayers.m_bLink3", dlg.m_bLink3 );
	GLOBAL_GetSettingInt( "ClearLayers.m_bLink4", dlg.m_bLink4 );

	// Finally perform the operation.
	// ------------------------------------------------------------------------

	int layerCount = m_pAnimation->LayerCount();

	for ( int frame = dlg.m_nFrom; frame <= dlg.m_nTo; frame++ ) {

		int frameNumber = frame - 1;

		for ( int layer = 0; layer < layerCount; layer++ ) {

			// Is this layer in the selected collection?
			// ----------------------------------------------------------------

			CClearFrameLayersDlg::selected_layers_collection_type::iterator it =
				std::find( dlg.m_SelectedLayers.begin(), dlg.m_SelectedLayers.end(), layer
			);

			if ( dlg.m_SelectedLayers.end() != it ) {

				// clear the image
				// ------------------------------------------------------------

				if ( dlg.m_bImage ) {

					BPT::CLayer * pLayer = m_pAnimation->GetLayer( frameNumber, layer );

					if ( pLayer ) {

						pLayer->SetVisualElement( 0 );

					}

				}

				// clear the layer notes
				// ------------------------------------------------------------

				if ( dlg.m_bLayerNotes ) {

					BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( frameNumber );

					if ( pLayerInfo ) {

						if ( pLayerInfo->HasNote() ) {

							pLayerInfo->ClearNote();

						}

					}

				}

				// clear the frame based info
				// ------------------------------------------------------------

				BPT::CFrame * pFrame = m_pAnimation->GetFramePtr( frameNumber );

				if ( pFrame ) {

					// clear the frame notes
					// --------------------------------------------------------

					if ( dlg.m_bFrameNotes ) {
	
						if ( pFrame->HasNote() ) {
	
							pFrame->ClearNote();
	
						}
	
					}

					// Clear the special case link points
					// --------------------------------------------------------

					if ( dlg.m_bLink1 ) {

						pFrame->m_Link1.x = 0;
						pFrame->m_Link1.y = 0;

					}

					if ( dlg.m_bLink2 ) {

						pFrame->m_Link2.x = 0;
						pFrame->m_Link2.y = 0;

					}

					if ( dlg.m_bLink3 ) {

						pFrame->m_Link3.x = 0;
						pFrame->m_Link3.y = 0;

					}

					if ( dlg.m_bLink4 ) {

						pFrame->m_Link4.x = 0;
						pFrame->m_Link4.y = 0;

					}

				}

			}

		}

	}

	return true;

}

// ----------------------------------------------------------------------------

BOOL CClearFrameLayersDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Hook up the spin controls
	// ------------------------------------------------------------------------
	
	CSpinButtonCtrl * pFromSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_FROM_SPIN);

	if ( pFromSpin ) {

		pFromSpin->SetRange( m_nMin, m_nMax );
		pFromSpin->SetPos( m_nFrom );

	}

	CSpinButtonCtrl * pToSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_TO_SPIN);

	if ( pToSpin ) {

		pToSpin->SetRange( m_nMin, m_nMax );
		pToSpin->SetPos( m_nTo );

	}

	// Fill in the layers list box
	// ------------------------------------------------------------------------

	if ( m_pAnimation ) {

		int layerCount = m_pAnimation->LayerCount();

		for ( int i = 0; i < layerCount; i++ ) {

			BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( i );

			if ( !pLayerInfo ) {

				return FALSE;

			}

			char * pName = pLayerInfo->GetName();

			if ( !pName ) {

				return FALSE;

			}

			int index;

			if ( LB_ERR == (index = m_SelectedLayersListBox.AddString( pName )) ) {

				return FALSE;

			}

			// Initial selection

			selected_layers_collection_type::iterator it =
				std::find( m_SelectedLayers.begin(), m_SelectedLayers.end(), index
			);

			if ( m_SelectedLayers.end() != it ) {

				m_SelectedLayersListBox.SetSel( index, TRUE );

			}

		}

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClearFrameLayersDlg::OnOK() 
{

	// Fill in the selected layers collection with the info from the list box
	// ------------------------------------------------------------------------

	m_SelectedLayers.clear();

	int selCount = m_SelectedLayersListBox.GetSelCount();

	if ( selCount ) {

		int itemCount = m_SelectedLayersListBox.GetCount();

		for ( int i = 0; i < itemCount; i++ ) {

			if ( 0 < m_SelectedLayersListBox.GetSel( i ) ) {

				m_SelectedLayers.push_back( i );

			}

		}

	} else {

		MessageBox( "Select at least one layer", "Yo", MB_ICONEXCLAMATION | MB_OK );

		return;

	}
	
	// ------------------------------------------------------------------------

	CDialog::OnOK();
}
