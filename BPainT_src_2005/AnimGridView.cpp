// AnimGridView.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"

#include "AnimGridView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ----------------------------------------------------------------------------

//#define ANIMGRID_ELEMENT_SIZE_SCROLLING

#define ANIMGRID_SCROLL_DELAY		(200)
#define USE_FAKE_INDICATOR_BITMAP

/////////////////////////////////////////////////////////////////////////////
// CAnimGridView

IMPLEMENT_DYNCREATE(CAnimGridView, CScrollView)

CAnimGridView::CAnimGridView() : 
	m_ElementSize( 0, 0 ), 
	m_ElementBorderSize( 0, 0 ),
	m_ThumbnailSize( 0, 0 ),
	m_ElementFrameSize( 0, 0 ),
	m_nScrollControlTimer( 0 )
{
	m_hCrossCrsr = 0;

	m_ClipRect.SetRectEmpty();

	m_BorderColor = (16 << 10) | (16 << 5) | (16); // 0

	m_EmptyCelColor = ~0;

#if 1 // BPT 5/25/01

	m_lastBeginFrame = -1;
	m_lastEndFrame = -1;
	m_lastBeginLayer = -1;
	m_lastEndLayer = -1;

#endif

	// ------------------------------------------------------------------------

	int r1 = 30;
	int g1 = 30;
	int b1 = 30;

	int r2 = 22;
	int g2 = 22;
	int b2 = 22;

	int r3 = 3;
	int g3 = 3;
	int b3 = 3;

	m_HeaderHColor = (r1 << 10) | (g1 << 5) | b1;
	m_HeaderBColor = (r2 << 10) | (g2 << 5) | b2;
	m_HeaderSColor = (r3 << 10) | (g3 << 5) | b3;

	// ------------------------------------------------------------------------

	r1 = 0;
	g1 = 18;
	b1 = 31;

	r2 = 0;
	g2 = 14;
	b2 = 24;

	r3 = 0;
	g3 = 4;
	b3 = 8;

	m_SelectedHeaderHColor = (r1 << 10) | (g1 << 5) | b1;
	m_SelectedHeaderBColor = (r2 << 10) | (g2 << 5) | b2;
	m_SelectedHeaderSColor = (r3 << 10) | (g3 << 5) | b3;

	// ------------------------------------------------------------------------

	r1 = 31;
	g1 = 18;
	b1 = 12;

	r2 = r1/2;
	g2 = g1/2;
	b2 = b1/2;

	r3 = r2/2;
	g3 = g2/2;
	b3 = b2/2;

#if 1

	m_MultiSelectionHColor = m_SelectedHeaderHColor;
	m_MultiSelectionBColor = m_SelectedHeaderBColor;
	m_MultiSelectionSColor = m_SelectedHeaderSColor;

#else

	m_MultiSelectionHColor = (r1 << 10) | (g1 << 5) | b1;
	m_MultiSelectionBColor = (r2 << 10) | (g2 << 5) | b2;
	m_MultiSelectionSColor = (r3 << 10) | (g3 << 5) | b3;

#endif

	// ------------------------------------------------------------------------

	m_StandardHeaderLevel = 1;

	m_CornerHeaderLevel = m_StandardHeaderLevel + 1;


#if 1 // BPT 6/27/01

#if defined( DEMO_SAVE_DISABLED ) // BPT 7/1/01

	m_bShowVisualElementIDs = (0 != GLOBAL_GetSettingInt( "bShowVisualElementIDs", 1 ));

#else

	m_bShowVisualElementIDs = (0 != GLOBAL_GetSettingInt( "bShowVisualElementIDs", 0 ));

#endif

#endif

}

CAnimGridView::~CAnimGridView()
{

#if 1 // BPT 6/27/01

	GLOBAL_PutSettingInt( "bShowVisualElementIDs", (m_bShowVisualElementIDs) ? 1 : 0 );

#endif

}


BEGIN_MESSAGE_MAP(CAnimGridView, CScrollView)
	//{{AFX_MSG_MAP(CAnimGridView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_GRID_SHOW_UIDS, OnGridShowUids)
	ON_UPDATE_COMMAND_UI(ID_GRID_SHOW_UIDS, OnUpdateGridShowUids)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimGridView drawing

void CAnimGridView::OnDraw(CDC* pDC)
{
	CRect clipBox;

	if ( NULLREGION != pDC->GetClipBox( &clipBox ) ) {

		if ( m_DisplayDib.GetHBitmap() ) {

			// Make sure our display canvas is setup and correct
			// ----------------------------------------------------------------
	
			CRect lpClipBox( clipBox );
	
			pDC->LPtoDP( &lpClipBox );
	
			EnsureCanvasRect( &clipBox, &lpClipBox );

			// ----------------------------------------------------------------

			CPoint scroll = GetScrollPosition();

			BPT::T_BlitAt(
				pDC->GetSafeHdc(), scroll.x, scroll.y, m_DisplayDib,
				1.0f, 1.0f, &m_DisplaySrcLimitRect
			);

		} else {

			pDC->PatBlt(
				clipBox.left, clipBox.top, 
				clipBox.Width(), clipBox.Height(), WHITENESS
			);
	
			CRect clientRect;
		
			GetClientRect( &clientRect );
		
			UINT dtFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
		
			pDC->DrawText( CString( "Future grid view." ), clientRect, dtFlags );

		}

	}
}

/////////////////////////////////////////////////////////////////////////////
// CAnimGridView diagnostics

#ifdef _DEBUG
void CAnimGridView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CAnimGridView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CBpaintDoc* CAnimGridView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBpaintDoc)));
	return (CBpaintDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------

namespace EXPERIMENTAL {

	// experimentation area

};

// ----------------------------------------------------------------------------

//
//	CAnimGridView::RenderRowHeader()
//

void
CAnimGridView::RenderRowHeader(
	bitmap_type * pBitmap, const int x, const int y,
	const int row, const RECT * pClip,
	CBpaintDoc* pDoc, CDC * pDC
)
{

	// -----------------------------------------------------

	CRect rect( CPoint( x, y ), CSize( m_HeaderSize.cx, m_ElementWithBorderSize.cy ) );

	bool bWasSelected = (row == m_pAnimation->CurrentLayer());

	bitmap_type::pixel_type HeaderHColor, HeaderBColor, HeaderSColor;

	if ( bWasSelected ) {

		HeaderHColor = m_SelectedHeaderHColor;
		HeaderBColor = m_SelectedHeaderBColor; 
		HeaderSColor = m_SelectedHeaderSColor;

	} else {

		HeaderHColor = m_HeaderHColor;
		HeaderBColor = m_HeaderBColor; 
		HeaderSColor = m_HeaderSColor;

	}

	// ------------------------------------------------------------------------

	BPT::T_DrawEmbossedRect(
		*pBitmap, rect, m_StandardHeaderLevel,
		HeaderHColor, HeaderBColor, HeaderSColor,
		BPT::TCopyROP< bitmap_type::pixel_type >(),
		pClip
	);

	// ------------------------------------------------------------------------

	if ( pDC ) {

		CRect infoRect = rect;

		CString str;

		if ( m_pAnimation ) {

			BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( row );

			if ( pLayerInfo ) {

				char * pName = pLayerInfo->GetName();

				if ( !pName ) {

					goto GENERATE_NAME;

				}

				if ( pLayerInfo->HasNote() ) {

					str.Format( "(*) %s", pName );

				} else {

					str.Format( "%s", pName );

				}

#if 1 // BPT 6/18/01

				if ( pLayerInfo->GetPalette() ) {

					CDC dc;

					if ( dc.CreateCompatibleDC( pDC ) ) {

						CBitmap * pOldBitmap = dc.SelectObject( &m_HasPaletteIndicator );

						pDC->BitBlt(
							rect.left + m_PalIndicatorOffset.x, 
							rect.top + m_PalIndicatorOffset.y, 
							m_PalIndicatorSize.cx, 
							m_PalIndicatorSize.cy, 
							&dc, 0, 0, SRCCOPY
						);

						dc.SelectObject( pOldBitmap );

					}

				}

#endif

			}

		} else {

	GENERATE_NAME:

			int set = (row / 26);
	
			if ( set ) {
		
				str.Format( "?%c%d", 'a' + (row % 26), set );
	
			} else {
	
				str.Format( "?%c", 'a' + (row % 26) );
	
			}

		}

		CRect textRect( CPoint( 0, 0 ), pDC->GetTextExtent( str ) );

		textRect.OffsetRect(
			infoRect.left + (infoRect.Width() - textRect.Width()) / 2,
			infoRect.top + (infoRect.Height() - textRect.Height()) / 2
		);

		int x = textRect.left;
		int y = textRect.top;

		if ( textRect.IntersectRect( &textRect, pClip ) ) {

			pDC->ExtTextOut( x, y, ETO_CLIPPED, &textRect, str, 0 );

		}

	}

	// -----------------------------------------------------

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::RenderThumbnail()
//

bool
CBpaintDoc::RenderThumbnail(
	composite_bitmap_type & canvasSizedWorkBitmap, composite_pixel_type backgroundColor,
	const int nFrame, const int nLayerStart, const int nLayerEnd,
	composite_bitmap_type * pBitmap, const RECT & location, 
	const RECT * pClip
)
{

#if 0

	return false;

#else

// This is a horrible horrible hack
// This is a horrible horrible hack
// This is a horrible horrible hack

	if ( m_bEnableRenderThumbnailsHack /* && (m_pAnimation->GetEndSelectedFrames() == -1) */ ) {

		composite_pixel_type chromaKey =  backgroundColor;

		canvasSizedWorkBitmap.ClearBuffer( chromaKey );
	
		BPT::TIsInRange<int> layerRangeFn( nLayerStart, nLayerEnd );

		T_RenderFrameLayersHelper(
			canvasSizedWorkBitmap, nFrame, layerRangeFn
		);

		composite_bitmap_type * pScaled;

		if ( m_bUseMoreAccurateThumbnailRenderer ) {

			//typedef BPT::T16bpp_5050_OP<composite_pixel_type,0x3def> thumbnail_top;
	
			typedef composite_copy_op_type thumbnail_top;
	
			pScaled = BPT::T_TransparentAwareCreateScaledBitmap<composite_bitmap_type,10>(
				canvasSizedWorkBitmap, m_ThumbnailSize.cx, m_ThumbnailSize.cy, chromaKey,
				thumbnail_top()
			);

		} else {

			pScaled = BPT::T_CreateScaledBitmap<composite_bitmap_type,10>(
				canvasSizedWorkBitmap, m_ThumbnailSize.cx, m_ThumbnailSize.cy
			);

		}
	
		if ( pScaled ) {
	
			BPT::T_Blit(
				*pBitmap, 
				location.left, location.top,
				*pScaled,
				BPT::TCopyROP< composite_bitmap_type::pixel_type >(),
				0, pClip
			);
	
			delete pScaled;
	
			return true;
	
		} else {
	
			return false;
	
		}

	} else {

		return false;
	
	}

// This is a horrible horrible hack
// This is a horrible horrible hack
// This is a horrible horrible hack

#endif

}

// ----------------------------------------------------------------------------

//
//	CAnimGridView::RenderThumbnail()
//

void __forceinline CAnimGridView::RenderThumbnail(
	CBpaintDoc* pDoc, const int nFrame, 
	const int nLayerStart, const int nLayerEnd,
	bitmap_type * pBitmap, const RECT & location, 
	const RECT * pClip
)
{
	if ( m_pAnimation && pDoc ) {

// NEED TO FIX THIS!!!
// NEED TO FIX THIS!!!
// NEED TO FIX THIS!!!
// NEED TO FIX THIS!!!

		CBpaintDoc::composite_pixel_type backgroundColor = MakeDisplayPixelType( 128, 128, 128 ); // BPT 6/15/01

// NEED TO FIX THIS!!!
// NEED TO FIX THIS!!!
// NEED TO FIX THIS!!!
// NEED TO FIX THIS!!!

		if ( pDoc->RenderThumbnail(
			m_TemporaryCanvasBitmap, backgroundColor, 
			nFrame, nLayerStart, nLayerEnd,
			pBitmap, location, pClip) ) {

			return;

		}

		BPT::T_SolidRectPrim(
			*pBitmap, 
			location,
			~m_EmptyCelColor, BPT::TCopyROP< bitmap_type::pixel_type >(),
			pClip
		);

	}

}

// ----------------------------------------------------------------------------

//#define ALT_LOOK_COL_HEADER

//
//	CAnimGridView::RenderColHeader()
//

void
CAnimGridView::RenderColHeader(
	bitmap_type * pBitmap, const int x, const int y,
	const int col, const RECT * pClip,
	CBpaintDoc* pDoc, CDC * pDC
)
{
	// -----------------------------------------------------

#if defined(ALT_LOOK_COL_HEADER)

	CRect rect( CPoint( x, y ), CSize( m_ElementSize.cx, m_HeaderSize.cy ) );

	CRect infoRect(
		CPoint(
			x + m_ElementFrameSize.cx - 1, y + m_ElementSize.cy
		), 
		CSize( 
			m_ElementSize.cx - m_ElementFrameSize.cx - 1, 
			m_HeaderSize.cy - (m_ElementSize.cy + m_ElementFrameSize.cy)
		)
	);

#else

	CRect rect( CPoint( x, y ), CSize( m_ElementSize.cx, m_ElementSize.cy ) );

	CRect infoRect(
		CPoint( x, y + m_ElementSize.cy ), 
		CSize( m_ElementSize.cx, m_HeaderSize.cy - m_ElementSize.cy )
	);

#endif

	// -----------------------------------------------------

	bool bWasSelected = (col == m_pAnimation->CurrentFrame());

	bitmap_type::pixel_type HeaderHColor, HeaderBColor, HeaderSColor;

	if ( bWasSelected ) {

		HeaderHColor = m_SelectedHeaderHColor;
		HeaderBColor = m_SelectedHeaderBColor; 
		HeaderSColor = m_SelectedHeaderSColor;

	} else {

		HeaderHColor = m_HeaderHColor;
		HeaderBColor = m_HeaderBColor; 
		HeaderSColor = m_HeaderSColor;

	}

#if defined(ALT_LOOK_COL_HEADER)

	// the outter box
	// ------------------------------------------------------------------------

	BPT::T_DrawEmbossedRect(
		*pBitmap, rect, 1,
		HeaderHColor, HeaderBColor, HeaderSColor,
		BPT::TCopyROP< bitmap_type::pixel_type >(),
		pClip
	);

	// thumbnail border
	// ------------------------------------------------------------------------

	BPT::T_DrawEmbossedRectOutline(
		*pBitmap,
		CRect(
			CPoint( x + m_ElementFrameSize.cx - 1, y + m_ElementFrameSize.cy - 1 ),
			CSize( m_ThumbnailSize.cx + 2, m_ThumbnailSize.cy + 2 )
		),
		HeaderSColor, HeaderBColor, HeaderHColor,
		BPT::TCopyROP< bitmap_type::pixel_type >(),
		pClip
	);

	// frame information
	// ------------------------------------------------------------------------

#if 1 

	BPT::T_DrawEmbossedRectOutline(
		*pBitmap,
		infoRect,
		HeaderSColor, HeaderBColor, HeaderHColor,
		BPT::TCopyROP< bitmap_type::pixel_type >(),
		pClip
	);

#endif

	// ------------------------------------------------------------------------

	if ( pDC ) {

		CString str;

		BPT::CFrame * pFrame = m_pAnimation->GetFramePtr( col );

		if ( pFrame ) {

			if ( pFrame->HasNote() || pFrame->HasLinkPointInfo() ) {
	
				str.Format( "%d!", col + 1 );

			} else {

				str.Format( "%d", col + 1 );

			}

		} else {

			str.Format( "%d", col + 1 );

		}

		CRect textRect( CPoint( 0, 0 ), pDC->GetTextExtent( str ) );

		textRect.OffsetRect(
			infoRect.left + (infoRect.Width() - textRect.Width()) / 2,
			infoRect.top + (infoRect.Height() - textRect.Height()) / 2
		);

		int x = textRect.left;
		int y = textRect.top;

		if ( textRect.IntersectRect( &textRect, pClip ) ) {

			pDC->ExtTextOut( x, y, ETO_CLIPPED, &textRect, str, 0 );

		}

	}

	// ------------------------------------------------------------------------

#else

	// Draw the thumbnail frame
	// ------------------------------------------------------------------------

	BPT::T_DrawEmbossedBorderRect(
		*pBitmap, 
		rect, 
		m_ElementFrameSize.cx - 2,
		HeaderHColor, HeaderBColor, HeaderSColor,
		BPT::TCopyROP< bitmap_type::pixel_type >(),
		pClip
	);

	// Draw the frame number
	// ------------------------------------------------------------------------

	BPT::T_DrawEmbossedRect(
		*pBitmap, infoRect, m_StandardHeaderLevel,
		HeaderHColor, HeaderBColor, HeaderSColor,
		BPT::TCopyROP< bitmap_type::pixel_type >(),
		pClip
	);

	// ------------------------------------------------------------------------

	if ( pDC ) {

		CString str;
	
		BPT::CFrame * pFrame = m_pAnimation->GetFramePtr( col );

		if ( pFrame ) {

			if ( pFrame->HasNote() || pFrame->HasLinkPointInfo()  ) {
	
				str.Format( "*%d", col + 1 );

			} else {

				str.Format( "%d", col + 1 );

			}

		} else {

			str.Format( "%d", col + 1 );

		}

		CRect textRect( CPoint( 0, 0 ), pDC->GetTextExtent( str ) );

		textRect.OffsetRect(
			infoRect.left + (infoRect.Width() - textRect.Width()) / 2,
			infoRect.top + (infoRect.Height() - textRect.Height()) / 2
		);

		int x = textRect.left;
		int y = textRect.top;

		if ( textRect.IntersectRect( &textRect, pClip ) ) {

			pDC->ExtTextOut( x, y, ETO_CLIPPED, &textRect, str, 0 );

		}

	}

	// ------------------------------------------------------------------------

#endif

	// Draw the frame preview thumbnail.
	// ------------------------------------------------------------------------


//	if (m_pAnimation->GetEndSelectedFrames() == -1) {
		
		RenderThumbnail(
			pDoc, col, 0, (m_GridSize.cy - 1),
			pBitmap, 
			CRect( 
				CPoint( x + m_ElementFrameSize.cx, y + m_ElementFrameSize.cy ), 
				m_ThumbnailSize
			),
			pClip
		);

//	}
	
	// Draw the border
	// -----------------------------------------------------

	if ( m_ElementBorderSize.cx ) {

		BPT::T_SolidRectPrim(
			*pBitmap, 
			CRect( CPoint( x + m_ElementSize.cx, y ), CSize( m_ElementBorderSize.cx, m_HeaderSize.cy ) ), 
			m_BorderColor, BPT::TCopyROP< bitmap_type::pixel_type >(),
			pClip
		);

	}

}

//
//	CAnimGridView::RenderCel()
//

void
CAnimGridView::RenderCel(
	bitmap_type * pBitmap, const int x, const int y,
	const int row, const int col, const RECT * pClip,
	CBpaintDoc* pDoc, CDC * pDC
)
{

	// Cheap bail out quick hack -- This of course is a 
	// lame fix for a higher level problem.
	// -----------------------------------------------------

	CRect fullRenderRect( CPoint( x, y ), m_ElementWithBorderSize );

	CRect possibleRect = fullRenderRect;

	if ( pClip ) {

		if ( !IntersectRect( &possibleRect, &possibleRect, pClip ) ) {

			return;

		}

	}

	// -----------------------------------------------------

	bool bWasSelected = false;

	bool bWasEmpty = true;
	
	if ( m_pAnimation ) {

		BPT::CLayer * pLayer = m_pAnimation->GetLayer( col, row );

		if ( pLayer ) {

			if ( pLayer->HasVisualElement() ) {

				bWasEmpty = false;

				CRect thumbnailRect( 
					CPoint( x + m_ElementFrameSize.cx, y + m_ElementFrameSize.cy ), 
					m_ThumbnailSize
				);

				if ( m_bShowVisualElementIDs ) { // BPT 6/27/01

					BPT::T_SolidRectPrim(
						*pBitmap, thumbnailRect, 
						m_EmptyCelColor, BPT::TCopyROP< bitmap_type::pixel_type >(),
						pClip
					);

				} else {

					RenderThumbnail(
						pDoc, col, row, row,
						pBitmap, 
						thumbnailRect,
						pClip
					);

				}

#if 1 // BPT 6/18/01

				BPT::CVisualElement * pVisualElement = pLayer->GetVisualElement();

				if ( pVisualElement ) {

					if ( m_bShowVisualElementIDs ) { // BPT 6/27/01

						CString str;

						str.Format( "%d", pVisualElement->ID() );

						CRect textRect( CPoint( 0, 0 ), pDC->GetTextExtent( str ) );
				
						textRect.OffsetRect(
							thumbnailRect.left + (thumbnailRect.Width() - textRect.Width()) / 2,
							thumbnailRect.top + (thumbnailRect.Height() - textRect.Height()) / 2
						);
				
						int x = textRect.left;
						int y = textRect.top;
				
						if ( textRect.IntersectRect( &textRect, pClip ) ) {
				
							pDC->ExtTextOut( x, y, ETO_CLIPPED, &textRect, str, 0 );
				
						}

					}

					if ( 3 >= pVisualElement->RefCount() ) {
	
						CDC dc;
		
						if ( dc.CreateCompatibleDC( pDC ) ) {
		
							CBitmap * pOldBitmap = dc.SelectObject( &m_ReusedCelIndicator );
		
							pDC->BitBlt(
								thumbnailRect.right - m_ReusedCelIndicatorSize.cx, 
								thumbnailRect.bottom - m_ReusedCelIndicatorSize.cy, 
								m_ReusedCelIndicatorSize.cx, 
								m_ReusedCelIndicatorSize.cy, 
								&dc, 0, 0, SRCCOPY
							);
		
							dc.SelectObject( pOldBitmap );
	
						}
	
					}

					pVisualElement->Release();

				}

#endif

			}

		} 

		bWasSelected = (row == m_pAnimation->CurrentLayer()) && 
			(col == m_pAnimation->CurrentFrame());

	}

	// -----------------------------------------------------

	if ( bWasEmpty ) {

		BPT::T_SolidRectPrim(
			*pBitmap, 
			CRect( CPoint( x, y ), m_ElementSize ), 
			m_EmptyCelColor, BPT::TCopyROP< bitmap_type::pixel_type >(),
			&possibleRect
		);

	}

	// -----------------------------------------------------

	if ( m_ElementBorderSize.cx ) {

		BPT::T_SolidRectPrim(
			*pBitmap, 
			CRect( CPoint( x + m_ElementSize.cx, y ), CSize( m_ElementBorderSize.cx, m_ElementSize.cy ) ), 
			m_BorderColor, BPT::TCopyROP< bitmap_type::pixel_type >(),
			&possibleRect
		);

	}

	// -----------------------------------------------------

	if ( m_ElementBorderSize.cy ) {

		BPT::T_SolidRectPrim(
			*pBitmap, 
			CRect( CPoint( x, y + m_ElementSize.cy ), CSize( m_ElementSize.cx + 1, m_ElementBorderSize.cy ) ), 
			m_BorderColor, BPT::TCopyROP< bitmap_type::pixel_type >(),
			&possibleRect
		);

	}

	// -----------------------------------------------------

	if ( bWasSelected ) {

		bitmap_type::pixel_type hColor = m_SelectedHeaderHColor;
		bitmap_type::pixel_type bColor = m_SelectedHeaderBColor;
		bitmap_type::pixel_type sColor = m_SelectedHeaderSColor;

#if 1 // BPT 5/25/01

		bool bInvertedColor = false;

		int bf = m_pAnimation->GetBeginSelectedFrames();
		int ef = m_pAnimation->GetEndSelectedFrames();
		int bl = m_pAnimation->GetBeginSelectedLayers();
		int el =  m_pAnimation->GetEndSelectedLayers();

		if ( (col >= bf && col <= ef) && (row >= bl && row <= el ) ) {

			bInvertedColor = true;

		} else if ( ((bf != ef) || (bl != el)) && (-1 != ef) && (-1 != el) ) {

			bInvertedColor = true;

		}

		if ( bInvertedColor ) {

			hColor = ~sColor;
			bColor = ~bColor;
			sColor = ~hColor;

		}

#endif

		BPT::T_DrawEmbossedBorderRect(
			*pBitmap, 
			CRect( CPoint( x, y ), m_ElementSize ), 
			m_ElementFrameSize.cx - 2,
			hColor, 
			bColor, 
			sColor,
			BPT::TCopyROP< bitmap_type::pixel_type >(),
			&possibleRect
		);

	} else {

		if (col >= m_pAnimation->GetBeginSelectedFrames() && col <= m_pAnimation->GetEndSelectedFrames()) {

			if (row >= m_pAnimation->GetBeginSelectedLayers() && row <= m_pAnimation->GetEndSelectedLayers()) {

				BPT::T_DrawEmbossedBorderRect(
					*pBitmap, 
					CRect( CPoint( x, y ), m_ElementSize ), 
					m_ElementFrameSize.cx - 2,
					m_MultiSelectionHColor, // m_SelectedHeaderHColor, 
					m_MultiSelectionBColor, // m_SelectedHeaderBColor, 
					m_MultiSelectionSColor, // m_SelectedHeaderSColor,
					BPT::TCopyROP< bitmap_type::pixel_type >(),
					&possibleRect
				);

				return;
				
			}
		}
		
		
		BPT::T_DrawEmbossedBorderRect(
			*pBitmap, 
			CRect( CPoint( x, y ), m_ElementSize ), 
			m_ElementFrameSize.cx - 2,
			m_EmptyCelColor, m_EmptyCelColor, m_EmptyCelColor,
			BPT::TCopyROP< bitmap_type::pixel_type >(),
			&possibleRect
		);

	}

}

// ----------------------------------------------------------------------------

//
//	CAnimGridView::RenderGrid()
//

void 
CAnimGridView::RenderGrid( const RECT * pOptionalClip /* = 0 */ )
{

	CBpaintDoc* pDoc = GetDocument();

	// Clip
	// ------------------------------------------------------------------------

	RECT clip;

	if ( pOptionalClip ) {

		if ( !IntersectRect( &clip, &m_DisplaySrcLimitRect, pOptionalClip ) ) {

			return;

		}

	} else {

		clip = m_DisplaySrcLimitRect;

	}

	// Render the grid elements
	// ------------------------------------------------------------------------

	CPoint scroll = GetScrollPosition();

	bitmap_type * pBitmap = m_DisplayDib.GetBitmapPtr();

	// moved these outside the 'if ( pBitmap )' to get code to compile in DEBUG mode
	// VisualStudio.NET was getting an internal compiler error.

	CDC * pDC = 0; 
	
	CBitmap* pOldBitmap = 0;

	if ( pBitmap ) {

		// Prepare the dc
		// --------------------------------------------------------------------
	
		CDC memDC;
	
	
		if ( memDC.CreateCompatibleDC( NULL ) ) {
	
			pDC = &memDC;

			// Select the bitmap as the render surface
	
			pOldBitmap = pDC->SelectObject(
				CBitmap::FromHandle( m_DisplayDib.GetHBitmap() )
			);
	
			// Set the text mode

			pDC->SetBkMode( TRANSPARENT );

//			pDC->SetTextAlign( TA_CENTER );
	
		} else {
	
			pDC = 0;
	
		}

#if defined( ANIMGRID_ELEMENT_SIZE_SCROLLING ) // grid alignment

		// This is a total hack to avoid display errors it should
		// be corrected in the future!!!!

		RECT hackResults[ 4 ];

		int hackCount = BPT::ReturnExternalClipRects(
			hackResults, &m_DisplaySrcLimitRect, &m_RealDisplaySrcLimitRect
		);

		for ( int hackCounter = 0; hackCounter < hackCount; hackCounter++ ) {

			OffsetRect( &hackResults[ hackCounter ], -scroll.x, -scroll.y );

			BPT::T_SolidRectPrim(
				*pBitmap, 
				hackResults[ hackCounter ], 
				m_EmptyCelColor, BPT::TCopyROP< bitmap_type::pixel_type >(),
				&clip
			);

		}

#endif

		// Render the corner.
		// --------------------------------------------------------------------

		CRect cornerClip;

		if ( cornerClip.IntersectRect( &m_CornerHeaderRect, &clip ) ) {
	
			BPT::T_DrawEmbossedRect(
				*pBitmap, m_CornerHeaderRect, m_CornerHeaderLevel,
				m_HeaderHColor, m_HeaderBColor, m_HeaderSColor,
				BPT::TCopyROP< bitmap_type::pixel_type >(),
				&cornerClip
			);

#if 1 // BPT 6/18/01

			if ( m_pAnimation && m_pAnimation->GetPalette() ) {

				CDC dc;

				if ( dc.CreateCompatibleDC( pDC ) ) {

					CBitmap * pOldBitmap = dc.SelectObject( &m_HasPaletteIndicator );

					pDC->BitBlt(
						m_CornerHeaderRect.left + m_PalIndicatorOffset.x, 
						m_CornerHeaderRect.top + m_PalIndicatorOffset.y, 
						m_PalIndicatorSize.cx, 
						m_PalIndicatorSize.cy, 
						&dc, 0, 0, SRCCOPY
					);

					dc.SelectObject( pOldBitmap );

				}

			}

#endif

		}

		// Render the cels (the clip rect should be modified...)
		// --------------------------------------------------------------------

		CRect celClipRect;

		if ( celClipRect.IntersectRect( &clip, m_CelsRect ) ) {

			int c1 = (celClipRect.left + scroll.x - m_CelsRect.left) / m_ElementWithBorderSize.cx;
			int c2 = (celClipRect.right + scroll.x + m_ElementWithBorderSize.cx - 1 - m_CelsRect.left) / m_ElementWithBorderSize.cx;
			int r1 = (celClipRect.top + scroll.y - m_CelsRect.top) / m_ElementWithBorderSize.cy;
			int r2 = (celClipRect.bottom + scroll.y + m_ElementWithBorderSize.cy - 1 - m_CelsRect.top) / m_ElementWithBorderSize.cy;
			int cx = m_ElementWithBorderSize.cx;
			int cy = m_ElementWithBorderSize.cy;
			int y = (m_CelsRect.top - scroll.y) + (r1 * cy);
		
			for ( int r = r1; r < r2; r++, y += cy ) {
		
				int x = (m_CelsRect.left - scroll.x) + (c1 * cx);
		
				for ( int c = c1; c < c2; c++, x += cx ) {
	
					RenderCel( pBitmap, x, y, r, c, &celClipRect, pDoc, pDC );
	
				}
		
			}

		}

		// Render the row headers
		// --------------------------------------------------------------------

		CRect clippedRowHeaderRect;

		if ( clippedRowHeaderRect.IntersectRect( &clip, m_RowHeaderRect ) ) {

			int cy = m_ElementWithBorderSize.cy;
			int r1 = (clippedRowHeaderRect.top + scroll.y - m_RowHeaderRect.top) / m_ElementWithBorderSize.cy;
			int r2 = (clippedRowHeaderRect.bottom + scroll.y + m_ElementWithBorderSize.cy - 1 - m_RowHeaderRect.top) / m_ElementWithBorderSize.cy;
			int y = (m_CelsRect.top - scroll.y) + (r1 * cy);

			for ( int r = r1; r < r2; r++, y += cy ) {

				RenderRowHeader( pBitmap, 0, y, r, &clippedRowHeaderRect, pDoc, pDC );

			}

		}

		// Render the col headers
		// --------------------------------------------------------------------

		CRect clippedColHeaderRect;

		if ( clippedColHeaderRect.IntersectRect( &clip, m_ColHeaderRect ) ) {

			int cx = m_ElementWithBorderSize.cx;
			int c1 = (clippedColHeaderRect.left + scroll.x - m_ColHeaderRect.left) / m_ElementWithBorderSize.cx;
			int c2 = (clippedColHeaderRect.right + scroll.x + m_ElementWithBorderSize.cx - 1 - m_ColHeaderRect.left) / m_ElementWithBorderSize.cx;
			int x = (m_CelsRect.left - scroll.x) + (c1 * cx);

			for ( int c = c1; c < c2; c++, x += cx ) {

				RenderColHeader( pBitmap, x, 0, c, &clippedColHeaderRect, pDoc, pDC );

			}

		}

		// shutdown the dc
		// --------------------------------------------------------------------

		if ( pDC ) {

			pDC->SelectObject( pOldBitmap );

		}

	}

}

//
//	CAnimGridView::EnsureCanvasRect()
//

void
CAnimGridView::EnsureCanvasRect( const CRect * pLPRect, const CRect *pDPRect )
{
	RECT smallestUpdateRect;
	RECT * pOperationClipRect;

	// Change the size of the display dib?
	// ------------------------------------------------------------------------

	CRect clientRect;

	GetClientRect( &clientRect );

	int cx = clientRect.Width();
	int cy = clientRect.Height();

	if ( (m_DisplayDib.Width() < cx) || (m_DisplayDib.Height() < cy) ) {

		int cw = max( 128, min( GetSystemMetrics( SM_CXSCREEN ), (cx + 128) ) );
		int ch = max( 128, min( GetSystemMetrics( SM_CYSCREEN ), (cy + 128) ) );

		if ( !m_DisplayDib.BPTDIB_DEBUG_CREATE( cw, ch, 0 ) ) {

			TRACE("Unable to create dib section!");

		}

		pOperationClipRect = 0;

	} else {

		if ( pDPRect ) {

			smallestUpdateRect = *pDPRect;

			pOperationClipRect = &smallestUpdateRect;

		} else {

			pOperationClipRect = 0;

		}

	}

	// Call the renderer
	// ------------------------------------------------------------------------

	RenderGrid( pOperationClipRect );

}

// ----------------------------------------------------------------------------

//
//	CAnimGridView::SyncInternals()
//

void 
CAnimGridView::SyncInternals()
{
	// Get the canvas size

	CBpaintDoc* pDoc = GetDocument();

	CSize canvasSize = pDoc->GetCanvasSize();

#if 1

	CSize tempSize = m_TemporaryCanvasBitmap.Size();

	if ( tempSize != canvasSize ) {

		m_TemporaryCanvasBitmap.Create( canvasSize.cx, canvasSize.cy );

	}

#endif

	BPT::CAnimation * pOld = m_pAnimation;
	
	m_pAnimation = pDoc->CurrentAnimation();

	bool clearDib = (m_pAnimation != pOld);

	// setup the sizes...

#if 1

	m_ThumbnailSize = pDoc->GetThumbnailSize();

#else

	m_ThumbnailSize.cx = canvasSize.cx / 16;
	m_ThumbnailSize.cy = canvasSize.cy / 16;

#endif

	// Element frame size

	m_ElementFrameSize.cx = 3;
	m_ElementFrameSize.cy = 3;
	
	// The element is the size of the thumbnail + the frame

	m_ElementSize.cx = m_ThumbnailSize.cx + m_ElementFrameSize.cx * 2;
	m_ElementSize.cy = m_ThumbnailSize.cy + m_ElementFrameSize.cy * 2;

	m_ElementSrcRect = CRect( CPoint( 0, 0 ), m_ThumbnailSize );

	m_ElementBorderSize.cx = 1;
	m_ElementBorderSize.cy = 1;

	// ------------------------------------------------------------------------

	CSize textDeltaSize( 0, 24 );

	// -----------------------------------------------------------------------

	if ( m_pAnimation ) {

		int layerCount = m_pAnimation->LayerCount();

		CSize textBorder( 8, 6 );

		for ( int lc = 0; lc < layerCount; lc++ ) {

			BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( lc );

			if ( pLayerInfo ) {

				CString layerName = pLayerInfo->GetName();

				if ( layerName ) {

					layerName = "(*) " + layerName;

					CDC tempDC;

					if ( tempDC.CreateCompatibleDC( NULL ) ) {

						CSize tsize = tempDC.GetTextExtent( layerName );

						tsize.cx += textBorder.cx;
						tsize.cy += textBorder.cy;
	
						if ( textDeltaSize.cx < tsize.cx ) {
	
							textDeltaSize.cx = tsize.cx;
	
						}
	
						if ( textDeltaSize.cy < tsize.cy ) {
	
							textDeltaSize.cy = tsize.cy;
	
						}

					}

				}

			}

		}

	}

	// If the element size is > than the element size.

	if ( textDeltaSize.cx < m_ElementSize.cx ) {

		textDeltaSize.cx = m_ElementSize.cx;

	}

	textDeltaSize.cy += m_ElementSize.cy;

	m_HeaderSize.cx = m_ElementBorderSize.cx + textDeltaSize.cx;
	m_HeaderSize.cy = m_ElementBorderSize.cy + textDeltaSize.cy;

	// ask the document about the 'active' animation

	if ( m_pAnimation ) {

		if ( m_GridSize.cx != m_pAnimation->FrameCount() ) {

			clearDib = true;

			m_GridSize.cx = m_pAnimation->FrameCount();

		}

		if ( m_GridSize.cy != m_pAnimation->LayerCount() ) {

			clearDib = true;

			m_GridSize.cy = m_pAnimation->LayerCount();

		}


	} else {

		m_GridSize.cx = 0;
		m_GridSize.cy = 0;

		clearDib = true;

	}

	m_CornerHeaderRect = CRect( CPoint( 0, 0 ), m_HeaderSize );

	m_ElementWithBorderSize.cx = m_ElementSize.cx + m_ElementBorderSize.cx;
	m_ElementWithBorderSize.cy = m_ElementSize.cy + m_ElementBorderSize.cy;

	// calc the size of all the grid elements

	CSize celsSize(
		(m_ElementSize.cx + m_ElementBorderSize.cx) * m_GridSize.cx,
		(m_ElementSize.cy + m_ElementBorderSize.cy) * m_GridSize.cy
	);

	// calc the rects...

	m_CelsRect = CRect( CPoint( m_HeaderSize.cx, m_HeaderSize.cy ), celsSize );
	m_ColHeaderRect = CRect( CPoint( m_HeaderSize.cx, 0 ), CSize( celsSize.cx, m_HeaderSize.cy ) );
	m_RowHeaderRect = CRect( CPoint( 0, m_HeaderSize.cy ), CSize( m_HeaderSize.cx, celsSize.cy ) );

	// set the limit rect

#if defined( ANIMGRID_ELEMENT_SIZE_SCROLLING ) // grid alignment

	CSize overhead = m_ElementWithBorderSize;

#else

	CSize overhead( 0, 0 );

#endif

	// setup the clipping rect...

	m_RealDisplaySrcLimitRect = CRect(
		CPoint( 0, 0 ), CSize(
			m_HeaderSize.cx + celsSize.cx, 
			m_HeaderSize.cy + celsSize.cy
		)
	);

	m_DisplaySrcLimitRect = CRect(
		CPoint( 0, 0 ), CSize(
			m_HeaderSize.cx + celsSize.cx + overhead.cx, 
			m_HeaderSize.cy + celsSize.cy + overhead.cy
		)
	);

	// set the scroll sizes

	SetScrollSizes( MM_TEXT, m_DisplaySrcLimitRect.Size() );

#if 1

	// Update the scrolling clip rectangles
	// ------------------------------------------------------------------------

	GetClientRect( m_ClipRect );

	m_HScrollClipRect = m_ClipRect;
	m_VScrollClipRect = m_ClipRect;

	m_HScrollClipRect.left = m_HeaderSize.cx;
	m_VScrollClipRect.top = m_HeaderSize.cy;

	m_CelsClipRect.IntersectRect( &m_CelsRect, &m_ClipRect );

#endif

	// ------------------------------------------------------------------------

	if ( clearDib ) {

#if 0

		bitmap_type * pBitmap = m_DisplayDib.GetBitmapPtr();

		if ( pBitmap ) {

			pBitmap->ClearBuffer( m_EmptyCelColor );

		}

#endif

	}

}

// ----------------------------------------------------------------------------

//
//	CAnimGridView::InvalidateLogicalRect()
//

void
CAnimGridView::InvalidateLogicalRect(
	const RECT & rc, const BOOL bErase, const BOOL bUpdate,
	const bool bHScrollAdjust, const bool bVScrollAdjust
)
{

#if 1

	RECT phRect = rc;

	CPoint scroll = GetScrollPosition();

	if ( bHScrollAdjust ) {

		OffsetRect( &phRect, -scroll.x, 0 );

	}

	if ( bVScrollAdjust ) {

		OffsetRect( &phRect, 0, -scroll.y );

	}

#else

	CWindowDC dc( this );

	OnPrepareDC( &dc );

	RECT phRect = rc;

	dc.LPtoDP( &phRect );

#endif

	InvalidateRect( &phRect, bErase );

	if ( bUpdate ) {

		UpdateWindow();

	}

}

//
//	CAnimGridView::InvalidateColHeader()
//

void 
CAnimGridView::InvalidateColHeader( const int col, const BOOL bUpdate )
{
	CRect rect( 
		CPoint( m_HeaderSize.cx + col * m_ElementWithBorderSize.cx, 0 ),
		CSize( m_ElementWithBorderSize.cx, m_HeaderSize.cy )
	);
	
	InvalidateLogicalRect( rect, FALSE, bUpdate, true, false );
}

//
//	CAnimGridView::InvalidateRowHeader()
//

void 
CAnimGridView::InvalidateRowHeader( const int row, const BOOL bUpdate )
{
	CRect rect( 
		CPoint( 0, m_HeaderSize.cy + row * m_ElementWithBorderSize.cy ),
		CSize( m_HeaderSize.cx, m_ElementWithBorderSize.cy )
	);

	InvalidateLogicalRect( rect, FALSE, bUpdate, false, true );
}

//
//	CAnimGridView::CalcCelRect()
//

void
CAnimGridView::CalcCelRect( CRect & out, const int row, const int col )
{
	out = CRect( 
		CPoint(
			m_HeaderSize.cx + col * m_ElementWithBorderSize.cx, 
			m_HeaderSize.cy + row * m_ElementWithBorderSize.cy
		), m_ElementWithBorderSize
	);
}

void
CAnimGridView::CalcCelRangeRect( CRect & out, const int brow, const int bcol, const int erow, const int ecol )
{
	CRect rect1;

	CalcCelRect(out, brow, bcol);

	for (int r = brow; r <= erow; r++) {
		for (int c = bcol; c <= ecol; c++) {
			CalcCelRect(rect1, r, c);
			out.UnionRect(out, rect1);
		}
	}
}

//
//	CAnimGridView::InvalidateCel()
//

void 
CAnimGridView::InvalidateCel( const int row, const int col, const BOOL bUpdate )
{
	CRect rect;
	
	CalcCelRect( rect, row, col );

	InvalidateLogicalRect( rect, FALSE, bUpdate );
}

//
//	CAnimGridView::ChangeSelection()
//

void 
CAnimGridView::ChangeSelection( const POINT NewSelection, const POINT OldSelection, const BOOL bUpdate )
{
	InvalidateColHeader( OldSelection.x, bUpdate );
	InvalidateColHeader( NewSelection.x, bUpdate );

	InvalidateRowHeader( OldSelection.y, bUpdate );
	InvalidateRowHeader( NewSelection.y, bUpdate );

	InvalidateCel( NewSelection.y, NewSelection.x, bUpdate );
	InvalidateCel( OldSelection.y, OldSelection.x, bUpdate );
}

/////////////////////////////////////////////////////////////////////////////
// CAnimGridView message handlers

void CAnimGridView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if ( pHint ) {

		if ( pHint->IsKindOf( RUNTIME_CLASS( CActiveAnimationChange ) ) ) {

			CActiveAnimationChange * pChange = (CActiveAnimationChange *)pHint;

			// For now just be lazy about what updates...

			switch ( lHint ) {

			default:
			case CActiveAnimationChange::NEW_ANIMATION:
				SyncInternals();
				InvalidateRect( NULL, TRUE );
				break;

			case CActiveAnimationChange::BOTH_CHANGE:
				ChangeSelection( pChange->m_NewSelection, pChange->m_OldSelection, TRUE );
				break;

			}

			// RONG: add animtion name grid view
			CString title;
			BPT::CAnimation *anim = GetDocument()->CurrentAnimation();

			if (anim) {

				title.Format("Animation grid ( %s : %s )", GetDocument()->GetTitle(), anim->GetName() );

			} else {

				title.Format("Animation grid ( %s )", GetDocument()->GetTitle() );

			}

			SetWindowText( title );

			GetParentFrame()->SetWindowText( title );
			

#if 1
			BrindCelIntoFullView(
				pChange->m_NewSelection.y, pChange->m_NewSelection.x
			);
#endif

			UpdateWindow();

		}

	}
}

BOOL CAnimGridView::OnEraseBkgnd(CDC* pDC) 
{
	CBrush br( GetSysColor( COLOR_WINDOW ) );
	
	FillOutsideRect( pDC, &br );

	return TRUE;
}

void CAnimGridView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);

	// ------------------------------------------------------------------------

	GetClientRect( m_ClipRect );

//	m_ClipRect = CRect( CPoint( 0, 0 ), CSize( cx, cy ) );

	if ( (m_DisplayDib.Width() < cx) || (m_DisplayDib.Height() < cy) ) {

		int cw = max( 128, min( GetSystemMetrics( SM_CXSCREEN ), (cx + 128) ) );
		int ch = max( 128, min( GetSystemMetrics( SM_CYSCREEN ), (cy + 128) ) );

		m_DisplayDib.BPTDIB_DEBUG_CREATE( cw, ch, 0 );

	}

	// Update the scrolling clip rectangles
	// ------------------------------------------------------------------------

	m_HScrollClipRect = m_ClipRect;
	m_VScrollClipRect = m_ClipRect;

	m_HScrollClipRect.left = m_HeaderSize.cx;
	m_VScrollClipRect.top = m_HeaderSize.cy;

	m_CelsClipRect.IntersectRect( &m_CelsRect, &m_ClipRect );

	// ------------------------------------------------------------------------

}

BOOL CAnimGridView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if ( HTCLIENT == nHitTest ) {

		// Where is the cursor at?
		// --------------------------------------------------------------------

		CPoint pt;

		GetCursorPos( &pt );

		ScreenToClient( &pt );

		// Over a grid element?
		// --------------------------------------------------------------------

		if ( m_CelsRect.PtInRect( pt ) || 
			m_ColHeaderRect.PtInRect( pt ) || 
			m_RowHeaderRect.PtInRect( pt ) ) {

			if ( m_hCrossCrsr ) {
				
				SetCursor( m_hCrossCrsr );
	
				return TRUE;
	
			}

		}

		// --------------------------------------------------------------------

	}
	
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

int CAnimGridView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	m_hCrossCrsr = AfxGetApp()->LoadCursor( IDC_GRID_CROSS_CURSOR );

#if 1 // BPT 6/18/01

	// ------------------------------------------------------------------------

	m_ReusedCelIndicator.LoadBitmap( IDB_REUSED_CEL );

#if 1

	m_ReusedCelIndicatorSize.cx = 9;
	m_ReusedCelIndicatorSize.cy = 9;

#else

	m_ReusedCelIndicatorSize = m_HasPaletteIndicator.GetBitmapDimension();

#endif

	// ------------------------------------------------------------------------

	m_HasPaletteIndicator.LoadBitmap( IDB_HAS_PALETTE ); // BPT 6/18/01

#if 1

	m_PalIndicatorSize.cx = 9;
	m_PalIndicatorSize.cy = 9;

#else

	m_PalIndicatorSize = m_HasPaletteIndicator.GetBitmapDimension();

#endif

	m_PalIndicatorOffset.x = 2;
	m_PalIndicatorOffset.y = 2;

#endif // BPT 6/18/01

	// ------------------------------------------------------------------------

	//m_HasCelsIndicator.LoadBitmap( IDB_HAS_CELS );

#if 1 // hack!

	CBpaintDoc* pDoc = GetDocument();

	CSize canvasSize = pDoc->GetCanvasSize();

	int cx = canvasSize.cx / 8;
	int cy = canvasSize.cy / 8;

	if ( m_HackElement.Create( cx, cy ) ) {

		bitmap_type * pBits = &m_HackElement;
	
		if ( pBits ) {
	
			pBits->ClearBuffer( 0 );
	
			for ( int i = 0; i < 20; i++ ) {
	
				CPoint pt( rand() % (cx + 1), rand() % (cy + 1) );
	
				int a = 32 + (rand() % 16);
				int b = 32 + (rand() % 16);
	
				BPT::T_SolidEllipsePrim(
					*pBits, pt, a, b, rand()
					,BPT::TCopyROP< bitmap_type::pixel_type >()
				);
	
			}
	
		}

	} else {

		return -1;

	}

#endif
	
	return 0;
}

void CAnimGridView::OnDestroy() 
{
	if ( m_hCrossCrsr ) DeleteObject( (HGDIOBJ) m_hCrossCrsr );

	CBpaintDoc * pDoc = GetDocument();

	if ( pDoc ) {

		pDoc->OnDestroyPopupGridView( this );

	}

	CScrollView::OnDestroy();
	
}

void CAnimGridView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();

	SyncInternals();

	if ( m_pAnimation ) { // BPT 6/19/01 no animation crash fix.
	
		m_pAnimation->SetSelectedFrames(-1, -1);
		m_pAnimation->SetSelectedLayers(-1, -1);

	}

	// RONG: add animtion name grid view
	CString title;
	BPT::CAnimation *anim = GetDocument()->CurrentAnimation();

	if (anim) {

		title.Format("Animation grid ( %s : %s )", GetDocument()->GetTitle(), anim->GetName() );

	} else {

		title.Format("Animation grid ( %s )", GetDocument()->GetTitle() );

	}

	SetWindowText( title );

	GetParentFrame()->SetWindowText( title );

	// Try to force the menu to be what we really want.
	// ------------------------------------------------------------------------

	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc ) {

		pDoc->EnsureProperMenu( this );

	}
}

// ----------------------------------------------------------------------------

BOOL CAnimGridView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll) 
{
	// Mostly the same code as the MFC source for the OnScrollBy()
	// ------------------------------------------------------------------------

	int xOrig, x;
	int yOrig, y;

	// don't scroll if there is no valid scroll range (ie. no scroll bar)
	CScrollBar* pBar;
	DWORD dwStyle = GetStyle();
	pBar = GetScrollBarCtrl(SB_VERT);
	if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
		(pBar == NULL && !(dwStyle & WS_VSCROLL)))
	{
		// vertical scroll bar not enabled
		sizeScroll.cy = 0;
	}
	pBar = GetScrollBarCtrl(SB_HORZ);
	if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
		(pBar == NULL && !(dwStyle & WS_HSCROLL)))
	{
		// horizontal scroll bar not enabled
		sizeScroll.cx = 0;
	}

	// adjust current x position
	xOrig = x = GetScrollPos(SB_HORZ);
	int xMax = GetScrollLimit(SB_HORZ);
	x += sizeScroll.cx;
	if (x < 0)
		x = 0;
	else if (x > xMax)
		x = xMax;

	// adjust current y position
	yOrig = y = GetScrollPos(SB_VERT);
	int yMax = GetScrollLimit(SB_VERT);
	y += sizeScroll.cy;
	if (y < 0)
		y = 0;
	else if (y > yMax)
		y = yMax;

#if defined( ANIMGRID_ELEMENT_SIZE_SCROLLING ) // grid alignment

	// how many cels are visible right now?

	CRect clientRect;

	GetClientRect( &clientRect );

	int colCount = (clientRect.Width() - m_HeaderSize.cx + m_ElementSize.cx - 1) / m_ElementSize.cx;
	int rowCount = (clientRect.Height() - m_HeaderSize.cy + m_ElementSize.cy - 1) / m_ElementSize.cy;

	// adjust the x scroll amount

	int col = (x / m_ElementWithBorderSize.cx);

	x = col * m_ElementWithBorderSize.cx;

	// adjust the y scroll amount

	int row = (y / m_ElementWithBorderSize.cy);

	y = row * m_ElementWithBorderSize.cy;

#endif

	// did anything change?
	if (x == xOrig && y == yOrig)
		return FALSE;

	if (bDoScroll)
	{
		// do scroll and update scroll positions

#if 1

		// This is the code that made me have to replace the whole implementation.

		int dx = -(x-xOrig);
		int dy = -(y-yOrig);

		if ( (0 != dx) && (0 != dy) ) {

			InvalidateRect( NULL, false );

		} else {

			CRect scrollClipRectangle;

			if ( dx ) {

				scrollClipRectangle = m_HScrollClipRect;

			} else if ( dy ) {

				scrollClipRectangle = m_VScrollClipRect;

			}

			ScrollWindow( dx, dy, NULL, &scrollClipRectangle );

		}

#else

		// this is the orginal code

		ScrollWindow(-(x-xOrig), -(y-yOrig));

#endif

		if (x != xOrig)
			SetScrollPos(SB_HORZ, x);
		if (y != yOrig)
			SetScrollPos(SB_VERT, y);
	}

	// Other scroll related code?
	// ------------------------------------------------------------------------

	return TRUE;
}

void CAnimGridView::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CScrollView::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}

void CAnimGridView::OnMove(int x, int y) 
{
	CScrollView::OnMove(x, y);
	
	// TODO: Add your message handler code here
	
}

// ----------------------------------------------------------------------------

void 
CAnimGridView::ClientCoordsToGridInfo(
	GridCursorInfo & outInfo, const CPoint pt,
	const GridCursorInfo * pOverrideInfo 
)
{
	// ------------------------------------------------------------------------

	CPoint scrollPt = GetDeviceScrollPosition();

	int x = (pt.x + scrollPt.x);
	int y = (pt.y + scrollPt.y);

	// ------------------------------------------------------------------------

	if ( pOverrideInfo ) {

		if ( GridCursorInfo::Type::COL == pOverrideInfo->type ) { goto WAS_COL; }
		if ( GridCursorInfo::Type::ROW == pOverrideInfo->type ) { goto WAS_ROW; }
		if ( GridCursorInfo::Type::CEL == pOverrideInfo->type ) { goto WAS_CEL; }

	}

	// ------------------------------------------------------------------------

	if ( m_ColHeaderRect.PtInRect( pt ) ) {

	WAS_COL:

		outInfo.type = GridCursorInfo::Type::COL;

		outInfo.info.x = (x - m_HeaderSize.cx) / m_ElementWithBorderSize.cx;
		outInfo.info.y = 0;

	} else if ( m_RowHeaderRect.PtInRect( pt ) ) {

	WAS_ROW:

		outInfo.type = GridCursorInfo::Type::ROW;

		outInfo.info.x = 0;
		outInfo.info.y = (y - m_HeaderSize.cy) / m_ElementWithBorderSize.cy;

	} else if ( m_CelsRect.PtInRect( pt ) ) {

	WAS_CEL:

		outInfo.type = GridCursorInfo::Type::CEL;

		outInfo.info.x = (x - m_HeaderSize.cx) / m_ElementWithBorderSize.cx;
		outInfo.info.y = (y - m_HeaderSize.cy) / m_ElementWithBorderSize.cy;

	} else {

		outInfo.type = GridCursorInfo::Type::UNKNOWN;
		outInfo.info = pt;

	}

}

// ----------------------------------------------------------------------------

//
//	CAnimGridView::InternalSetCapture()
//

void 
CAnimGridView::InternalSetCapture()
{
	if ( 0 == GetCapture() ) {

		SetCapture();

		if ( 0 == m_nScrollControlTimer ) {
	
			m_nScrollControlTimer = SetTimer(
				1234, ANIMGRID_SCROLL_DELAY, 0
			);

		}

	}

}

//
//	CAnimGridView::InternalReleaseCapture()
//

void 
CAnimGridView::InternalReleaseCapture()
{
	if ( this == GetCapture() ) {

		ReleaseCapture();

		if ( m_nScrollControlTimer ) {
	
			KillTimer( m_nScrollControlTimer );

			m_nScrollControlTimer = 0;
	
		}

	}

}

// ----------------------------------------------------------------------------

void CAnimGridView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GridCursorInfo info;

	ClientCoordsToGridInfo( info, point, 0 );

	// ------------------------------------------------------------------------

	if ( MK_SHIFT & nFlags ) { // BPT 5/25/01
//	if ( ::GetAsyncKeyState(VK_SHIFT) & 0x8000) {

		if ( m_pAnimation ) { // BPT 6/21/01

			m_pAnimation->SetSelectedFrames(m_pAnimation->GetBeginSelectedFrames(), info.info.x);
			m_pAnimation->SetSelectedLayers(m_pAnimation->GetBeginSelectedLayers(), info.info.y);
	
			InvalidateRect( NULL, TRUE );

		}

	} else {

		if ( m_pAnimation ) { // BPT 6/21/01

			if ( m_pAnimation->GetEndSelectedFrames() != -1 ) {
				
				InvalidateRect( NULL, TRUE );
				
			}
	
			m_pAnimation->SetSelectedFrames(info.info.x, -1 );
			m_pAnimation->SetSelectedLayers(info.info.y, -1 );

		}

		// ------------------------------------------------------------------------

		CBpaintDoc* pDoc = GetDocument();

		if ( pDoc ) {

			if ( GridCursorInfo::Type::CEL == info.type ) {

				pDoc->Request_SmartFrameLayerChange( info.info.x, info.info.y );

			} else if ( GridCursorInfo::Type::COL == info.type ) {

				pDoc->Request_FrameChange( info.info.x );

			} else if ( GridCursorInfo::Type::ROW == info.type ) {

				pDoc->Request_LayerChange( info.info.y );

			}

		}

	}
	
	// ------------------------------------------------------------------------

	InternalSetCapture();

	// Record the button down event info
	// ------------------------------------------------------------------------

	m_ButtonDownInfo = info;

	// ------------------------------------------------------------------------

#if 1 // BPT 5/25/01

	if ( m_pAnimation ) { // BPT 6/21/01

		m_lastBeginFrame = m_pAnimation->GetBeginSelectedFrames();
		m_lastEndFrame = m_pAnimation->GetEndSelectedFrames();
		m_lastBeginLayer = m_pAnimation->GetBeginSelectedLayers();
		m_lastEndLayer = m_pAnimation->GetEndSelectedLayers();

	}

#endif

}

void CAnimGridView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	GridCursorInfo info;
	
	ClientCoordsToGridInfo( info, point, &m_ButtonDownInfo );

	if ( m_pAnimation ) { // BPT 6/21/01

		if (m_pAnimation->GetBeginSelectedFrames() == info.info.x && 
			m_pAnimation->GetBeginSelectedLayers() == info.info.y) {
	
			m_pAnimation->SetSelectedFrames(m_pAnimation->GetBeginSelectedFrames(), -1 );
			m_pAnimation->SetSelectedLayers(m_pAnimation->GetBeginSelectedLayers(), -1 );
			
		} else {
	
			m_pAnimation->SetSelectedFrames(m_pAnimation->GetBeginSelectedFrames(), info.info.x);
			m_pAnimation->SetSelectedLayers(m_pAnimation->GetBeginSelectedLayers(), info.info.y);
			
		}

	}

	InternalReleaseCapture();

	// ------------------------------------------------------------------------

#if 1 // BPT 5/25/01

	if ( m_pAnimation ) { // BPT 6/21/01

		m_lastBeginFrame = m_pAnimation->GetBeginSelectedFrames();
		m_lastEndFrame = m_pAnimation->GetEndSelectedFrames();
		m_lastBeginLayer = m_pAnimation->GetBeginSelectedLayers();
		m_lastEndLayer = m_pAnimation->GetEndSelectedLayers();

	}

#endif

}

void CAnimGridView::OnMouseMove(UINT nFlags, CPoint point) 
{
	GridCursorInfo info;

	CBpaintDoc* pDoc = GetDocument();

	// Process button is down logic
	// ------------------------------------------------------------------------

	if ( (this == GetCapture()) ) {

		ClientCoordsToGridInfo( info, point, &m_ButtonDownInfo );

#if 1 // BPT 5/25/01 

		bool bControl = (0 != (MK_CONTROL & nFlags));

		if ( (0 != pDoc) && bControl ) {

			// Select the grid cel type
			// ----------------------------------------------------------------

			if ( GridCursorInfo::Type::CEL == info.type ) {
	
				pDoc->Request_SmartFrameLayerChange( info.info.x, info.info.y );
	
			} else if ( (GridCursorInfo::Type::COL == info.type) ) { //&& bControl ) {
	
				pDoc->Request_FrameChange( info.info.x );
	
			} else if ( (GridCursorInfo::Type::ROW == info.type) ) { // && bControl ) {
	
				pDoc->Request_LayerChange( info.info.y );
	
			}

			return;
	
		}

#endif

#if 1 // BPT 5/25/01

		// Fix auto scrolling when outside grid and capture set

		BrindCelIntoFullView( info.info.y, info.info.x );

#endif

		if ( m_pAnimation ) { // BPT 6/21/01

			if (!(m_pAnimation->GetBeginSelectedFrames() == info.info.x && 
				m_pAnimation->GetBeginSelectedLayers() == info.info.y)) {
				
				m_pAnimation->SetSelectedFrames(m_pAnimation->GetBeginSelectedFrames(), info.info.x);
				m_pAnimation->SetSelectedLayers(m_pAnimation->GetBeginSelectedLayers(), info.info.y);
				
				if ( (-1 == m_lastBeginFrame) || (-1 == m_lastBeginLayer) ) {
					
					InvalidateRect( NULL, FALSE );
	
				} else {
	
		//			if (m_pAnimation->GetEndSelectedFrames() == -1) {
	
						CRect rect1, rect2;
	
						CalcCelRangeRect(rect1, m_lastBeginLayer, m_lastBeginFrame, m_lastEndLayer, m_lastEndFrame);
	
						CalcCelRangeRect(rect2, m_pAnimation->GetBeginSelectedLayers(),
												m_pAnimation->GetBeginSelectedFrames(), 
												m_pAnimation->GetEndSelectedLayers(),
												m_pAnimation->GetEndSelectedFrames()				
										);
	
	#if 1 // BPT 5/25/01
						
						CRect intersection;
	
						if ( intersection.IntersectRect( &rect1, &rect2 ) ) {
	
							RECT rects[ 4 ];
	
							int count, counter;
	
							// Redraw no longer selected cels
							// ----------------------------------------------------
	
							count = BPT::ReturnExternalClipRects( rects, &rect1, &intersection );
	
							for ( counter = 0; counter < count; counter++ ) {
								
								InvalidateLogicalRect( rects[ counter ], FALSE, true, true );
	
							}
	
							// Redraw newly selected cels
							// ----------------------------------------------------
	
							count = BPT::ReturnExternalClipRects( rects, &rect2, &intersection );
	
							for ( counter = 0; counter < count; counter++ ) {
								
								InvalidateLogicalRect( rects[ counter ], FALSE, true, true );
	
							}
	
						} else {
	
							InvalidateLogicalRect( rect1, FALSE, true, true );
							InvalidateLogicalRect( rect2, FALSE, true, true );
	
						}
	
	#else
						
						rect1.UnionRect(rect1, rect2);
	
						InvalidateLogicalRect( rect1, FALSE, true, true ); // BPT 5/25/01
	
						//InvalidateRect( rect1, FALSE );
	
	#endif
	
		//			}
					
				}
	
				m_lastBeginFrame = m_pAnimation->GetBeginSelectedFrames();
				m_lastEndFrame = m_pAnimation->GetEndSelectedFrames();
				m_lastBeginLayer = m_pAnimation->GetBeginSelectedLayers();
				m_lastEndLayer = m_pAnimation->GetEndSelectedLayers();
	
			}

		}
		
//		RenderGrid();
		
	} else {

		ClientCoordsToGridInfo( info, point, 0 );

	}

	// Ask the document to potentially give focus to us :)
	// ------------------------------------------------------------------------

	if ( pDoc ) {

		pDoc->HandleAutoFocus( this );

	}

}

void CAnimGridView::OnRButtonDown(UINT nFlags, CPoint point) 
{

#if 1

	GridCursorInfo info;

	ClientCoordsToGridInfo( info, point, 0 );

	// ------------------------------------------------------------------------

	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc ) {

		if ( GridCursorInfo::Type::CEL == info.type ) {

			pDoc->Request_SmartFrameLayerChange( info.info.x, info.info.y );

		} else if ( GridCursorInfo::Type::COL == info.type ) {

			pDoc->Request_FrameChange( info.info.x );

		} else if ( GridCursorInfo::Type::ROW == info.type ) {

			pDoc->Request_LayerChange( info.info.y );

		}

	}

	// ------------------------------------------------------------------------

	ClientToScreen( &point );

	OnContextMenu( this, point );

#else

	GridCursorInfo info;

	ClientCoordsToGridInfo( info, point, 0 );

	// ???

	// ------------------------------------------------------------------------

	InternalSetCapture();

	// Record the button down event info
	// ------------------------------------------------------------------------

	m_ButtonDownInfo = info;

#endif

}

void CAnimGridView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	InternalReleaseCapture();
}

void CAnimGridView::OnCancelMode() 
{
	CScrollView::OnCancelMode();

	InternalReleaseCapture();
}

// ----------------------------------------------------------------------------

//
//	CAnimGridView::BrindCelIntoFullView()
//

void 
CAnimGridView::BrindCelIntoFullView( const int row, const int col )
{
	CRect celRect;
	
	CalcCelRect( celRect, row, col );

	CPoint scroll = GetScrollPosition();

	celRect.OffsetRect( -scroll.x, -scroll.y );

	// ------------------------

	if ( celRect.right > m_CelsClipRect.right ) {

		scroll.x += (m_CelsClipRect.right - celRect.right);

	}

	if ( celRect.left < m_CelsClipRect.left ) {

		scroll.x += (m_CelsClipRect.left - celRect.left);

	}

	// ------------------------

	if ( celRect.bottom > m_CelsClipRect.bottom ) {

		scroll.y += (m_CelsClipRect.bottom - celRect.bottom);

	}

	if ( celRect.top < m_CelsClipRect.top ) {

		scroll.y += (m_CelsClipRect.top - celRect.top);

	}

	// ------------------------

	CPoint xpt = GetScrollPosition();

	if ( xpt != scroll ) {

		OnScrollBy( CSize( xpt.x - scroll.x, xpt.y - scroll.y ), true );

	}

}

void CAnimGridView::OnTimer(UINT nIDEvent) 
{
	if ( this == GetCapture() ) {

		CPoint pt;

		GetCursorPos( &pt );

		ScreenToClient( &pt );

		UINT nFlags = 0;

#if 1 // BPT 5/23/01

		if ( 0x8000 & ::GetAsyncKeyState(VK_SHIFT) ) {

			nFlags |= MK_SHIFT;

		}

		if ( 0x8000 & ::GetAsyncKeyState(VK_CONTROL) ) {

			nFlags |= MK_CONTROL;

		}

		if ( 0x8000 & ::GetAsyncKeyState(VK_LBUTTON) ) {

			nFlags |= MK_LBUTTON;

		}

		if ( 0x8000 & ::GetAsyncKeyState(VK_RBUTTON) ) {

			nFlags |= MK_RBUTTON;

		}

#endif

		OnMouseMove( nFlags, pt );

	}
}

void CAnimGridView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	GridCursorInfo info;

	ClientCoordsToGridInfo( info, point, 0 );

	// ------------------------------------------------------------------------

	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc ) {

		if ( GridCursorInfo::Type::ROW == info.type ) {

			pDoc->Request_LayerProperties( info.info.y );

		} else if ( GridCursorInfo::Type::COL == info.type ) {

			pDoc->Request_FrameProperties( info.info.x );

		} else if ( GridCursorInfo::Type::CEL == info.type ) {

			pDoc->Request_FrameChange( info.info.x );

		} else {

			CWnd * pWnd = AfxGetMainWnd();

			if ( pWnd ) {

				pWnd->PostMessage( WM_COMMAND, ID_ANIM_SELECT_DIALOG );

			}

		}

	}

}

void CAnimGridView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	GridCursorInfo info;

	CPoint clientPoint = point;

	ScreenToClient( &clientPoint );

	ClientCoordsToGridInfo( info, clientPoint, 0 );

	// ------------------------------------------------------------------------

	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc ) {

		if ( GridCursorInfo::Type::ROW == info.type ) {

			CMenu menu;

			if ( menu.LoadMenu(IDR_ANIM_GRID_LAYER_CONTEXT) ) {

				if ( CMenu * pSubMenu = menu.GetSubMenu(0) ) {

					pSubMenu->TrackPopupMenu(
						TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this
					);

				}

			}

		} else if ( GridCursorInfo::Type::COL == info.type ) {

			CMenu menu;

			if ( menu.LoadMenu(IDR_ANIM_GRID_FRAME_CONTEXT) ) {

				if ( CMenu * pSubMenu = menu.GetSubMenu(0) ) {

					pSubMenu->TrackPopupMenu(
						TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this
					);

				}

			}

		} else if ( GridCursorInfo::Type::CEL == info.type ) {

			CMenu menu;

			if ( menu.LoadMenu(IDR_ANIM_GRID_CEL_CONTEXT) ) {

				if ( CMenu * pSubMenu = menu.GetSubMenu(0) ) {

#if 1 // BPT 6/27/01

					if ( m_bShowVisualElementIDs ) {

						pSubMenu->CheckMenuItem( ID_GRID_SHOW_UIDS, MF_CHECKED | MF_BYCOMMAND );

					} else {

						pSubMenu->CheckMenuItem( ID_GRID_SHOW_UIDS, MF_UNCHECKED | MF_BYCOMMAND );

					}

#endif

					pSubMenu->TrackPopupMenu(
						TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this
					);

				}

			}

		} else {

			// Elsewhere

		}

	}

}

void CAnimGridView::OnGridShowUids() 
{
	m_bShowVisualElementIDs = !m_bShowVisualElementIDs;

	InvalidateRect( NULL, FALSE );

	UpdateWindow();

}

void CAnimGridView::OnUpdateGridShowUids(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bShowVisualElementIDs ? 1 : 0 );
}
