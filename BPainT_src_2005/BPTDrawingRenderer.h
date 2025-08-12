// BPTDrawingRenderer.h: interface for the BPTDrawingRenderer class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	This code is way more complex because of how the templated 
//	primitives are used.  It should be fairly fast, but at the
//	sacrifice of design.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTDRAWINGRENDERER_H__FF50D90F_8E06_486F_80FB_BBE4322B305B__INCLUDED_)
#define AFX_BPTDRAWINGRENDERER_H__FF50D90F_8E06_486F_80FB_BBE4322B305B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------------

#define BRUSH_CREATE_COMPRESSED_IMAGE

// ----------------------------------------------------------------------------

#include "BPTUtility.h"
#include "BPTPrimitives.h"

#if defined(BRUSH_CREATE_COMPRESSED_IMAGE)
#include "BPTSRL.h"
#endif


// ----------------------------------------------------------------------------

namespace BPT {

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TPixelClipperObject<>
	//
	//	-- This class is the base type for the various brush renderers.
	//

	template< class RT >
	class TPixelClipperObject {

	public: // traits

		typedef RT render_target_type;

	private: // data

	public: // interface

		TPixelClipperObject() { /* Empty */ }

		virtual ~TPixelClipperObject() { /* Empty */ }

		virtual void Prepare( render_target_type & renderTarget ) { /* Empty */ }

		virtual bool Predicate(
			const bool outterPredicateResult,
			const int x, const int y
		) {

			return outterPredicateResult;
		
		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// rectangle renderer base template
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TRectangleRenderer<>
	//
	//	-- This class is the base type for the various brush renderers.
	//

	template< class RT, class CL >
	class TRectangleRenderer : public TISupportProperties<>  {

	public: // traits

		typedef TISupportProperties<> base_class_type;
		typedef RT render_target_type;
		typedef CL clip_object_type;
		typedef typename render_target_type::surface_type::pixel_type pixel_type;
		typedef typename base_class_type::property_value_type property_value_type;

	private: // data

	public: // interface

		TRectangleRenderer() { /* Empty */ }

		virtual ~TRectangleRenderer() { /* Empty */ }

		virtual void Prepare( render_target_type & renderTarget ) { /* Empty */ }

		virtual void Render(
			render_target_type & renderTarget, const RECT & rect,
			clip_object_type * pClipper
		) { /* Empty */ }

		// --------------------------------------------------------------------

		virtual void SetColor( const pixel_type value ) { /* Empty */ }

	};

	// ========================================================================
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ========================================================================

	//
	//	TOpRectRenderer<>
	//

	template< class T, class TOP >
	class TOpRectRenderer : public T {

	public: // traits

		typedef typename T::render_target_type render_target_type;
		typedef typename T::clip_object_type clip_object_type;
		typedef typename T::pixel_type pixel_type;
		typedef typename T::property_value_type property_value_type;

	private: // data

		TOP m_Op;
		pixel_type m_Value;

	public: // interface

		// --------------------------------------------------------------------

		TOpRectRenderer( TOP op = TOP() ) :
			m_Value( pixel_type( 0 ) ), m_Op( op )
		{
			/* Empty */
		}

		// --------------------------------------------------------------------

		virtual void Prepare( render_target_type & renderTarget ) { /* Empty */ }

		// --------------------------------------------------------------------

		virtual void Render(
			render_target_type & renderTarget, const RECT & rect,
			clip_object_type * pClipper
		) {

			// ----------------------------------------------------------------

			typename render_target_type::surface_type::pointer pCanvas =
				renderTarget.GetCanvas();

			if ( !pCanvas ) {

				/* NOP */

				return;

			}

			// ----------------------------------------------------------------

			if ( pClipper ) {

#if 0
				pClipper->T_SolidRectPrim(
					*pCanvas, rect, m_Value, m_Op
				);

#endif

			} else {

				T_SolidRectPrim(
					*pCanvas, rect, m_Value, m_Op
				);

			}

			renderTarget.AddDirtyRect( rect );
		
		}

		// extended renderer interface 
		// --------------------------------------------------------------------

		virtual void SetColor( const pixel_type value ) {

			m_Value = value;

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Brush renderer base template
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TBrushRenderer<>
	//
	//	-- This class is the base type for the various brush renderers.
	//

	template< class RT, class CL >
		class TBrushRenderer : public TISupportProperties<> {

	public: // traits

		typedef TISupportProperties<> base_class_type;
		typedef RT render_target_type;
		typedef CL clip_object_type;
		typedef typename render_target_type::surface_type::pixel_type pixel_type;
		typedef typename base_class_type::property_value_type property_value_type;

	private: // data

	public: // interface

		// Standard renderer interface
		// --------------------------------------------------------------------

		TBrushRenderer() { /* Empty */ }

		virtual ~TBrushRenderer() { /* Empty */ }

		virtual void Prepare( render_target_type & renderTarget ) { /* Empty */ }

		virtual void Render(
			render_target_type & renderTarget, const int x, const int y,
			clip_object_type * pClipper
		) { /* Empty */ }

		// --------------------------------------------------------------------

		virtual bool GetDefaultPosition( POINT & out) {

			out.x = 0;
			out.y = 0;

			return true;
		
		}

		// --------------------------------------------------------------------

		virtual void Stamp( render_target_type & renderTarget, clip_object_type * pClipper ) {

			POINT defaultPos;

			GetDefaultPosition( defaultPos );

			Render( renderTarget, defaultPos.x, defaultPos.y, pClipper );

		}

		// extended renderer interface 
		// --------------------------------------------------------------------

		virtual void SetSize( const int w, const int h ) { /* Empty */ }
		virtual void SetColor( const pixel_type value ) { /* Empty */ }

	};

	// ========================================================================
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ========================================================================

	//
	//	TEllipseBrushRenderer
	//

	template< class T, class TOP >
	class TEllipseBrushRenderer : public T {

	public: // traits

		typedef typename T::render_target_type render_target_type;
		typedef typename T::clip_object_type clip_object_type;
		typedef typename T::pixel_type pixel_type;
		typedef typename T::property_value_type property_value_type;

	private: // data

		int m_A;
		int m_B;
		TOP m_Op;
		POINT m_LastPos;
		pixel_type m_Value;

	public: // interface

		// --------------------------------------------------------------------

		TEllipseBrushRenderer( TOP op = TOP() ) :
			m_A( 0 ), m_B( 0 ), m_Value( pixel_type( 0 ) ), m_Op( op )
		{
			m_LastPos.x = 0;
			m_LastPos.y = 0;
		}

		// --------------------------------------------------------------------

		virtual void Prepare( render_target_type & renderTarget ) { /* Empty */ }

		// --------------------------------------------------------------------

		virtual bool GetDefaultPosition( POINT & out) {

			out = m_LastPos;

			return true;
		
		}

		// --------------------------------------------------------------------

		virtual void Render(
			render_target_type & renderTarget, const int x, const int y,
			clip_object_type * pClipper
		) {

			POINT center = { x, y };

			m_LastPos = center;

			// ----------------------------------------------------------------

			typename render_target_type::surface_type::pointer pCanvas =
				renderTarget.GetCanvas();

			if ( !pCanvas ) {

				/* NOP */

				return;

			}

			// ----------------------------------------------------------------

			if ( pClipper ) {

#if 0

				pClipper->T_SolidEllipsePrim(
					*pCanvas, center, m_A, m_B, m_Value, m_Op
				);

#endif

			} else {

				T_SolidEllipsePrim(
					*pCanvas, center, m_A, m_B, m_Value, m_Op
				);

			}

			RECT dirtyRect = { x - m_A, y - m_B, x + m_A + 1, y + m_B + 1 };

			renderTarget.AddDirtyRect( dirtyRect );
		
		}

		// extended renderer interface 
		// --------------------------------------------------------------------

		virtual void SetSize( const int w, const int h ) {

			m_A = w;
			m_B = h;
		
		}

		virtual void SetColor( const pixel_type value ) {

			m_Value = value;

		}

	};

	// ========================================================================
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ========================================================================

	//
	//	TRectangleBrushRenderer
	//

	template< class T, class TOP >
	class TRectangleBrushRenderer : public T {

	public: // traits

		typedef typename T::render_target_type render_target_type;
		typedef typename T::clip_object_type clip_object_type;
		typedef typename T::pixel_type pixel_type;
		typedef typename T::property_value_type property_value_type;

	private: // data

		int m_A;
		int m_B;
		TOP m_Op;
		POINT m_LastPos;
		pixel_type m_Value;

	public: // interface

		// --------------------------------------------------------------------

		TRectangleBrushRenderer( TOP op = TOP() ) :
			m_A( 0 ), m_B( 0 ), m_Value( pixel_type( 0 ) ), m_Op( op )
		{
			m_LastPos.x = 0;
			m_LastPos.y = 0;
		}

		// --------------------------------------------------------------------

		virtual void Prepare( render_target_type & renderTarget ) { /* Empty */ }

		// --------------------------------------------------------------------

		virtual bool GetDefaultPosition( POINT & out) {

			out = m_LastPos;

			return true;
		
		}

		// --------------------------------------------------------------------

		virtual void Render(
			render_target_type & renderTarget, const int x, const int y,
			clip_object_type * pClipper
		) {

			m_LastPos.x = x;
			m_LastPos.y = y;

			// ----------------------------------------------------------------

			typename render_target_type::surface_type::pointer pCanvas =
				renderTarget.GetCanvas();

			if ( !pCanvas ) {

				/* NOP */

				return;

			}

			// ----------------------------------------------------------------

			RECT dirtyRect = { x - m_A, y - m_B, x + m_A + 1, y + m_B + 1 };

			if ( pClipper ) {

#if 0

				pClipper->T_SolidRectPrim(
					*pCanvas, dirtyRect, m_Value, m_Op
				);

#endif

			} else {

				T_SolidRectPrim(
					*pCanvas, dirtyRect, m_Value, m_Op
				);

			}

			renderTarget.AddDirtyRect( dirtyRect );
		
		}

		// extended renderer interface 
		// --------------------------------------------------------------------

		virtual void SetSize( const int w, const int h ) {

			m_A = w;
			m_B = h;
		
		}

		virtual void SetColor( const pixel_type value ) {

			m_Value = value;

		}

	};

	// ========================================================================
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ========================================================================

	//
	//	TBitmapBrushRenderer
	//

	template< class T, class TOP, class BM = typename T::render_target_type::surface_type >
	class TBitmapBrushRenderer : public T {

	public: // traits

		typedef BM brush_bitmap_type;
		typedef typename T::render_target_type render_target_type;
		typedef typename T::clip_object_type clip_object_type;
		typedef typename T::pixel_type pixel_type;
		typedef typename T::property_value_type property_value_type;

		enum BRUSH_HANDLE {

			CENTER = 0
			,TOP_LEFT = 1
			,BOTTOM_RIGHT = 2

		};

	private: // data

		int m_W;
		int m_H;
		TOP m_Op;

		POINT m_DefaultPos;
		BRUSH_HANDLE m_BrushHandle;
		brush_bitmap_type * m_pCurrentBitmap;

#if defined( BRUSH_CREATE_COMPRESSED_IMAGE )

		typedef TSRLCompressedImage< TSRLCompressor< typename BM::pixel_type > > compressed_image_type;

		compressed_image_type * m_pCompressedImage;

#endif

	private: // internal methods

		void CalcRenderRect( RECT & out, const int x, const int y ) {

			switch ( m_BrushHandle ) {

			default:
			case CENTER:
				{
					int hw = (m_W / 2);
					int hh = (m_H / 2);

					int xx = x - hw;
					int yy = y - hh;

					SetRect( &out, xx, yy, xx + m_W, yy + m_H );
				}
				break;

			case TOP_LEFT:
				SetRect( &out, x, y, x + m_W, y + m_H );
				break;

			case BOTTOM_RIGHT:
				SetRect( &out, x - m_W, y - m_H, x + 1, y + 1 );
				break;

			}


		}

	public: // interface

		// --------------------------------------------------------------------

		TBitmapBrushRenderer( TOP op = TOP() ) :
			m_Op( op ), m_BrushHandle( CENTER ),
			m_pCurrentBitmap( 0 )
		{
			m_DefaultPos.x = 0;
			m_DefaultPos.y = 0;

#if defined( BRUSH_CREATE_COMPRESSED_IMAGE )

			m_pCompressedImage = 0;
#endif
			
		}

		// --------------------------------------------------------------------

		~TBitmapBrushRenderer() {

#if defined( BRUSH_CREATE_COMPRESSED_IMAGE )

			if ( m_pCompressedImage ) {

				delete m_pCompressedImage;

			}

#endif

		}

		// --------------------------------------------------------------------

		virtual void Prepare( render_target_type & renderTarget ) { /* Empty */ }

		// --------------------------------------------------------------------

		virtual bool GetDefaultPosition( POINT & out) {

			out = m_DefaultPos;

			return true;
		
		}

		// --------------------------------------------------------------------

		virtual void Render(
			render_target_type & renderTarget, const int x, const int y,
			clip_object_type * pClipper
		) {

			// ----------------------------------------------------------------

			typename render_target_type::surface_type::pointer pCanvas =
				renderTarget.GetCanvas();

			if ( !pCanvas ) {

				/* NOP */

				return;

			}

			// ----------------------------------------------------------------

			if ( (0 != m_W) && (0 != m_H) && m_pCurrentBitmap ) {

				RECT dirtyRect;
	
				CalcRenderRect( dirtyRect, x, y );

				// deal with the grid
				// ------------------------------------------------------------

				CGridInfo * pGridInfo = renderTarget.GetGridInfo();

				if ( pGridInfo ) {

					POINT pt = { dirtyRect.left, dirtyRect.top };

					pGridInfo->Transform( &pt );

					OffsetRect( &dirtyRect, pt.x - dirtyRect.left, pt.y - dirtyRect.top );

				}

				// ------------------------------------------------------------

				int px = dirtyRect.left;
				int py = dirtyRect.top;

				if ( pClipper ) {

#if 0
					// This should use the clipper object to blit
		
					T_Blit(
						*pCanvas, px, py, *m_pCurrentBitmap, m_Op, 0, 0, 0
					);

#endif
		
				} else {

#if defined( BRUSH_CREATE_COMPRESSED_IMAGE )

					if ( m_pCompressedImage ) {

						m_pCompressedImage->Blit( *pCanvas, px, py, m_Op );
			
					} else {

						T_Blit(
							*pCanvas, px, py, 
							*m_pCurrentBitmap, m_Op, 0, 0, 0
						);
		
					}

#else

					T_Blit(
						*pCanvas, px, py, 
						*m_pCurrentBitmap, m_Op, 0, 0, 0
					);

#endif

				}
	
				renderTarget.AddDirtyRect( dirtyRect );

			}
		
		}

		// --------------------------------------------------------------------

		virtual void Stamp( render_target_type & renderTarget, clip_object_type * pClipper ) {

			BRUSH_HANDLE oldHandle = m_BrushHandle;

			m_BrushHandle = TOP_LEFT;

			POINT defaultPos;

			GetDefaultPosition( defaultPos );

			Render( renderTarget, defaultPos.x, defaultPos.y, pClipper );

			m_BrushHandle = oldHandle;

		}

		// extended renderer interface 
		// --------------------------------------------------------------------

		virtual void SetSize( const int w, const int h ) {

			if ( m_pCurrentBitmap ) {

				m_W = m_pCurrentBitmap->Width();
				m_H = m_pCurrentBitmap->Height();

			} else {

				m_W = w;
				m_H = h;

			}

		}

		// Transfer operator methods
		// --------------------------------------------------------------------

		void SetTransferOp( TOP op ) {

			m_Op = op;

		}

		// Brush data interface
		// --------------------------------------------------------------------

		typename brush_bitmap_type::pointer GetBitmapPtr() {

			return m_pCurrentBitmap;

		}

		void SetBitmapInfo(
			typename brush_bitmap_type::pointer pBmp, const POINT * pPos,
			const typename brush_bitmap_type::pixel_type chromakey ) {

			if ( pPos ) {

				m_DefaultPos = *pPos;

			} else {

				m_DefaultPos.x = 0;
				m_DefaultPos.y = 0;

			}

			m_pCurrentBitmap = pBmp;

			if ( m_pCurrentBitmap ) {

				m_W = m_pCurrentBitmap->Width();
				m_H = m_pCurrentBitmap->Height();

			} else {

				m_W = 0;
				m_H = 0;

			}

#if defined( BRUSH_CREATE_COMPRESSED_IMAGE )

			if ( m_pCompressedImage ) {

				delete m_pCompressedImage;

			}

			m_pCompressedImage = new compressed_image_type();

			if ( m_pCompressedImage ) {

				TIsNotValue<typename brush_bitmap_type::pixel_type> predicate( chromakey );

				if ( !m_pCompressedImage->Create( *m_pCurrentBitmap, predicate ) ) {

					delete m_pCompressedImage;

					m_pCompressedImage = 0;

				}

// ----------------------------------------------------------------------------
#if 0 // debug stats

				if ( m_pCompressedImage ) {

					CString str;

					str.Format( "Raw %d compressed %d.\n",
						m_W * m_H * sizeof( typename brush_bitmap_type::pixel_type ),
						m_pCompressedImage->CompressedSizeEstimate()
					);

					MessageBox( AfxGetMainWnd()->GetSafeHwnd(), str, "YO!", MB_OK );

				}

#endif
// ----------------------------------------------------------------------------


			}

#endif

		}

		// brush 'property' interface
		// --------------------------------------------------------------------

		// scale (x & y), angle, flip etc...

	};

	// ========================================================================
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ========================================================================

};

// ----------------------------------------------------------------------------

#endif // !defined(AFX_BPTDRAWINGRENDERER_H__FF50D90F_8E06_486F_80FB_BBE4322B305B__INCLUDED_)
