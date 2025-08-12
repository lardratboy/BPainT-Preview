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
#include "MakeStencilDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMakeStencilDlg dialog


CMakeStencilDlg::CMakeStencilDlg( CBpaintDoc * pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CMakeStencilDlg::IDD, pParent), m_PalDataMediator( pDoc )
{

	m_LastCount = -1;
	m_LastSlot = -1;

	m_bLButtonDown = false;
	m_dwActiveRemoveBits = 0;
	m_dwActiveAddBits = 0;

	m_PalDataMediator.SetOwner( this );

	m_PaletteCtrl.SetMediator( &m_PalDataMediator );

	//{{AFX_DATA_INIT(CMakeStencilDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMakeStencilDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMakeStencilDlg)
    DDX_Control(pDX, IDC_SELECTED_COLORS, m_PaletteCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMakeStencilDlg, CDialog)
	//{{AFX_MSG_MAP(CMakeStencilDlg)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_INVERT, OnInvert)
	ON_BN_CLICKED(IDC_USED, OnUsed)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMakeStencilDlg message handlers

void CMakeStencilDlg::OnClear() 
{
	for ( int slot = 0; slot < 256; slot++ ) {

		DWORD dwFlags = m_PalDataMediator.GetColorStyleBits( slot );

		if ( mediator_type::SELECTED & dwFlags ) {

			m_PalDataMediator.ModifyColorStyleBits( slot, mediator_type::SELECTED, 0 );

		}

	}

	Palette_UpdateInfoMessage( m_LastSlot );

	m_PaletteCtrl.InvalidateWholePalette( true );
}

void CMakeStencilDlg::OnInvert() 
{
	for ( int slot = 0; slot < 256; slot++ ) {

		DWORD dwFlags = m_PalDataMediator.GetColorStyleBits( slot );

		if ( mediator_type::SELECTED & dwFlags ) {

			m_PalDataMediator.ModifyColorStyleBits( slot, mediator_type::SELECTED, 0 );

		} else {

			m_PalDataMediator.ModifyColorStyleBits( slot, 0, mediator_type::SELECTED );

		}

	}

	Palette_UpdateInfoMessage( m_LastSlot );

	m_PaletteCtrl.InvalidateWholePalette( true );
}

void CMakeStencilDlg::OnUsed() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CMakeStencilDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CMakeStencilDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	Palette_UpdateInfoMessage( 0 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMakeStencilDlg::Palette_UpdateInfoMessage( const int slot ) {

	// ------------------------------------------------------------------------

	if ( m_LastSlot != slot ) {

		CString text;

		text.Format( "Over %d", slot );

		SetDlgItemText( IDC_SLOT_TEXT, text );

		m_LastSlot = slot;

	}

	// ------------------------------------------------------------------------

	int count = 0;

	for ( int counter = 0; counter < 256; counter++ ) {

		DWORD dwFlags = m_PalDataMediator.GetColorStyleBits( counter );

		if ( mediator_type::SELECTED & dwFlags ) {

			++count;

		}

	}

	if ( m_LastCount != count ) {

		CString text;

		text.Format( "Count %d", count );

		SetDlgItemText( IDC_COUNT_TEXT, text );

		m_LastCount = count;

	}

}

