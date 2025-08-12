// TranslateFrameLayersDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "TranslateFrameLayersDlg.h"
#include "BpaintDoc.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTranslateFrameLayersDlg dialog


CTranslateFrameLayersDlg::CTranslateFrameLayersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTranslateFrameLayersDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTranslateFrameLayersDlg)
	m_nFrom = 0;
	m_nTo = 0;
	m_nXDelta = 0;
	m_nXDelta = 0;
	m_nDDX = 0;
	m_nDDY = 0;
	//}}AFX_DATA_INIT

	m_nMin = 0;
	m_nMax = 0;
}


void CTranslateFrameLayersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranslateFrameLayersDlg)
	DDX_Control(pDX, IDC_SRC_LAYERS_LIST, m_SrcLayersListBox);
	DDX_Text(pDX, IDC_SRC_FROM_EDIT, m_nFrom);
	DDX_Text(pDX, IDC_SRC_TO_EDIT, m_nTo);
	DDX_Text(pDX, IDC_X_DELTA_EDIT, m_nXDelta);
	DDX_Text(pDX, IDC_Y_DELTA_EDIT, m_nYDelta);
	DDX_Text(pDX, IDC_DDX_EDIT, m_nDDX);
	DDX_Text(pDX, IDC_DDY_EDIT, m_nDDY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTranslateFrameLayersDlg, CDialog)
	//{{AFX_MSG_MAP(CTranslateFrameLayersDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ----------------------------------------------------------------------------

bool 
CBpaintDoc::TranslateFrameLayersDialogHandler( CWnd * pParent )
{
	CTranslateFrameLayersDlg dlg( pParent );

	// Setup the dlg members
	// ------------------------------------------------------------------------

	dlg.m_nMin = 1;
	dlg.m_nMax = m_pAnimation->FrameCount();
	dlg.m_nFrom = m_pAnimation->CurrentFrame() + 1;
	dlg.m_nTo = dlg.m_nFrom;
	dlg.m_nXDelta = GLOBAL_GetSettingInt( "TranslateFrameLayers.m_Delta.x", 0 );
	dlg.m_nYDelta = GLOBAL_GetSettingInt( "TranslateFrameLayers.m_Delta.y", 0 );
	dlg.m_nDDX = GLOBAL_GetSettingInt( "TranslateFrameLayers.m_Delta.ddx", 0 );
	dlg.m_nDDY = GLOBAL_GetSettingInt( "TranslateFrameLayers.m_Delta.ddy", 0 );
	dlg.m_pAnimation = m_pAnimation;
	dlg.m_SrcSelectedLayers.push_back( m_pAnimation->CurrentLayer() );

	// Do the modal dialog 
	// ------------------------------------------------------------------------

	if ( IDOK != dlg.DoModal() ) {

		return false;

	}

	GLOBAL_PutSettingInt( "TranslateFrameLayers.m_Delta.x", dlg.m_nXDelta );
	GLOBAL_PutSettingInt( "TranslateFrameLayers.m_Delta.y", dlg.m_nYDelta );
	GLOBAL_PutSettingInt( "TranslateFrameLayers.m_Delta.ddx", dlg.m_nDDX );
	GLOBAL_PutSettingInt( "TranslateFrameLayers.m_Delta.ddy", dlg.m_nDDY );

	// Finally perform the operation.
	// ------------------------------------------------------------------------

	int layerCount = m_pAnimation->LayerCount();

	for ( int frame = dlg.m_nFrom; frame <= dlg.m_nTo; frame++ ) {

		int frameNumber = frame - 1;

		for ( int layer = 0; layer < layerCount; layer++ ) {

			// Is this layer in the selected collection?
			// ----------------------------------------------------------------

			CTranslateFrameLayersDlg::selected_layers_collection_type::iterator it =
				std::find( dlg.m_SrcSelectedLayers.begin(), dlg.m_SrcSelectedLayers.end(), layer
			);

			if ( dlg.m_SrcSelectedLayers.end() != it ) {

				// Move the layer if it has a visual element.
				// ------------------------------------------------------------

				BPT::CLayer * pLayer = m_pAnimation->GetLayer( frameNumber, layer );

				if ( pLayer ) {

					if ( pLayer->HasVisualElement() ) {

						pLayer->Shift( dlg.m_nXDelta, dlg.m_nYDelta );

					}
							
				}

				// ------------------------------------------------------------

			}

			dlg.m_nXDelta += dlg.m_nDDX;
			dlg.m_nYDelta += dlg.m_nDDY;

		}

	}

	return true;

}

/////////////////////////////////////////////////////////////////////////////
// CTranslateFrameLayersDlg message handlers

BOOL CTranslateFrameLayersDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Hook up the frame from & to spin controls
	// ------------------------------------------------------------------------
	
	CSpinButtonCtrl * pSrcFromSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SRC_FROM_SPIN);

	if ( pSrcFromSpin ) {

		pSrcFromSpin->SetRange( m_nMin, m_nMax );
		pSrcFromSpin->SetPos( m_nFrom );

	}

	CSpinButtonCtrl * pSrcToSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SRC_TO_SPIN);

	if ( pSrcToSpin ) {

		pSrcToSpin->SetRange( m_nMin, m_nMax );
		pSrcToSpin->SetPos( m_nTo );

	}

	// Hook up the delta spin controls
	// ------------------------------------------------------------------------
	
	CSpinButtonCtrl * pDXSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_X_DELTA_SPIN);

	if ( pDXSpin ) {

		pDXSpin->SetRange( -1024, 1024 );
		pDXSpin->SetPos( m_nXDelta );

	}

	CSpinButtonCtrl * pDYSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_Y_DELTA_SPIN);

	if ( pDYSpin ) {

		pDYSpin->SetRange( -1024, 1024 );
		pDYSpin->SetPos( m_nYDelta );

	}

	// Hook up the delta spin controls
	// ------------------------------------------------------------------------
	
	CSpinButtonCtrl * pDDXSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_DDX_SPIN);

	if ( pDDXSpin ) {

		pDDXSpin->SetRange( -1024, 1024 );
		pDDXSpin->SetPos( m_nDDX );

	}

	CSpinButtonCtrl * pDDYSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_DDY_SPIN);

	if ( pDDYSpin ) {

		pDDYSpin->SetRange( -1024, 1024 );
		pDDYSpin->SetPos( m_nDDY );

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

			if ( LB_ERR == (index = m_SrcLayersListBox.AddString( pName )) ) {

				return FALSE;

			}

			// Initial selection

			selected_layers_collection_type::iterator it =
				std::find( m_SrcSelectedLayers.begin(), m_SrcSelectedLayers.end(), index
			);

			if ( m_SrcSelectedLayers.end() != it ) {

				m_SrcLayersListBox.SetSel( index, TRUE );

			}

		}

	}

	// ------------------------------------------------------------------------

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTranslateFrameLayersDlg::OnOK() 
{
	// Fill in the selected layers collection with the info from the list box
	// ------------------------------------------------------------------------

	m_SrcSelectedLayers.clear();

	int selCount = m_SrcLayersListBox.GetSelCount();

	if ( selCount ) {

		int itemCount = m_SrcLayersListBox.GetCount();

		for ( int i = 0; i < itemCount; i++ ) {

			if ( 0 < m_SrcLayersListBox.GetSel( i ) ) {

				m_SrcSelectedLayers.push_back( i );

			}

		}

	} else {

		MessageBox( "Select at least one layer", "Yo", MB_ICONEXCLAMATION | MB_OK );

		return;

	}

	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
