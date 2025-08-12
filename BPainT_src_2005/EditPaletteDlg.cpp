// MakeStencilDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "EditPaletteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditPaletteDlg dialog

// Global palette clipboard...there is a better way to do this.

static BPT::CAnnotatedPalette::color_storage_type PaletteClipboardColors[ 256 ];
static int PaletteClipboardUsed[ 256 ];
static int PaletteClipboardEmpty = true;

CEditPaletteDlg::CEditPaletteDlg( CBpaintDoc * pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CEditPaletteDlg::IDD, pParent), m_PalDataMediator( pDoc ), m_WindowTitle( "Edit Palette" )
{

	m_pEditedPalette = &m_InternalPalette1;

	m_bEnableRepaint = true;

	m_LastCount = -1;
	m_LastSlot = -1;

	m_HeadSlot = 0;
	m_TailSlot = 0;

	m_bLButtonDown = false;

	m_pDocument = pDoc;
	m_bInit = true;

	m_PalDataMediator.SetOwner( this );

	m_PaletteCtrl.SetMediator( &m_PalDataMediator );

	//{{AFX_DATA_INIT(CEditPaletteDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEditPaletteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditPaletteDlg)
    DDX_Control(pDX, IDC_SELECTED_COLORS, m_PaletteCtrl);
	DDX_Control(pDX, IDC_G_SCROLLBAR, m_GScrollBar);
	DDX_Control(pDX, IDC_R_SCROLLBAR, m_RScrollBar);
	DDX_Control(pDX, IDC_B_SCROLLBAR, m_BScrollBar);
	DDX_Check(pDX, IDC_LIVE_UPDATES, m_bLiveUpdates);
	//}}AFX_DATA_MAP
}

//	DDX_Text(pDX, IDC_R_EDIT, m_rValue);
//	DDV_MinMaxInt(pDX, m_rValue, 0, 255);
//	DDX_Text(pDX, IDC_G_EDIT, m_gValue);
//	DDV_MinMaxInt(pDX, m_gValue, 0, 255);
//	DDX_Text(pDX, IDC_B_EDIT, m_bValue);
//	DDV_MinMaxInt(pDX, m_bValue, 0, 255);

BEGIN_MESSAGE_MAP(CEditPaletteDlg, CDialog)
	//{{AFX_MSG_MAP(CEditPaletteDlg)
	ON_BN_CLICKED(IDC_USED, OnUsed)
	ON_WM_SETCURSOR()
	ON_EN_CHANGE(IDC_R_EDIT, OnChangeREdit)
	ON_EN_CHANGE(IDC_G_EDIT, OnChangeGEdit)
	ON_EN_CHANGE(IDC_B_EDIT, OnChangeBEdit)
	ON_BN_CLICKED(IDC_LIVE_UPDATES, OnLiveUpdates)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_FILL, OnFill)
	ON_BN_CLICKED(IDC_SPREAD, OnSpread)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_FROM_PARENT, OnFromParent)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_UNDO, OnBnClickedUndo)
END_MESSAGE_MAP()

//
// ---------------------------------------------------------------------------
//

//
//	CBpaintDoc::OnEditPalette2()
//

void CBpaintDoc::OnEditPalette2(
	CWnd * pOwner
	,BPT::CAnnotatedPalette * pPalette
	,const bool bCanRepaint
	,const bool bModifyDocumentPalette
)
{
	// ------------------------------------------------------------------------

	CEditPaletteDlg paletteDlg( this, pOwner );

	BPT::CAnnotatedPalette * pPal = pPalette;

	if ( !pPal ) {

		TRACE( "No current palette!\n" );

		return; // icky

	}

	// ------------------------------------------------------------------------

	paletteDlg.m_HeadSlot = Get_L_Color();
	paletteDlg.m_rValue = pPal->GetSlot_R( paletteDlg.m_HeadSlot );
	paletteDlg.m_gValue = pPal->GetSlot_G( paletteDlg.m_HeadSlot );
	paletteDlg.m_bValue = pPal->GetSlot_B( paletteDlg.m_HeadSlot );
	paletteDlg.m_OriginalPalette.CopyColorsFrom( pPal );
	paletteDlg.m_pEditedPalette->CopyColorsFrom( pPal );
	paletteDlg.m_bEnableRepaint = bCanRepaint;

	// ------------------------------------------------------------------------

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/6/02

	// get or build the palette name
	// ------------------------------------------------------------------------

	char errorName[ _MAX_PATH ];

	char * pszName = pPalette->GetName();

	if ( !pszName ) {

		pszName = errorName;

		sprintf( errorName, "Untitled palette %p", pPalette );

	}

	paletteDlg.m_WindowTitle = "Edit \"" + CString( pszName ) + "\"";

#else
#if 1 

	if ( GetShowcasePalette() == pPal ) {

		paletteDlg.m_WindowTitle = "Edit " + GetCurrentPaletteSourceName();

	} else if ( GetAnimationPalette() == pPal ) {

		paletteDlg.m_WindowTitle = "Edit " + GetCurrentPaletteSourceName();

	} else {

		paletteDlg.m_WindowTitle = "Edit " + GetCurrentPaletteSourceName();

	}
	
#endif
#endif

	// ------------------------------------------------------------------------

	if ( IDOK == paletteDlg.DoModal() ) {

		if ( bModifyDocumentPalette ) {

			Request_FullPaletteChange( 
				paletteDlg.m_pEditedPalette, true, true
			);

		} else {

			pPalette->CopyColorsFrom( paletteDlg.m_pEditedPalette );

		}

		SetModifiedFlag();

		// save dialog settings...

	} else {

		if ( bModifyDocumentPalette ) {

			Request_FullPaletteChange( 
				&paletteDlg.m_OriginalPalette, true, true
			);

		}

	}
}

//
//	OnEditPalette()
//

void CBpaintDoc::OnEditPalette() 
{

#if 1  // BPT 6/15/01

	// Deal with the multiple potential palettes issue
	// ------------------------------------------------------------------------

	if ( !ConfirmMultiplePaletteBehavior() ) {

		return;
		
	}

#endif

	OnEditPalette2( AfxGetMainWnd(), GetCurrentPalette(), true, true );

}

// ----------------------------------------------------------------------------

BOOL CEditPaletteDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText( m_WindowTitle );

	m_bInit = false;

	m_TailSlot = m_HeadSlot;

	SelectSlot( m_HeadSlot, true, true );
	UpdateScrollBars();
	Palette_UpdateInfoMessage( 0 );

	CWnd * pLiveUpdateWnd = GetDlgItem( IDC_LIVE_UPDATES );

	if ( pLiveUpdateWnd ) {

		pLiveUpdateWnd->EnableWindow( m_bEnableRepaint ? TRUE : FALSE );

	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CEditPaletteDlg message handlers

void CEditPaletteDlg::OnUsed() 
{
	// TODO: Add your control notification handler code here
	
}

void CEditPaletteDlg::HandleLiveUpdate()
{
	// Ugly read back of the palette info to get the proper multi palette values
	// ------------------------------------------------------------------------

	if ( (!m_bInit) && (m_pDocument && m_bLiveUpdates) ) {

		m_pDocument->Request_FullPaletteChange( m_pEditedPalette, true, true );

#if 0 // BPT 6/7/02 

		BPT::CAnnotatedPalette * pPal = m_pDocument->GetCurrentPalette();
		
		if ( pPal ) {

			m_pEditedPalette->CopyColorsFrom( pPal, false );

		}

#endif

	}

}

void CEditPaletteDlg::OnFill() 
{
	BeginUndo();

	int r = m_pEditedPalette->GetSlot_R( m_HeadSlot);
	int g = m_pEditedPalette->GetSlot_G( m_HeadSlot);
	int b = m_pEditedPalette->GetSlot_B( m_HeadSlot);

	for (int i = m_HeadSlot; i <= m_TailSlot; i++) {

		m_pEditedPalette->SetSlot( i, r, g, b);
		m_PaletteCtrl.GetMediator()->SetDisplayColorForPaletteSlot( i, MakeDisplayPixelType(r, g, b) );

	}

	HandleLiveUpdate();
	
	UpdateScrollBars();

}

void CEditPaletteDlg::OnSpread() 
{
	BeginUndo();

	int r1 = m_pEditedPalette->GetSlot_R( m_HeadSlot);
	int g1 = m_pEditedPalette->GetSlot_G( m_HeadSlot);
	int b1 = m_pEditedPalette->GetSlot_B( m_HeadSlot);

	int r2 = m_pEditedPalette->GetSlot_R( m_TailSlot);
	int g2 = m_pEditedPalette->GetSlot_G( m_TailSlot);
	int b2 = m_pEditedPalette->GetSlot_B( m_TailSlot);

	int count = m_TailSlot - m_HeadSlot + 1;

#if 1 // BPT 6/19/01

	float t = 0.0f;

	float td;

	if ( 1 < count ) {

		td = 1.0f / (count - 1);

	} else {

		td = 0.0f;

	}

	float dr = (float)(r2 - r1);
	float dg = (float)(g2 - g1);
	float db = (float)(b2 - b1);

#endif

	for (int i = 0; i < count; i++) {

#if 1 // BPT 6/19/01 

		// Improved accuracy of the math so that the head and tail colors
		// don't change during the spread operation.
		// --------------------------------------------------------------------

		int r = r1 + (int)((dr * t) + 0.5f);
		int g = g1 + (int)((dg * t) + 0.5f);
		int b = b1 + (int)((db * t) + 0.5f);

		t += td;

#else

		int r = r1 + (int)(((r2 - r1) / (float)count) * i);
		int g = g1 + (int)(((g2 - g1) / (float)count) * i);
		int b = b1 + (int)(((b2 - b1) / (float)count) * i);

#endif
		
		m_pEditedPalette->SetSlot( i + m_HeadSlot, r, g, b);
		m_PaletteCtrl.GetMediator()->SetDisplayColorForPaletteSlot( i + m_HeadSlot, MakeDisplayPixelType(r, g, b) );

	}

	HandleLiveUpdate();

	UpdateScrollBars();

}

void CEditPaletteDlg::OnPaste() 
{
	if ( !PaletteClipboardEmpty ) {

		BeginUndo();

		int slot = m_HeadSlot;
		int cbSlot = 0;

		while( slot < 256 && cbSlot < 256) {

			if ( PaletteClipboardUsed[ cbSlot ] ) {

				int r = GetRValue( PaletteClipboardColors[ cbSlot ] );
				int g = GetGValue( PaletteClipboardColors[ cbSlot ] );
				int b = GetBValue( PaletteClipboardColors[ cbSlot ] );

				m_pEditedPalette->SetSlot( slot, r, g, b);
				m_PaletteCtrl.GetMediator()->SetDisplayColorForPaletteSlot( slot, MakeDisplayPixelType(r, g, b) );

				++slot;

			}

			++ cbSlot;

		}

		HandleLiveUpdate();

		UpdateScrollBars();

	}
}

void CEditPaletteDlg::OnCopy() 
{
	int i;

	for ( i = 0; i < 256; i++ ) {

		PaletteClipboardUsed[ i ] = 0;

		if ( IsSlotSelected( i ) ) {
		
			PaletteClipboardColors[ i ] = m_pEditedPalette->GetSlot( i );
			PaletteClipboardUsed[ i ] = 1;

		}

	}
	
	PaletteClipboardEmpty = false;

}

BOOL CEditPaletteDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CEditPaletteDlg::UpdateScrollBars()
{
	m_rValue = m_pEditedPalette->GetSlot_R( m_HeadSlot );
	m_gValue = m_pEditedPalette->GetSlot_G( m_HeadSlot );
	m_bValue = m_pEditedPalette->GetSlot_B( m_HeadSlot );

	bool bFromParent = (BPT::CAnnotatedPalette::PFLAG_FROM_PARENT &  m_pEditedPalette->GetSlotFlags( m_HeadSlot )) ? true : false;

	SetDlgItemInt( IDC_R_EDIT, m_rValue);
	SetDlgItemInt( IDC_G_EDIT, m_gValue);
	SetDlgItemInt( IDC_B_EDIT, m_bValue);

	// setup the R scrollbar control

	CScrollBar * pRScroll = (CScrollBar *)GetDlgItem(IDC_R_SCROLLBAR);
 
	if ( pRScroll ) {

		CWnd * pREdit = GetDlgItem(IDC_R_EDIT);

		if ( pREdit ) {

			pREdit->EnableWindow( bFromParent ? FALSE : TRUE );

		}

		if ( bFromParent ) {

			pRScroll->EnableWindow( FALSE );

		} else {

			pRScroll->SetScrollRange( 0, 255 );
			pRScroll->SetScrollPos( m_rValue );
			pRScroll->EnableWindow( TRUE );

		}


	}

	// setup the G scrollbar control

	CScrollBar * pGScroll = (CScrollBar *)GetDlgItem(IDC_G_SCROLLBAR);

	if ( pGScroll ) {

		CWnd * pGEdit = GetDlgItem(IDC_G_EDIT);

		if ( pGEdit ) {

			pGEdit->EnableWindow( bFromParent ? FALSE : TRUE );

		}

		if ( bFromParent ) {

			pGScroll->EnableWindow( FALSE );

		} else {

			pGScroll->SetScrollRange( 0, 255 );
			pGScroll->SetScrollPos( m_gValue );

		}

	}

	// setup the B scrollbar control

	CScrollBar * pBScroll = (CScrollBar *)GetDlgItem(IDC_B_SCROLLBAR);

	if ( pBScroll ) {

		CWnd * pBEdit = GetDlgItem(IDC_B_EDIT);

		if ( pBEdit ) {

			pBEdit->EnableWindow( bFromParent ? FALSE : TRUE );

		}

		if ( bFromParent ) {

			pBScroll->EnableWindow( FALSE );

		} else {

			pBScroll->SetScrollRange( 0, 255 );
			pBScroll->SetScrollPos( m_bValue );

		}

	}

	// update the other controls
	// ------------------------------------------------------------------------

	CString text;

	text.Format("Color settings for %d", m_HeadSlot);
	SetDlgItemText( IDC_RGB_STATIC, text );
	CheckDlgButton( IDC_FROM_PARENT, bFromParent ? 1 : 0 );

	// Update the palette control 
	// --------------------------------------------------------------------

	for ( int i = 0; i < 256; i++ ) {

		m_PalDataMediator.SetDisplayColorForPaletteSlot(
			i, m_pEditedPalette->GetCLUTValue( i )
		);

		DWORD dwFlags = m_pEditedPalette->GetSlotFlags( i );

		if ( BPT::CAnnotatedPalette::PFLAG_FROM_PARENT & dwFlags ) {

			m_PalDataMediator.ModifyColorStyleBits( i, 0, mediator_type::MARKED );

		} else {

			m_PalDataMediator.ModifyColorStyleBits( i, mediator_type::MARKED, 0 );

		}

	}

	m_PaletteCtrl.InvalidateWholePalette( true );

}

void CEditPaletteDlg::Palette_UpdateInfoMessage( const int slot ) {

	// ------------------------------------------------------------------------

	if ( m_LastSlot != slot ) {

		CString text;

		m_LastSlot = slot; // fix flicker

		text.Format( "Slot %d", slot );
		SetDlgItemText( IDC_OVER_STATIC, text );

		int r = m_pEditedPalette->GetSlot_R( slot );
		int g = m_pEditedPalette->GetSlot_G( slot );
		int b = m_pEditedPalette->GetSlot_B( slot );

		text.Format( "R: %d", r);
		SetDlgItemText( IDC_R_STATIC, text );
		text.Format( "G: %d", g);
		SetDlgItemText( IDC_G_STATIC, text );
		text.Format( "B: %d", b);
		SetDlgItemText( IDC_B_STATIC, text );

	}

}

// ----------------------------------------------------------------------------

void CEditPaletteDlg::UpdateColorPreview()
{
	m_pEditedPalette->SetSlot( m_HeadSlot, m_rValue, m_gValue, m_bValue );

	m_PaletteCtrl.GetMediator()->SetDisplayColorForPaletteSlot( m_HeadSlot, MakeDisplayPixelType(m_rValue, m_gValue, m_bValue) );

	m_PaletteCtrl.InvalidateSlot( m_HeadSlot, true );

	if ( m_pDocument && m_bLiveUpdates && (!m_bInit) ) {

		m_pDocument->Request_ColorRGBChange(
			m_HeadSlot, m_rValue, m_gValue, m_bValue,
			true, false
		);

	}

}

void CEditPaletteDlg::OnLiveUpdates() 
{
	m_bLiveUpdates = (0 != IsDlgButtonChecked(IDC_LIVE_UPDATES));

	HandleLiveUpdate();

	UpdateScrollBars();
}

void CEditPaletteDlg::OnChangeREdit() 
{
	CScrollBar *pScrollBar = (CScrollBar *)GetDlgItem(IDC_R_SCROLLBAR);

	m_rValue = GetDlgItemInt(IDC_R_EDIT);
	
	pScrollBar->SetScrollPos(m_rValue);

	UpdateColorPreview();
}

void CEditPaletteDlg::OnChangeGEdit() 
{
	CScrollBar *pScrollBar = (CScrollBar *)GetDlgItem(IDC_G_SCROLLBAR);

	m_gValue = GetDlgItemInt(IDC_G_EDIT);

	pScrollBar->SetScrollPos(m_gValue);

	UpdateColorPreview();
}

void CEditPaletteDlg::OnChangeBEdit() 
{
	CScrollBar *pScrollBar = (CScrollBar *)GetDlgItem(IDC_B_SCROLLBAR);

	m_bValue = GetDlgItemInt(IDC_B_EDIT);

	pScrollBar->SetScrollPos(m_bValue);

	UpdateColorPreview();
}

void CEditPaletteDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int pos = pScrollBar->GetScrollPos();

	switch (nSBCode) {

	case SB_LINELEFT:
		pos += -1;
		break;

	case SB_LINERIGHT:
		pos += 1;
		break;

	case SB_PAGELEFT:
		pos += -10;
		break;

	case SB_PAGERIGHT:
		pos += 10;
		break;

	case SB_THUMBTRACK:
		pos = nPos;

	}

	if (pScrollBar == GetDlgItem(IDC_R_SCROLLBAR)) {

		SetDlgItemInt(IDC_R_EDIT, pos);

	} else if (pScrollBar == GetDlgItem(IDC_G_SCROLLBAR)) {

		SetDlgItemInt(IDC_G_EDIT, pos);

	} else if (pScrollBar == GetDlgItem(IDC_B_SCROLLBAR)) {

		SetDlgItemInt(IDC_B_EDIT, pos);

	}

	pScrollBar->SetScrollPos(pos);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEditPaletteDlg::OnFromParent() 
{
	BeginUndo();

	bool bSelected = (0 != IsDlgButtonChecked(IDC_FROM_PARENT));

	DWORD setFlags = (bSelected) ? BPT::CAnnotatedPalette::PFLAG_FROM_PARENT : 0;

	DWORD clearFlags = (!bSelected) ? BPT::CAnnotatedPalette::PFLAG_FROM_PARENT : 0;

	bool bChange = false;

	for ( int i = 0; i < 256; i++ ) {

		if ( IsSlotSelected( i ) ) {

			m_pEditedPalette->ChangeSlotFlags( i, setFlags, clearFlags );

			bChange = true;

		}

	}

	if ( bChange ) {
	
		HandleLiveUpdate();

		UpdateScrollBars();

	}

}

void CEditPaletteDlg::OnBnClickedUndo()
{
	HandleUndo();
}
