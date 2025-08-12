// PaletteEditorDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "PaletteEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaletteEditorDlg dialog


CPaletteEditorDlg::CPaletteEditorDlg( CBpaintDoc * pDoc, CWnd* pParent /*=NULL*/ )
	: CDialog(CPaletteEditorDlg::IDD, pParent), m_PalDataMediator( pDoc )
{

	m_bInternalUpdate = true;

	m_pDoc = pDoc;

	m_PalDataMediator.SetOwner( this );

	m_PaletteCtrl.SetMediator( &m_PalDataMediator );

	//{{AFX_DATA_INIT(CPaletteEditorDlg)
	m_R_value = 0;
	m_G_value = 0;
	m_B_value = 0;
	m_bLiveUpdates = TRUE;
	//}}AFX_DATA_INIT
}


void CPaletteEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaletteEditorDlg)
	DDX_Text(pDX, IDC_R_EDIT, m_R_value);
	DDV_MinMaxInt(pDX, m_R_value, 0, 255);
	DDX_Text(pDX, IDC_G_EDIT, m_G_value);
	DDV_MinMaxInt(pDX, m_G_value, 0, 255);
	DDX_Text(pDX, IDC_B_EDIT, m_B_value);
	DDV_MinMaxInt(pDX, m_B_value, 0, 255);
    DDX_Control(pDX, IDC_PALETTE, m_PaletteCtrl);
	DDX_Check(pDX, IDC_LIVE_UPDATES, m_bLiveUpdates);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaletteEditorDlg, CDialog)
	//{{AFX_MSG_MAP(CPaletteEditorDlg)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_LIVE_UPDATE_CHECK, OnLiveUpdateCheck)
	ON_EN_CHANGE(IDC_R_EDIT, OnChangeREdit)
	ON_EN_CHANGE(IDC_G_EDIT, OnChangeGEdit)
	ON_EN_CHANGE(IDC_B_EDIT, OnChangeBEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ============================================================================

void CBpaintDoc::OnEditPalette() 
{
	CPaletteEditorDlg paletteDlg( this, AfxGetMainWnd() );

	BPT::CAnnotatedPalette * pPal = GetCurrentPalette();

	if ( !pPal ) {

		TRACE( "No current palette!\n" );

		return; // icky

	}

	// ------------------------------------------------------------------------

	paletteDlg.m_nActiveSlot = Get_L_Color();
	paletteDlg.m_R_value = pPal->GetSlot_R( paletteDlg.m_nActiveSlot );
	paletteDlg.m_G_value = pPal->GetSlot_G( paletteDlg.m_nActiveSlot );
	paletteDlg.m_B_value = pPal->GetSlot_B( paletteDlg.m_nActiveSlot );
	paletteDlg.m_OriginalPalette.CopyColorsFrom( pPal );
	paletteDlg.m_ModifiedPalette.CopyColorsFrom( pPal );

	// ------------------------------------------------------------------------

	if ( IDOK == paletteDlg.DoModal() ) {

		Request_FullPaletteChange( 
			&paletteDlg.m_ModifiedPalette, true, true
		);

		// save dialog settings...

	} else {

		Request_FullPaletteChange( 
			&paletteDlg.m_OriginalPalette, true, true
		);

	}
}

// ============================================================================

/////////////////////////////////////////////////////////////////////////////
// CPaletteEditorDlg message handlers

BOOL CPaletteEditorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Get a copy of the original colors to be able to restore and
	// perform other operations

	if ( !m_pDoc ) {

		TRACE( "No document?\n" );

		return FALSE;

	}

	// setup the spin controls

	CSpinButtonCtrl * p_R_Spin = (CSpinButtonCtrl *)GetDlgItem(IDC_R_SPIN);

	if ( p_R_Spin ) {

		p_R_Spin->SetRange( 0, 255 );
		p_R_Spin->SetPos( m_R_value );

	}

	CSpinButtonCtrl * p_G_Spin = (CSpinButtonCtrl *)GetDlgItem(IDC_G_SPIN);

	if ( p_G_Spin ) {

		p_G_Spin->SetRange( 0, 255 );
		p_G_Spin->SetPos( m_G_value );

	}

	CSpinButtonCtrl * p_B_Spin = (CSpinButtonCtrl *)GetDlgItem(IDC_B_SPIN);

	if ( p_B_Spin ) {

		p_B_Spin->SetRange( 0, 255 );
		p_B_Spin->SetPos( m_B_value );

	}

	SyncDialogToActiveSlot();

	m_bInternalUpdate = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CPaletteEditorDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}


void CPaletteEditorDlg::OnLiveUpdateCheck() 
{
	m_bInternalUpdate = true;

	UpdateData();

	m_bInternalUpdate = false;

	SyncDialogToActiveSlot();
}

void CPaletteEditorDlg::SyncDialogToActiveSlot()
{

	m_bInternalUpdate = true;

	// ------------------------------------------------------------------------

	m_R_value = m_ModifiedPalette.GetSlot_R( m_nActiveSlot );

	SetDlgItemInt( IDC_R_EDIT, m_R_value );

	// ------------------------------------------------------------------------

	m_G_value = m_ModifiedPalette.GetSlot_G( m_nActiveSlot );

	SetDlgItemInt( IDC_G_EDIT, m_G_value );

	// ------------------------------------------------------------------------

	m_B_value = m_ModifiedPalette.GetSlot_B( m_nActiveSlot );

	SetDlgItemInt( IDC_B_EDIT, m_B_value );

	// ------------------------------------------------------------------------

	m_bInternalUpdate = false;

}

// ----------------------------------------------------------------------------

//
//	CPaletteEditorDlg::SetActiveSlot()
//

void CPaletteEditorDlg::SetActiveSlot( const int nSlot )
{
	m_PalDataMediator.ModifyColorStyleBits( m_nActiveSlot, mediator_type::SELECTED, 0 );
	m_PaletteCtrl.InvalidateSlot( m_nActiveSlot, false );

	m_nActiveSlot = nSlot;

	m_PalDataMediator.ModifyColorStyleBits( m_nActiveSlot, 0, mediator_type::SELECTED );
	m_PaletteCtrl.InvalidateSlot( m_nActiveSlot, true );
	
	SyncDialogToActiveSlot();
}

//
//	CPaletteEditorDlg::HandleSlotUpdate()
//

void CPaletteEditorDlg::HandleSlotUpdate( const int nSlot )
{
	// Update the palette
	// ------------------------------------------------------------------------

	m_R_value = max( 0, min( 255, GetDlgItemInt( IDC_R_EDIT ) ) );
	m_G_value = max( 0, min( 255, GetDlgItemInt( IDC_G_EDIT ) ) );
	m_B_value = max( 0, min( 255, GetDlgItemInt( IDC_B_EDIT ) ) );

	m_ModifiedPalette.SetSlot( nSlot, m_R_value, m_G_value, m_B_value );

	// Need to repaint the color...
	// ------------------------------------------------------------------------

	SDisplayPixelType displayColor = MakeDisplayPixelType(
		m_R_value, m_G_value, m_B_value
	);

	m_PalDataMediator.SetDisplayColorForPaletteSlot( m_nActiveSlot, displayColor );

	m_PaletteCtrl.InvalidateSlot( m_nActiveSlot, true );

	// ------------------------------------------------------------------------

	HandleLiveUpdate();
}

//
//	CPaletteEditorDlg::HandleLiveUpdate()
//

void CPaletteEditorDlg::HandleLiveUpdate()
{
	if ( m_bLiveUpdates ) {

		m_pDoc->Request_FullPaletteChange( 
			&m_ModifiedPalette, true, false
		);

	}
}

// ----------------------------------------------------------------------------

void CPaletteEditorDlg::OnChangeREdit() 
{
	if ( !m_bInternalUpdate ) {

		HandleSlotUpdate( m_nActiveSlot );

	}
}

void CPaletteEditorDlg::OnChangeGEdit() 
{
	if ( !m_bInternalUpdate ) {

		HandleSlotUpdate( m_nActiveSlot );

	}
}

void CPaletteEditorDlg::OnChangeBEdit() 
{
	if ( !m_bInternalUpdate ) {

		HandleSlotUpdate( m_nActiveSlot );

	}
}
