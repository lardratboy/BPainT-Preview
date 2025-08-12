// BPTDrawingPen.h: interface for the BPTDrawingPen class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	The way pen's and rop's interact is really really complicated
//	this needs to be simplified at some point, but that will take
//	a bunch of time and I don't have room for that in my schedule
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTDRAWINGPEN_H__36963C0E_2405_45FE_8CF4_6E2F581A92B2__INCLUDED_)
#define AFX_BPTDRAWINGPEN_H__36963C0E_2405_45FE_8CF4_6E2F581A92B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BPTDrawingRenderer.h"

namespace BPT {

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// pen abstract
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class RT >
	class TDrawingPen {

	public: // Traits

		typedef RT render_target_type;
		typedef TDrawingPen<RT> this_type;
		typedef this_type & reference;
		typedef this_type * pointer;
		typedef const this_type & const_reference;
		typedef const this_type * const_pointer;
		typedef typename RT::surface_type::pixel_type pixel_type;

	public: // Interface

		TDrawingPen() { /* Empty */ }

		virtual ~TDrawingPen() { }

		// The actual low level 'pen' methods -- these are the work horses!
		// --------------------------------------------------------------------

		virtual void Brush( RT & renderTarget, const int x, const int y ) {}
		virtual void Rectangle( RT & renderTarget, const RECT & rect ) {}
		virtual void Stamp( RT & renderTarget ) {}

		// Prepare for renderer
		// --------------------------------------------------------------------

		virtual void PrepareForRender( RT & renderTarget ) {}

		// Predicate override.
		// --------------------------------------------------------------------

		virtual bool Predicate(
			const bool outterPredicateResult,
			const int x, const int y
		) {

			return outterPredicateResult;
		
		}

		virtual pixel_type OverridePixelValue( const pixel_type pixel, const int x, const int y ) {

			return pixel;

		}

		// Helper methods (can be overriden but why bother)
		// --------------------------------------------------------------------

		virtual void Brush( RT & renderTarget, const POINT & at ) {

			Brush( renderTarget, at.x, at.y );
		
		}

		virtual void Pixel( RT & renderTarget, const POINT & at ) {

			RECT rect = { at.x, at.y, at.x + 1, at.y + 1 };

			Rectangle( renderTarget, rect );
		
		}

		virtual void operator()( RT & renderTarget, const int x, const int y ) {

			Brush( renderTarget, x, y );

		}

		virtual void operator()( RT & renderTarget, const POINT & at ) {

			Brush( renderTarget, at );

		}

		virtual void operator()( RT & renderTarget, const RECT & rect ) {

			Rectangle( renderTarget, rect );

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// flexible pen template
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template<
		class BASE_TYPE
		,class BRUSH_RENDERER
		,class RECT_RENDERER
		,class CLIP_OBJECT
		,class PIXEL_OVERRIDER
	>
	class TFlexiblePen : public BASE_TYPE {

		// should I declare the renderer types friends so they can
		// call no public members?

	public: // Traits

		typedef TFlexiblePen<
			BASE_TYPE,BRUSH_RENDERER,RECT_RENDERER,CLIP_OBJECT,PIXEL_OVERRIDER
		> this_type;

		typedef BASE_TYPE base_type;
		typedef typename base_type::render_target_type RT;
		typedef BRUSH_RENDERER brush_renderer_type;
		typedef RECT_RENDERER rect_renderer_type;
		typedef CLIP_OBJECT clip_object_type;
		typedef PIXEL_OVERRIDER pixel_override_object_type;
		typedef typename base_type::pixel_type pixel_type;

	private:

		BRUSH_RENDERER * m_pBrushRenderer;
		RECT_RENDERER * m_pRectRenderer;
		CLIP_OBJECT * m_pClipObject;
		PIXEL_OVERRIDER * m_pPixelOverrider;

	public:

		// construction/destruction
		// --------------------------------------------------------------------

		TFlexiblePen(
			BRUSH_RENDERER * pBrushRenderer = 0
			,RECT_RENDERER * pRectRenderer = 0
			,CLIP_OBJECT * pClipObject = 0
			,PIXEL_OVERRIDER * pPixelOverrider = 0
		) : m_pBrushRenderer( pBrushRenderer )
			, m_pRectRenderer( pRectRenderer )
			, m_pClipObject( pClipObject )
			, m_pPixelOverrider( pPixelOverrider )
		{
			/* Empty */
		}

		// --------------------------------------------------------------------

		virtual void PrepareForRender( RT & renderTarget ) {
		
			if ( m_pClipObject ) {

				m_pClipObject->Prepare( renderTarget );

			}

			if ( m_pBrushRenderer ) {

				m_pBrushRenderer->Prepare( renderTarget );

			}

			if ( m_pRectRenderer ) {

				m_pRectRenderer->Prepare( renderTarget );

			}

			if ( m_pPixelOverrider ) {

				m_pPixelOverrider->Prepare( renderTarget );

			}

		}

		// --------------------------------------------------------------------

		virtual bool Predicate(
			const bool outterPredicateResult,
			const int x, const int y
		) {

			if ( m_pClipObject ) {

				return m_pClipObject->Predicate(
					typename base_type::Predicate( outterPredicateResult, x, y ), x, y
				);

			}

			return typename base_type::Predicate( outterPredicateResult, x, y );
		
		}

		virtual pixel_type OverridePixelValue( const pixel_type pixel, const int x, const int y ) {

			if ( m_pPixelOverrider ) {

				return m_pPixelOverrider->OverridePixelValue( 
					typename base_type::OverridePixelValue( pixel, x, y ), x, y 
				);

			}

			return typename base_type::OverridePixelValue( pixel, x, y );

		}

		// --------------------------------------------------------------------

		virtual void Brush( RT & renderTarget, const int x, const int y ) {
		
			if ( m_pBrushRenderer ) {

				m_pBrushRenderer->Render(
					renderTarget, x, y, m_pClipObject
				);

			}

		}

		virtual void Stamp( RT & renderTarget ) {

			if ( m_pBrushRenderer ) {

				m_pBrushRenderer->Stamp( renderTarget, m_pClipObject );

			}
		
		}

		virtual void Rectangle( RT & renderTarget, const RECT & rect ) {

			if ( m_pRectRenderer ) {

				m_pRectRenderer->Render(
					renderTarget, rect, m_pClipObject
				);

			}
		
		}

		// Query methods (should these be protected?)
		// --------------------------------------------------------------------

		brush_renderer_type * GetBrushRenderer() {
			
			return m_pBrushRenderer;

		}

		rect_renderer_type * GetRectRenderer() {
			
			return m_pRectRenderer;

		}

		clip_object_type * GetClipObject() {
			
			return m_pClipObject;

		}

		pixel_override_object_type * GetPixelOverrider() {

			return m_pPixelOverrider;

		}

		// Set methods (should these be protected?)
		// --------------------------------------------------------------------

		brush_renderer_type * SetBrushRenderer( brush_renderer_type * pNew ) {

			brush_renderer_type * pOld = m_pBrushRenderer;

			m_pBrushRenderer = pNew;
			
			return pOld;

		}

		rect_renderer_type * SetRectRenderer( rect_renderer_type * pNew ) {
			
			rect_renderer_type * pOld = m_pRectRenderer;

			m_pRectRenderer = pNew;
			
			return pOld;

		}

		clip_object_type * SetClipObject( clip_object_type * pNew ) {
			
			clip_object_type * pOld = m_pClipObject;

			m_pClipObject = pNew;
			
			return pOld;

		}

		pixel_override_object_type * SetPixelOverrider( pixel_override_object_type * pNew ) {
			
			pixel_override_object_type * pOld = m_pPixelOverrider;

			m_pPixelOverrider = pNew;
			
			return pOld;

		}

	};

}; // namespace BPT

#endif // !defined(AFX_BPTDRAWINGPEN_H__36963C0E_2405_45FE_8CF4_6E2F581A92B2__INCLUDED_)
