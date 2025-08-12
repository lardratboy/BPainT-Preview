// SimplePaletteView.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//

#include "stdafx.h"
#include "bpaint.h"

#include "bpaintDoc.h"
#include "SimplePaletteView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define USE_EMBOSSED_OUTLINE

/////////////////////////////////////////////////////////////////////////////
// CSimplePaletteView

IMPLEMENT_DYNCREATE(CSimplePaletteView, CView)

CSimplePaletteView::CSimplePaletteView() : 
	m_ElementSize( 11, 11 )
	,m_Overhead( 32 )
	,m_Last_L_Color( -1 )
	,m_Last_R_Color( -1 )
	,m_hGenericPaletteCrsr( 0 )
	,m_hChoose_L_Crsr( 0 )
	,m_hChoose_R_Crsr( 0 )
	,m_ButtonMode( CSimplePaletteView::BUTTON_MODE::NONE )
{
	m_PaletteArea.SetRectEmpty();
	m_StatusArea.SetRectEmpty();
}

CSimplePaletteView::~CSimplePaletteView()
{
}


BEGIN_MESSAGE_MAP(CSimplePaletteView, CView)
	//{{AFX_MSG_MAP(CSimplePaletteView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CANCELMODE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

//
//	CSimplePaletteView::GetColorForSlot()
//

CSimplePaletteView::dib_type::bitmap_type::pixel_type 
CSimplePaletteView::GetColorForSlot( const int slot )
{
	CBpaintDoc * pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	return pDoc->GetDisplayColorForPaletteSlot( slot );
}

//
//	CSimplePaletteView::RedrawPaletteBitmap()
//

void CSimplePaletteView::RedrawPaletteBitmap( const CRect * pRect )
{
	CBpaintDoc * pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	dib_type::bitmap_type * pBits = m_DisplayDib.GetBitmapPtr();

	if ( pBits ) {

		// Clear out the area
		// --------------------------------------------------------------------

		if ( pRect ) {

			BPT::T_SolidRectPrim(
				*pBits
				,*pRect
				,~0
				,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
				,pRect
			);

		} else {

			pBits->ClearBuffer( ~0 );

		}

		// Render the palette
		// --------------------------------------------------------------------

		CSize renderSize( m_ElementSize.cx - 1, m_ElementSize.cy - 1 );

		for ( int i = 0; i < 16; i++ ) {

			int x = m_PaletteArea.left + (i * m_ElementSize.cx);

			for ( int j = 0; j < 16; j++ ) {

				// ------------------------------------------------------------

				int y = m_PaletteArea.top + (j * m_ElementSize.cy);

				int color = i * 16 + j;

				dib_type::bitmap_type::pixel_type colorValue = pDoc->GetDisplayColorForPaletteSlot( color );

//				dib_type::bitmap_type::pixel_type colorValue = GetColorForSlot( color ); // BPT 6/16/01

				CRect colorRect( CPoint( x, y ), renderSize );

				if ( color == pDoc->GetClearColor() ) {

					colorRect.InflateRect( -1, -1 );

				}

				if ( (color == m_Last_L_Color) || (color == m_Last_R_Color) ) {

					colorRect.InflateRect( -1, -1 );

					BPT::T_RectPrim(
						*pBits
						,colorRect
						,0
						,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
						,pRect
					);

					colorRect.InflateRect( -1, -1 );

					if ( color == m_Last_R_Color ) {

						colorRect.InflateRect( -1, -1 );

					}

				}

				// render element
				// ------------------------------------------------------------

				BPT::T_SolidRectPrim(
					*pBits
					,colorRect
					,colorValue
					,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
					,pRect
				);

			}

		}

		// Render the selected colors
		// --------------------------------------------------------------------

		dib_type::bitmap_type::pixel_type lColor = GetColorForSlot( m_Last_L_Color );

		BPT::T_SolidRectPrim(
			*pBits
			,m_LStatusRect
			,lColor
			,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
			,pRect
		);

		if ( -1 != m_Last_L_Color ) {

			DrawSelectionForSlot( pBits, m_Last_L_Color, pRect );

		}

		// --------------------------------------------------------------------

		dib_type::bitmap_type::pixel_type rColor = GetColorForSlot( m_Last_R_Color );

		BPT::T_SolidRectPrim(
			*pBits
			,m_RStatusRect
			,rColor
			,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
			,pRect
		);

		if ( -1 != m_Last_R_Color ) {

			DrawSelectionForSlot( pBits, m_Last_R_Color, pRect );

		}

	}
}

//
//	CSimplePaletteView::DrawSelectionForSlot()
//

void
CSimplePaletteView::DrawSelectionForSlot(
	dib_type::bitmap_type * pBits, const int slot,
	const CRect * pClipRect
)
{
	CRect slotRect;

	CalcColorRect( slotRect, slot );

	--slotRect.left;
	--slotRect.top;

	BPT::T_RectPrim(
		*pBits
		,slotRect
		,0
		,BPT::TInvertDstTransferROP< dib_type::bitmap_type::pixel_type >()
		,pClipRect
	);

}

// ----------------------------------------------------------------------------

//
//	CSimplePaletteView::SetElementSize()
//

bool CSimplePaletteView::SetElementSize( const CSize size, const int overhead, const bool adjustFrameWnd )
{
	// Set the internal vars
	// ------------------------------------------------------------------------

	m_Overhead = overhead;

	CSize bitmapSize( 1 + size.cx * 16, 2 + size.cy * 16 + m_Overhead );

	m_PaletteArea = CRect( CPoint( 1, 1 ), CSize( size.cx * 16, size.cy * 16 ) );

	m_StatusArea = CRect( CPoint( 1, m_PaletteArea.bottom ), CSize( bitmapSize.cx, m_Overhead ) );

	CSize halfSize( ((m_StatusArea.Width() / 2) - 1), m_StatusArea.Height() );

	m_LStatusRect = CRect( CPoint( 1, m_StatusArea.top ), halfSize );
	m_RStatusRect = CRect( CPoint( m_StatusArea.Width() - halfSize.cx - 1, m_StatusArea.top ), halfSize );

	m_ElementSize = size;

	// Create the internal bitmap
	// ------------------------------------------------------------------------

	if ( !m_DisplayDib.BPTDIB_DEBUG_CREATE( bitmapSize.cx, bitmapSize.cy, 0 ) ) {

		return false;

	}

	RedrawPaletteBitmap( 0 );

	// Set the parent size to reflect the size of the bitmap...
	// ------------------------------------------------------------------------

	if ( adjustFrameWnd ) {

		CWnd * pWnd = GetParentFrame();
	
		if ( pWnd ) {

			// -----------------------------------------------------------------

			CRect viewRect( CPoint( 0, 0 ), bitmapSize );

			AdjustWindowRectEx( &viewRect, GetStyle(), FALSE, GetExStyle() );

			CSize viewSize = viewRect.Size();

			// -----------------------------------------------------------------

			CRect rect( CPoint( 0, 0 ), viewSize );
	
			AdjustWindowRectEx(
				&rect, pWnd->GetStyle(), (0 != pWnd->GetMenu()), pWnd->GetExStyle()
			);
	
			int w = rect.Width();
			int h = rect.Height();
	
			pWnd->SetWindowPos( 0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER );

		}

	}

	return true;

}

/////////////////////////////////////////////////////////////////////////////
// CSimplePaletteView drawing

void CSimplePaletteView::OnDraw(CDC* pDC)
{
	CRect clipBox;

	if ( NULLREGION != pDC->GetClipBox( &clipBox ) ) {

		if ( m_DisplayDib.GetHBitmap() ) {

			BPT::T_BlitAt( pDC->GetSafeHdc(), 0, 0, m_DisplayDib );

#if 1
			{
				int oldBKMode = pDC->SetBkMode( TRANSPARENT );
//				int oldTAMode = pDC->SetTextAlign( TA_CENTER | TA_BASELINE );
//				int oldRop = pDC->SetROP2( R2_NOT );

				DWORD dtFlags = DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE;

				// ------------------------------------------------------------

				if ( -1 != m_Last_L_Color ) {

					CPoint lc = m_LStatusRect.CenterPoint();

					CRect rect = m_LStatusRect;
	
					CString lStr;
					lStr.Format( "%03d", m_Last_L_Color );

					// fun

					CSize textSize = pDC->GetTextExtent( lStr );

					textSize.cx += 8;

					CRect border(
						CPoint(
							rect.left + (rect.Width() - textSize.cx) / 2,
							rect.top + (rect.Height() - textSize.cy) / 2
						),
						textSize
					);

					pDC->PatBlt(
						border.left + 1, border.top + 1,
						border.Width() - 2, border.Height() - 2,
						WHITENESS
					);

#if defined(USE_EMBOSSED_OUTLINE)
					pDC->DrawEdge( border, EDGE_SUNKEN, BF_RECT );
#else
					pDC->Rectangle( border );
#endif

					pDC->DrawText( lStr, rect, dtFlags );

				}

				// ------------------------------------------------------------

				if ( -1 != m_Last_R_Color ) {

					CPoint rc = m_RStatusRect.CenterPoint();

					CRect rect = m_RStatusRect;
	
					CString rStr;
					rStr.Format( "%03d", m_Last_R_Color );

					// fun

					CSize textSize = pDC->GetTextExtent( rStr );

					textSize.cx += 8;

					CRect border(
						CPoint(
							rect.left + (rect.Width() - textSize.cx) / 2,
							rect.top + (rect.Height() - textSize.cy) / 2
						),
						textSize
					);

					pDC->PatBlt(
						border.left + 1, border.top + 1,
						border.Width() - 2, border.Height() - 2,
						WHITENESS
					);

#if defined(USE_EMBOSSED_OUTLINE)
					pDC->DrawEdge( border, EDGE_SUNKEN, BF_RECT );
#else
					pDC->Rectangle( border );
#endif

					pDC->DrawText( rStr, rect, dtFlags );


				}

				// ------------------------------------------------------------

//				pDC->SetROP2( oldRop );
//				pDC->SetTextAlign( oldTAMode );
				pDC->SetBkMode( oldBKMode );

			}
#endif

		} else {

			pDC->PatBlt(
				clipBox.left, clipBox.top, 
				clipBox.right - clipBox.left,
				clipBox.bottom - clipBox.top,
				BLACKNESS
			);
	
		}

	}

}

/////////////////////////////////////////////////////////////////////////////
// CSimplePaletteView diagnostics

#ifdef _DEBUG
void CSimplePaletteView::AssertValid() const
{
	CView::AssertValid();
}

void CSimplePaletteView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBpaintDoc* CSimplePaletteView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBpaintDoc)));
	return (CBpaintDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

//
//	CSimplePaletteView::CalcColorRect()
//

void
CSimplePaletteView::CalcColorRect( CRect & rect, const int slot )
{
	int x = ((slot / 16) * m_ElementSize.cx) + m_PaletteArea.left;
	int y = ((slot % 16) * m_ElementSize.cy) + m_PaletteArea.top;

	rect = CRect( CPoint( x, y ), m_ElementSize );
}

//
//	CSimplePaletteView::InvalidateColor()
//

void 
CSimplePaletteView::InvalidateColor( const int slot, const bool bUpdate )
{
	if ( -1 != slot ) {

		CRect rect;
	
		CalcColorRect( rect, slot );
	
		--rect.left;
		--rect.top;
	
		RedrawPaletteBitmap( &rect );
	
		InvalidateRect( rect, false );
	
		if ( bUpdate ) {
	
			UpdateWindow();
	
		}

	}

}

/////////////////////////////////////////////////////////////////////////////
// CSimplePaletteView message handlers

void CSimplePaletteView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if ( pHint ) {

		if ( pHint->IsKindOf( RUNTIME_CLASS( CColorChange ) ) ) {

			switch ( lHint ) {

			default:
			case CColorChange::WHOLE_PALETTE_CHANGE:
				InvalidateRect( NULL, false );
				RedrawPaletteBitmap( 0 );
				UpdateWindow();

#if 1 // BPT 6/17/01
{				
				CString title = GetDocument()->GetCurrentPaletteSourceName() + " : " + GetDocument()->GetTitle();
			
				SetWindowText( title );
			
				GetParentFrame()->SetWindowText( title );
}
#endif // BPT 6/17/01

				break;

			case CColorChange::L_COLOR_CHANGE:
#if 1
				m_Last_L_Color = ((CColorChange *)pHint)->GetSlot();
				InvalidateRect( NULL, false );
				RedrawPaletteBitmap( 0 );
				UpdateWindow();
#else
				InvalidateRect( m_LStatusRect, false );
				RedrawPaletteBitmap( &m_LStatusRect );
				InvalidateColor( m_Last_L_Color, false );
				m_Last_L_Color = ((CColorChange *)pHint)->GetSlot();
				InvalidateColor( m_Last_L_Color, true );
#endif
				break;

			case CColorChange::R_COLOR_CHANGE:
#if 1
				m_Last_R_Color = ((CColorChange *)pHint)->GetSlot();
				InvalidateRect( NULL, false );
				RedrawPaletteBitmap( 0 );
				UpdateWindow();
#else
				InvalidateRect( m_RStatusRect, false );
				RedrawPaletteBitmap( &m_RStatusRect );
				InvalidateColor( m_Last_R_Color, false );
				m_Last_R_Color = ((CColorChange *)pHint)->GetSlot();
				InvalidateColor( m_Last_R_Color, true );
#endif
				break;

			}

		}

	}
}

void CSimplePaletteView::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CView::PreSubclassWindow();
}

void CSimplePaletteView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	CBpaintDoc* pDoc = GetDocument();

#if 1 // try to change the parent frames styles

	CWnd * pWnd = GetParentFrame();

	if ( pWnd ) {

		pWnd->ModifyStyle(
			// remove
			WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME,
			// add
			WS_BORDER
		);

		pWnd->ModifyStyleEx(
			0, WS_EX_PALETTEWINDOW | WS_EX_TOOLWINDOW
		);

#if 0

		if ( pDoc ) {

			pDoc->EnsureProperMenu( this );

		}

#else

		CMenu * pMenu = pWnd->GetMenu();

		if ( pMenu ) {

			pWnd->SetMenu( 0 );

			if ( pMenu ) {

				pMenu->DestroyMenu();

			}

		}

#endif

	}

#endif

	// ------------------------------------------------------------------------

	if ( pDoc ) {

		m_Last_L_Color = pDoc->Get_L_Color();
		m_Last_R_Color = pDoc->Get_R_Color();

	}

	SetElementSize( m_ElementSize, m_ElementSize.cy * 4, (0 != pWnd) );

	// ------------------------------------------------------------------------
	
	CString title = "Palette ( " + GetDocument()->GetTitle() + " )";

	SetWindowText( title );

	GetParentFrame()->SetWindowText( title );

}

BOOL CSimplePaletteView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class

	cs.style &= ~WS_BORDER;
	cs.dwExStyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);
	
	return CView::PreCreateWindow(cs);
}

// ----------------------------------------------------------------------------

int CSimplePaletteView::GetColorFromPoint( CPoint point )
{

	int x = max( m_PaletteArea.left, min( point.x, (m_PaletteArea.right - 1)) );
	int y = max( m_PaletteArea.top, min( point.y, (m_PaletteArea.bottom - 1)) );

	x = (x - m_PaletteArea.left) / m_ElementSize.cx;
	y = (y - m_PaletteArea.top) / m_ElementSize.cy;

	return max(0,min(255,((x * 16) + y)));

}

void CSimplePaletteView::HandleColorSelection( UINT nFlags, CPoint point ) 
{
	// what color is the user clicking on/over
	// ------------------------------------------------------------------------

	int color = GetColorFromPoint( point );

	// Set the color?
	// ------------------------------------------------------------------------

	if ( this == GetCapture() ) {

		CBpaintDoc * pDoc = GetDocument();

		ASSERT_VALID(pDoc);

		if ( MK_LBUTTON & nFlags ) {

			if ( color != m_Last_L_Color ) {

				pDoc->Request_L_ColorChange( color );

			}

		}

		if ( MK_RBUTTON & nFlags ) {

			if ( color != m_Last_R_Color ) {

				pDoc->Request_R_ColorChange( color );

			}

		}

	}

}

// ----------------------------------------------------------------------------

void CSimplePaletteView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CBpaintDoc * pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	HandleColorSelection( nFlags, point );

	pDoc->Request_ColorRGBEdit( GetColorFromPoint( point ), this );
}

void CSimplePaletteView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_ButtonMode = CSimplePaletteView::BUTTON_MODE::LEFT;

	SetCapture();

	HandleColorSelection( nFlags, point );

}

void CSimplePaletteView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_ButtonMode = CSimplePaletteView::BUTTON_MODE::NONE;

	HandleColorSelection( nFlags, point );

	if ( this == GetCapture() ) {

		ReleaseCapture();

	}

}

void CSimplePaletteView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_ButtonMode = CSimplePaletteView::BUTTON_MODE::RIGHT;

	SetCapture();

	HandleColorSelection( nFlags, point );
}

void CSimplePaletteView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_ButtonMode = CSimplePaletteView::BUTTON_MODE::NONE;

	HandleColorSelection( nFlags, point );

	if ( this == GetCapture() ) {

		ReleaseCapture();

	}

}

void CSimplePaletteView::OnMouseMove(UINT nFlags, CPoint point) 
{
	HandleColorSelection( nFlags, point );

	if ( this == GetCapture() ) {

		OnSetCursor( this, HTCLIENT, WM_MOUSEMOVE );

	}
}

// ----------------------------------------------------------------------------

BOOL CSimplePaletteView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default

	if ( HTCLIENT == nHitTest ) {

		CPoint pt;

		GetCursorPos( &pt );

		ScreenToClient( &pt );

		if ( m_PaletteArea.PtInRect( pt ) ) {

			switch ( m_ButtonMode ) {

			case CSimplePaletteView::BUTTON_MODE::LEFT:
				if ( m_hChoose_L_Crsr ) {

					SetCursor( m_hChoose_L_Crsr );
					return TRUE;

				}
				break;

			case CSimplePaletteView::BUTTON_MODE::RIGHT:
				if ( m_hChoose_R_Crsr ) {

					SetCursor( m_hChoose_R_Crsr );
					return TRUE;

				}
				break;

			}

			if ( m_hGenericPaletteCrsr ) {

				SetCursor( m_hGenericPaletteCrsr );

				return TRUE;

			}

		}

	}
	
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CSimplePaletteView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

int CSimplePaletteView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_hGenericPaletteCrsr = AfxGetApp()->LoadCursor( IDC_PALETTE_CURSOR );
	m_hChoose_L_Crsr = AfxGetApp()->LoadCursor( IDC_CHOOSE_L_COLOR );
	m_hChoose_R_Crsr = AfxGetApp()->LoadCursor( IDC_CHOOSE_R_COLOR );
	
	return 0;
}

void CSimplePaletteView::OnDestroy() 
{
	if ( m_hChoose_R_Crsr ) DeleteObject( (HGDIOBJ) m_hChoose_R_Crsr );
	if ( m_hChoose_L_Crsr ) DeleteObject( (HGDIOBJ) m_hChoose_L_Crsr );
	if ( m_hGenericPaletteCrsr ) DeleteObject( (HGDIOBJ) m_hGenericPaletteCrsr );

	CBpaintDoc * pDoc = GetDocument();

	if ( pDoc ) {

		pDoc->OnDestroyPaletteView();

	}
	
	CView::OnDestroy();
}

void CSimplePaletteView::OnCancelMode() 
{
	CView::OnCancelMode();
	
	if ( this == GetCapture() ) {

		ReleaseCapture();

	}

	m_ButtonMode = CSimplePaletteView::BUTTON_MODE::NONE;
}

