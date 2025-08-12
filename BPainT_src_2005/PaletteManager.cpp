// PaletteManager.cpp : header file
//
//	Copyright (c) 2002, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "PaletteManager.h"
#include "EnterStringDlg.h"
#include "BPTpcxio.h"

// CPaletteManager dialog

IMPLEMENT_DYNAMIC(CPaletteManager, CDialog)
CPaletteManager::CPaletteManager(CWnd* pParent /*=NULL*/)
	: CDialog(CPaletteManager::IDD, pParent)
{
	m_pDoc = 0;
	m_pShowcase = 0;
	m_pDefaultPalette = 0;
	m_pSelectedPalette = 0;
}

CPaletteManager::~CPaletteManager()
{
}

void CPaletteManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PALETTE_LIST, m_PaletteList);
}


BEGIN_MESSAGE_MAP(CPaletteManager, CDialog)
	ON_BN_CLICKED(IDC_EDIT_PALETTE, OnBnClickedEditPalette)
	ON_BN_CLICKED(IDC_NEW_PALETTE, OnBnClickedNewPalette)
	ON_BN_CLICKED(IDC_COPY_PALETTE, OnBnClickedCopyPalette)
	ON_BN_CLICKED(IDC_DELETE_PALETTE, OnBnClickedDeletePalette)
	ON_LBN_SELCHANGE(IDC_PALETTE_LIST, OnLbnSelchangePaletteList)
	ON_LBN_DBLCLK(IDC_PALETTE_LIST, OnLbnDblclkPaletteList)
	ON_BN_CLICKED(IDC_RENAME_PALETTE, OnBnClickedRenamePalette)
	ON_BN_CLICKED(IDC_LOAD_PALETTE, OnBnClickedLoadPalette)
END_MESSAGE_MAP()


// CPaletteManager message handlers

void CPaletteManager::OnBnClickedEditPalette()
{
	if ( m_pDoc && m_pSelectedPalette ) {

		m_pDoc->OnEditPalette2( this, m_pSelectedPalette, false, false );

		GeneratePalettePreview();

		UpdatePalettePreview();

		SetModifiedFlag();

	}
}

void CPaletteManager::OnBnClickedNewPalette()
{
	if ( m_pShowcase ) {

		// setup the dialog
		// --------------------------------------------------------------------

		CEnterStringDlg dlg(this);

		dlg.m_Title = "New Palette...";

		dlg.m_DescriptionString = "Enter Palette Name:";

		dlg.m_EditString.Format( "Palette (%04d)", m_pShowcase->PaletteCount() );

		if ( IDOK != dlg.DoModal() ) {

			return;

		}

		// --------------------------------------------------------------------

		BPT::CAnnotatedPalette * pPalette = m_pShowcase->CreatePalette();

		if ( pPalette ) {

			pPalette->SetName( dlg.m_EditString );

			m_pSelectedPalette = pPalette;

			FillPaletteListbox();

			SyncButtonsForSelectedPalette();

			SetModifiedFlag();

		}

	}
}

void CPaletteManager::OnBnClickedCopyPalette()
{
	if ( m_pShowcase && m_pSelectedPalette ) {

		// setup the dialog
		// --------------------------------------------------------------------

		CEnterStringDlg dlg(this);

		dlg.m_Title = "Copy Palette...";

		dlg.m_DescriptionString = "Enter Palette Name:";

		char * pSrcName = m_pSelectedPalette->GetName();

		if ( pSrcName ) {

			dlg.m_EditString.Format( "Copy of \"%.32s\"", pSrcName );

		} else {

			dlg.m_EditString.Format( "Copy of (%p)", m_pSelectedPalette );

		}

		if ( IDOK != dlg.DoModal() ) {

			return;

		}

		// --------------------------------------------------------------------

		BPT::CAnnotatedPalette * pPalette = m_pShowcase->DuplicatePalette( m_pSelectedPalette );

		if ( pPalette ) {

			pPalette->SetName( dlg.m_EditString );

			m_pSelectedPalette = pPalette;

			FillPaletteListbox();

			SyncButtonsForSelectedPalette();

			SetModifiedFlag();

		}

	}
}

void CPaletteManager::OnBnClickedDeletePalette()
{
	if (
		m_pShowcase 
		&& m_pSelectedPalette
		&& (1 == m_pSelectedPalette->RefCount())
		&& (m_pSelectedPalette != m_pDefaultPalette)
	) {

		if ( IDYES != MessageBox(
			"You CAN'T UNDO this operation continue?",
			"Confirm Delete Palette", MB_YESNO | MB_ICONQUESTION ) ) {

			return;

		}

		m_pShowcase->AbandonPalette( m_pSelectedPalette );

		m_pSelectedPalette = m_pDefaultPalette;

		SetModifiedFlag();

		FillPaletteListbox();

		SyncButtonsForSelectedPalette();

	}
}

bool CPaletteManager::AddPaletteToList( BPT::CAnnotatedPalette * pPalette )
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

	int item = m_PaletteList.AddString( pszName );

	if ( LB_ERR == item ) {

		return false;

	}

	m_PaletteList.SetItemData( item, (DWORD)pPalette );

	// ----------------------------------------------------------------

	if ( pPalette == m_pSelectedPalette ) {

		m_PaletteList.SetCurSel( item );

	}

	return true;

}

bool CPaletteManager::FillPaletteListbox()
{
	m_PaletteList.ResetContent();

	// Add the default palette
	// ------------------------------------------------------------------------

	if ( m_pDefaultPalette ) {

		if ( !AddPaletteToList( m_pDefaultPalette ) ) {

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

	return true;
}

BOOL CPaletteManager::OnInitDialog()
{
	CDialog::OnInitDialog();

	// get the default palette to make things easier

	if ( m_pShowcase ) {

		m_pDefaultPalette = m_pShowcase->GetDefaultPalette();

	} else {

		m_pDefaultPalette = 0;

	}

	if ( !m_pSelectedPalette ) {
		
		m_pSelectedPalette = m_pDefaultPalette;

	}

	// make our dib for the palette preview

	CWnd * pPreviewWnd = GetDlgItem( IDC_PALETTE_PREVIEW );

	if ( pPreviewWnd ) {

		CRect previewClient;
		
		pPreviewWnd->GetClientRect( &previewClient );

		m_PalettePreviewDib.Create( previewClient.Size() );

	}

	// fill in the palette listbox

	FillPaletteListbox();

	SyncButtonsForSelectedPalette();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaletteManager::EnableDlgItem( const int nID, const bool bEnable ) {

	CWnd * pWnd = GetDlgItem( nID );

	if ( pWnd ) {

		pWnd->EnableWindow( bEnable ? TRUE : FALSE );

	}
}

void CPaletteManager::SyncButtonsForSelectedPalette()
{
	EnableDlgItem( IDC_LOAD_PALETTE, (0 != m_pSelectedPalette) );
	EnableDlgItem( IDC_NEW_PALETTE, (0 != m_pShowcase) );
	EnableDlgItem( IDC_EDIT_PALETTE, (0 != m_pDoc) && (0 != m_pSelectedPalette) );
	EnableDlgItem( IDC_COPY_PALETTE, (0 != m_pSelectedPalette) );

	if (
		m_pSelectedPalette
		&& (m_pSelectedPalette != m_pDefaultPalette)
	) {

		EnableDlgItem( IDC_RENAME_PALETTE, true );

		// Only allow unused palettes to be removed!
		// yes this isn't all that friendly, but until there
		// is a catalog of what objects are holding outstanding
		// references to the object this is needed.

		EnableDlgItem( IDC_DELETE_PALETTE, (1 == m_pSelectedPalette->RefCount()) );

	} else {

		EnableDlgItem( IDC_DELETE_PALETTE, false );
		EnableDlgItem( IDC_RENAME_PALETTE, false );

	}

	// Finally update the palette preview

	GeneratePalettePreview();

	UpdatePalettePreview();
}

void CPaletteManager::OnLbnSelchangePaletteList()
{
	int item = m_PaletteList.GetCurSel();

	if ( (LB_ERR != item) && m_pShowcase ) {

		BPT::CAnnotatedPalette * pPal = (BPT::CAnnotatedPalette *)m_PaletteList.GetItemDataPtr( item );

		if ( ((BPT::CAnnotatedPalette *)-1) != pPal ) {

			m_pSelectedPalette = pPal;

			SyncButtonsForSelectedPalette();

		}

	}
}

void CPaletteManager::OnLbnDblclkPaletteList()
{
	OnLbnSelchangePaletteList();
	OnBnClickedEditPalette();
}

void CPaletteManager::OnBnClickedRenamePalette()
{
	if ( m_pSelectedPalette ) {

		// setup the dialog
		// --------------------------------------------------------------------

		CEnterStringDlg dlg(this);

		dlg.m_Title = "Rename Palette...";

		dlg.m_DescriptionString = "Enter Palette Name:";

		char * pSrcName = m_pSelectedPalette->GetName();

		if ( pSrcName ) {

			dlg.m_EditString = pSrcName;

		} else {

			dlg.m_EditString.Format( "Palette (%p)", m_pSelectedPalette );

		}

		if ( IDOK != dlg.DoModal() ) {

			return;

		}

		SetModifiedFlag();

		m_pSelectedPalette->SetName( dlg.m_EditString );

		FillPaletteListbox();

	}
}

// ----------------------------------------------------------------------------

//
//	CPaletteManager::GeneratePalettePreview()
//

void 
CPaletteManager::GeneratePalettePreview()
{
	dib_type::bitmap_type * pBitmap = m_PalettePreviewDib.GetBitmapPtr();

	if ( pBitmap ) {

		pBitmap->ClearBuffer( ~0 );

		if ( !m_pSelectedPalette ) {

			return;

		}

		int w = pBitmap->Width() / 16;

		int h = pBitmap->Height() / 16;

		int y = (pBitmap->Height() - (h * 16)) / 2;

		int xReset = (pBitmap->Width() - (w * 16)) / 2;

		CRect rect( xReset, y, xReset + w, y + h );

		for ( int r = 0; r < 16; r++ ) {

			rect.left = xReset;

			rect.right = xReset + w;

			int slot = r;

			for ( int c = 0; c < 16; c++ ) {

				BPT::T_SolidRectPrim(
					*pBitmap
					,rect
					,m_pSelectedPalette->GetCLUTValue( slot )
					,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
				);

				slot += 16;

				rect.OffsetRect( w, 0 );

			}

			rect.OffsetRect( 0, h );

		}

	}
}

//
//	CPaletteManager::UpdatePalettePreview()
//

void 
CPaletteManager::UpdatePalettePreview()
{
	CWnd * pPreviewWnd = GetDlgItem( IDC_PALETTE_PREVIEW );

	if ( pPreviewWnd && m_PalettePreviewDib.GetHBitmap() ) {

		CDC * pDC = pPreviewWnd->GetDC();

		if ( pDC ) {

			BPT::T_BlitAt( pDC->GetSafeHdc(), 0, 0, m_PalettePreviewDib );

			pPreviewWnd->ReleaseDC( pDC );

		}

	}

}

// ----------------------------------------------------------------------------

void CPaletteManager::OnBnClickedLoadPalette()
{
	if ( !m_pSelectedPalette ) return;

	CFileDialog opf( 
		TRUE, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_LONGNAMES, 
		_T("Bitmap Files (*.bmp)|*.bmp||"),
		this
	);

	if ( IDOK != opf.DoModal() ) {

		return;

	}

	BPT::C8bppPicture * pPictureHack = BPT::Load8BppPicture( 
		opf.GetFileName( )
	);

	if ( pPictureHack ) {

		int nTotalColors = min( BPT::C8bppPicture::PALETTE_ENTRIES, m_pSelectedPalette->TotalColors() );

		for ( int i = 0; i < nTotalColors; i++ ) {
		
			m_pSelectedPalette->SetSlot(
				i
				,pPictureHack->m_PaletteData[ i ].rgbRed
				,pPictureHack->m_PaletteData[ i ].rgbGreen
				,pPictureHack->m_PaletteData[ i ].rgbBlue
			);

		}

		// need to ask the editor to init itself with this picture...

		delete pPictureHack;

		// Make sure that the palettes are managed (catch all implmentation...)
		// --------------------------------------------------------------------

		if ( m_pShowcase ) {

			m_pShowcase->ManageSharedPaletteEntries(); // 6/17/01

		}

		SetModifiedFlag();

		SyncButtonsForSelectedPalette();
	
	} else {

		MessageBox(
			"BMP loader didn't like that file (must be 256 colors)",
			"Oops!", MB_OK | MB_ICONWARNING | MB_TASKMODAL
		);

	}

}

//
//	CPaletteManager::SetModifiedFlag()
//

void 
CPaletteManager::SetModifiedFlag()
{
	if ( m_pDoc ) {

		m_pDoc->SetModifiedFlag();

	}
}

