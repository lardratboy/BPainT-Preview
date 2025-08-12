// bpaintView.cpp : implementation of the CBpaintView class
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//

#include "stdafx.h"
#include "bpaint.h"

#include "bpaintView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ----------------------------------------------------------------------------

#define BPTVIEW_USE_CENTER_BORDER_CODE
//#define BPTVIEW_USE_GDI_STRETCH

//#define USE_WINDOW_COLOR_FOR_BORDER

/////////////////////////////////////////////////////////////////////////////

CBpaintView * CBpaintView::s_pControllingScrollView = 0;
int CBpaintView::s_nScrollControlTimer = 0;
bool CBpaintView::s_bFavorLeft = false;
bool CBpaintView::s_bFavorTop = false;

/////////////////////////////////////////////////////////////////////////////
// CBpaintView

IMPLEMENT_DYNCREATE(CBpaintView, CScrollView)

BEGIN_MESSAGE_MAP(CBpaintView, CScrollView)
	//{{AFX_MSG_MAP(CBpaintView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_VZOOM_DEC, OnVzoomDec)
	ON_UPDATE_COMMAND_UI(ID_VZOOM_DEC, OnUpdateVzoomDec)
	ON_COMMAND(ID_VZOOM_INC, OnVzoomInc)
	ON_UPDATE_COMMAND_UI(ID_VZOOM_INC, OnUpdateVzoomInc)
	ON_COMMAND(ID_VCMD_CENTER_ON_CURSOR, OnVcmdCenterOnCursor)
	ON_UPDATE_COMMAND_UI(ID_VCMD_CENTER_ON_CURSOR, OnUpdateVcmdCenterOnCursor)
	ON_COMMAND(ID_VCMD_CURSOR_ZOOM, OnVcmdCursorZoom)
	ON_UPDATE_COMMAND_UI(ID_VCMD_CURSOR_ZOOM, OnUpdateVcmdCursorZoom)
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_COMMAND(ID_VSET_IMEDIATE_UPDATES, OnVsetImediateUpdates)
	ON_UPDATE_COMMAND_UI(ID_VSET_IMEDIATE_UPDATES, OnUpdateVsetImediateUpdates)
	ON_COMMAND(ID_VSET_CENTER_SYNC_SCROLL, OnVsetCenterSyncScroll)
	ON_UPDATE_COMMAND_UI(ID_VSET_CENTER_SYNC_SCROLL, OnUpdateVsetCenterSyncScroll)
	ON_COMMAND(ID_VSET_DISABLE_AUTO_SCROLL, OnVsetDisableAutoScroll)
	ON_UPDATE_COMMAND_UI(ID_VSET_DISABLE_AUTO_SCROLL, OnUpdateVsetDisableAutoScroll)
	ON_COMMAND(ID_VSET_SHOW_FATBITS, OnVsetShowFatbits)
	ON_UPDATE_COMMAND_UI(ID_VSET_SHOW_FATBITS, OnUpdateVsetShowFatbits)
	ON_COMMAND(ID_VSET_SHOW_GRID, OnVsetShowGrid)
	ON_UPDATE_COMMAND_UI(ID_VSET_SHOW_GRID, OnUpdateVsetShowGrid)
	ON_COMMAND(ID_VCMD_REQUEST_OTHERS_CENTER_ON_CURSOR, OnVcmdRequestOthersCenterOnCursor)
	ON_COMMAND(ID_VSET_COMPOSITE_MODE, OnVsetCompositeMode)
	ON_UPDATE_COMMAND_UI(ID_VSET_COMPOSITE_MODE, OnUpdateVsetCompositeMode)
	ON_COMMAND(ID_CENTER_VIEW, OnCenterView)
	ON_COMMAND(ID_2_VSET_COMPOSITE_MODE, On2VsetCompositeMode)
	ON_UPDATE_COMMAND_UI(ID_2_VSET_COMPOSITE_MODE, OnUpdate2VsetCompositeMode)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_VZOOM_1X,ID_VZOOM_10X,OnZoom)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VZOOM_1X,ID_VZOOM_10X,OnUpdateZoom)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBpaintView construction/destruction

CBpaintView::CBpaintView() :
	m_pDisplayDibSection(0) 
	,m_pNormalDibSection(0)
	,m_LogicalCanvasSize(0,0)
	,m_PhysicalCanvasSize(0,0)
	,m_Zoom(2,2)
	,m_bCenterSync( true )
	,m_bImediateUpdates( true )
	,m_bEditView( false )
	,m_bAnimationView( true )
	,m_bDontSyncPositions( false )
	,m_bReportViewPosChanges( true )
	,m_bFatBitLines( false )
	,m_bCenterOtherViewsOnMButton( true )
	,m_bLiveTrackOtherViews( false )
	,m_bShowGridLines( false )
	,m_ZoomFactor(2)
	,m_LastZoomFactor(3)
{

#if defined( DEMO_SAVE_DISABLED ) // BPT 7/1/01

	m_bFatBitLines = true;

#endif

	// Get the fat bits color
	// ------------------------------------------------------------------------

	dib_type::bitmap_type::pixel_type fatColor = GLOBAL_GetSettingInt(
		"FatBitsColor", MakeDisplayPixelType(
			0, //120, 
			0, //175, 
			0  //167  
		)
	);

	m_FatBitsHColor = GLOBAL_GetSettingInt( "FatBitsHColor", fatColor );
	m_FatBitsVColor = GLOBAL_GetSettingInt( "FatBitsVColor", fatColor );

	// Get the fat bits color
	// ------------------------------------------------------------------------

	dib_type::bitmap_type::pixel_type gridColor = GLOBAL_GetSettingInt(
		"GridLinesColor", MakeDisplayPixelType( 255, 255, 255 )
	);

	m_GridLinesHColor = GLOBAL_GetSettingInt( "GridLinesHColor", gridColor );
	m_GridLinesVColor = GLOBAL_GetSettingInt( "GridLinesVColor", gridColor );

	// ------------------------------------------------------------------------

	m_BackgroundRectCount = 0;

	SetRectEmpty( &m_ForegroundRect );

	for ( int i = 0; i < 4; i++ ) {

		SetRectEmpty( &m_BackgroundRects[ i ] );

	}

}

CBpaintView::~CBpaintView()
{

#if 0 // Moved to the OnDestroy() method

	if ( (this == s_pControllingScrollView) && s_nScrollControlTimer ) {

		KillTimer( s_nScrollControlTimer );

		s_nScrollControlTimer = 0;

	}

#endif
}

BOOL CBpaintView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBpaintView drawing

void CBpaintView::OnDraw(CDC* pDC)
{
	CRect clipBox;

	if ( NULLREGION != pDC->GetClipBox( &clipBox ) ) {

		clipBox.InflateRect( 1, 1 );

		// Make sure our display canvas is setup and correct
		// --------------------------------------------------------------------

		CRect lpClipBox( clipBox );

		pDC->LPtoDP( &lpClipBox );

#if !defined(BPTVIEW_USE_GDI_STRETCH)

		M_EnsureCanvasRect( &clipBox, &lpClipBox );

#endif

		// Finally do some drawing
		// --------------------------------------------------------------------

		if ( m_pDisplayDibSection ) {

			// Draw the bitmap data
			// ----------------------------------------------------------------

			CPoint backOff( 0, 0 );

			CPoint at( m_ForegroundRect.left, m_ForegroundRect.top );

			if ( (CSize(1,1) == m_Zoom) && (m_pDisplayDibSection == m_pNormalDibSection) ) { // BPT 7/7/02

				BPT::T_BlitAt( pDC->GetSafeHdc(), at.x, at.y, *m_pDisplayDibSection );

			} else {

				CPoint scroll = GetScrollPosition();

				backOff = scroll;

				CRect limitedSrcRect( CPoint( 0, 0 ), m_LogicalCanvasSize );

#if !defined(BPTVIEW_USE_GDI_STRETCH)

				BPT::T_BlitAt(
					pDC->GetSafeHdc(), scroll.x + at.x, scroll.y + at.y, *m_pDisplayDibSection,
					1.0f, 1.0f, &limitedSrcRect
				);

#else
	
				BPT::T_BlitAt(
					pDC->GetSafeHdc(), at.x, at.y, *m_pNormalDibSection,
					m_Zoom.cx, m_Zoom.cy, &limitedSrcRect
				);

#endif

			}

			// Fill in the background rects if any
			// ----------------------------------------------------------------

			if ( m_BackgroundRectCount ) {

#if defined( USE_WINDOW_COLOR_FOR_BORDER )

				CBrush br( GetSysColor( COLOR_WINDOW ) );

				int patbltOp = PATCOPY;

				CBrush * pOldBrush = pDC->SelectObject( &br );

#else

				int patbltOp = WHITENESS;

#endif

				clipBox.OffsetRect( -backOff.x, -backOff.y );

				for ( int i = 0; i < m_BackgroundRectCount; i++ ) {

					CRect intersection;

					if ( intersection.IntersectRect( &clipBox, &m_BackgroundRects[ i ] ) ) {

						pDC->PatBlt(
							intersection.left + backOff.x, intersection.top + backOff.y, 
							intersection.Width(), intersection.Height(),
							patbltOp
						);

					}

				}

#if defined( USE_WINDOW_COLOR_FOR_BORDER )

				if ( pOldBrush ) {

					pDC->SelectObject( pOldBrush );

				}

#endif

			}

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

void CBpaintView::OnInitialUpdate()
{
	// perform the simple init stuff

	CScrollView::OnInitialUpdate();

	M_SetupCanvasStructures( 1, false, true );

#if 1

	// try to make the size of the window the same size as the scroll view
	// unless that would make it larger than the MDI client rect.

	CFrameWnd * pFrame = GetParentFrame();

	if ( pFrame ) {

		CWnd * pWnd = pFrame->GetParent();

		if ( pWnd ) {

			CRect grandparentsClientRect;

			pWnd->GetClientRect( &grandparentsClientRect );

			pFrame->MoveWindow( &grandparentsClientRect );

			ResizeParentToFit();

#if 0 // center 

			CRect rect;

			pFrame->GetWindowRect( &rect );

			int x = (grandparentsClientRect.Width() - rect.Width()) / 2;
			int y = (grandparentsClientRect.Height() - rect.Height()) / 2;

			rect.OffsetRect( x - rect.left, y - rect.top );

			pFrame->MoveWindow( &rect );

#endif

		}

	}

#endif

	SyncTitleBar();

	// Try to force the menu to be what we really want.
	// ------------------------------------------------------------------------

	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc ) {

		pDoc->EnsureProperMenu( this );

	}
}

/////////////////////////////////////////////////////////////////////////////
// CBpaintView diagnostics

#ifdef _DEBUG
void CBpaintView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CBpaintView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CBpaintDoc* CBpaintView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBpaintDoc)));
	return (CBpaintDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBpaintView message handlers

BOOL CBpaintView::OnEraseBkgnd(CDC* pDC) 
{
	CBrush br( GetSysColor( COLOR_WINDOW ) );
	
	FillOutsideRect( pDC, &br );

	return TRUE;
	
}

void CBpaintView::OnSize(UINT nType, int cx, int cy) 
{

#if 0 // ???

	CScrollView::OnSize(nType, cx, cy);

#endif
	
	// if the display DIBSECTION is the same as the normal dibsection
	// then we're done :)
	// ------------------------------------------------------------------------

	if ( m_pDisplayDibSection == &m_ZoomDibSection ) {

		int nw = min( (m_pNormalDibSection->Width() * m_Zoom.cx), cx );
		int nh = min( (m_pNormalDibSection->Height() * m_Zoom.cy), cy );

		if ( M_NeedToResizeZoomDib( nw, nh ) ) {
	
			if ( !m_ZoomDibSection.BPTDIB_DEBUG_CREATE( nw, nh, 0 ) ) {
	
				TRACE("Unable to create zoom dib section!");
	
			}
	
		}

	}

	// Setup the background / foreground rects
	// ------------------------------------------------------------------------

	SyncBorderSizes();

}

// ============================================================================

//
//	CBpaintView::OnCancelMode()
//

void CBpaintView::OnCancelMode() 
{
	CScrollView::OnCancelMode();

	if ( this == GetCapture() ) {

		ReleaseCapture();

	}

	CBpaintDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	pDoc->Dispatch_OnCancelMode( this );

}

//
//	CBpaintView::OnLButtonDown()
//

void CBpaintView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CBpaintDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	M_ClientToLogical( point );

	pDoc->Dispatch_OnLButtonDown( this, nFlags, point );

	SetCapture();
}

//
//	CBpaintView::OnLButtonUp()
//

void CBpaintView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( this == GetCapture() ) ReleaseCapture();

	CBpaintDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	M_ClientToLogical( point );

	pDoc->Dispatch_OnLButtonUp( this, nFlags, point );

}

//
//	CBpaintView::OnMouseMove()
//

void CBpaintView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CBpaintDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	M_ClientToLogical( point );

	pDoc->Dispatch_OnMouseMove( this, nFlags, point );

	if ( (MK_MBUTTON & nFlags) || m_bLiveTrackOtherViews ) {

		if ( m_bCenterOtherViewsOnMButton ) {

			OnVcmdRequestOthersCenterOnCursor();

		}

	}

}

//
//	CBpaintView::OnRButtonDown()
//

void CBpaintView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CBpaintDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	M_ClientToLogical( point );

	pDoc->Dispatch_OnRButtonDown( this, nFlags, point );

	SetCapture();
}

//
//	CBpaintView::OnRButtonUp()
//

void CBpaintView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if ( this == GetCapture() ) ReleaseCapture();

	CBpaintDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	M_ClientToLogical( point );

	pDoc->Dispatch_OnRButtonUp( this, nFlags, point );

}

//
//	CBpaintView::OnMButtonDown()
//

void CBpaintView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	CBpaintDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	M_ClientToLogical( point );

	pDoc->Dispatch_OnMButtonDown( this, nFlags, point );

	if ( m_bCenterOtherViewsOnMButton ) {

		OnVcmdRequestOthersCenterOnCursor();

	}

	SetCapture();
}

//
//	CBpaintView::OnMButtonUp()
//

void CBpaintView::OnMButtonUp(UINT nFlags, CPoint point) 
{
	if ( this == GetCapture() ) ReleaseCapture();

	CBpaintDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	M_ClientToLogical( point );

	pDoc->Dispatch_OnMButtonUp( this, nFlags, point );

	if ( m_bCenterOtherViewsOnMButton ) {

		OnVcmdRequestOthersCenterOnCursor();

	}

}

//
//	CBpaintView::OnMButtonDblClk()
//

void CBpaintView::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	CBpaintDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	M_ClientToLogical( point );

	M_CenterLogicalPoint( point );

	if ( m_bCenterOtherViewsOnMButton ) {

		if ( MK_CONTROL & nFlags ) {

			m_bLiveTrackOtherViews = !m_bLiveTrackOtherViews;

		}

		OnVcmdRequestOthersCenterOnCursor();

	}

	CursorPosToLogical( point );

	pDoc->Dispatch_OnMouseMove( this, nFlags, point );

}

// ============================================================================

//
//	CBpaintView::M_GetCanvasDib()
//

CBpaintDoc::dib_type * 
CBpaintView::M_GetCanvasDib()
{
	CBpaintDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	// Ask for the animation dib if one
	// ------------------------------------------------------------------------

	if ( m_bAnimationView ) {

		CBpaintDoc::dib_type * ptr = pDoc->M_GetAnimationViewDib();

		if ( ptr ) {

			return ptr;

		}

	}

	// Get down to business and ask for either the edit or composite dib
	// ------------------------------------------------------------------------

	if ( m_bEditView ) {

		return pDoc->M_GetEditViewDib();

	}

	return pDoc->M_GetCompositeViewDib();

}

//
//	CBpaintView::M_NeedToResizeZoomDib()
//

bool 
CBpaintView::M_NeedToResizeZoomDib( const int cx, const int cy )
{
	return
		((m_ZoomDibSection.Width() < cx) ||
		(m_ZoomDibSection.Height() < cy));
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace BPT {

	//
	//	TCyclicPatternInvertDstTransferROP
	//	

	template< class SURFACE, class DST_TYPE = typename SURFACE::pixel_type, class SRC_TYPE = DWORD >
	class TCyclicPatternInvertDstTransferROP {

	private:

		int m_TestPos;

		//
		//	NextTestPattern()
		//

		SRC_TYPE NextTestPattern() {

			return 1 << (m_TestPos++ & 31);

		}

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

	public:

		TCyclicPatternInvertDstTransferROP( const int testPos ) : m_TestPos(0) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) {

			if ( s & NextTestPattern() ) {

				d = 0; // ~d;

			}

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) {

			while ( first != last ) {

				(*this)( *output, *first );

				++output;
				++first;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) {

			while ( first != last ) {
	
				(*this)( *first, s );

				++first;

			}

		}

	};

}; // namespace BPT

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//
//	CBpaintView::M_EnsureCanvasRect()
//

void
CBpaintView::M_EnsureCanvasRect( const CRect * pLPRect, const CRect *pDPRect )
{
	// if the display DIBSECTION is the same as the normal dibsection
	// then we're done :)
	// ------------------------------------------------------------------------

	if ( m_pDisplayDibSection == m_pNormalDibSection ) {

		return /* nop */;

	}

	RECT smallestUpdateRect;
	RECT * pOperationClipRect;

	// Change the size of the zoom bitmap?
	// ------------------------------------------------------------------------

	CRect clientRect;

	GetClientRect( &clientRect );

	int nw = min( (m_pNormalDibSection->Width() * m_Zoom.cx), clientRect.Width() );
	int nh = min( (m_pNormalDibSection->Height() * m_Zoom.cy), clientRect.Height() );

	if ( M_NeedToResizeZoomDib( nw, nh ) ) {

		if ( !m_ZoomDibSection.BPTDIB_DEBUG_CREATE( nw, nh, 0 ) ) {

			TRACE("Unable to create zoom dib section!");

		}

		pOperationClipRect = 0;

	} else {

		if ( pDPRect ) {

			smallestUpdateRect = *pDPRect;

			OffsetRect( &smallestUpdateRect, -m_ForegroundRect.left, -m_ForegroundRect.top );

			pOperationClipRect = &smallestUpdateRect;

		} else {

			pOperationClipRect = 0;

		}

	}

	// Need to 'zoom' the canvas... 
	// ------------------------------------------------------------------------

/* 
	FUTURE changes

		-- This is the place to deal with the virtual bitmap.
		-- also any underlays would need to be rendered here as well...


*/

	CPoint scroll = GetScrollPosition();

	if ( (1 != m_Zoom.cx) || (1 != m_Zoom.cy) ) {

		BPT::T_ScaleUpBlit(
			*m_ZoomDibSection.GetBitmapPtr()
			,-scroll.x
			,-scroll.y
			,*m_pNormalDibSection->GetBitmapPtr()
			,(m_pNormalDibSection->Width() * m_Zoom.cx)
			,(m_pNormalDibSection->Height() * m_Zoom.cy)
			,BPT::TCopyROP< CBpaintDoc::dib_type::bitmap_type::pixel_type >()
			,0
			,pOperationClipRect
		);

	} else {

		BPT::T_Blit(
			*m_ZoomDibSection.GetBitmapPtr()
			,-scroll.x
			,-scroll.y
			,*m_pNormalDibSection->GetBitmapPtr()
			,BPT::TCopyROP< CBpaintDoc::dib_type::bitmap_type::pixel_type >()
			,0
			,pOperationClipRect
		);

	}

	// Render the 'fat bits'
	// ------------------------------------------------------------------------

	if ( m_bFatBitLines && ((1 != m_Zoom.cx) && (1 != m_Zoom.cy)) ) {

		CBpaintDoc::dib_type::bitmap_type * pBitmap = m_ZoomDibSection.GetBitmapPtr();
	
		int fw = m_pNormalDibSection->Width();
		int fh = m_pNormalDibSection->Height();

		int fsw = fw * m_Zoom.cx;
		int fsh = fh * m_Zoom.cy;

		// ------------------------------------------------------------------------

		typedef BPT::T16bpp_5050_OP<CBpaintDoc::dib_type::bitmap_type::pixel_type,0x3def> fatbits_transfer_op;
//		typedef BPT::TCopyROP< CBpaintDoc::dib_type::bitmap_type::pixel_type > fatbits_transfer_op;

		// Check to see if we are to override the fatbits colors
		// ------------------------------------------------------------------------

		CBpaintDoc::dib_type::bitmap_type::pixel_type drawHColor = m_FatBitsHColor;
		CBpaintDoc::dib_type::bitmap_type::pixel_type drawVColor = m_FatBitsVColor;

		CBpaintDoc *pDoc = GetDocument();

		if ( pDoc ) {

			if ( pDoc->UseEditorChromakeyAsFatbitsColor() ) {

				drawHColor = pDoc->GetEditorChromakeyDisplayValue();

				drawVColor = drawHColor;

			}

		}


		// Draw the -h- fat bit lines
		// ------------------------------------------------------------------------

		if ( 1 != m_Zoom.cy ) {
	
			int yo = -scroll.y;
		
			for ( int fy = 0; fy < fh; fy++ ) {
		
				BPT::T_HLinePrim(
					*pBitmap, 0, yo, fsw, drawHColor,
					fatbits_transfer_op(),
					pOperationClipRect
				);
		
				yo += m_Zoom.cy;
		
			}

		}
	
		// Draw the -v- fat bit lines
		// ------------------------------------------------------------------------
	
		if ( 1 != m_Zoom.cx ) {

			int xo = -scroll.x;
		
			for ( int fx = 0; fx < fw; fx++ ) {
		
				BPT::T_VLinePrim(
					*pBitmap, xo, 0, fsh, drawVColor,
					fatbits_transfer_op(),
					pOperationClipRect
				);
		
				xo += m_Zoom.cx;
		
			}

		}

	}

	// render grid lines (should there be multiple rules, different colors for groups?)
	// ------------------------------------------------------------------------

	if ( m_bShowGridLines ) {

		CBpaintDoc::dib_type::bitmap_type * pBitmap = m_ZoomDibSection.GetBitmapPtr();
	
		int fw = m_pNormalDibSection->Width();
		int fh = m_pNormalDibSection->Height();

		int fsw = fw * m_Zoom.cx;
		int fsh = fh * m_Zoom.cy;

		// ------------------------------------------------------------------------

//		typedef BPT::TCopyROP< CBpaintDoc::dib_type::bitmap_type::pixel_type > grid_transfer_op;
//		typedef BPT::TInvertDstTransferROP<CBpaintDoc::dib_type::bitmap_type::pixel_type> grid_transfer_op;
		typedef BPT::T16bpp_5050_OP<CBpaintDoc::dib_type::bitmap_type::pixel_type,0x3def> grid_transfer_op;

		// Check to see if we are to override the grid line colors
		// ------------------------------------------------------------------------

		grid_transfer_op::src_type drawHColors[ 4 ];
		grid_transfer_op::src_type drawVColors[ 4 ];

		drawHColors[0] = drawVColors[0] = MakeDisplayPixelType(0,0,0);
		drawHColors[1] = drawVColors[1] = MakeDisplayPixelType(64,64,64);
		drawHColors[2] = drawVColors[2] = MakeDisplayPixelType(64,64,64);
		drawHColors[3] = drawVColors[3] = MakeDisplayPixelType(64,64,64);

		// range of colors

		// Get the grid size from the document
		// ------------------------------------------------------------------------

		CBpaintDoc *pDoc = GetDocument();

		BPT::CGridInfo gridInfo( CSize(1,1) );

		if ( pDoc ) {

			pDoc->GetGridInfo( &gridInfo );

		}

		CSize gridSize = gridInfo.m_GridSize;

		CSize renderGridSize( gridSize.cx * m_Zoom.cx, gridSize.cy * m_Zoom.cy );

		// Draw the -h- grid lines
		// ------------------------------------------------------------------------

		if ( 1 != gridSize.cy ) {
	
			int yo = (gridInfo.m_GridOffset.y * m_Zoom.cy) - scroll.y;

			int counter = 0;

			for ( int fy = 0; fy < fh; fy += gridSize.cy ) {

				BPT::T_HLinePrim(
					*pBitmap, 0, yo, fsw, drawHColors[ counter & 3 ],
					grid_transfer_op(),
					pOperationClipRect
				);
		
				yo += renderGridSize.cy;

				++counter;
		
			}

		}
	
		// Draw the -v- grid lines
		// ------------------------------------------------------------------------
	
		if ( 1 != gridSize.cx ) {

			int xo = (gridInfo.m_GridOffset.x * m_Zoom.cx) - scroll.x;

			int counter = 0;
		
			for ( int fx = 0; fx < fw; fx += gridSize.cx ) {
		
				BPT::T_VLinePrim(
					*pBitmap, xo, 0, fsh, drawVColors[ counter & 3 ],
					grid_transfer_op(),
					pOperationClipRect
				);
		
				xo += renderGridSize.cx;
		
				++counter;

			}

		}

	}

}

//
//	SyncBorderSizes()
//

void
CBpaintView::SyncBorderSizes()
{
	// ------------------------------------------------------------------------

	m_LogicalCanvasSize.cx = m_PhysicalCanvasSize.cx * m_Zoom.cx;
	m_LogicalCanvasSize.cy = m_PhysicalCanvasSize.cy * m_Zoom.cy;

	// ------------------------------------------------------------------------

	CRect clientRect;

	GetClientRect( &clientRect );

	// ------------------------------------------------------------------------

	int lastCount = m_BackgroundRectCount;

#if defined( BPTVIEW_USE_CENTER_BORDER_CODE )

	if ( (clientRect.Width() > m_LogicalCanvasSize.cx) ||
		(clientRect.Height() > m_LogicalCanvasSize.cy) ) {

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// ICKY ICKY ICKY

		CRect backgroundClipRect = clientRect;

		// find the center

		CSize clientSize = clientRect.Size();

		if ( clientSize.cx > m_LogicalCanvasSize.cx ) {

			m_ViewCanvasSize.cx = clientSize.cx;

			m_ForegroundRect.left = (clientSize.cx - m_LogicalCanvasSize.cx) / 2;

		} else {

			m_ViewCanvasSize.cx = m_LogicalCanvasSize.cx;

			m_ForegroundRect.left = 0;

			backgroundClipRect.right += (m_LogicalCanvasSize.cx - clientSize.cx);

		}

		m_ForegroundRect.right = m_ForegroundRect.left + m_LogicalCanvasSize.cx;

		// --------

		if ( clientSize.cy > m_LogicalCanvasSize.cy ) {

			m_ViewCanvasSize.cy = clientSize.cy;

			m_ForegroundRect.top = (clientSize.cy - m_LogicalCanvasSize.cy) / 2;

		} else {

			m_ViewCanvasSize.cy = m_LogicalCanvasSize.cy;

			m_ForegroundRect.top = 0;

			backgroundClipRect.bottom += (m_LogicalCanvasSize.cy - clientSize.cy);

		}

		m_ForegroundRect.bottom = m_ForegroundRect.top + m_LogicalCanvasSize.cy;

		// --------

		m_BackgroundRectCount = BPT::ReturnExternalClipRects(
			m_BackgroundRects, &backgroundClipRect, &m_ForegroundRect
		);

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ICKY ICKY ICKY

	} else {

#endif

		m_BackgroundRectCount = 0;
	
		m_ForegroundRect.left = 0;
		m_ForegroundRect.top = 0;
		m_ForegroundRect.right = m_LogicalCanvasSize.cx;
		m_ForegroundRect.bottom = m_LogicalCanvasSize.cy;

		m_ViewCanvasSize = m_LogicalCanvasSize;

#if defined( BPTVIEW_USE_CENTER_BORDER_CODE )

	}

#endif

#if 1

	SetScrollSizes(MM_TEXT, m_ViewCanvasSize );

#endif

	// ------------------------------------------------------------------------

	InvalidateRect( NULL, false );

}

//
//	CBpaintView::M_SetupCanvasStructures()
//

void 
CBpaintView::M_SetupCanvasStructures( const int zoomFactor, const bool invalidateWindow, const bool bCenter )
{
	// ------------------------------------------------------------------------

	m_pNormalDibSection = M_GetCanvasDib();

	CSize size( m_pNormalDibSection->Width(), m_pNormalDibSection->Height() );

	// Need to figure out the scale

	CBpaintDoc * pDoc = GetDocument();

	ASSERT_VALID( pDoc );

	CSize pixelScale;

	pDoc->GetPixelAspectRatio( &pixelScale );

	CSize zoom( zoomFactor * pixelScale.cx, zoomFactor * pixelScale.cy );

	// Set the display dib
	// ------------------------------------------------------------------------

	if ( CSize(1,1) == zoom ) {

#if defined(BPT_ALWAYS_USE_ZOOM_BUFFER)

		m_pDisplayDibSection = &m_ZoomDibSection;

#else

		if ( m_bShowGridLines ) {

			m_pDisplayDibSection = &m_ZoomDibSection;

		} else {

			m_pDisplayDibSection = m_pNormalDibSection;

		}

#endif

	} else {

		m_pDisplayDibSection = &m_ZoomDibSection;

		m_LastZoomFactor = zoomFactor;

	}

	// Setup the helper variables
	// ------------------------------------------------------------------------

	m_ZoomFactor = zoomFactor;

	m_Zoom = zoom;

	m_PhysicalCanvasSize = size;

	// Deal with the centering issues if any
	// ------------------------------------------------------------------------

	SyncBorderSizes();

	// Finally setup the scrolling information
	// ------------------------------------------------------------------------

#if 0

	SetScrollSizes(MM_TEXT, m_ViewCanvasSize );

#endif

	if ( invalidateWindow ) {

		InvalidateRect( NULL, false );

	}

	if ( bCenter ) {
		
		M_CenterLogicalPoint( CPoint( size.cx/2, size.cy/2 ) );

	}

}

//
//	CBpaintView::M_SetZoom()
//

void 
CBpaintView::M_SetZoom( const int zoomFactor, const bool invalidateWindow )
{
	if ( 0 >= zoomFactor ) {

		return;

	}

	if ( zoomFactor != m_ZoomFactor ) {

		M_SetupCanvasStructures( zoomFactor, invalidateWindow, false );

		SyncTitleBar();

	}

}

//
//	CBpaintView::M_ClientToLogical()
//

void
CBpaintView::M_ClientToLogical( CPoint & pt )
{
	ASSERT(0 != m_Zoom.cx);
	ASSERT(0 != m_Zoom.cy);

	CPoint scrollPt = GetDeviceScrollPosition();

	pt.x = ((pt.x - m_ForegroundRect.left) + scrollPt.x) / m_Zoom.cx;
	pt.y = ((pt.y - m_ForegroundRect.top) + scrollPt.y) / m_Zoom.cy;

}

//
//	CBpaintView::M_ViewDPtoLP()
//

void
CBpaintView::M_ViewDPtoLP( LPPOINT lpPoints, const int nCount )
{
    ASSERT(0 != m_Zoom.cx);
    ASSERT(0 != m_Zoom.cy);
    ASSERT(m_nMapMode > 0);

	{
		CWindowDC dc( this );
		OnPrepareDC( &dc );
		dc.DPtoLP( lpPoints, nCount );
	}

	for ( int i = 0; i < nCount; i++ ) {

		// adjust for the 'border'

		lpPoints[ i ].x -= m_ForegroundRect.left;
		lpPoints[ i ].y -= m_ForegroundRect.top;

		// adjust for the zoom

		lpPoints[ i ].x /= m_Zoom.cx;
		lpPoints[ i ].y /= m_Zoom.cy;

	}
}

//
//	CBpaintView::M_ViewLPtoDP()
//

void
CBpaintView::M_ViewLPtoDP( LPPOINT lpPoints, const int nCount )
{
    ASSERT(m_nMapMode > 0);

	for ( int i = 0; i < nCount; i++ ) {

		lpPoints[ i ].x *= m_Zoom.cx;
		lpPoints[ i ].y *= m_Zoom.cy;

		// adjust for the 'border'

		lpPoints[ i ].x += m_ForegroundRect.left;
		lpPoints[ i ].y += m_ForegroundRect.top;

	}

    CWindowDC dc( this );
    OnPrepareDC( &dc );
    dc.LPtoDP( lpPoints, nCount );
}

//
//	CBpaintView::M_GetLogicalViewRect()
//

void 
CBpaintView::M_GetLogicalViewRect( CRect & rect )
{
    ASSERT(0 != m_Zoom.cx);
    ASSERT(0 != m_Zoom.cy);

	GetClientRect( &rect );

	rect += GetScrollPosition();

	rect.left /= m_Zoom.cx;
	rect.top /= m_Zoom.cy;
	rect.right = ((rect.right + m_Zoom.cx - 1) / m_Zoom.cx);
	rect.bottom = ((rect.bottom + m_Zoom.cy - 1) / m_Zoom.cy);

}

//
//	CBpaintView::M_ScrollToLogicalPosition()
//

void 
CBpaintView::M_ScrollToLogicalPosition( const CPoint & pt )
{
	CPoint npt( pt.x * m_Zoom.cx, pt.y * m_Zoom.cy );

	CSize totalSize = GetTotalSize();

	CRect clientRect;

	GetClientRect( &clientRect );

	CSize limit(
		max( 0, (totalSize.cx - clientRect.Width()) ), 
		max( 0, (totalSize.cy - clientRect.Height()) )
	);

	npt.x = max( 0, min( npt.x, limit.cx ));
	npt.y = max( 0, min( npt.y, limit.cy ));

	// call OnScrollBy() to get the other views to sync up.
	// ------------------------------------------------------------------------

	CPoint xpt = GetScrollPosition();
	OnScrollBy( CSize( npt.x - xpt.x, npt.y - xpt.y ), true );

}

//
//	CBpaintView::M_CenterLogicalPoint()
//

void 
CBpaintView::M_CenterLogicalPoint( const CPoint & logicalPt )
{
	CRect logicalRect;

	M_GetLogicalViewRect( logicalRect );

	M_ScrollToLogicalPosition(
		CPoint( 
			logicalPt.x - (logicalRect.Width() / 2),
			logicalPt.y - (logicalRect.Height() / 2)
		)
	);
}

//
//	CBpaintView::M_MakeLogicalPointVisible()
//

void 
CBpaintView::M_MakeLogicalPointVisible( const CPoint & logicalPt, const CSize * pBorder )
{
	// Determine the current logical rect with border adjustment.
	// ------------------------------------------------------------------------

	CRect logicalRect;

	M_GetLogicalViewRect( logicalRect );

	CSize border = (pBorder) ? *pBorder : CSize(0,0);

	logicalRect.left += border.cx;
	logicalRect.top += border.cy;
	logicalRect.right -= border.cx;
	logicalRect.bottom -= border.cy;

	// Determine if we need to scroll and if so scroll the "minimum".
	// ------------------------------------------------------------------------

	if ( !logicalRect.PtInRect( logicalPt ) ) {

		CPoint pt;

		// determine new x scroll position
		// --------------------------------------------------------------------

		pt.x = logicalRect.left - border.cx;

		if ( logicalPt.x >= logicalRect.right ) {

			pt.x += (logicalPt.x - (logicalRect.right));

		} else if ( logicalPt.x < logicalRect.left ) {

			pt.x += (logicalPt.x - logicalRect.left);

		}

		// determine new y scroll position
		// --------------------------------------------------------------------

		pt.y = logicalRect.top - border.cy;

		if ( logicalPt.y >= logicalRect.bottom ) {

			pt.y += (logicalPt.y - (logicalRect.bottom));

		} else if ( logicalPt.y < logicalRect.top ) {

			pt.y += (logicalPt.y - logicalRect.top);

		}

		// Ask the scrolling primitive to scroll.
		// --------------------------------------------------------------------

		M_ScrollToLogicalPosition( pt );

	}

}

//
//	CBpaintView::M_InvalidateLogicalRect()
//

void 
CBpaintView::M_InvalidateLogicalRect( const CRect * pRect, const bool bErase )
{
	if ( pRect ) {

		// Scale up the coordinates

		CRect phRect(
			pRect->left * m_Zoom.cx, 
			pRect->top * m_Zoom.cy,
			pRect->right * m_Zoom.cx + 1,
			pRect->bottom * m_Zoom.cy + 1
		);

		// Convert logical coords to physical coords

		CWindowDC dc( this );
		OnPrepareDC( &dc );
		dc.LPtoDP( &phRect );

		// adjust for the potential border.

		phRect.OffsetRect( m_ForegroundRect.left, m_ForegroundRect.top );

		// increase

		--phRect.left;
		--phRect.top;

		++phRect.right;
		++phRect.bottom;

		// finally do something

		InvalidateRect( &phRect, bErase );

	} else {

		InvalidateRect( NULL, bErase );

	}

}

void 
CBpaintView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// Check to see if there was a hint and if so what type

	if ( pHint ) {

		if ( pHint->IsKindOf( RUNTIME_CLASS( CEditorRectChange ) ) ) {

			CRect rect( ((CEditorRectChange *)pHint)->GetRect() );

			switch ( lHint ) {

			default:
			case CEditorRectChange::INVALIDATE_RECT: // Update message

				InvalidateLogicalRect( &rect, false );

				if ( m_bImediateUpdates &&
					(CEditorRectChange::SILENT_INVALIDATE_RECT != lHint) ) {

					UpdateWindow();

				}
				break;

			case CEditorRectChange::SYNC_VIEW_RECT: // Scroll message
				if ( this != pSender ) {

					AutoSyncViewPos( rect );

				}
				break;

			case CEditorRectChange::CENTER_VIEW_REQUEST: // Scroll message
				if ( this != pSender ) {

					bool oldMode = m_bReportViewPosChanges;

					m_bReportViewPosChanges = false;

					M_CenterLogicalPoint( rect.CenterPoint() );

					m_bReportViewPosChanges = oldMode;

				}
				break;

			}

		}

	}

}

BOOL CBpaintView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if ( !( MK_CONTROL & nFlags) ) {

		return CScrollView::OnMouseWheel(nFlags, zDelta, pt);

	}

	CPoint crsrPos;
	
	CursorPosToLogical( crsrPos );

	if ( 0 < zDelta ) {

		if ( 1 < m_ZoomFactor ) {

			ZoomDelta( -1, &crsrPos );

		}

	} else if ( 0 > zDelta ) {

		if ( 32 > m_ZoomFactor ) {

			ZoomDelta( +1, &crsrPos );

		}

	}

	return TRUE;

}

void
CBpaintView::ZoomDelta( const int delta, const CPoint * pCenter )
{
	// ------------------------------------------------------------------------

	CPoint center;

	if ( pCenter ) {

		center = *pCenter;

	} else {

		CRect logicalRect;
	
		M_GetLogicalViewRect( logicalRect );
	
		center = CPoint(
			((logicalRect.right - 1) + logicalRect.left) / 2, 
			((logicalRect.bottom - 1) + logicalRect.top) / 2
		);

	}

	// ------------------------------------------------------------------------

	M_SetZoom( m_ZoomFactor + delta, true );

	// ------------------------------------------------------------------------

	M_CenterLogicalPoint( center );

}

void CBpaintView::OnVzoomDec() 
{
	CPoint pt;

	ZoomDelta( -1, &CursorPosToLogical( pt ) );
}

void CBpaintView::OnUpdateVzoomDec(CCmdUI* pCmdUI) 
{
	if ( 1 < m_ZoomFactor ) {

		pCmdUI->Enable( TRUE );

	} else {

		pCmdUI->Enable( FALSE );

	}
}

void CBpaintView::OnVzoomInc() 
{
	CPoint pt;

	ZoomDelta( +1, &CursorPosToLogical( pt ) );
}

void CBpaintView::OnUpdateVzoomInc(CCmdUI* pCmdUI) 
{
	if ( 32 > m_ZoomFactor ) {

		pCmdUI->Enable( TRUE );

	} else {

		pCmdUI->Enable( FALSE );

	}
}

BOOL CBpaintView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll) 
{
	if ( CScrollView::OnScrollBy( sizeScroll, bDoScroll ) ) {

		if ( bDoScroll ) {

			if ( !s_pControllingScrollView ) {

				s_pControllingScrollView = this;

			}

			if ( this != s_pControllingScrollView ) {

				return TRUE;

			}

			// Reset the timer (destroy any existing)
			// ----------------------------------------------------------------

			if ( s_nScrollControlTimer ) {

				KillTimer( s_nScrollControlTimer );

			}

			s_nScrollControlTimer = SetTimer(
				SCROLL_CONTROL_TIMER_INTERNAL_ID, 500, 0
			);

			if ( !s_nScrollControlTimer ) {

				s_pControllingScrollView = 0;

			}

			// Detect the type of scrolling and have the other views try to
			// keep that direction's pixels in view. (might be lame)
			// ----------------------------------------------------------------

			s_bFavorLeft = (0 > sizeScroll.cx);
			s_bFavorTop = (0 > sizeScroll.cy);

			// Get the current logical rect
			// ----------------------------------------------------------------

			if ( m_bReportViewPosChanges ) {
	
				CRect logicalRect;
				M_GetLogicalViewRect( logicalRect );
	
				// Need to report the new logcial view coordinates.
		
				CBpaintDoc* pDoc = GetDocument();
				ASSERT_VALID(pDoc);
		
				CEditorRectChange logicalViewRect( logicalRect );
		
				pDoc->UpdateAllViews( this, CEditorRectChange::SYNC_VIEW_RECT, &logicalViewRect );

			}

			SyncTitleBar();

		}

		return TRUE;

	}

	return FALSE;
}

// ----------------------------------------------------------------------------

CPoint &  
CBpaintView::CursorPosToLogical( CPoint & pt )
{
	GetCursorPos( &pt );
	ScreenToClient( &pt );
	M_ClientToLogical( pt );
	return pt;
}

void 
CBpaintView::ZoomLogicalPosition( const CPoint & pt, const int zoomFactor )
{
	if ( zoomFactor != m_ZoomFactor ) {

		ZoomDelta( zoomFactor - m_ZoomFactor, &pt );

	} else {

		M_CenterLogicalPoint( pt );
	
	}

}

// ----------------------------------------------------------------------------

void CBpaintView::OnVcmdCenterOnCursor() 
{
	// Figure out where the cursor is for the view
	// ask the view to scroll that point to the center.

	CPoint pt;

	M_CenterLogicalPoint( CursorPosToLogical( pt ) );

}

void CBpaintView::OnUpdateVcmdCenterOnCursor(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

// ----------------------------------------------------------------------------

void CBpaintView::OnVcmdCursorZoom() 
{
	// Get the view relative logical cursor position

	CPoint pt;

	CursorPosToLogical( pt );

	// Okay now have some fun!

	if ( 1 == m_ZoomFactor ) {

		ZoomLogicalPosition( pt, m_LastZoomFactor );

	} else {

		ZoomLogicalPosition( pt, 1 );

	}
}

void CBpaintView::OnUpdateVcmdCursorZoom(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

// ----------------------------------------------------------------------------

void
CBpaintView::OnZoom( UINT nID )
{
	int d = ((nID - ID_VZOOM_1X) + 1) - m_ZoomFactor;
	ZoomDelta( d );
}

void 
CBpaintView::OnUpdateZoom( CCmdUI* pCmdUI )
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( (pCmdUI->m_nID == (m_ZoomFactor + ID_VZOOM_1X - 1)) ? 1 : 0 );

}

// ----------------------------------------------------------------------------

void 
CBpaintView::AutoSyncViewPos( const CRect & rect )
{
	// ------------------------------------------------------------------------

	if ( m_bDontSyncPositions ) {

		return;

	}

	// ------------------------------------------------------------------------

	CRect logicalRect;
	
	M_GetLogicalViewRect( logicalRect );

	// Check to see if were in simple center sync mode
	// ------------------------------------------------------------------------

	if ( m_bCenterSync ) {

		M_CenterLogicalPoint( rect.CenterPoint() );
		return;

	}

	// If not in center mode switch to keep visible mode.
	// ------------------------------------------------------------------------

	int x;

	if ( s_bFavorLeft ) {

		x = min( rect.left, max( rect.right - 1, logicalRect.left ) );

	} else {

		x = min( rect.right - logicalRect.Width(), max( rect.right - 1, logicalRect.left ) );

	}

	int y;

	if ( s_bFavorTop ) {

		y = min( rect.top, max( rect.bottom - 1, logicalRect.top ) );

	} else {

		y = min( rect.bottom - logicalRect.Height(), max( rect.bottom - 1, logicalRect.top ) );

	}

	M_ScrollToLogicalPosition( CPoint( x, y ) );

}

void CBpaintView::OnTimer(UINT nIDEvent) 
{
	CScrollView::OnTimer(nIDEvent);

	if ( SCROLL_CONTROL_TIMER_INTERNAL_ID == nIDEvent ) {

		s_pControllingScrollView = 0;

		if ( s_nScrollControlTimer ) {

			KillTimer( s_nScrollControlTimer );
			s_nScrollControlTimer = 0;

		}

	}

}

BOOL CBpaintView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CBpaintDoc* pDoc = GetDocument();
	
	ASSERT_VALID(pDoc);

	// --------------------------------------------------------------------

	CPoint point;

	CursorPosToLogical( point );
	
	// --------------------------------------------------------------------

	if ( pDoc->m_pPickupColorHackDlg ) {

		// Make sure that the cursor is inside 'canvas' area of the view
		// -----------------------------------------------------------------

		CRect logical;

		M_GetLogicalViewRect( logical );

		CRect limit( CPoint(0,0), CSize(pDoc->GetCanvasSize()) );

		if ( !logical.IntersectRect( &logical, &limit ) ) {

			return FALSE;

		}

		if ( !logical.PtInRect( point ) ) {

			return FALSE;

		}

		// If the user is holding down the middle mouse button try
		// to center the cursor location...
		// -----------------------------------------------------------------

		if ( 0x8000 & GetAsyncKeyState( VK_CONTROL ) ) {

			SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEALL ) );

			if ( 0x8000 & GetAsyncKeyState( VK_LBUTTON ) ) {

				M_CenterLogicalPoint( point );

			}

			return TRUE;

		}

	}

	HCURSOR hCrsr = pDoc->GetViewCursor( this, point, nHitTest );
	
	if ( hCrsr ) {

		SetCursor( hCrsr );

		return TRUE;

	} else if ( pDoc->m_pPickupColorHackDlg ) {

		return FALSE;

	}

	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

int CBpaintView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Do this here to avoid assert.

	SetScrollSizes( MM_TEXT, CSize(1,1) );
	
	return 0;
}

void CBpaintView::OnVsetImediateUpdates() 
{
	m_bImediateUpdates = !m_bImediateUpdates;
}

void CBpaintView::OnUpdateVsetImediateUpdates(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();

	pCmdUI->SetCheck( m_bImediateUpdates ? 1 : 0 );
}

void CBpaintView::OnVsetCenterSyncScroll() 
{
	m_bCenterSync = !m_bCenterSync;
}

void CBpaintView::OnUpdateVsetCenterSyncScroll(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_bCenterSync ? 1 : 0 );
}

void CBpaintView::OnVsetDisableAutoScroll() 
{
	m_bDontSyncPositions = !m_bDontSyncPositions;
}

void CBpaintView::OnUpdateVsetDisableAutoScroll(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_bDontSyncPositions ? 1 : 0 );
}

void CBpaintView::OnVsetShowFatbits() 
{
	m_bFatBitLines = !m_bFatBitLines;
	InvalidateRect( NULL, FALSE );
}

void CBpaintView::OnUpdateVsetShowFatbits(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );
	pCmdUI->SetCheck( m_bFatBitLines ? 1 : 0 );
}

void CBpaintView::OnVsetShowGrid() 
{
	m_bShowGridLines = !m_bShowGridLines;
	M_SetupCanvasStructures( m_ZoomFactor, true, false );
}

void CBpaintView::OnUpdateVsetShowGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );
	pCmdUI->SetCheck( m_bShowGridLines ? 1 : 0 );
}

void CBpaintView::OnVcmdRequestOthersCenterOnCursor() 
{
	CBpaintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPoint pt;

	CRect rect( CursorPosToLogical( pt ), CSize( 1, 1 ) );
		
	CEditorRectChange logicalViewRect( rect );
		
	pDoc->UpdateAllViews( this, CEditorRectChange::CENTER_VIEW_REQUEST, &logicalViewRect );
}

void CBpaintView::OnVsetCompositeMode() 
{
	m_bEditView = !m_bEditView;

	// Need to switch the dib section used for blitting!

	m_pNormalDibSection = M_GetCanvasDib();

	if ( m_pDisplayDibSection != &m_ZoomDibSection ) {

		m_pDisplayDibSection = m_pNormalDibSection;

	}

	// Invalidate the window

	InvalidateRect( NULL, FALSE );

	// This should probably tell the document somehow

	CBpaintDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	pDoc->ForceFullRedraw();

	SyncTitleBar();
}

void CBpaintView::OnUpdateVsetCompositeMode(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();

	pCmdUI->SetCheck( (!m_bEditView) ? 1 : 0 );
}

void CBpaintView::On2VsetCompositeMode() 
{
	OnVsetCompositeMode();
}

void CBpaintView::OnUpdate2VsetCompositeMode(CCmdUI* pCmdUI) 
{
	OnUpdateVsetCompositeMode( pCmdUI );
}

void CBpaintView::OnCenterView() 
{
	M_CenterLogicalPoint( CPoint( m_PhysicalCanvasSize.cx/2, m_PhysicalCanvasSize.cy/2 ) );
}

// ----------------------------------------------------------------------------

void CBpaintView::SyncTitleBar() {

	// ------------------------------------------------------------------------

	CString docTitle = GetDocument()->GetTitle();

	if (GetDocument()->CurrentAnimation()) {

		docTitle = docTitle + " : " + GetDocument()->CurrentAnimation()->GetName();

	}
	
	// ------------------------------------------------------------------------

	CString modeStr;

	if ( m_bEditView ) {

		modeStr = "Edit";

	} else {

		modeStr = "Layered edit";

	}

	// ------------------------------------------------------------------------

	CRect logicalRect;

	M_GetLogicalViewRect( logicalRect );

	// ------------------------------------------------------------------------

 	CString finalTitle;

	CSize pixelScale;

	GetDocument()->GetPixelAspectRatio( &pixelScale );

	if ( CSize(1,1) == pixelScale ) {

		finalTitle.Format( "%s, zoom x %d, view %d, %d , %d, %d ( %s )",
			modeStr, m_ZoomFactor, 
			logicalRect.left, logicalRect.top, logicalRect.right - 1, logicalRect.bottom - 1, 
			docTitle
		);

	} else {

		finalTitle.Format( "%s, zoom (x:%d,y:%d), view %d, %d , %d, %d ( %s )",
			modeStr, m_ZoomFactor * pixelScale.cx, m_ZoomFactor * pixelScale.cy,
			logicalRect.left, logicalRect.top, logicalRect.right - 1, logicalRect.bottom - 1, 
			docTitle
		);

	}

	// ------------------------------------------------------------------------

	SetWindowText( finalTitle );

	GetParentFrame()->SetWindowText( finalTitle );

}

// ----------------------------------------------------------------------------

//
//	CBpaintView::GetInternalSetting()
//

int CBpaintView::GetInternalSetting(
	const char * sectionName, const char * baseEntryName,
	const char * entry, const int value
)
{
	CString finalEntryName;

	finalEntryName.Format( "%s.%s", baseEntryName, entry );

	return GLOBAL_GetSettingInt( finalEntryName, value, sectionName );
}

//
//	CBpaintView::PutInternalSetting()
//

void CBpaintView::PutInternalSetting(
	const char * sectionName, const char * baseEntryName, 
	const char * entry, const int value
)
{
	CString finalEntryName;

	finalEntryName.Format( "%s.%s", baseEntryName, entry );

	GLOBAL_PutSettingInt( finalEntryName, value, sectionName );

}

// ----------------------------------------------------------------------------

//
//	CBpaintView::StoreInternalSettings()
//

bool CBpaintView::StoreInternalSettings(
	const char * sectionName, const char * baseEntryName
)
{

	PutInternalSetting( sectionName, baseEntryName, "bShowGridLines", m_bShowGridLines ? 1 : 0  );

	// -------------------------------------------------------------------------

	PutInternalSetting( sectionName, baseEntryName, "ZoomFactor", m_ZoomFactor );

	// -------------------------------------------------------------------------

	PutInternalSetting( sectionName, baseEntryName, "bCenterSync", m_bCenterSync ? 1 : 0 );

	// -------------------------------------------------------------------------

	PutInternalSetting( sectionName, baseEntryName, "bImediateUpdates", m_bImediateUpdates ? 1 : 0  );

	// -------------------------------------------------------------------------

	PutInternalSetting( sectionName, baseEntryName, "bDontSyncPositions", m_bDontSyncPositions ? 1 : 0  );

	// -------------------------------------------------------------------------

	PutInternalSetting( sectionName, baseEntryName, "bFatBitLines", m_bFatBitLines ? 1 : 0  );

	// -------------------------------------------------------------------------

	PutInternalSetting( sectionName, baseEntryName, "bCenterOtherViewsOnMButton", m_bCenterOtherViewsOnMButton ? 1 : 0  );
	
	// -------------------------------------------------------------------------

	return true;
}

//
//	CBpaintView::RestoreInternalSettings()
//

bool CBpaintView::RestoreInternalSettings(
	const char * sectionName, const char * baseEntryName
)
{

	m_bShowGridLines = (1 == GetInternalSetting( sectionName, baseEntryName, "bShowGridLines", m_bShowGridLines ? 1 : 0 ));

	// -------------------------------------------------------------------------

	m_ZoomFactor = GetInternalSetting( sectionName, baseEntryName, "ZoomFactor", m_ZoomFactor );

	// -------------------------------------------------------------------------

	m_bCenterSync = (1 == GetInternalSetting( sectionName, baseEntryName, "bCenterSync", m_bCenterSync ? 1 : 0 ));

	// -------------------------------------------------------------------------

	m_bImediateUpdates = (1 == GetInternalSetting( sectionName, baseEntryName, "bImediateUpdates", m_bImediateUpdates ? 1 : 0 ));

	// -------------------------------------------------------------------------

	m_bDontSyncPositions = (1 == GetInternalSetting( sectionName, baseEntryName, "bDontSyncPositions", m_bDontSyncPositions ? 1 : 0 ));

	// -------------------------------------------------------------------------

	m_bFatBitLines = (1 == GetInternalSetting( sectionName, baseEntryName, "bFatBitLines", m_bFatBitLines ? 1 : 0 ));

	// -------------------------------------------------------------------------

	m_bCenterOtherViewsOnMButton = (1 == GetInternalSetting(
		sectionName, baseEntryName, "bCenterOtherViewsOnMButton", 
		m_bCenterOtherViewsOnMButton ? 1 : 0 ));

	// -------------------------------------------------------------------------

	M_SetupCanvasStructures( m_ZoomFactor, true, false );

	// -------------------------------------------------------------------------

	SyncTitleBar();

	return true;
}

// ----------------------------------------------------------------------------

void CBpaintView::OnDestroy() 
{
	if ( (this == s_pControllingScrollView) && s_nScrollControlTimer ) {

		KillTimer( s_nScrollControlTimer );

		s_nScrollControlTimer = 0;

		s_pControllingScrollView = 0;

	}

	CScrollView::OnDestroy();
}
