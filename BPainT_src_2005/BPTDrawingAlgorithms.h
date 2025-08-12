// BPTDrawingAlgorithms.h: interface for the BPTDrawingAlgorithms class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	This file contains the 'algorithms' for the drawing tools, these
//	functions don't perform the rasterization that is left to the
//	pen & render target types.  These 'algorithms' will probably need
//	to changed when thick primitives with complex ROP's are introduced.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTDRAWINGALGORITHMS_H__40FBA707_CA67_441D_8683_A3DF85E6A1A1__INCLUDED_)
#define AFX_BPTDRAWINGALGORITHMS_H__40FBA707_CA67_441D_8683_A3DF85E6A1A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <math.h>

namespace BPT {

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Bresenham's line drawing algorithm based on Graphic Gems I pg. 685
	//	-- I chose a linear implmentation so that this function can be made to
	//	-- handle thick primtives efficiently by eleminating overdraw.
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class T >
	T __forceinline xx_ABS( const T a ) {

		return (0 > (a)) ? (-a) : (a);

	}

	template< class T >
	T __forceinline xx_SGN( const T a ) {

		return (0 > (a)) ? (-1) : (1);

	}

	//
	//	T_LineDDA
	//	

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_LineDDA(
		RENDER_TARGET & renderTarget
		,const POINT & from
		,const POINT & to
		,PEN_TYPE & pen
	) {

		int x1 = from.x;
		int y1 = from.y;
		int x2 = to.x;
		int y2 = to.y;
		
		int dx = x2-x1; int ax = xx_ABS(dx)<<1; int sx = xx_SGN(dx);
		int dy = y2-y1; int ay = xx_ABS(dy)<<1; int sy = xx_SGN(dy);
		
		int x = x1;
		int y = y1;
		
		if ( ax >= ay ) {		/* x dominant */
		
			int d = ay - (ax >> 1);
		
			for (;;) {

				pen( renderTarget, x, y );
		
			    if ( x == x2 ) return;
		
			    if ( d >= 0 ) {
		
					y += sy;
					d -= ax;
			    }
		
			    x += sx;
			    d += ay;
			}
		
		} else {			/* y dominant */
		
			int d = ax - (ay >> 1);
		
			for (;;) {
		
				pen( renderTarget, x, y );
		
			    if ( y == y2 ) return;
		
			    if ( d >= 0 ) {
		
					x += sx;
					d -= ay;
		
			    }
		
			    y += sy;
			    d += ax;
		
			}
		
		}

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Rectangle outline using the lineDDA preparing for future optimizations
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_EmptyRect(
		RENDER_TARGET & renderTarget
		,const RECT & rect
		,PEN_TYPE & pen
	) {

		// Setup the corner coordinates as points
		// --------------------------------------------------------------------

		POINT tl = { rect.left, rect.top };
		POINT br = { (rect.right - 1), (rect.bottom - 1) };

		POINT p;

		// Draw top (>)
		// --------------------------------------------------------------------

		p.x = br.x;
		p.y = tl.y;

		T_LineDDA( renderTarget, tl, p, pen );

		// Check for a completely horizontal or vertical line
		// --------------------------------------------------------------------

		if ( (tl.x == br.x) && (tl.y == br.y) ) {

			return /* done */;

		}

		// Draw right (V)
		// --------------------------------------------------------------------

		T_LineDDA( renderTarget, p, br, pen );

		// Draw bottom (<)
		// --------------------------------------------------------------------

		p.x = tl.x;
		p.y = br.y;

		T_LineDDA( renderTarget, br, p, pen );

		// Draw left (^)
		// --------------------------------------------------------------------

		T_LineDDA( renderTarget, p, tl, pen );

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Solid Rectangle 
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_SolidRect(
		RENDER_TARGET & renderTarget
		,const RECT & rect
		,PEN_TYPE & pen
	) {

		pen( renderTarget, rect );

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Crop Rectangle 
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_CropRect(
		RENDER_TARGET & renderTarget
		,const RECT & rect
		,PEN_TYPE & pen
	) {

		// Clip the operation to be within the bitmap limits
		// --------------------------------------------------------------------

		RECT bitmapLimits = renderTarget.GetCanvas()->Rect();

		RECT clippedRect = bitmapLimits;
	
		if ( !IntersectRect( &clippedRect, &bitmapLimits, &rect ) ) {
	
			return /* NOP */;
	
		}
	
		// Perform the operation
		// --------------------------------------------------------------------

		RECT rects[ 4 ];

		int count = ReturnExternalClipRects( rects, &bitmapLimits, &clippedRect );

		for ( int i = 0; i < count; i++ ) {

			T_SolidRect( renderTarget, rects[ i ], pen );

		}

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Midpoint ellipse algorithm
	// code is based on code from "Computer Graphics C version" pg 109.
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class TMidpointEllipseAlgorithm {

	private:

		template< class RENDER_TARGET, class PEN_TYPE >
		void Render(
			RENDER_TARGET & renderTarget
			,const POINT & center
			,const int x
			,const int y
			,PEN_TYPE & pen
			) const {

			pen( renderTarget, center.x + x, center.y + y );

			if ( x ) pen( renderTarget, center.x - x, center.y + y );

			if ( y ) {

				pen( renderTarget, center.x + x, center.y - y );

				if ( x ) pen( renderTarget, center.x - x, center.y - y );

			}

		}

		int round( const double value ) const {

			return (int)( value + 0.5 );

		}

	public:

		template< class RENDER_TARGET, class PEN_TYPE >
		void __forceinline operator()( 
			RENDER_TARGET & renderTarget
			,const POINT & center
			,const int a
			,const int b
			,PEN_TYPE & pen
			) const {

			// Check for special cases
			// ----------------------------------------------------------------

			if ( 0 == b ) {

				POINT pa = { center.x - a, center.y };
				POINT pb = { center.x + a, center.y };

				T_LineDDA( renderTarget, pa, pb, pen );
				return ;

			}

			// prepare 'step' variables
			// ----------------------------------------------------------------

			int Rx = a;
			int Ry = b;

			int Rx2 = (Rx * Rx);
			int Ry2 = (Ry * Ry);

			int twoRx2 = Rx2 * 2;
			int twoRy2 = Ry2 * 2;

			int x = 0;
			int y = Ry;
			int px = 0;
			int py = twoRx2 * y;

			// plot the first points & do 'region' 1
			// ----------------------------------------------------------------

			Render( renderTarget, center, x, y, pen );

			int p = round( Ry2 - (Rx2 * Ry) + (0.25 * Rx2) );

			while ( px < py ) {

				++x;

				px += twoRy2;

				if ( 0 > p ) {

					p += (Ry2 + px);

				} else {

					--y;

					py -= twoRx2;

					p += (Ry2 + px - py);

				}

				Render( renderTarget, center, x, y, pen );

			}

			// Render 'region' 2 
			// ----------------------------------------------------------------

			p = round(
				Ry2 * (x + 0.5) * (x + 0.5) + Rx2 * (y - 1) * (y - 1) - Rx2*Ry2
			);

			while ( 0 < y ) {

				--y;

				py -= twoRx2;

				if ( 0 < p ) {

					p += (Rx2 - py);

				} else {

					++x;

					px += twoRy2;

					p += (Rx2 - py + px);

				}

				Render( renderTarget, center, x, y, pen );

			}

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ellipse parametric equation
	//	-- this is possibly a 'slower' way of doing this but the code uses
	//	-- the lineDDA primitive which will be optimized for overlapped 
	//	-- rendering which is crucial for compex transfer operations.
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class TParametricEllipseAlgorithm {

	private:

		void __forceinline fn(
			POINT & out, const POINT & center, 
			const float af, const float bf, const float t ) const {

			// calculate the point rounding up or down as necessary
			// ----------------------------------------------------------------

			float xt = af * cosf( t );
			float yt = bf * sinf( t );

			if ( 0.0f > xt ) xt -= 0.5f;
			if ( 0.0f < xt ) xt += 0.5f;
			if ( 0.0f > yt ) yt -= 0.5f;
			if ( 0.0f < yt ) yt += 0.5f;

			out.x = center.x + (int)xt;
			out.y = center.y + (int)yt;

		}

	public:

		template< class RENDER_TARGET, class PEN_TYPE >
		void __forceinline operator()( 
			RENDER_TARGET & renderTarget
			,const POINT & center
			,const int a
			,const int b
			,PEN_TYPE & pen
			) const {

			// Check to see if we have a degenerate case 
			// ----------------------------------------------------------------

			if ( (0 == a) || (0 == b) ) {

				if ( 0 == a ) {

					if ( 0 == b ) {

						pen( renderTarget, center.x, center.y );

					} else {

						// vertical line

						POINT aa = { center.x, center.y - b };
						POINT bb = { center.x, center.y + b };

						T_LineDDA( renderTarget, aa, bb, pen );

					}

				} else {

					if ( 0 == b ) {

						pen( renderTarget, center.x, center.y );

					} else {

						// horizontal line

						POINT aa = { center.x - a, center.y };
						POINT bb = { center.x + a, center.y };

						T_LineDDA( renderTarget, aa, bb, pen );

					}

				}

				return /* done */;

			}

			// now get down to the ellipse renderer.
			// ----------------------------------------------------------------

			float af = a;
			float bf = b;

			float pi = 3.141592654f;

			// Use an approximation of the circumfrence of an ellipse as
			// the number of line segments to draw scaled by the ratio of
			// the major to minor axis, seems to make the ellipse look smoother
			// it's still seems to have a couple of 'spikes'.

			float h;

			if ( a > b ) {

				h = (bf / af);

			} else {

				h = (af / bf);

			}

			int N = (int)(((pi * ( (af + bf) * 1.5f - sqrtf(af*bf) )) + 0.5f) * h);

			float t = 0.0f;
			float dt = (pi * 2.0f) / N;

			POINT p;

			fn( p, center, af, bf, t );

			for ( int i = 0; i < N; i++ ) {

				POINT lp = p;

				t += dt;

				fn( p, center, af, bf, t );

				T_LineDDA( renderTarget, lp, p, pen );

			}

		}

	};

	// ------------------------------------------------------------------------

	//
	//	T_Ellipse
	//	

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_Ellipse(
		RENDER_TARGET & renderTarget
		,const POINT & center
		,const int a
		,const int b
		,PEN_TYPE & pen
	) {

#if 0
		TParametricEllipseAlgorithm ellipseGenerator;
#else
		TMidpointEllipseAlgorithm ellipseGenerator;
#endif

		ellipseGenerator( renderTarget, center, a, b, pen );

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Solid Ellipse code
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TMidpointSolidEllipseAlgorithm<>
	//

	class TMidpointSolidEllipseAlgorithm {

	private:

		template< class RENDER_TARGET, class PEN_TYPE >
		void __forceinline RenderSpans(
			RENDER_TARGET & renderTarget
			,const POINT & center
			,const int x
			,const int y
			,PEN_TYPE & pen
			) const {

			// Build 'rectangles' for the renderer
			// ----------------------------------------------------------------

			int lx = center.x - x;
			int cx = (x * 2) + 1;

			RECT top = { lx, center.y - y, lx + cx, center.y - y + 1 };
			RECT bot = { lx, center.y + y, lx + cx, center.y + y + 1 };

			// Call the renderer for both the rects
			// ----------------------------------------------------------------

			pen( renderTarget, top );
			pen( renderTarget, bot );

		}

		int round( const double value ) const {

			return (int)( value + 0.5 );

		}

	public:

		TMidpointSolidEllipseAlgorithm() { /* Empty */ }

		template< class RENDER_TARGET, class PEN_TYPE >
		void __forceinline operator()( 
			RENDER_TARGET & renderTarget
			,const POINT & center
			,const int a
			,const int b
			,PEN_TYPE & pen
			) const {

			// prepare 'step' variables
			// ----------------------------------------------------------------

			int Rx = a;
			int Ry = b;

			int Rx2 = (Rx * Rx);
			int Ry2 = (Ry * Ry);

			int twoRx2 = Rx2 * 2;
			int twoRy2 = Ry2 * 2;

			int x = 0;
			int y = Ry;
			int px = 0;
			int py = twoRx2 * y;

			// plot the first points & do 'region' 1
			// ----------------------------------------------------------------

			int lx = x;
			int ly = y;

			int p = round( Ry2 - (Rx2 * Ry) + (0.25 * Rx2) );

			while ( px < py ) {

				++x;

				px += twoRy2;

				if ( 0 > p ) {

					p += (Ry2 + px);

				} else {

					RenderSpans( renderTarget, center, lx, ly, pen );

					ly = --y;

					py -= twoRx2;

					p += (Ry2 + px - py);

				}

				lx = x;

			}

			RenderSpans( renderTarget, center, lx, ly, pen );

			// Render 'region' 2 
			// ----------------------------------------------------------------

			p = round(
				Ry2 * (x + 0.5) * (x + 0.5) + Rx2 * (y - 1) * (y - 1) - Rx2*Ry2
			);

			while ( 0 < y ) {

				--y;

				py -= twoRx2;

				if ( 0 < p ) {

					p += (Rx2 - py);

				} else {

					++x;

					px += twoRy2;

					p += (Rx2 - py + px);

				}

				RenderSpans( renderTarget, center, x, y, pen );

			}

		}

	};

	// ------------------------------------------------------------------------

	//
	//	T_SolidEllipse
	//	

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_SolidEllipse(
		RENDER_TARGET & renderTarget
		,const POINT & center
		,const int a
		,const int b
		,PEN_TYPE & pen
	) {

		TMidpointSolidEllipseAlgorithm ellipseGenerator;

		ellipseGenerator( renderTarget, center, a, b, pen );

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Solid Rectangle
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	T_SolidRectangle
	//	

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_SolidRectangle(
		RENDER_TARGET & renderTarget
		,const RECT & rect
		,PEN_TYPE & pen
	) {

		pen( renderTarget, rect );

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Stamp brush
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	T_StampBrush
	//	

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_StampBrush(
		RENDER_TARGET & renderTarget
		,PEN_TYPE & pen
	) {

		pen.Stamp( renderTarget );

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Brush at
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	T_DrawBrushAt<>
	//	

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_DrawBrushAt(
		RENDER_TARGET & renderTarget
		,const POINT & at
		,PEN_TYPE & pen
	) {

		pen( renderTarget, at );

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Curve rendering methods
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/*

	NOTE: These should really use forward differencing but these will work

*/

	//
	//	T_BezierCurve<>()
	//

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_BezierCurve(
		RENDER_TARGET & renderTarget
		,const POINT & a
		,const POINT & b
		,const POINT & c
		,const POINT & d
		,const int nLineSegments
		,PEN_TYPE & pen
	) {

		// prepare variables
		// --------------------------------------------------------------------

		float cx0 = (float)a.x; float cy0 = (float)a.y;
		float cx1 = (float)b.x; float cy1 = (float)b.y;
		float cx2 = (float)c.x; float cy2 = (float)c.y;
		float cx3 = (float)d.x; float cy3 = (float)d.y;

		float td = 1.0f / (float)((1 < nLineSegments) ? nLineSegments : 1);
		float t = 0.0f;

		// draw the line segments
		// --------------------------------------------------------------------

		POINT lp = a;

		POINT p;

		for ( int i = 0; i < nLineSegments; i++ ) {

			float it = 1.0f - t;
			float t2 = t * t;
			float t3 = t2 * t;
			float it2 = it * it;
			float it3 = it2 * it;

			float x = (cx0 * it3) + (cx1 * 3.0f * t * it2) + (cx2 * 3.0f * t2 * it) + (cx3 * t3);
			float y = (cy0 * it3) + (cy1 * 3.0f * t * it2) + (cy2 * 3.0f * t2 * it) + (cy3 * t3);

			p.x = (int)(x + 0.5f);
			p.y = (int)(y + 0.5f);

			T_LineDDA( renderTarget, lp, p, pen );

			lp = p;

			t += td;

		}

		T_LineDDA( renderTarget, lp, d, pen );

	}

	//
	//	T_dPaintCurve<>()
	//

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_dPaintCurve(
		RENDER_TARGET & renderTarget
		,const POINT & a
		,const POINT & b
		,const POINT & c
		,const int nLineSegments
		,PEN_TYPE & pen
	) {

		// Project the AB & CB line segments

		POINT m = { ((a.x + b.x + c.x) / 3), ((a.y + b.y + c.y) / 3) };
		POINT d = { ((a.x + b.x) / 2), ((a.y + b.y) / 2) };
		POINT e = { ((b.x + c.x) / 2), ((b.y + c.y) / 2) };
		POINT g = { ((d.x + e.x) / 2), ((d.y + e.y) / 2) };
		POINT p = { b.x - (m.x - g.x), b.y - (m.y - g.y) };
		POINT c1 = { (p.x - (m.x - d.x)), (p.y - (m.y - d.y)) };
		POINT c2 = { (p.x - (m.x - e.x)), (p.y - (m.y - e.y)) };

		// Call a 4 point curve primitive with the 'new' control points

		T_BezierCurve( renderTarget, a, c1, c2, c, nLineSegments, pen );

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	FLOOD FILL CORE
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TFloodFillPrim<>
	//
	//	Based on Paul Heckbert's seed fill algorithm in Gems I pg 275
	//

	template< class RENDER_TARGET, class PREDICATE, class PEN >
	class TFloodFillPrim {

	private:

		// Stack data / types
		// --------------------------------------------------------------------

		struct segment {

			int y, xl, xr, dy;

			segment( const int py, const int pxl, const int pxr, const int pdy ) :
				y( py ), xl( pxl ), xr( pxr ), dy( pdy ) {

				/* Empty */

			}

		};

		std::vector< segment > m_Stack;

		// Stack related methods
		// --------------------------------------------------------------------

		void __forceinline Push(
			const int y, const int xl, const int xr, const int dy, 
			const RECT & clipRect
		) {

			int ny = y + dy;

			if ( (ny >= clipRect.top) && (ny < clipRect.bottom) ) {

				m_Stack.push_back( segment( ny, xl, xr, dy ) );

			}

		}

		void __forceinline Pop( int & y, int & xl, int & xr, int & dy ) {

			segment & back = m_Stack.back();

			y = back.y;
			xl = back.xl;
			xr = back.xr;
			dy = back.dy;

			m_Stack.pop_back();

		}

		// --------------------------------------------------------------------

	public:

		TFloodFillPrim() { /* Empty */ }

		void FillMethod(
			RENDER_TARGET & renderTarget, const int xx, const int yy, 
			PREDICATE predicate, PEN & pen,
			const RECT * pClipRect = 0 ) {

			// Get the canvas for the render target passed in.
			// ----------------------------------------------------------------

			typedef RENDER_TARGET::surface_type SURFACE;

			RENDER_TARGET::surface_type::pointer pSurface = renderTarget.GetCanvas();

			if ( !pSurface ) {

				return /* NOP */;

			}

			// Reserve enough space on the 'stack' for 16x the height
			// ----------------------------------------------------------------

			m_Stack.reserve( pSurface->Height() * 16);

			// Determine the write 'limits' for this operation
			// ----------------------------------------------------------------

			RECT bitmapLimits = pSurface->Rect();

			RECT clippedRect;

			if ( pClipRect ) {
	
				if ( !IntersectRect( &clippedRect, &bitmapLimits, pClipRect ) ) {
		
					return /* NOP */;
		
				}

			} else {

				clippedRect = bitmapLimits;

			}

			// Make sure that initial point is with in the bitmap limits and
			// that is satisfies the predicate, if not we're done :)
			// ----------------------------------------------------------------

			int x = xx;
			int y = yy;

			if ( (x < clippedRect.left) || (y < clippedRect.top) ||
				(x >= clippedRect.right) || (y >= clippedRect.bottom) ) {

				return /* NOP */;

			}

			if ( !pen.Predicate( 
				predicate( 
					pen.OverridePixelValue( 
						*pSurface->Iterator( x, y ), x, y 
					)
				), x, y ) ) {

				return /* NOP */;

			}

			// Okay now that we're past the bozo test lets get down to business
			// ----------------------------------------------------------------

			Push( y, x, x, 1, clippedRect );
			
			Push( y + 1, x, x, -1, clippedRect );

			int safetyCounter = pSurface->Height() * pSurface->Width() / 16;

			while ( !m_Stack.empty() ) {

				if ( 0 >= --safetyCounter ) {

					break;

				}

				// Pop something off the stack to process
				// ------------------------------------------------------------

				int l, dy, x1, x2;

				Pop( y, x1, x2, dy );

				// Scan left
				// ------------------------------------------------------------

				x = x1;

				if ( x >= clippedRect.left ) {

					SURFACE::reverse_iterator rit = pSurface->rIterator( x, y );
					SURFACE::reverse_iterator start = rit;

					while ( pen.Predicate( predicate( 
						pen.OverridePixelValue( *rit, x, y ) ), x, y ) ) {

						++rit;

						if ( --x < clippedRect.left ) {

							break;

						}

					}

					if ( start != rit ) {

						// fill (x to x1], y 

						RECT rect = { x + 1, y, x1 + 1, y + 1 };

						pen( renderTarget, rect );

					}

				}

				if ( x >= x1 ) {

					goto SKIP;

				}

				// Scan right
				// ------------------------------------------------------------

				l = x + 1;

				if ( l < x1 ) {

					Push( y, l, x1 - 1, -dy, clippedRect );

				}

				x = x1 + 1;

				do {

					// Right span length
					// --------------------------------------------------------

					if ( x < clippedRect.right ) {

						int sx = x;
	
						SURFACE::pixel_iterator it = pSurface->Iterator( x, y );
						SURFACE::pixel_iterator start = it;
	
						while ( pen.Predicate( predicate( 
							pen.OverridePixelValue( *it, x, y ) ), x, y ) ) {
	
							++it;
	
							if ( ++x >= clippedRect.right ) {
	
								break;
	
							}
	
						}
	
						if ( start != it ) {
	
							// fill [sx to x), y 
	
							RECT rect = { sx, y, x, y + 1 };

							pen( renderTarget, rect );
	
						}

					}

					// More area's to process pushed onto the stack.
					// --------------------------------------------------------

					Push( y, l, x - 1, dy, clippedRect );

					if ( x > x2 + 1 ) {

						Push( y, x2 + 1, x - 1, -dy, clippedRect );

					}

					// Search more to the right.
					// --------------------------------------------------------

				SKIP:

					if ( ++x <= x2 ) {

						SURFACE::pixel_iterator it = pSurface->Iterator( x, y );

						while ( !pen.Predicate( predicate( 
							pen.OverridePixelValue( *it++, x, y ) ), x, y ) ) {

							++x;

							if ( x > x2 ) {

								break;

							}

						}

					}

					l = x;

				} while ( x <= x2 );

			}

		}

		void operator()(
			RENDER_TARGET & renderTarget, const int xx, const int yy, 
			PREDICATE predicate, PEN & pen,
			const RECT * pClipRect = 0 ) {

			FillMethod( renderTarget, xx, yy, predicate, pen, pClipRect );

		}

	};

	//
	//	T_FloodFill<>()
	//

	template< class RENDER_TARGET, class PEN_TYPE >
	void __forceinline T_SimpleFloodFill(
		RENDER_TARGET & renderTarget
		,const POINT & p
		,PEN_TYPE & pen
	) {

		typedef RENDER_TARGET::surface_type::pixel_type pixel_type;

		TIsValue<pixel_type> predicate(
			pen.OverridePixelValue(
				*(renderTarget.GetCanvas()->Iterator( p.x, p.y )), p.x, p.y
			)
		);

		TFloodFillPrim<RENDER_TARGET, TIsValue<pixel_type>, PEN_TYPE> fillObj;

		fillObj.FillMethod( 
			renderTarget
			,p.x
			,p.y 
			,predicate
			,pen
		);

	}

}; // namespace BPT

#endif // !defined(AFX_BPTDRAWINGALGORITHMS_H__40FBA707_CA67_441D_8683_A3DF85E6A1A1__INCLUDED_)
