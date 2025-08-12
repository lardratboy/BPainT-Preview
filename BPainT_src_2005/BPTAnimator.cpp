// BPTAnimator.cpp: implementation of the BPTAnimator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bpaint.h"
#include "BPTAnimator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace BPT {

	// ------------------------------------------------------------------------

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

	// ------------------------------------------------------------------------
	// OPTIMIZE METHODS
	// ------------------------------------------------------------------------


	// ------------------------------------------------------------------------
	// ADJUST  METHODS
	// ------------------------------------------------------------------------

	//
	//	CAnimation::AdjustVisualReferences()
	//

	bool 
	CLayer::AdjustVisualReferences(
		CVisualElement * pOldElement
    	,CVisualElement * pNewElement
		,const POINT * pDelta
		,const POINT * pMoveTo
	)
	{

		// Check the existance of a visual for this layer
		// --------------------------------------------------------------------

		CVisualElement * pCurrent = GetVisualElement();

		if ( !pCurrent ) {

			return false;

		}

		pCurrent->Release();

		if ( pOldElement != pCurrent ) {

			return false;

		}

#ifdef _DEBUG

POINT oldPos = GetPos();

#endif

		// Okay we've got one to work on do the operation.
		// --------------------------------------------------------------------

		if ( pOldElement != pNewElement ) {
	
			SetVisualElement( pNewElement );
	
		}

		// Handle any MoveTo request
		// --------------------------------------------------------------------

		if ( pMoveTo ) {

			MoveTo( *pMoveTo );

		}

		// Handle any delta request
		// --------------------------------------------------------------------

		if ( pDelta ) {

			Shift( pDelta->x, pDelta->y );

		}

#ifdef _DEBUG

POINT newPos = GetPos();

TRACE( "VE %p old %d, %d new %d, %d\n", this, oldPos.x, oldPos.y, newPos.x, newPos.y );

#endif


		return true;
	}

	//
	//	CAnimation::AdjustVisualReferences()
	//

	bool 
	CFrame::AdjustVisualReferences(
		CVisualElement * pOldElement
    	,CVisualElement * pNewElement
		,const POINT * pDelta
		,const POINT * pMoveTo
	)
	{
		bool bDidSomething = false;

		layer_collection_type::iterator it = m_Layers.begin();

		for ( ; it != m_Layers.end(); it++ ) {

			if ( (*it)->AdjustVisualReferences( pOldElement, pNewElement, pDelta, pMoveTo ) ) {

				bDidSomething = true;

			}

		}

		return bDidSomething;
	}

	//
	//	CAnimation::AdjustVisualReferences()
	//

	bool 
	CAnimation::AdjustVisualReferences(
		CVisualElement * pOldElement
    	,CVisualElement * pNewElement
		,const POINT * pDelta
		,const POINT * pMoveTo
	)
	{
		bool bDidSomething = false;

		frame_collection_type::iterator it = m_Frames.begin();

		for ( ; it != m_Frames.end(); it++ ) {

			if ( (*it)->AdjustVisualReferences( pOldElement, pNewElement, pDelta, pMoveTo ) ) {

				bDidSomething = true;

			}

		}

		return bDidSomething;
	}

#endif // BPT 5/22/01

	// A visual element reference methods
	// ------------------------------------------------------------------------

	CVisualElementReference::~CVisualElementReference()
	{
		if ( m_pVisualElement ) {

			m_pVisualElement->Release();

		}

	}

	bool CVisualElementReference::CheckStoredPixelSize( const int value ) {

		if ( !m_pVisualElement ) return false;

		return m_pVisualElement->CheckStoredPixelSize( value );

	}

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01 

	RECT 
	CVisualElementReference::BoundingRect(
		const BLITFX * pOptionalBlitFX,
		const SIZE * pAlignmentCanvas
	)
	{

		if ( m_pVisualElement ) {

			return m_pVisualElement->BoundingRect(
				&m_At, pOptionalBlitFX, pAlignmentCanvas
			);

		}

		RECT emptyRect;

		SetRectEmpty( &emptyRect );

		return emptyRect;

	}

#endif // BPT 5/22/01

	CVisualElement * CVisualElementReference::GetVisualElement() const {

		if ( m_pVisualElement ) {

			m_pVisualElement->AddRef();

		}

		return m_pVisualElement;

	}

	void CVisualElementReference::SetVisualElement( CVisualElement * pVisualElement ) {

		if ( m_pVisualElement ) {

			m_pVisualElement->Release();

		}

		m_pVisualElement = pVisualElement;

		if ( m_pVisualElement ) {

			m_pVisualElement->AddRef();

		}

	}

	// ------------------------------------------------------------------------

}; // namespace BPT

