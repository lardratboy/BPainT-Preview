#if !defined(__TCOMPOSITE_OP_BASE_H__)
#define __TCOMPOSITE_OP_BASE_H__

// TCompositeOpBase.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------------

#include "bpaint.h"
#include "BpaintDoc.h"
#include "CompositeFrameOpsDlg.h"

// ----------------------------------------------------------------------------

//
//	TSimpleCompositeLayersOp
//
//	-- This template class is the framework for simple operations that are
//	-- only different because of the ROP used or the post operation applied.
//

template< class ROP >
class TSimpleCompositeLayersOp : public CCompositeFrameOpsDlg::COperation {

public: // traits

	typedef CCompositeFrameOpsDlg::COperation base_type;

	typedef base_type::layer_collection_type layer_collection_type;

	typedef base_type::SFrameRangeInfo SFrameRangeInfo;

	typedef ROP ROP;

private:

	ROP m_Rop;

public:

	// --------------------------------------------------------------------

	virtual const char * GetName() const {

		return "Combine layers";

	}

	// --------------------------------------------------------------------

	virtual void ChangeROP( ROP rop ) {

		m_Rop = rop;

	}

	// --------------------------------------------------------------------

	virtual bool PostFrameOp( CBpaintDoc::editor_bitmap_type & surface ) {

		/* NOP */

		return true;

	}

	// --------------------------------------------------------------------

	virtual bool Perform(
		class CBpaintDoc * pDocument, 
		BPT::CAnimation * pDstAnimation, 
		SFrameRangeInfo & dstRangeInfo, 
		layer_collection_type & dstLayersCollection,
		BPT::CAnimation * pSrcAnimation, 
		SFrameRangeInfo & srcRangeInfo, 
		layer_collection_type & srcLayersCollection,
		const int value
		) {

		// --------------------------------------------------------------------

		BPT::CAnimationShowcase * pShowcase = pDocument->GetShowcaseObject();

		if ( !pShowcase ) {

			TRACE( "Unable to get animation showcase from the document?\n" );

			return false;

		}

		// --------------------------------------------------------------------

		CBpaintDoc::editor_bitmap_type opCanvasSurface;

		CSize canvasSize = pDocument->GetCanvasSize();

		if ( !opCanvasSurface.Create( canvasSize.cx, canvasSize.cy ) ) {

			TRACE( "Unable to create operation canvas surface %dx%d\n", canvasSize.cx, canvasSize.cy );

			return false;

		}

		// --------------------------------------------------------------------

		CBpaintDoc::editor_pixel_type chromakey = pDocument->GetClearColor();

		// Create the layer selection functor.
		// ------------------------------------------------------------------------
	
		BPT::TIsInCollection<
			int,layer_collection_type
		> fnLayerSelection( srcLayersCollection );

		// --------------------------------------------------------------------

		int nSrcFrame = srcRangeInfo.m_nFrom - 1;

		for ( int dstFrameCounter = dstRangeInfo.m_nFrom; 
			dstFrameCounter <= dstRangeInfo.m_nTo; dstFrameCounter++, nSrcFrame++ ) {

			int nDstFrame = dstFrameCounter - 1;

			// Build the 'composite'
			// ----------------------------------------------------------------

			opCanvasSurface.ClearBuffer( chromakey );

#if 1 
			
			// Explicitly call the render method for each layer ignoring palette's
			// I'm not sure why the composite op's code doesn't work with the 
			// adaptor ROP to ignore palettes, but it just doesn't so it had to be
			// overcome! -BPT

			BPT::CFrame * pFrame = pSrcAnimation->GetFramePtr( nSrcFrame );

			if ( pFrame ) {

				int layerCount = pFrame->LayerCount();

				for ( int layer = 0; layer < layerCount; layer++ ) {

					BPT::CLayer * pLayer = pFrame->GetLayerPtr( layer );

					if ( pLayer && fnLayerSelection( layer ) ) {

						pLayer->Render(
							opCanvasSurface, 0, 0, m_Rop
						);

					}

				}

			}

#else
	
			pSrcAnimation->RenderFrame(
				opCanvasSurface, nSrcFrame, 0, 0,
				m_Rop, fnLayerSelection, 0, 0
			);

#endif

			// do any post frame operation to it
			// ----------------------------------------------------------------

			if ( !PostFrameOp( opCanvasSurface ) ) {

				TRACE( "Unable to perform post frame op on frame %d\n", dstFrameCounter );

				return false;

			}

			// Build a new visual element from the operation canvas
			// ----------------------------------------------------------------

			BPT::CVisualElement * pNewVisualElement = 0;

			// find the bounding rectangle to determine if the component was 
			// cleared by the operation...
			// ------------------------------------------------------------------
		
			CRect captureRect;
		
			BPT::T_FindBoundingRect(
				captureRect, opCanvasSurface, BPT::TIsNotValue<
					CBpaintDoc::editor_pixel_type
				>( chromakey ), 0
			);
		
			if ( !captureRect.IsRectEmpty() ) {
		
				// Ask the showcase to create us a new visual element
				// --------------------------------------------------------------------
		
				pNewVisualElement = pShowcase->CreateVisualElementFromSurface(
					opCanvasSurface, chromakey, &captureRect
				);
			
				if ( !pNewVisualElement ) {

					TRACE( "Unable to create visual element for frame %d\n", dstFrameCounter );

					return false;

				}

			}

			// Get the dest frame pointer
			// ----------------------------------------------------------------

			BPT::CFrame * pDstFrame = pDstAnimation->GetFramePtr( nDstFrame );

			if ( !pDstFrame ) {

				TRACE( "Unable to get dst animation frame %d\n", dstFrameCounter );

				return false;

			}

			// Now set all the destination layers to the results
			// ----------------------------------------------------------------

			int dstLayerCount = pDstFrame->LayerCount();

			for ( int dstLayerCounter = 0; dstLayerCounter < dstLayerCount; dstLayerCounter++ ) {

				layer_collection_type::iterator it = std::find(
					dstLayersCollection.begin(), dstLayersCollection.end(), dstLayerCounter
				);

				if ( dstLayersCollection.end() != it ) {

					BPT::CLayer * pLayer = pDstFrame->GetLayerPtr( dstLayerCounter );

					if ( !pLayer ) {

						TRACE(" Unable to get frame %d layer %d object\n", dstFrameCounter, dstLayerCounter );

						return false;

					}

					pLayer->SetVisualElement( pNewVisualElement, &CPoint( 0, 0 ) );

				}

			}

		}

		return true;

	}

};

// ----------------------------------------------------------------------------

#endif // !defined(__TCOMPOSITE_OP_BASE_H__)

