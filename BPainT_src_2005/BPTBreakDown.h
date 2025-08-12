// BPTBreakDown.h
//
//	Copyright (c) 2002, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//		The repeated calls to Iterator could be optimized with fixed 
//		offsets... If done with a single offset instead of an xy it 
//		would be reduce the stack demands by 4 bytes per recurse.
//		and most likely it would be faster.  The code would however
//		be limited to surfaces with simple memory policies. 
//
//////////////////////////////////////////////////////////////////////

#if !defined(BPTBREAKDOWN_H__8F1B0EE4_1839_4680_B099_780B8FFCA779_INCLUDED_)
#define BPTBREAKDOWN_H__8F1B0EE4_1839_4680_B099_780B8FFCA779_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

namespace BPT {

	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------

	//
	//	TBreakDownSurface
	//
	//	-- THIS CODE SHOULD USE A PREDICATE OBJECT INSTEAD OF A CHROMAKEY!!! 
	//

	template< class SURFACE > class TBreakDownSurface {

	private:

		SURFACE * m_pSearchSurface;

		SURFACE m_WorkingSurface;

		SURFACE m_CaptureSurface;

		RECT m_BoundingRect;

		RECT m_SearchLimit;

		typename typename SURFACE::pixel_type m_Chromakey;

		typedef std::vector< CPoint > point_stack_collection_type;

		point_stack_collection_type m_PointStack;

#ifdef _DEBUG
		point_stack_collection_type::size_type m_StackHighMark;
#endif

	private:

		inline bool ShouldCapture( const int x, const int y ) {

			// make sure that the new point is within the bitmap search area

			if (
				(x < m_SearchLimit.left) || (y < m_SearchLimit.top) ||
				(x >= m_SearchLimit.right) || (y >= m_SearchLimit.bottom) ) {

				return false;

			}

			// check to see if we've already captured the area or it's empty

			if ( m_Chromakey != *m_CaptureSurface.Iterator( x, y ) ) return false;

			if ( m_Chromakey == *(m_pSearchSurface->Iterator( x, y )) ) return false;

			return true;

		}

		//
		//	CaptureAdjacent()
		//
		//	-- this routine might be more of a memory hog than a recursive version?
		//

		inline void
		CaptureAdjacent(
			const int sx
			,const int sy
			,const int sw
			,const int sh
			,const bool bSearchCorners
		) {

			m_PointStack.push_back( CPoint( sx, sy ) );

			// capture this pixel

			while ( m_PointStack.size() ) {

				// fetch the location to visit from the 'stack'

				CPoint p = m_PointStack.back();

				m_PointStack.pop_back();

				// capture this location

				*m_CaptureSurface.Iterator( p.x, p.y ) = *(m_pSearchSurface->Iterator( p.x, p.y ));

				// increase the bounding rectangle to include this new point

				if ( p.x < m_BoundingRect.left ) m_BoundingRect.left = p.x;
				if ( p.x >= m_BoundingRect.right ) m_BoundingRect.right = (p.x + 1);
				if ( p.y < m_BoundingRect.top ) m_BoundingRect.top = p.y;
				if ( p.y >= m_BoundingRect.bottom ) m_BoundingRect.bottom = (p.y + 1);

#if 1

				// visit any valid pixels in the search rect around this pixel

				for ( int dy = -sh; dy <= sh; dy++ ) {

					for ( int dx = -sw; dx <= sw; dx++ ) {

						if ( (0 == dx) && (0 == dy) ) continue;

						if ( (!bSearchCorners) && (dx == dy) ) continue;

						if ( ShouldCapture( p.x + dx, p.y + dy ) ) {

							m_PointStack.push_back( CPoint( p.x + dx, p.y + dy ));

						}

					}

				}

#else

				// search the U,L,R,D adjacent pixels

				if ( ShouldCapture( p.x, p.y - 1 ) )
					m_PointStack.push_back( CPoint( p.x, p.y - 1 ));

				if ( ShouldCapture( p.x - 1, p.y ) )
					m_PointStack.push_back( CPoint( p.x - 1, p.y ));

				if ( ShouldCapture( p.x + 1, p.y ) )
					m_PointStack.push_back( CPoint( p.x + 1, p.y ));

				if ( ShouldCapture( p.x, p.y + 1 ) )
					m_PointStack.push_back( CPoint( p.x, p.y + 1 ));

				if ( bSearchCorners ) {

					// search the TL,TR,BL,BR adjacent pixels

					if ( ShouldCapture( p.x - 1, p.y - 1 ) )
						m_PointStack.push_back( CPoint( p.x - 1, p.y - 1 ));

					if ( ShouldCapture( p.x + 1, p.y - 1 ) )
						m_PointStack.push_back( CPoint( p.x + 1, p.y - 1 ));

					if ( ShouldCapture( p.x - 1, p.y + 1 ) )
						m_PointStack.push_back( CPoint( p.x - 1, p.y + 1 ));

					if ( ShouldCapture( p.x + 1, p.y + 1 ) )
						m_PointStack.push_back( CPoint( p.x + 1, p.y + 1 ));

				}

#endif

				// collect debugging stats
				// ------------------------------------------------------------

#ifdef _DEBUG
				m_StackHighMark = max( m_StackHighMark, m_PointStack.size() );
#endif

			}

		}

#ifdef _DEBUG

		void DEBUG_Prep() {

			m_StackHighMark = 0;

		}

#endif // _DEBUG

	public:

		// --------------------------------------------------------------------

		TBreakDownSurface() : m_Chromakey(0), m_pSearchSurface(0) {

			SetRectEmpty( &m_BoundingRect );
		
#ifdef _DEBUG
			m_StackHighMark = 0;
#endif

		}

		~TBreakDownSurface() {

			Release();

		}

		void Release() {

			m_WorkingSurface.Destroy();

			m_CaptureSurface.Destroy();

			m_pSearchSurface = 0;

			m_PointStack.clear();

		}

		// --------------------------------------------------------------------

		//
		//	BreakDown()
		//

		template<
			class CAPTURE_FUNCTOR
		>
		bool 
		BreakDown(
			CAPTURE_FUNCTOR & fnCapturer
			,SURFACE & surface
			,const typename SURFACE::pixel_type chromakey
			,const bool bSearchCorners
			,const bool bGroupSinglePixels
			,const RECT * pOptionalSubRect
			,const int adjacentSearchWidth
			,const int adjacentSearchHeight
		) {

#ifdef _DEBUG
			DEBUG_Prep();
#endif

			// make sure search rectangle is valid for this surface
			// ----------------------------------------------------------------

			m_SearchLimit = surface.Rect();

			if ( pOptionalSubRect ) {

				if ( !IntersectRect( &m_SearchLimit, &m_SearchLimit, pOptionalSubRect ) ) {

#ifdef _DEBUG
					TRACE( "Invalid sub rect passed (%d,%d,%d,%d)\n",
						pOptionalSubRect->left, pOptionalSubRect->top,
						pOptionalSubRect->right, pOptionalSubRect->bottom
					);
#endif

					return false;

				}

			}

			// create the working surfaces
			// ----------------------------------------------------------------

			m_Chromakey = chromakey;

			if ( !m_WorkingSurface.Clone( surface ) ) {

#ifdef _DEBUG
				TRACE( "Unable to clone surface (%dx%d)\n", surface.Width(), surface.Height() );
#endif
				
				return false;

			}

			if ( !m_CaptureSurface.Create( surface.Width(), surface.Height() ) ) {
				
#ifdef _DEBUG
				TRACE( "Unable to create capture surface (%dx%d)\n", surface.Width(), surface.Height() );
#endif

				return false;

			}

			m_PointStack.reserve(
				(m_SearchLimit.right - m_SearchLimit.left) * (m_SearchLimit.bottom - m_SearchLimit.top)
			);

			// If requested create the grouped single pixel image
			// ----------------------------------------------------------------

			SURFACE groupedSinglePixelsSurface;

			if ( bGroupSinglePixels ) {

				if ( !groupedSinglePixelsSurface.Create( surface.Width(), surface.Height() ) ) {

#ifdef _DEBUG
					TRACE( "Unable to create single pixels surface (%dx%d)\n", surface.Width(), surface.Height() );
#endif

					return false;

				}

			}

			// Okay process the working surface
			// ----------------------------------------------------------------

			TCopyROP<typename SURFACE::pixel_type> copyOp;

			TSingleColorTransparentSrcTransferROP<typename SURFACE::pixel_type> eraseOp(
				chromakey, chromakey
			);

			m_BoundingRect = m_SearchLimit;

			m_pSearchSurface = &m_WorkingSurface;

			// clear the single pixel surface to be all chromakey
			// how does this work with the predicate...
			// ----------------------------------------------------------------

			bool bGroupedPixelsSurfaceHasPixels = false;

			RECT m_GroupedSinglePixelsRect;

			if ( bGroupSinglePixels ) {

				T_SolidRectPrim(
					groupedSinglePixelsSurface, m_SearchLimit, chromakey, copyOp
				);

				SetRectEmpty( &m_GroupedSinglePixelsRect );

			}

			// Okay process the working surface
			// ----------------------------------------------------------------

			for ( int y = m_SearchLimit.top; y < m_SearchLimit.bottom; y++ ) {

				typename SURFACE::pixel_iterator it = m_WorkingSurface.Iterator( m_SearchLimit.left, y );

				for ( int x = m_SearchLimit.left; x < m_SearchLimit.right; x++ ) {

					typename SURFACE::pixel_type value = *it++;

					if ( m_Chromakey == value ) continue; // future precicate...

					// prepare for capture by erasing last capture

					if ( !IsRectEmpty( &m_BoundingRect ) ) {

						T_SolidRectPrim(
							m_CaptureSurface, m_BoundingRect, chromakey, copyOp
						);

					}

					SetRect( &m_BoundingRect, x, y, x + 1, y + 1 );

					CaptureAdjacent(
						x, y, adjacentSearchWidth, adjacentSearchHeight, bSearchCorners
					);

					SIZE found;

					found.cx = m_BoundingRect.right - m_BoundingRect.left;
					found.cy = m_BoundingRect.bottom - m_BoundingRect.top;

					// determine if this is a single pixel and group it if requested
					// --FUTURE--
					// this test could/should be a predicate object...
					// the predicate object should be passed the capture surface
					// and the bounding rect, that way pixel density etc could
					// be measured to determine if it should capture it or group it.

					if ( bGroupSinglePixels && ((1 == found.cx) && (1 == found.cy)) ) {

						*groupedSinglePixelsSurface.Iterator( x, y ) = value;

						bGroupedPixelsSurfaceHasPixels = true;

						if ( !IsRectEmpty(&m_GroupedSinglePixelsRect) ) {

							UnionRect(
								&m_GroupedSinglePixelsRect
								,&m_GroupedSinglePixelsRect
								,&m_BoundingRect
							);
							
						} else {

							m_GroupedSinglePixelsRect = m_BoundingRect;

						}

					} else {

						// call the capture for the area

						if ( !fnCapturer( m_CaptureSurface, m_BoundingRect, chromakey ) ) {

#ifdef _DEBUG
							TRACE( "(1) fnCapturer failed to capture (%d,%d,%d,%d)\n",
								m_BoundingRect.left, m_BoundingRect.top,
								m_BoundingRect.right, m_BoundingRect.bottom
							);
#endif

							Release();

							return false;

						}

					}

					// erase captured image from the working surface

					T_Blit(
						m_WorkingSurface
						,0
						,0
						,m_CaptureSurface
						,eraseOp
						,0
						,&m_BoundingRect
					);

				}

			}

			// check to see if we need to capture the grouped single pixels
			// ----------------------------------------------------------------

			if ( bGroupedPixelsSurfaceHasPixels && bGroupSinglePixels ) {

				if ( !fnCapturer( groupedSinglePixelsSurface, m_GroupedSinglePixelsRect, chromakey ) ) {

#ifdef _DEBUG
					TRACE( "(2) fnCapturer failed to capture (%d,%d,%d,%d)\n",
						m_BoundingRect.left, m_BoundingRect.top,
						m_BoundingRect.right, m_BoundingRect.bottom
					);
#endif

					Release();

					return false;

				}

			}

			// final cleanup
			// ----------------------------------------------------------------

			Release();

			return true;

		}

		// --------------------------------------------------------------------

		//
		//	CaptureImageAtXY()
		//

		template<
			class CAPTURE_FUNCTOR
		>
		bool 
		CaptureImageAtXY(
			CAPTURE_FUNCTOR & fnCapturer
			,SURFACE & surface
			,const int x
			,const int y
			,const typename SURFACE::pixel_type chromakey
			,const bool bSearchCorners
			,const RECT * pOptionalSubRect
			,const int adjacentSearchWidth
			,const int adjacentSearchHeight
		) {

#ifdef _DEBUG
			DEBUG_Prep();
#endif

			// make sure search rectangle & location is valid for this surface
			// ----------------------------------------------------------------

			m_SearchLimit = surface.Rect();

			if ( pOptionalSubRect ) {

				if ( !IntersectRect( &m_SearchLimit, &m_SearchLimit, pOptionalSubRect ) ) {

					return false;

				}

			}

			if (
				(x < m_SearchLimit.left) || (y < m_SearchLimit.top) ||
				(x >= m_SearchLimit.right) || (y >= m_SearchLimit.bottom) ) {

				return false;

			}

			if ( chromakey == *surface.Iterator( x, y ) ) {

				return false;

			}

			// create the working surface
			// ----------------------------------------------------------------

			m_Chromakey = chromakey;

			if ( !m_CaptureSurface.Create( surface.Width(), surface.Height() ) ) return false;

			m_pSearchSurface = &surface;

			m_PointStack.reserve(
				(m_SearchLimit.right - m_SearchLimit.left) * (m_SearchLimit.bottom - m_SearchLimit.top)
			);

			// Okay process the working surface
			// ----------------------------------------------------------------

			TCopyROP<typename SURFACE::pixel_type> copyOp;

			T_SolidRectPrim(
				m_CaptureSurface, m_SearchLimit, chromakey, copyOp
			);

			SetRect( &m_BoundingRect, x, y, x + 1, y + 1 );

			CaptureAdjacent( x, y, adjacentSearchWidth, adjacentSearchHeight, bSearchCorners );

			// call the capture for the area

			bool bCaptured = fnCapturer( m_CaptureSurface, m_BoundingRect, chromakey );

			Release();

			return bCaptured;

		}

	}; // class TBreakDownSurface

}; // namespace BPT

#endif /* !defined(BPTBREAKDOWN_H__8F1B0EE4_1839_4680_B099_780B8FFCA779_INCLUDED_) */
