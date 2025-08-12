// SimplePaletteCtrl.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "SimplePaletteCtrl.h"

#if 1 // hack!
#include "BPTPrimitives.h"
#include "BPTRasterOps.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

char CSimplePaletteCtrl::s_ClassName[] = "BPT_SimplePaletteCtrl";

/////////////////////////////////////////////////////////////////////////////
// CSimplePaletteCtrl

CSimplePaletteCtrl::CSimplePaletteCtrl() : m_pMediator( 0 ), m_ElementSize( 0, 0 )
{
	m_hCursor = AfxGetApp()->LoadCursor( IDC_PALETTE_CURSOR ); //IDC_STENCIL_CURSOR );

	RegisterWindowClass();

	m_PaletteArea.SetRectEmpty();
}

CSimplePaletteCtrl::~CSimplePaletteCtrl()
{
	if ( m_hCursor ) {

		DeleteObject( (HGDIOBJ) m_hCursor );

	}
}

BEGIN_MESSAGE_MAP(CSimplePaletteCtrl, CWnd)
	//{{AFX_MSG_MAP(CSimplePaletteCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Create helpers

BOOL CSimplePaletteCtrl::Create(CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle /*=WS_VISIBLE*/)
{
	return CWnd::Create(s_ClassName, _T(""), dwStyle, rect, pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// Internal methods

BOOL CSimplePaletteCtrl::RegisterWindowClass()
{
	WNDCLASS wndcls;

	HINSTANCE hInst = AfxGetInstanceHandle();

	if ( !(::GetClassInfo(hInst, s_ClassName, &wndcls)) ) {

		// otherwise we need to register a new class

		wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc      = ::DefWindowProc;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = hInst;
		wndcls.hIcon            = NULL;
		wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = s_ClassName;
		
		if ( !AfxRegisterClass(&wndcls) ) {
		
			AfxThrowResourceException();
			return FALSE;
		
		}

    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSimplePaletteCtrl message handlers

BOOL CSimplePaletteCtrl::OnEraseBkgnd(CDC* pDC) 
{

	if ( m_DisplayDib.GetHBitmap() ) {

		return TRUE;

	}
	
	return CWnd::OnEraseBkgnd(pDC);
}

void CSimplePaletteCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if ( m_DisplayDib.GetHBitmap() ) {

		CRect clientRect;

		GetClientRect( &clientRect );

		float xScale = 1.0f; //(float)clientRect.Width() / (float) m_DisplayDib.Width();
		float yScale = 1.0f; //(float)clientRect.Height() / (float) m_DisplayDib.Height();

		BPT::T_BlitAt(
			dc.GetSafeHdc(), 0, 0, m_DisplayDib,
			xScale, yScale, 0 
		);

	} else {

		dc.PatBlt(
			dc.m_ps.rcPaint.left, dc.m_ps.rcPaint.top, 
			dc.m_ps.rcPaint.right - dc.m_ps.rcPaint.left,
			dc.m_ps.rcPaint.bottom - dc.m_ps.rcPaint.top,
			WHITENESS
		);

	}

	// Do not call CWnd::OnPaint() for painting messages
}

void CSimplePaletteCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

	CWnd::PreSubclassWindow();

	// Create the bitmap
	// ------------------------------------------------------------------------

	CRect rect;

	GetClientRect( &rect );

	CSize rectSize = rect.Size();

	m_DisplayDib.BPTDIB_DEBUG_CREATE( rectSize.cx, rectSize.cy, 0 );

	dib_type::bitmap_type * pBits = m_DisplayDib.GetBitmapPtr();

	if ( pBits ) {

		// ------------------------------------------------------------------------

		CRect rect( CPoint( 0, 0 ), pBits->Size() );
	
		BPT::T_RectPrim(
			*pBits, rect, 0,
			BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
		);

		// ------------------------------------------------------------------------

		rect.InflateRect( -1, -1 );

#if 0


		BPT::T_DrawEmbossedRect(
			*pBits, rect, 1, 0, ~0, 0,
			BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
		);

#else

		BPT::T_SolidRectPrim(
			*pBits, rect, ~0,
			BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
		);

#endif

	}

	// Fill in the important support values
	// ------------------------------------------------------------------------

	CSize borderSize( 2, 2 );

	CSize adjustedRectSize(
		(rectSize.cx - (borderSize.cx * 2)),
		(rectSize.cy - (borderSize.cy * 2))
	);

	m_ElementSize.cx = adjustedRectSize.cx / 16;
	m_ElementSize.cy = adjustedRectSize.cy / 16;

	CSize palSize( m_ElementSize.cx * 16, m_ElementSize.cy * 16 );

	m_PaletteArea.left = borderSize.cx + (adjustedRectSize.cx - palSize.cx) / 2;
	m_PaletteArea.top = borderSize.cy + (adjustedRectSize.cy - palSize.cy) / 2;
	m_PaletteArea.right = m_PaletteArea.left + palSize.cx;
	m_PaletteArea.bottom = m_PaletteArea.top + palSize.cy;

	// ------------------------------------------------------------------------

	RedrawPaletteBitmap( 0 );
	
}

void CSimplePaletteCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}


void CSimplePaletteCtrl::PostNcDestroy() 
{
	m_DisplayDib.Destroy();
	
	CWnd::PostNcDestroy();
}

// ----------------------------------------------------------------------------

//
//	CSimplePaletteCtrl::RedrawPaletteBitmap()
//

void CSimplePaletteCtrl::RedrawPaletteBitmap( const CRect * pClipRect )
{

	if ( !m_pMediator || (0 == m_ElementSize.cx) ) {

		return;

	}

	dib_type::bitmap_type * pBits = m_DisplayDib.GetBitmapPtr();

	if ( pBits ) {

		// Clear out the area
		// --------------------------------------------------------------------

		if ( pClipRect ) {

			BPT::T_SolidRectPrim(
				*pBits
				,*pClipRect
				,~0
				,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
				,pClipRect
			);

		} else {

			BPT::T_SolidRectPrim(
				*pBits
				,m_PaletteArea
				,~0
				,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
				,pClipRect
			);

		}

		// Render the palette
		// --------------------------------------------------------------------

		CSize renderSize( min( m_ElementSize.cx, m_ElementSize.cy ) - 1, m_ElementSize.cy - 1 );

		for ( int i = 0; i < 16; i++ ) {

			int x = m_PaletteArea.left + (i * m_ElementSize.cx);

			for ( int j = 0; j < 16; j++ ) {

				// ------------------------------------------------------------

				int y = m_PaletteArea.top + (j * m_ElementSize.cy);

				int slot = i * 16 + j;

				DWORD dwStyle = m_pMediator->GetColorStyleBits( slot );

				dib_type::bitmap_type::pixel_type colorValue = m_pMediator->GetDisplayColorForPaletteSlot( slot );

				CPoint pt( x, y );

				CRect colorRect( pt, renderSize );

				CRect originalColorRect = colorRect;

				CRect wholeRect( CPoint( x, y ), CSize( m_ElementSize.cx - 1, m_ElementSize.cy ) );

				// Adjust color rectangle
				// ------------------------------------------------------------

				if ( CPaletteDataMediator::SELECTED & dwStyle ) {

					BPT::T_RectPrim(
						*pBits
						,colorRect
						,0
						,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
						,pClipRect
					);

					colorRect.InflateRect( -1, -1 );

#if 0

					BPT::T_RectPrim(
						*pBits
						,colorRect
						,~0
						,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
						,pClipRect
					);


#endif
					colorRect.InflateRect( -1, -1 );

				}

				// render element
				// ------------------------------------------------------------

				BPT::T_SolidRectPrim(
					*pBits
					,colorRect
					,colorValue
					,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
					,pClipRect
				);


#if 1 // BPT 6/19/01

				// Show marked colors (yes this is ugly so what)
				// ------------------------------------------------------------

				if ( CPaletteDataMediator::MARKED & dwStyle ) {

					BPT::T_RectPrim(
						*pBits
						,originalColorRect
						,0
						,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
						,pClipRect
					);

					BPT::T_SolidEllipsePrim(
						*pBits
						,originalColorRect.CenterPoint()
						,(originalColorRect.Width()*1/4) - 1
						,(originalColorRect.Height()*1/4) - 1
						,0
						,BPT::TInvertDstTransferROP< dib_type::bitmap_type::pixel_type >()
						,pClipRect

					);

				}
#endif

				// Render selection information
				// ------------------------------------------------------------

				if ( CPaletteDataMediator::SELECTED & dwStyle ) {

					int xp = colorRect.right + 2;
					int yp = ((colorRect.bottom - 1) + colorRect.top) / 2;

					int cx = m_ElementSize.cx - renderSize.cx;

					int w = min( cx, m_ElementSize.cy );

					int count = (w) / 2;

					w -= 1;

					if ( 0 < w ) {

						// draw middle line
	
						BPT::T_HLinePrim(
							*pBits, xp, yp, w
							,0 
							,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
							,pClipRect
						);
	
						// draw the others
	
						for ( int i = 1; i <= count; i++ ) {
		
							BPT::T_HLinePrim(
								*pBits, xp + i, yp - i, ((count - i) + 1) //w - i
								,0 
								,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
								,pClipRect
							);
	
							BPT::T_HLinePrim(
								*pBits, xp + i, yp + i, ((count - i) + 1) //w - i
								,0 
								,BPT::TCopyROP< dib_type::bitmap_type::pixel_type >()
								,pClipRect
							);
	
						}

					}

				}

			}

		}

	}

}

// ----------------------------------------------------------------------------

//
//	CSimplePaletteCtrl::CalcSlotRect()
//

void
CSimplePaletteCtrl::CalcSlotRect( CRect & rect, const int slot )
{
	int x = ((slot / 16) * m_ElementSize.cx) + m_PaletteArea.left;
	int y = ((slot % 16) * m_ElementSize.cy) + m_PaletteArea.top;

	rect = CRect( CPoint( x, y ), m_ElementSize );
}

//
//	CSimplePaletteCtrl::InvalidateSlot();
//

void CSimplePaletteCtrl::InvalidateSlot( const int slot, const bool bUpdate )
{
	CRect rect;

	CalcSlotRect( rect, slot );

	rect.InflateRect( 1, 1 );

	RedrawPaletteBitmap( &rect );

	InvalidateRect( &rect, FALSE );

	if ( bUpdate ) {

		UpdateWindow();

	}
}

//
//	CSimplePaletteCtrl::GetSlotFromPoint()
//

int CSimplePaletteCtrl::GetSlotFromPoint( CPoint point )
{
	int x = max( m_PaletteArea.left, min( point.x, (m_PaletteArea.right - 1)) );
	int y = max( m_PaletteArea.top, min( point.y, (m_PaletteArea.bottom - 1)) );

	x = (x - m_PaletteArea.left) / m_ElementSize.cx;
	y = (y - m_PaletteArea.top) / m_ElementSize.cy;

	return (x * 16) + y;
}

void CSimplePaletteCtrl::InvalidateWholePalette( const bool bUpdate )
{
	RedrawPaletteBitmap( 0 );

	InvalidateRect( &m_PaletteArea, FALSE );

	if ( bUpdate ) {

		UpdateWindow();

	}
}

// ----------------------------------------------------------------------------

//
//	CSimplePaletteCtrl::OnLButtonDown()
//

void CSimplePaletteCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int slot = GetSlotFromPoint( point );

	SetCapture();

	if ( m_pMediator ) {

		m_pMediator->OnLButtonDown( slot, (this == GetCapture()), nFlags, point );

	}

}

//
//	CSimplePaletteCtrl::OnLButtonUp()
//

void CSimplePaletteCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int slot = GetSlotFromPoint( point );

	if ( m_pMediator ) {

		m_pMediator->OnLButtonUp( slot, (this == GetCapture()), nFlags, point );

	}

	if ( this == GetCapture() ) {

		ReleaseCapture();

	}
	
}

//
// CSimplePaletteCtrl::OnRButtonDown()
//

void CSimplePaletteCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int slot = GetSlotFromPoint( point );

	SetCapture();

	if ( m_pMediator ) {

		m_pMediator->OnRButtonDown( slot, (this == GetCapture()), nFlags, point );

	}

}

//
//	CSimplePaletteCtrl::OnRButtonUp()
//

void CSimplePaletteCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	int slot = GetSlotFromPoint( point );

	if ( m_pMediator ) {

		m_pMediator->OnRButtonUp( slot, (this == GetCapture()), nFlags, point );

	}

	if ( this == GetCapture() ) {

		ReleaseCapture();

	}

}

//
//	CSimplePaletteCtrl::OnMouseMove()
//

void CSimplePaletteCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	int slot = GetSlotFromPoint( point );

	if ( m_pMediator ) {

		m_pMediator->OnMouseMove( slot, (this == GetCapture()), nFlags, point );

	}
}

//
//	CSimplePaletteCtrl::OnCancelMode()
//

void CSimplePaletteCtrl::OnCancelMode() 
{
	CWnd::OnCancelMode();
	
	if ( m_pMediator ) {

		m_pMediator->OnCancelMode( this == GetCapture() );

	}

	if ( this == GetCapture() ) {

		ReleaseCapture();

	}
}

// ----------------------------------------------------------------------------

void CSimplePaletteCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int slot = GetSlotFromPoint( point );

	if ( m_pMediator ) {

		m_pMediator->OnLButtonDblClk( slot, (this == GetCapture()), nFlags, point );

	}
}

void CSimplePaletteCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	int slot = GetSlotFromPoint( point );

	if ( m_pMediator ) {

		m_pMediator->OnRButtonDblClk( slot, (this == GetCapture()), nFlags, point );

	}
}

BOOL CSimplePaletteCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default

	if ( HTCLIENT == nHitTest ) {

		if ( m_hCursor ) {

			SetCursor( m_hCursor );

		}

		return TRUE;

	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
