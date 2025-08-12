// CCopyFramesDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "CopyFramesDlg.h"
#include "BPTRasterOps.h"
#include "BpaintDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCopyFramesDlg dialog


CCopyFramesDlg::CCopyFramesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCopyFramesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCopyFramesDlg)
	m_nDstFrom = 0;
	m_nSrcFrom = 0;
	m_nSrcTo = 0;
	//}}AFX_DATA_INIT

	m_bFirstTime = true;
	m_pShowcase = 0;
	m_pSrcAnimation = 0;
	m_pDstAnimation = 0;

}


void CCopyFramesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCopyFramesDlg)
	DDX_Control(pDX, IDC_COPY_TYPE, m_CopyTypeCombo);
	DDX_Control(pDX, IDC_SRC_LAYERS_LIST, m_SrcLayersListBox);
	DDX_Control(pDX, IDC_DST_LAYERS_LIST, m_DstLayersListBox);
	DDX_Control(pDX, IDC_DST_ANIM_COMBO, m_DstAnimCombo);
	DDX_Control(pDX, IDC_SRC_ANIM_COMBO, m_SrcAnimCombo);
	DDX_Text(pDX, IDC_DST_FROM_EDIT, m_nDstFrom);
	DDX_Text(pDX, IDC_SRC_FROM_EDIT, m_nSrcFrom);
	DDX_Text(pDX, IDC_SRC_TO_EDIT, m_nSrcTo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCopyFramesDlg, CDialog)
	//{{AFX_MSG_MAP(CCopyFramesDlg)
	ON_CBN_SELCHANGE(IDC_SRC_ANIM_COMBO, OnSelchangeSrcAnimCombo)
	ON_CBN_SELCHANGE(IDC_DST_ANIM_COMBO, OnSelchangeDstAnimCombo)
	ON_CBN_SELCHANGE(IDC_COPY_TYPE, OnSelchangeCopyType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::CompositeFramesOperationDialogHandler()
//

bool 
CBpaintDoc::CopyFramesDialogHandler( CWnd * pParent )
{
	CCopyFramesDlg dlg( pParent );

	// Setup the dlg members
	// ------------------------------------------------------------------------

	dlg.m_pShowcase = m_pShowcase;
	dlg.m_pSrcAnimation = m_pAnimation;
	dlg.m_pDstAnimation = m_pAnimation;

	// Select the current frame & layer by default
	// ------------------------------------------------------------------------

	dlg.m_nSrcFrom = dlg.m_pSrcAnimation->CurrentFrame() + 1;
	dlg.m_nSrcTo = dlg.m_nSrcFrom;
	dlg.m_nDstFrom = dlg.m_pDstAnimation->CurrentFrame() + 1;
	dlg.m_SrcSelectedLayers.push_back( dlg.m_pSrcAnimation->CurrentLayer() );
	dlg.m_DstSelectedLayers.push_back( dlg.m_pDstAnimation->CurrentLayer() );

	// Do the modal dialog 
	// ------------------------------------------------------------------------

	if ( IDOK != dlg.DoModal() ) {

		return false;

	}

	// Finally perform the operation.
	// ------------------------------------------------------------------------

	//	dlg.m_pDstAnimation, dlg.m_DstRangeInfo, dlg.m_DstSelectedLayers,
	//	dlg.m_pSrcAnimation, dlg.m_SrcRangeInfo, dlg.m_SrcSelectedLayers,

	bool bError = false;

	int frameCount = dlg.m_DstRangeInfo.m_nTo - dlg.m_DstRangeInfo.m_nFrom + 1;

	int srcFrame = dlg.m_SrcRangeInfo.m_nFrom - 1;
	int dstFrame = dlg.m_DstRangeInfo.m_nFrom - 1;

	// BPT: added (to get the cyclic frame copies...)

	int srcFrameModulo = ((dlg.m_SrcRangeInfo.m_nTo - dlg.m_SrcRangeInfo.m_nFrom) + 1);

	for ( int i = 0; i < frameCount; i++ ) {

		// BPT: changed (needs to be expanded to support reversing & ping pong...)

		BPT::CFrame * pSrcFrame = dlg.m_pSrcAnimation->GetFramePtr( srcFrame + (i % srcFrameModulo) );

		BPT::CFrame * pDstFrame = dlg.m_pDstAnimation->GetFramePtr( dstFrame + i );

		if (pDstFrame == NULL || pSrcFrame == NULL) {

			break;

		}

		// BPT: changed	(now using the class trait for the collection type...)

		CCopyFramesDlg::selected_layers_collection_type::iterator srcIt;
		CCopyFramesDlg::selected_layers_collection_type::iterator dstIt;

		srcIt = dlg.m_SrcSelectedLayers.begin();

		for ( dstIt = dlg.m_DstSelectedLayers.begin(); dstIt != dlg.m_DstSelectedLayers.end(); dstIt++, srcIt++ ) {

			// See if we ran out of source layers
			
			if (srcIt == dlg.m_SrcSelectedLayers.end()) {

				if (dlg.m_CopyType == 0) {
					
					srcIt = dlg.m_SrcSelectedLayers.begin();

				} else {
					
					break;

				}

			}
			
			// Get the source & dest layer pointers

			BPT::CLayer * pSrcLayer = pSrcFrame->GetLayerPtr( *(srcIt) );
			BPT::CLayer * pDstLayer = pDstFrame->GetLayerPtr( *(dstIt) );

			// Ask the dst layer to copy the src layers info...

			if ( !pDstLayer->CopyFrom( pSrcLayer ) ) {

				bError = true;

			}

		}

	}

	if ( bError ) {

		MessageBox( 
			AfxGetMainWnd()->GetSafeHwnd(), 
			"Error during layer information transfer", "ERROR!", 
			MB_ICONERROR | MB_OK
		);

	}

	// If we got this far return true that we did something.
	// ------------------------------------------------------------------------

	return true;
}

// ----------------------------------------------------------------------------
// CCopyFramesDlg message handlers

BOOL CCopyFramesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// ------------------------------------------------------------------------

	if ( !m_pShowcase ) {

		return FALSE;

	}

	m_bFirstTime = true;
	
	// Fill in the information areas
	// ------------------------------------------------------------------------

	SetupInformation( 
		m_pSrcAnimation, 
		IDC_SRC_FROM_EDIT, IDC_SRC_FROM_SPIN, 
		IDC_SRC_TO_EDIT, IDC_SRC_TO_SPIN, 
		m_SrcLayersListBox, m_SrcSelectedLayers,
		m_SrcRangeInfo, IDC_SRC_STATIC, "Source"
	);

	SetupInformation( 
		m_pDstAnimation, 
		IDC_DST_FROM_EDIT, IDC_DST_FROM_SPIN, 
		IDC_DST_TO_EDIT, IDC_DST_TO_SPIN, 
		m_DstLayersListBox, m_DstSelectedLayers,
		m_DstRangeInfo, IDC_DST_STATIC, "Dest"
	);

	// Fill in the copy type combo box
	// ------------------------------------------------------------------------

	m_CopyTypeCombo.AddString("Fixed Size");
	m_CopyTypeCombo.AddString("Cycle Source");

	m_CopyTypeCombo.SetCurSel(0);
	OnSelchangeCopyType();

	// Fill in the animation combo boxes
	// ------------------------------------------------------------------------

	if ( !FillComboBoxWithAnimationList( m_SrcAnimCombo, m_pSrcAnimation ) ) {

		return FALSE;

	}

	if ( !FillComboBoxWithAnimationList( m_DstAnimCombo, m_pDstAnimation ) ) {

		return FALSE;

	}

	// Finish up
	// ------------------------------------------------------------------------

	m_bFirstTime = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ----------------------------------------------------------------------------

bool
CCopyFramesDlg::GetListBoxSelections(
	selected_layers_collection_type & collection, CListBox & listBox
)
{
	collection.clear();

	int selCount = listBox.GetSelCount();

	if ( !selCount ) {

		return true;

	}

	int itemCount = listBox.GetCount();

	for ( int i = 0; i < itemCount; i++ ) {

		if ( 0 < listBox.GetSel( i ) ) {

			collection.push_back( i );

		}

	}

	return true;
	
}

// ----------------------------------------------------------------------------

//
//	CCopyFramesDlg::SetupInformation()
//

bool CCopyFramesDlg::SetupInformation(
	BPT::CAnimation * pAnimation,
	const int fromEdit, const int fromSpin,
	const int toEdit, const int toSpin,
	CListBox & listBox, selected_layers_collection_type & collection,
	SFrameRangeInfo & rangeInfo, const int staticInfoID, const char * lpstrName
)
{
	// Fill in the selection collection with the list box selections
	// --------------------------------------------------------------------

	if ( !m_bFirstTime ) {

		if ( !GetListBoxSelections( collection, listBox ) ) {

			return false;

		}

	}

	// Get the frame limits
	// --------------------------------------------------------------------

	rangeInfo.m_nMin = 1;
	rangeInfo.m_nMax = pAnimation->FrameCount();

	// Get the current values and limit them to the animation limits
	// --------------------------------------------------------------------

	rangeInfo.m_nFrom = GetDlgItemInt( fromEdit );
	rangeInfo.m_nTo = GetDlgItemInt( toEdit );

	rangeInfo.m_nFrom = max( rangeInfo.m_nMin, min( rangeInfo.m_nMax, rangeInfo.m_nFrom ) );
	rangeInfo.m_nTo = max( rangeInfo.m_nMin, min( rangeInfo.m_nMax, rangeInfo.m_nTo ) );

	// --------------------------------------------------------------------

	CString infoText;

	infoText.Format( "%s: Frames %d to %d", lpstrName, rangeInfo.m_nMin, rangeInfo.m_nMax );

	SetDlgItemText( staticInfoID, infoText );

	// Set the edit controls to the limited value
	// --------------------------------------------------------------------

	SetDlgItemInt( fromEdit, rangeInfo.m_nFrom );
	SetDlgItemInt( toEdit, rangeInfo.m_nTo );

	// Fill in the frame information controls
	// --------------------------------------------------------------------

	CSpinButtonCtrl * pSrcFromSpin = (CSpinButtonCtrl *)GetDlgItem(fromSpin);

	if ( pSrcFromSpin ) {

		pSrcFromSpin->SetRange( rangeInfo.m_nMin, rangeInfo.m_nMax );
		pSrcFromSpin->SetPos( rangeInfo.m_nFrom );

	}

	CSpinButtonCtrl * pSrcToSpin = (CSpinButtonCtrl *)GetDlgItem(toSpin);

	if ( pSrcToSpin ) {

		pSrcToSpin->SetRange( rangeInfo.m_nMin, rangeInfo.m_nMax );
		pSrcToSpin->SetPos( rangeInfo.m_nTo );

	}

	// --------------------------------------------------------------------

	listBox.ResetContent();

	if ( pAnimation ) {

		// Validate the selection collection
		// --------------------------------------------------------------------

		int layerCount = pAnimation->LayerCount();

		selected_layers_collection_type::iterator validateIt;

		for ( validateIt = collection.begin(); validateIt != collection.end(); ) {

			if ( (*validateIt) >= layerCount ) {

				validateIt = collection.erase( validateIt );

			} else {

				++validateIt;

			}

		}

		// Fill in the list box
		// --------------------------------------------------------------------

		for ( int i = 0; i < layerCount; i++ ) {

			BPT::CLayerInfo * pLayerInfo = pAnimation->GetLayerInfo( i );

			if ( !pLayerInfo ) {

				return false;

			}

			char * pName = pLayerInfo->GetName();

			if ( !pName ) {

				return false;

			}

			int index;

			if ( LB_ERR == (index = listBox.AddString( pName )) ) {

				return false;

			}

			// Select the items in the selection collection

			selected_layers_collection_type::iterator it =
				std::find( collection.begin(), collection.end(), index
			);

			if ( collection.end() != it ) {

				listBox.SetSel( index, TRUE );

			}

		}

	}

	return true;
}

//
//	CCopyFramesDlg::FillComboBoxWithAnimationList()
//

bool 
CCopyFramesDlg::FillComboBoxWithAnimationList(
	CComboBox & comboBox, BPT::CAnimation * pSelectedAnim
)
{
	comboBox.ResetContent();

	int animCount = m_pShowcase->AnimationCount();

	BPT::CAnimation * pFirst = m_pShowcase->FirstAnimation();

	BPT::CAnimation * pAnim = pFirst;

	for ( int i = 0; i < animCount; i++ ) {

		int item = comboBox.AddString( pAnim->GetName() );

		if ( CB_ERR == item ) {

			return FALSE;

		}

		comboBox.SetItemData( item, (DWORD)pAnim );

		if ( pAnim == pSelectedAnim ) {

			comboBox.SetCurSel( item );

		}

		pAnim = m_pShowcase->NextAnimation( pAnim );

		if ( pAnim == pFirst ) {

			break;

		}

	}

	return true;
}

//
//	CCopyFramesDlg::CheckFrameRangeInfo()
//

bool CCopyFramesDlg::CheckFrameRangeInfo(
	const int fromEdit, const int toEdit, SFrameRangeInfo & rangeInfo
)
{
	// Get the current values and limit them to the animation limits
	// --------------------------------------------------------------------

	int m_nFrom = GetDlgItemInt( fromEdit );
	int m_nTo = GetDlgItemInt( toEdit );

	rangeInfo.m_nFrom = max( rangeInfo.m_nMin, min( rangeInfo.m_nMax, m_nFrom ) );
	rangeInfo.m_nTo = max( rangeInfo.m_nMin, min( rangeInfo.m_nMax, m_nTo ) );

	// Set the edit controls to the limited value
	// --------------------------------------------------------------------

	SetDlgItemInt( fromEdit, rangeInfo.m_nFrom );
	SetDlgItemInt( toEdit, rangeInfo.m_nTo );

	// --------------------------------------------------------------------

	return (m_nFrom == rangeInfo.m_nFrom) && (m_nTo == rangeInfo.m_nTo);

}

//
//	CCopyFramesDlg::OnOK()
//

void CCopyFramesDlg::OnOK() 
{

	// --------------------------------------------------------------------

	if ( !CheckFrameRangeInfo( 
		IDC_SRC_FROM_EDIT, IDC_SRC_TO_EDIT, m_SrcRangeInfo) ) {

		MessageBox( "Invalid source frame range entered, Please validate change", "Yo!", MB_ICONQUESTION | MB_OK );

		CWnd * pWnd = GetDlgItem( IDC_SRC_FROM_EDIT );

		if ( pWnd ) {

			pWnd->SetFocus();

		}

		return;

	}

	// --------------------------------------------------------------------

	if (m_CopyType == 0) {

		SetDlgItemInt(IDC_DST_TO_EDIT, GetDlgItemInt(IDC_DST_FROM_EDIT) + m_SrcRangeInfo.m_nTo - m_SrcRangeInfo.m_nFrom);

	}

	if ( !CheckFrameRangeInfo( 
		IDC_DST_FROM_EDIT, IDC_DST_TO_EDIT, m_DstRangeInfo) ) {

		MessageBox( "Invalid dest frame range entered, Please validate change", "Yo!", MB_ICONQUESTION | MB_OK );

		CWnd * pWnd = GetDlgItem( IDC_DST_FROM_EDIT );

		if ( pWnd ) {

			pWnd->SetFocus();

		}

		return;

	}

//	m_DstRangeInfo.m_nTo = m_DstRangeInfo.m_nFrom + m_SrcRangeInfo.m_nTo - m_SrcRangeInfo.m_nFrom;
	
	// Validate the range sizes
	// ------------------------------------------------------------------------

	int srcOpLength = (m_SrcRangeInfo.m_nTo - m_SrcRangeInfo.m_nFrom) + 1;
	int dstOpLength = (m_DstRangeInfo.m_nTo - m_DstRangeInfo.m_nFrom) + 1;

	if ( m_CopyType == 0 && srcOpLength != dstOpLength ) {

		// Build the message string
		// --------------------------------------------------------------------

		CString messageTxt;

		MessageBox( "Invalid frame range entered, Please validate change", "Yo!", MB_ICONQUESTION | MB_OK );

		CWnd * pWnd = GetDlgItem( IDC_SRC_FROM_EDIT );

		if ( pWnd ) {

			pWnd->SetFocus();

		}

		return;

	}

	// --------------------------------------------------------------------

	if ( !GetListBoxSelections( m_SrcSelectedLayers, m_SrcLayersListBox ) ) {

		MessageBox( "Unable to fetch src selections", "ERROR", MB_ICONERROR | MB_OK );

		CDialog::OnCancel();

		return;

	}

	if ( !GetListBoxSelections( m_DstSelectedLayers, m_DstLayersListBox ) ) {

		MessageBox( "Unable to fetch dst selections", "ERROR", MB_ICONERROR | MB_OK );

		CDialog::OnCancel();

		return;

	}

	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CCopyFramesDlg::OnSelchangeSrcAnimCombo() 
{
	int item = m_SrcAnimCombo.GetCurSel();

	if ( CB_ERR != item ) {

		BPT::CAnimation * pAnim = (BPT::CAnimation *)m_SrcAnimCombo.GetItemDataPtr( item );

		if ( ((BPT::CAnimation *)-1) != pAnim ) {

			m_pSrcAnimation = pAnim;

			SetupInformation( 
				pAnim, 
				IDC_SRC_FROM_EDIT, IDC_SRC_FROM_SPIN, 
				IDC_SRC_TO_EDIT, IDC_SRC_TO_SPIN, 
				m_SrcLayersListBox, m_SrcSelectedLayers,
				m_SrcRangeInfo, IDC_SRC_STATIC, "Source"
			);

		}

	}
}

void CCopyFramesDlg::OnSelchangeDstAnimCombo() 
{
	int item = m_DstAnimCombo.GetCurSel();

	if ( CB_ERR != item ) {

		BPT::CAnimation * pAnim = (BPT::CAnimation *)m_DstAnimCombo.GetItemDataPtr( item );

		if ( ((BPT::CAnimation *)-1) != pAnim ) {

			m_pDstAnimation = pAnim;

			SetupInformation( 
				pAnim, 
				IDC_DST_FROM_EDIT, IDC_DST_FROM_SPIN, 
				0, 0, 
				m_DstLayersListBox, m_DstSelectedLayers,
				m_DstRangeInfo, IDC_DST_STATIC, "Dest"
			);

		}

	}
}


void CCopyFramesDlg::OnSelchangeCopyType() 
{
	m_CopyType = m_CopyTypeCombo.GetCurSel();

	if (m_CopyType == 0) {

		CWnd * pWnd = GetDlgItem( IDC_DST_TO_EDIT );
		pWnd->EnableWindow( false );

	}

	if (m_CopyType == 1) {

		CWnd * pWnd = GetDlgItem( IDC_DST_TO_EDIT );
		pWnd->EnableWindow( true );

	}

}
