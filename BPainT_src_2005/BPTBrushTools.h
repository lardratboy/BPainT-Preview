// BPTBrushTools.h
//
//	Copyright (c) 2002, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//////////////////////////////////////////////////////////////////////

#if !defined(BPTBRUSHTOOLS_H__73582BC5_B520_471d_8D1F_B83F139AB97D_INCLUDED_)
#define BPTBRUSHTOOLS_H__73582BC5_B520_471d_8D1F_B83F139AB97D_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ============================================================================

#include "BPTBreakDown.h"

// ============================================================================
// Brush Mediator/reciever
// ============================================================================

//
//	CPickupBrushMediator
//

class CPickupBrushMediator : public BPT::TISupportProperties<> {

public: // traits

	typedef CBpaintDoc::editor_bitmap_type bitmap_type;
	typedef CBpaintDoc::editor_bitmap_type::pointer bitmap_pointer;

private: // data

	CBpaintDoc * m_pDoc;

	CPickupBrushMediator(); // Hidden

	bitmap_pointer m_pLastBitmap;
	bitmap_pointer m_pCurrentBitmap;
	bitmap_pointer m_pTransformedBitmap;

	RECT m_PickupRect;

	enum {

		SPECIAL_CASE_SCALE_SHIFT	= 10

	};

	bitmap_type::pixel_type m_InitialBrushChromakey;

private: // Internal methods

	// Deformations
	// ------------------------------------------------------------------------

	bitmap_pointer QuadDeformBrush( 
		bitmap_pointer pBitmap, const POINT * pQuad, 
		const bitmap_type::pixel_type chromakey,
		RECT * pWantBounding = 0) {

		// ---------------------------------------------------------------------

		bitmap_pointer pNew = new bitmap_type();

		if ( !pNew ) {

			return 0;

		}

		// Find the bounding rect for this quad
		// ---------------------------------------------------------------------
		
		RECT boundingRect = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };

		for ( int i = 0; i < 4; i++ ) {

			if ( pQuad[ i ].x < boundingRect.left ) boundingRect.left = pQuad[ i ].x;
			if ( pQuad[ i ].y < boundingRect.top ) boundingRect.top = pQuad[ i ].y;
			if ( pQuad[ i ].x > boundingRect.right ) boundingRect.right = pQuad[ i ].x;
			if ( pQuad[ i ].y > boundingRect.bottom ) boundingRect.bottom = pQuad[ i ].y;

		}

		if ( pWantBounding ) {

			*pWantBounding = boundingRect;

		}

		int dx = -boundingRect.left;
		int dy = -boundingRect.top;

		// Adjust the quad so that the it has positive coordinates
		// ---------------------------------------------------------------------

		POINT correctedQuad[ 4 ];

		for ( i = 0; i < 4; i++ ) {

			correctedQuad[ i ].x = pQuad[ i ].x + dx;
			correctedQuad[ i ].y = pQuad[ i ].y + dy;

		}

		// Create and clear the transformed bitmap
		// ---------------------------------------------------------------------

		int w = boundingRect.right - boundingRect.left;
		int h = boundingRect.bottom - boundingRect.top;

		if ( !pNew->Create( w, h ) ) {

			TRACE( "Unable to size transformed bitmap to %dx%d\n", w, h );

			delete pNew;

			return 0;

		}

		pNew->ClearBuffer( chromakey );

		// Finally do the operation
		// ---------------------------------------------------------------------

		BPT::T_DrawSurfaceToQuad(
			*pNew, correctedQuad,
			*pBitmap, BPT::TCopyROP<bitmap_type::pixel_type>(),
			0
		);

		return pNew;

	}

public: // interface

	CPickupBrushMediator( CBpaintDoc * pDoc ) :
		m_pDoc( pDoc ), m_pCurrentBitmap( 0 ), 
		m_pTransformedBitmap( 0 ), m_pLastBitmap( 0 ), m_InitialBrushChromakey(0)
		{
			SetRectEmpty( &m_PickupRect );
		}
	
	~CPickupBrushMediator() {

		if ( m_pLastBitmap ) delete m_pLastBitmap;
		if ( m_pCurrentBitmap ) delete m_pCurrentBitmap;
		if ( m_pTransformedBitmap ) delete m_pTransformedBitmap;

	}

	bool PickupBrush( const RECT & rect ) {

		// Basic error checking
		// --------------------------------------------------------------------

		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;

		if ( (0 >= w) || (0 >= h) ) {

			TRACE( "Invalid rect size %d, %d passed to PickupBrush\n", w, h );

			return false;

		}

		SIZE canvasSize = m_pDoc->GetCanvasSize();

		RECT clipRect = { 0, 0, canvasSize.cx, canvasSize.cy };

		RECT clippedRect;

		if ( !IntersectRect( &clippedRect, &rect, &clipRect ) ) {

			TRACE( "Rect %d,%d,%d,%d wasn't valid\n", rect.left, rect.top, rect.right, rect.bottom );
			return false;

		}

		// get the paint logic object
		// --------------------------------------------------------------------

		CBpaintDoc::paint_logic_type::pointer pLogic = m_pDoc->GetPaintLogicPtr();

		if ( !pLogic ) {

			TRACE0( "CPickupBrushMediator has no paint logic pointer?\n" );

			return false;

		}

// DESIGN *********************************************************************
// DESIGN *********************************************************************
// DESIGN *********************************************************************

		bitmap_pointer pEditorBitmap = pLogic->GetCurentBitmapPtr();

		if ( !pEditorBitmap ) {

			TRACE0( "Unable to get current bitmap from the paint logic?\n" );

			// How should this error be dealt with?

			return false;

		}

		// determine the chromakey
		// --------------------------------------------------------------------

		bitmap_type::pixel_type chromakey = m_pDoc->GetBrushChromakey();

		m_InitialBrushChromakey = chromakey;

		// find a solid pixel inside the search rect
		// --------------------------------------------------------------------

		RECT boundingRect;

		if ( !BPT::T_FindBoundingRect(
			boundingRect, *pEditorBitmap,
			BPT::TIsNotValue<bitmap_type::pixel_type>( chromakey ),
			&clippedRect ) ) {

			return false;

		}

		// Get the default coordinates for this brush.
		// --------------------------------------------------------------------

		m_PickupRect = clippedRect; // could be boundingRect;

		w = clippedRect.right - clippedRect.left;
		h = clippedRect.bottom - clippedRect.top;

		// Backup the old brush by swapping the object pointers with the last
		// --------------------------------------------------------------------

		bitmap_pointer t = m_pCurrentBitmap;
		m_pCurrentBitmap = m_pLastBitmap;
		m_pLastBitmap = t;

		// Create a new bitmap object by resizing or creating the bitmap object
		// --------------------------------------------------------------------

		if ( !m_pCurrentBitmap ) {

			m_pCurrentBitmap = new bitmap_type();

			if ( !m_pCurrentBitmap ) {

				TRACE( "Error: Unable to create bitmap object\n" );
				return false;

			}

		}

		// This probably should use a 'smart' create method, which would
		// not reallocate the bitmap unless the current bitmap wasn't 
		// big enough to hold the new brush, this might help keep heap
		// fragmentation down.

		if ( !m_pCurrentBitmap->Create( w, h ) ) {

			TRACE( "Error: Unable to create bitmap size (%d, %d)\n", w, h );

			// How should this error be dealt with?

			return false;

		}

		// 'Pickup' the pixels (How should the stencil be handled?)
		// could use the blit2 function.
		// --------------------------------------------------------------------

		BPT::T_Blit(
			*m_pCurrentBitmap, -m_PickupRect.left, -m_PickupRect.top, *pEditorBitmap,
			CBpaintDoc::paint_copy_op_type(), 
			0, 0, 0
		);

		// --------------------------------------------------------------------

		DestroyTransformedBrush();

// DESIGN *********************************************************************
// DESIGN *********************************************************************
// DESIGN *********************************************************************

		return true;

	}

	void ReportChange() {

		// Report brush change to the document.
		// --------------------------------------------------------------------

		m_pDoc->BrushHasChanged( true );

	}

	bitmap_pointer GetBitmapPtr() {

		if ( m_pTransformedBitmap ) {

			return m_pTransformedBitmap;

		}

		return m_pCurrentBitmap;

	}

	POINT GetDefaultPos() {

		POINT pt = { m_PickupRect.left, m_PickupRect.top };

		return pt;

	}

	RECT GetPickupRect() {

		return m_PickupRect;

	}

	// Helpers
	// ------------------------------------------------------------------------

	void DestroyTransformedBrush() {

		if ( m_pTransformedBitmap ) {

			delete m_pTransformedBitmap;

			m_pTransformedBitmap = 0;

		}

	}

	// Brush actions
	// ------------------------------------------------------------------------

	void RestoreBrush() {

		DestroyTransformedBrush();

		m_pDoc->BrushHasChanged( false );

	}

	void RestoreToBackup() {

		if ( m_pLastBitmap ) {

			bitmap_pointer t = m_pCurrentBitmap;
			m_pCurrentBitmap = m_pLastBitmap;
			m_pLastBitmap = t;

			DestroyTransformedBrush();

		}

		m_pDoc->BrushHasChanged( false );

	}

	// Transformations 
	// ------------------------------------------------------------------------
	
	bool ScaledRotate(
		const float fAngle, const float fXScale, const float fYScale, 
		const bitmap_type::pixel_type chromakey, const bool bUseOriginal,
		const CSize * pForceSize = 0) {

		// Get the currently active brush image
		// --------------------------------------------------------------------

		bitmap_pointer pCurrent = GetBitmapPtr();
		
		if ( !pCurrent ) {

			return false;

		}

		// Check for special case
		// --------------------------------------------------------------------

		if ( (90.0f == fAngle) && (1.0f == fXScale) && (1.0f == fYScale) ) {

			bitmap_pointer pNew = BPT::T_Create90DegreeRotatedBitmap( *pCurrent );

			if ( !pNew ) {

				return false; 

			}

			DestroyTransformedBrush();
			m_pTransformedBitmap = pNew;
			m_pDoc->BrushHasChanged( false );
			return true;

		}

		// Create the rotated quad
		// --------------------------------------------------------------------

		int w, h;

		if ( pForceSize ) {

			w = pForceSize->cx;
			h = pForceSize->cy;

		} else {

			w = max( 1, (int)(pCurrent->Width() * fXScale));
			h = max( 1, (int)(pCurrent->Height() * fYScale));

		}

		// Switch out which bitmap to use
		// --------------------------------------------------------------------

		if ( bUseOriginal ) {

			pCurrent = m_pCurrentBitmap;

		}

		// Check for special case scale
		// --------------------------------------------------------------------

#if 1

		if ( 0.0f == fAngle ) {

			bitmap_pointer pNew;

			if ( (w < pCurrent->Width()) && (h < pCurrent->Height()) ) {

				typedef BPT::TCopyROP<bitmap_type::pixel_type> transfer_op_type;
	
				pNew = BPT::T_TransparentAwareCreateScaledBitmap<
					bitmap_type, SPECIAL_CASE_SCALE_SHIFT, transfer_op_type
				>(
					*pCurrent, w, h, chromakey, transfer_op_type()
				);

			} else {

				pNew = BPT::T_CreateScaledBitmap<
					bitmap_type, SPECIAL_CASE_SCALE_SHIFT
				>(
					*pCurrent, w, h
				);

			}

			if ( !pNew ) {

				return false; 

			}

			DestroyTransformedBrush();
			m_pTransformedBitmap = pNew;
			m_pDoc->BrushHasChanged( false );
			return true;

		}

#endif

		// We've exhausted our special cases drop into the texture mapper.
		// --------------------------------------------------------------------

		int x = -w/2;
		int y = -h/2;

		int x2 = x + w;
		int y2 = y + h;

#if 1 

		// Hack to make the rotate 90 work (this makes the brush grow the more you
		// rotate it, quite lame!)

		if ( 0.0f != fAngle ) {

			x2 += 1;
			y2 += 1;

		}

#endif

		POINT quad[ 4 ] = {
			{ x, y }, { x2, y }, { x2, y2 }, { x, y2 }
		};

		float radAngle = (3.141592654f * fAngle) / 180.0f;
		float sa = sinf( radAngle );
		float ca = cosf( radAngle );

		for ( int j = 0; j < 4; j++ ) {

			float tx = (float)quad[ j ].x;
			float ty = (float)quad[ j ].y;

			quad[ j ].x = (int)(tx * ca - ty * sa);
			quad[ j ].y = (int)(tx * sa + ty * ca);

		}

		// ask the deformation code to do it's job
		// --------------------------------------------------------------------

		bitmap_pointer pNew = QuadDeformBrush( pCurrent, quad, chromakey );

		DestroyTransformedBrush();

		if ( pNew ) {

			m_pTransformedBitmap = pNew;

			m_pDoc->BrushHasChanged( false );

			return true;

		}

		return false;

	}

	bool BLITFXTransform(
		const BPT::BLITFX blitFX, const bitmap_type::pixel_type chromakey
	) {

		// Get the currently active brush image
		// --------------------------------------------------------------------

		bitmap_pointer pCurrent = GetBitmapPtr();

		if ( !pCurrent ) {

			return false;

		}

		// --------------------------------------------------------------------

		bitmap_pointer pNew = new bitmap_type();

		if ( pNew ) {

			int w = pCurrent->Width();
			int h = pCurrent->Height();

			// ----------------------------------------------------------------

			if ( !pNew->Create( w, h ) ) {

				TRACE( "Unable to size brush %dx%d\n", w, h );

				delete pNew;

				return false;

			}

			// Ask the blitter to perform the operation.
			// ----------------------------------------------------------------
	
			BPT::T_Blit(
				*pNew, 0, 0, *pCurrent,
				CBpaintDoc::paint_copy_op_type(), 
				&blitFX, 0, 0
			);

			// ----------------------------------------------------------------

			DestroyTransformedBrush();

			m_pTransformedBitmap = pNew;

			m_pDoc->BrushHasChanged( false );

			return true;

		}

		return false;

	}

	bool ForceSize(
		CSize & size, const bitmap_type::pixel_type chromakey, 
		const bool bUseOriginal = false
	) {

		// Get the currently active brush image
		// --------------------------------------------------------------------

		bitmap_pointer pCurrent = GetBitmapPtr();

		if ( !pCurrent ) {

			return false;

		}

		// Figure out the scale	(lame)
		// --------------------------------------------------------------------

		float xScale = (float)size.cx / (float)pCurrent->Width();
		float yScale = (float)size.cy / (float)pCurrent->Height();

		return ScaledRotate(
			0.0f, xScale, yScale, chromakey, bUseOriginal, &size
		);

	}

	bool DeformBrush( const POINT * pQuad, const bitmap_type::pixel_type chromakey ) {

		// Get the currently active brush image
		// --------------------------------------------------------------------

		bitmap_pointer pCurrent = GetBitmapPtr();

		if ( !pCurrent ) {

			return false;

		}

		// ----------------------------------------------------------------

		bitmap_pointer pNew = QuadDeformBrush(
			pCurrent, pQuad, chromakey, &m_PickupRect
		);

		if ( !pNew ) {

			return false;

		}

		// ----------------------------------------------------------------

		DestroyTransformedBrush();

		m_pTransformedBitmap = pNew;

		m_pDoc->BrushHasChanged( false );

		return true;

	}

	// ------------------------------------------------------------------------

	bool ExternalSetBrushToBitmap(
		bitmap_pointer pRequestedBitmap, 
		const bitmap_type::pixel_type chromakey,
		const bool bOwnBitmap
	) {

		// --------------------------------------------------------------------

		if ( !pRequestedBitmap ) {

			return false;

		}

		// --------------------------------------------------------------------

		bitmap_pointer pNew;

		if ( bOwnBitmap ) {

			pNew = pRequestedBitmap;

		} else {

			// Create / size the bitmap
			// ----------------------------------------------------------------

			pNew = new bitmap_type();

			if ( !pNew ) {

				TRACE( "Failed to create bitmap object\n" );

				return false;

			}

			if ( !pNew->Create( pRequestedBitmap->Width(), pRequestedBitmap->Height() ) ) {

				TRACE( 
					"Failed to create bitmap %dx%d\n", 
					pRequestedBitmap->Width(), pRequestedBitmap->Height()
				);

				delete pNew;

				return false;

			}

			// ----------------------------------------------------------------

			BPT::T_Blit(
				*pNew, 0, 0, *pRequestedBitmap, 
				CBpaintDoc::paint_copy_op_type()
			);

		}

		// Remove any transformed brush
		// --------------------------------------------------------------------

		DestroyTransformedBrush();

		// Backup the existing brush 
		// --------------------------------------------------------------------

		if ( m_pLastBitmap ) {

			delete m_pLastBitmap;

		}

		m_pLastBitmap = m_pCurrentBitmap;

		// Finally set the brush and return that it was ineed set
		// ----------------------------------------------------------------

		m_pCurrentBitmap = pNew;

		int forceChromakey = chromakey;

		m_pDoc->BrushHasChanged( false, &forceChromakey );

		m_InitialBrushChromakey = chromakey;

		return true;

	}

	// New attached capture
	// ------------------------------------------------------------------------

	bool operator()( bitmap_type & surface, RECT & rect, bitmap_type::pixel_type chromakey ) {

		m_PickupRect = rect;

		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;

		// Backup the old brush by swapping the object pointers with the last
		// --------------------------------------------------------------------

		bitmap_pointer t = m_pCurrentBitmap;
		m_pCurrentBitmap = m_pLastBitmap;
		m_pLastBitmap = t;

		// Create a new bitmap object by resizing or creating the bitmap object
		// --------------------------------------------------------------------

		if ( !m_pCurrentBitmap ) {

			m_pCurrentBitmap = new bitmap_type();

			if ( !m_pCurrentBitmap ) {

				TRACE( "Error: Unable to create bitmap object\n" );

				return false;

			}

		}

		// This probably should use a 'smart' create method, which would
		// not reallocate the bitmap unless the current bitmap wasn't 
		// big enough to hold the new brush, this might help keep heap
		// fragmentation down.

		if ( !m_pCurrentBitmap->Create( w, h ) ) {

			TRACE( "Error: Unable to create bitmap size (%d, %d)\n", w, h );

			// How should this error be dealt with?

			return false;

		}

		// Pickup the pixels (How should the stencil be handled?)
		// --------------------------------------------------------------------

		BPT::T_Blit(
			*m_pCurrentBitmap, -m_PickupRect.left, -m_PickupRect.top, surface,
			CBpaintDoc::paint_copy_op_type(), 
			0, 0, 0
		);

		// --------------------------------------------------------------------

		DestroyTransformedBrush();

		m_InitialBrushChromakey = chromakey;

		return true;

	}

	bool PickupAdjacent( RECT & rect ) {

		if ( !m_pDoc ) return false;

		// get the paint logic object
		// --------------------------------------------------------------------

		CBpaintDoc::paint_logic_type::pointer pLogic = m_pDoc->GetPaintLogicPtr();

		if ( !pLogic ) {

			TRACE0( "CPickupBrushMediator has no paint logic pointer?\n" );

			return false;

		}

		// get the editor bitmap
		// --------------------------------------------------------------------

		bitmap_pointer pEditorBitmap = pLogic->GetCurentBitmapPtr();

		if ( !pEditorBitmap ) {

			TRACE0( "Unable to get current bitmap from the paint logic?\n" );

			// How should this error be dealt with?

			return false;

		}

		// determine the chromakey
		// --------------------------------------------------------------------

		bitmap_type::pixel_type chromakey = m_pDoc->GetBrushChromakey();

		// find a solid pixel inside the search rect
		// --------------------------------------------------------------------

		POINT found;

		if ( !BPT::T_FindPredicateXYInRect(
			found, *pEditorBitmap,
			BPT::TIsNotValue<bitmap_type::pixel_type>( chromakey ),
			&rect ) ) {

			return false;

		}

		// Okay now use the BreakDownSurface template class to find the 
		// adjacent pixels
		// --------------------------------------------------------------------

		BPT::TBreakDownSurface<bitmap_type> breakdown;

		if ( !breakdown.CaptureImageAtXY(
			*this, *pEditorBitmap, found.x, found.y, chromakey, true, 0, 1, 1 ) ) {

			return false;

		}

		return true;

	}

	// effects
	// ------------------------------------------------------------------------

	// Transformations 
	// ------------------------------------------------------------------------
	
	bool Outline(
		const bitmap_type::pixel_type outlineColor,
		const bitmap_type::pixel_type chromakey
	) {

		// Get the currently active brush image
		// --------------------------------------------------------------------

		bitmap_pointer pCurrent = GetBitmapPtr();

		if ( !pCurrent ) {

			return false;

		}

		// Create the bitmap
		// --------------------------------------------------------------------

		bitmap_pointer pNew = new bitmap_type;

		if ( !pNew ) {

			return false; 

		}

		if ( !pNew->Create( pCurrent->Width() + 2, pCurrent->Height() + 2 ) ) {

			delete pNew;

			return false;

		}

		// Do the outline operation in a really cheesy fashion
		// --------------------------------------------------------------------

		pNew->ClearBuffer( chromakey );

		typedef BPT::TMultiModeBrushTransferROP< bitmap_type::pixel_type > helper_type;

		// top

		BPT::T_Blit( 
			*pNew, 1, 0, *pCurrent, helper_type(
				helper_type::SINGLE, outlineColor, chromakey
			 )
		);

		// left

		BPT::T_Blit( 
			*pNew, 0, 1, *pCurrent, helper_type(
				helper_type::SINGLE, outlineColor, chromakey
			 )
		);

		// right

		BPT::T_Blit( 
			*pNew, 2, 1, *pCurrent, helper_type(
				helper_type::SINGLE, outlineColor, chromakey
			 )
		);

		// bottom

		BPT::T_Blit( 
			*pNew, 1, 2, *pCurrent, helper_type(
				helper_type::SINGLE, outlineColor, chromakey
			 )
		);

		// put the original back down

		BPT::T_Blit( 
			*pNew, 1, 1, *pCurrent, helper_type(
				helper_type::SOURCE, outlineColor, chromakey
			 )
		);

		// Adjust the pickup rect.
		// --------------------------------------------------------------------

		--m_PickupRect.left;
		--m_PickupRect.top;

		++m_PickupRect.right;
		++m_PickupRect.bottom;

		// --------------------------------------------------------------------

		DestroyTransformedBrush();

		m_pTransformedBitmap = pNew;

		m_pDoc->BrushHasChanged( false );

		return true;

	}

	bitmap_type::pixel_type GetInitialChromakey() const {

		return m_InitialBrushChromakey;

	}

	// properties interface?
	// ------------------------------------------------------------------------

};

// ============================================================================
// Brush pickup tools
// ============================================================================

//
//	CPickupBrushTool
//

class CPickupBrushTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

	CPickupBrushMediator * m_pBrushMediator;

	bool m_bForceRender;

	CPickupBrushTool(); // Hidden

public: // Interface

	CPickupBrushTool( CPickupBrushMediator * pBrushMediator ) : 
		m_pBrushMediator( pBrushMediator), m_bForceRender( false ) { /* Empty */ }

	virtual void InputUp( tool_msg_type & info ) {

		m_bForceRender = true;
		
		base_type::InputUp( info );

		Repaint( info );

		m_bForceRender = false;

		// calculate the rect.
		// ------------------------------------------------------------

		RECT rect;

		CalcRectFromUpAndDownPos( rect );

		SIZE size;

		size.cx = rect.right - rect.left;
		size.cy = rect.bottom - rect.top;

		// Ask the brush mediator to make this selection a brush.
		// ------------------------------------------------------------

		bool bUsedAdjacentPickup = false;

		if ( m_pBrushMediator ) {

#if 1 
			// Try to make the pickup brush process smoother.
			// I don't think this will effect anything but it might.

			m_LastPos = m_CurrentPos;
			m_DownPos = m_CurrentPos;
			m_UpPos = m_CurrentPos;
			m_ButtonDown = false;

#endif

			bool bTrimEdgeSinglePixel = (0 == size.cx) && (0 == size.cy)
				&& (tool_msg_type::X_TRIM_EDGE & m_InternalState.buttonDownFlags );

			if ( (((1 == size.cx) && (1 == size.cy)) ||
				tool_msg_type::X_CONTROL & m_InternalState.buttonDownFlags) ||
				bTrimEdgeSinglePixel
				) {

#if 1 // BPT 10/9/02

				if ( bTrimEdgeSinglePixel ) {

					// inflate the rect to be a single pixel

					++rect.right;
					++rect.bottom;

				}

				// Try to isolate the image touching any on pixel 
				// contained in the rectangle

				if ( !m_pBrushMediator->PickupAdjacent( rect ) ) {

					return;

				}

				bUsedAdjacentPickup = true;

#endif

			} else {

				if ( !m_pBrushMediator->PickupBrush( rect ) ) {

					return;

				}

			}

		}

		// ------------------------------------------------------------

		if ( tool_msg_type::X_RBUTTON & m_InternalState.buttonDownFlags ) {

			if ( bUsedAdjacentPickup ) {

				// erase only the picked up image

				tool_msg_type::render_target_type::surface_type::pointer pCanvas = 
					info.renderTarget.GetCanvas();

				CPickupBrushMediator::bitmap_pointer pBrushSurface =
					m_pBrushMediator->GetBitmapPtr();

				if ( pCanvas && pBrushSurface ) {

					RECT theRect = m_pBrushMediator->GetPickupRect();

					tool_msg_type::render_target_type::surface_type::pixel_type chromakey =
						m_pBrushMediator->GetInitialChromakey();

					BPT::TSingleColorTransparentSrcTransferROP<
						tool_msg_type::render_target_type::surface_type::pixel_type
					> singleColorTransferOp( chromakey, chromakey );

					BPT::T_Blit(
						*pCanvas, theRect.left, theRect.top, 
						*pBrushSurface, singleColorTransferOp, 0, 0, 0
					);

					info.renderTarget.AddDirtyRect( theRect );

				} else {

					// fall back to the solid rect erase

					BPT::T_SolidRect( info.renderTarget, m_pBrushMediator->GetPickupRect(), *info.pPen ); // rect -> m_PickupRect 10/9/02

				}

			} else {

				BPT::T_SolidRect( info.renderTarget, m_pBrushMediator->GetPickupRect(), *info.pPen ); // rect -> m_PickupRect 10/9/02

			}

		}

		UpdateEcho( info );

#if 1 // BPT 10/9/02

		// Handle the stencil
		// ------------------------------------------------------------

		ToolRenderPostProcess( info );

#endif

		// Ask the brush mediator to make this selection a brush.
		// ------------------------------------------------------------

		if ( m_pBrushMediator ) {

			m_pBrushMediator->ReportChange();

		}

	}

	virtual void Repaint( tool_msg_type & info ) {

		EraseLastChanges( info );

		if ( m_ButtonDown || m_bForceRender ) {

			// calculate the rect.

			RECT rect;

			CalcRectFromUpAndDownPos( rect );

			// okay get to work.

			if ( m_ButtonDown ) {

				SetOverlay( info, tool_msg_type::paint_logic_type::OVERLAY_RECT, &rect );

			} else {

				SetOverlay( info, tool_msg_type::paint_logic_type::OVERLAY_NONE, 0 );

			}
	
			base_type::Repaint( info );
	
			ToolRenderPostProcess( info );

		} else {

			RECT rect = { m_CurrentPos.x, m_CurrentPos.y, m_CurrentPos.x + 1, m_CurrentPos.y + 1 };

			SetOverlay( info, tool_msg_type::paint_logic_type::OVERLAY_CROSS_HAIRS, &rect );

		}

	}

};

#endif // !defined(BPTBRUSHTOOLS_H__73582BC5_B520_471d_8D1F_B83F139AB97D_INCLUDED_)
