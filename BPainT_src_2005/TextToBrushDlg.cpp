// TextToBrushDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "TextToBrushDlg.h"

#include "BpaintDoc.h"
#include "BPTTools.h"
#include "BPTRasterOps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextToBrushDlg dialog


CTextToBrushDlg::CTextToBrushDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextToBrushDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextToBrushDlg)
	//}}AFX_DATA_INIT
	m_strText = _T("");

	m_bUserSelectedAFont = false;

	memset( &m_SelectedLogicalFont, 0, sizeof(m_SelectedLogicalFont) );

}


void CTextToBrushDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextToBrushDlg)
	DDX_Control(pDX, IDC_TEXT_EDIT, m_GreedyEditCtrl);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_TEXT_EDIT, m_strText);
}


BEGIN_MESSAGE_MAP(CTextToBrushDlg, CDialog)
	//{{AFX_MSG_MAP(CTextToBrushDlg)
	ON_BN_CLICKED(IDC_FONT_BUTTON, OnFontButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextToBrushDlg message handlers

// ============================================================================

bool 
CBpaintDoc::TextToBrushDialogHandler( CWnd * pParent )
{
	CTextToBrushDlg dlg( pParent );

// ----------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// put the dialog box info

	dlg.m_strText = "Hello world!";

	// Set the font settings to the system font
	// ------------------------------------------------------------------------

	HFONT hSystemFont = (HFONT)GetStockObject(SYSTEM_FONT);
	
	if ( hSystemFont ) {

		GetObject( 
			hSystemFont, sizeof(dlg.m_SelectedLogicalFont), 
			&dlg.m_SelectedLogicalFont 
		);

	}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ----------------------------------------------------------------------------

	if ( IDOK == dlg.DoModal() ) {

		// Gather all the information from the dialog to
		// --------------------------------------------------------------------

		CFont selectedFont;

		if ( !selectedFont.CreateFontIndirect( &dlg.m_SelectedLogicalFont ) ) {

			TRACE( "Failed to create CFont object\n" );

			AfxMessageBox( "Failed to create CFont object?", MB_ICONERROR | MB_OK );

			return false;

		}

// ----------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// get the dialog box info
// This needs to determine the size of the bitmap to render to...

#if 1

		// For now just make it the canvas size

		CSize textBitmapSize = m_CanvasSize;

#else

		CSize textBitmapSize( 1, 1 );
		
		{
			CDC measureTextDC;
	
			if ( measureTextDC.CreateCompatibleDC( NULL ) ) {

				CFont * pOldFont = measureTextDC.SelectObject( &selectedFont );

				textBitmapSize = measureTextDC.GetTextExtent( dlg.m_strText );

				CRect measureRect = CRect( CPoint(0,0), textBitmapSize );

				measureTextDC.DrawText(
					dlg.m_strText, &measureRect, 
					(DT_NOPREFIX | DT_TABSTOP | DT_CALCRECT) | (4 << 24)
				);

				textBitmapSize.cx = max( measureRect.Height(), measureRect.Width());
				textBitmapSize.cy = textBitmapSize.cx;

				measureTextDC.SelectObject( pOldFont );

			}

		}

#endif

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ----------------------------------------------------------------------------

		// Create the dib that the text will be rendered into
		// --------------------------------------------------------------------

		dib_type textDib;

		if ( !textDib.BPTDIB_DEBUG_CREATE( textBitmapSize.cx, textBitmapSize.cy, 0 ) ) {

			TRACE( "Failed to create dib section %dx%d\n", textBitmapSize.cx, textBitmapSize.cy  );

			AfxMessageBox( "Failed to create temporary dib?", MB_ICONERROR | MB_OK );

			return false;

		}

		// Clear the dib section with black text should be rendered in white
		// --------------------------------------------------------------------

		dib_type::bitmap_type * pDibBitmap = textDib.GetBitmapPtr();

		if ( !pDibBitmap ) {

			AfxMessageBox( "Unable to get dib bitmap?", MB_ICONERROR | MB_OK );

			return false;

		}

		pDibBitmap->ClearBuffer( 0 );

		// Perform the rendering using GDI 
		// --------------------------------------------------------------------

		{
			// Create the DC for rendering
			// ----------------------------------------------------------------

			CDC memDC;
	
			if ( !memDC.CreateCompatibleDC( NULL ) ) {

				AfxMessageBox( "Failed to create rendering CDC!", MB_ICONERROR | MB_OK );

				return false;

			}

			// ----------------------------------------------------------------

			CFont * pOldFont = memDC.SelectObject( &selectedFont );
	
			// Select the bitmap as the render surface
			// ----------------------------------------------------------------

			CBitmap * pOldBitmap = memDC.SelectObject(
				CBitmap::FromHandle( textDib.GetHBitmap() )
			);
	
			// Perform the rendering here
			// ----------------------------------------------------------------

			memDC.SetBkMode( TRANSPARENT );
			memDC.SetTextColor( RGB( 255, 255, 255 ) );

// ----------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// This should be replaced with something that formats the text properly...

			memDC.DrawText(
				dlg.m_strText, 
				CRect( CPoint(0,0), textBitmapSize ), 
				(DT_NOPREFIX | DT_TABSTOP) | (4 << 24)
			);

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ----------------------------------------------------------------------------

			// Put the dc back the way it was for proper disposal and were done
			// ----------------------------------------------------------------

			memDC.SelectObject( pOldBitmap );
			memDC.SelectObject( pOldFont );

		}

		// Create the editor bitmap that the dib will be transfered to
		// --------------------------------------------------------------------

		editor_bitmap_type brushBitmap;

		if ( !brushBitmap.Create( textBitmapSize.cx, textBitmapSize.cy ) ) {

			TRACE( "Failed to create editor bitmap %dx%d\n", textBitmapSize.cx, textBitmapSize.cy  );

			AfxMessageBox( "Failed to create brush bitmap", MB_ICONERROR | MB_OK );

			return false;

		}

		brushBitmap.ClearBuffer( m_ChromaKey );

		// Transfer the dib contents to the editor bitmap type
		// --------------------------------------------------------------------

		BPT::TSingleColorTransparentSrcTransferROP<
			editor_pixel_type, dib_type::bitmap_type::pixel_type
		> top( editor_pixel_type( m_Color_L ), dib_type::bitmap_type::pixel_type( 0 ) );

		BPT::T_Blit( brushBitmap, 0, 0, *pDibBitmap, top );

		// Find the bounding rectangle for the brush so it's only as big
		// as it needs to be.
		// --------------------------------------------------------------------

		CRect captureRect;

		BPT::T_FindBoundingRect(
			captureRect, brushBitmap, 
			BPT::TIsNotValue<editor_pixel_type>( m_ChromaKey ), 0
		);
				
		if ( !captureRect.IsRectEmpty() ) {

			// Create the captured rect.
			// --------------------------------------------------------------------

			editor_bitmap_type boundedBitmap;

			CSize boundedTextBrushSize = captureRect.Size();

			if ( !boundedBitmap.Create( boundedTextBrushSize.cx, boundedTextBrushSize.cy ) ) {
	
				TRACE( "Failed to create bounded editor bitmap %dx%d\n", boundedTextBrushSize.cx, boundedTextBrushSize.cy  );
	
				AfxMessageBox( "Failed to create bounded brush bitmap", MB_ICONERROR | MB_OK );
	
				return false;
	
			}

			// Blit the 'active' contents from the work bitmap.
			// --------------------------------------------------------------------

			BPT::T_Blit(
				boundedBitmap, -captureRect.left, -captureRect.top, 
				brushBitmap, paint_copy_op_type()
			);

			// Finally set the brush
			// --------------------------------------------------------------------
	
			if ( !m_pPickupBrushMediator->ExternalSetBrushToBitmap(
				&boundedBitmap, m_ChromaKey, false ) ) {
	
				AfxMessageBox( "Failed to set brush?", MB_ICONERROR | MB_OK );
	
				return false;
	
			}

		}

		return true;

	}

	return false;
}

// ============================================================================

BOOL CTextToBrushDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextToBrushDlg::OnFontButton() 
{
	// ------------------------------------------------------------------------

	CFontDialog dlg( NULL, CF_EFFECTS | CF_SCREENFONTS, NULL, this );

	if ( IDOK != dlg.DoModal() ) {

		return;

	}

	dlg.GetCurrentFont( &m_SelectedLogicalFont );

	m_bUserSelectedAFont = true;

}
