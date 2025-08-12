// BPTBlitter.h: interface for the BPTBlitter class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	This file contains the basic blitter shells the raster operations
//	are used abstractly by these blitter functions. 
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTBLITTER_H__681A3A57_7AAF_4AD7_A5BA_BE2A3BD11191__INCLUDED_)
#define AFX_BPTBLITTER_H__681A3A57_7AAF_4AD7_A5BA_BE2A3BD11191__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>

#include "BPTRasterOps.h"

#define STRICT_ENFORCEMENT_OF_TEXTURE_COORDINATES

namespace BPT {

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Blitter template function
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	BLITFX
	//

	struct BLITFX {

		enum {

			HFLIP				= 0x00000001
			,VFLIP				= 0x00000002

		};

		DWORD dwFlags;

		BLITFX( const DWORD flags ) : dwFlags( flags ) { /* Empty */ }

	};

	// ------------------------------------------------------------------------

	//
	//	T_Blit
	//

	template< class DST_SURFACE, class SRC_SURFACE, class TOP >
	void __forceinline T_Blit(
		DST_SURFACE & dstSurface
		,const int x
		,const int y
		,SRC_SURFACE & srcSurface
		,TOP op = TOP()
		,const BLITFX * blitFX = 0
		,const RECT * optionalDstClipRect = 0
		,const RECT * optionalSrcSubRect = 0
	) {

		// Clip the optional clipping rect to the dest bitmap limits
		// --------------------------------------------------------------------

		RECT dstLimitsRect = dstSurface.Rect();

		RECT clippedDstRect;

		if ( optionalDstClipRect ) {

			if ( !IntersectRect( &clippedDstRect, &dstLimitsRect, optionalDstClipRect) ) {

				return /* NOP */;

			}

		} else {

			clippedDstRect = dstLimitsRect;

		}

		// Get the source operation size
		// --------------------------------------------------------------------

		RECT srcLimitsRect = srcSurface.Rect();

		RECT clippedSrcRect;

		if ( optionalSrcSubRect ) {

			if ( !IntersectRect( &clippedSrcRect, &srcLimitsRect, optionalSrcSubRect) ) {

				return /* NOP */;

			}

		} else {

			clippedSrcRect = srcLimitsRect;

		}

		// Perform a simple clipping operation to detect NOP
		// --------------------------------------------------------------------

		SIZE clippedSrcRectSize = SizeOfRect( &clippedSrcRect );

		RECT dstOperation = {
			x, y, x + clippedSrcRectSize.cx, y + clippedSrcRectSize.cy
		};

		RECT clippedRect;
			
		if ( !IntersectRect( &clippedRect, &clippedDstRect, &dstOperation) ) {

			return /* NOP */;

		}

		// Setup the general loop variables
		// --------------------------------------------------------------------

		int cx = clippedRect.right - clippedRect.left;
		int cy = clippedRect.bottom - clippedRect.top;

		int sx = ((clippedRect.left - x) + clippedSrcRect.left);
		int sy = ((clippedRect.top - y) + clippedSrcRect.top);

		// Check for flipping and adjust the dest position and step amount.
		// --------------------------------------------------------------------

		int dx, dy, ddx, ddy;

		if ( blitFX ) {

			if ( BLITFX::HFLIP & blitFX->dwFlags ) {

				sx = (clippedSrcRect.right - (sx + cx));
				dx = (clippedRect.right - 1);
				ddx = -1;

			} else {

				dx = clippedRect.left;
				ddx = 1;

			}

			if ( BLITFX::VFLIP & blitFX->dwFlags ) {

				sy = (clippedSrcRect.bottom - (sy + cy));
				dy = (clippedRect.bottom - 1);
				ddy = -1;

			} else {

				dy = clippedRect.top;
				ddy = 1;

			}

		} else {

			dx = clippedRect.left;
			ddx = 1;

			dy = clippedRect.top;
			ddy = 1;

		}

		// Process the non clipped spans
		// --------------------------------------------------------------------

		if ( 1 == ddx ) {

			for ( int ly = 0; ly < cy; ly++ ) {

				DST_SURFACE::pixel_iterator dstIT = dstSurface.Iterator( dx, dy );
				SRC_SURFACE::pixel_iterator srcIT = srcSurface.Iterator( sx, sy );

				op.InputMajor( srcIT, srcIT + cx, dstIT );

				dy += ddy;
				++sy;
	
			}

		} else {

			for ( int ly = 0; ly < cy; ly++ ) {

				DST_SURFACE::reverse_iterator dstIT = dstSurface.rIterator( dx, dy );
				SRC_SURFACE::pixel_iterator srcIT = srcSurface.Iterator( sx, sy );

				op.InputMajor( srcIT, srcIT + cx, dstIT );

				dy += ddy;
				++sy;
	
			}

		}

	}

	//
	//	T_Blit
	//

	template< class DST_SURFACE, class SRC_SURFACE, class TOP >
	void T_Blit(
		DST_SURFACE * pDstSurface
		,const int x
		,const int y
		,SRC_SURFACE * pSrcSurface
		,TOP op = TOP()
		,const BLITFX * blitFX = 0
		,const RECT * optionalDstClipRect = 0
		,const RECT * optionalSrcSubRect = 0
	) {

		T_Blit(
			*pDstSurface, x, y, *pSrcSurface, op, blitFX, 
			optionalDstClipRect, optionalSrcSubRect
		);

	}

	// ------------------------------------------------------------------------
	// Texture mapping calculations is the code written by Chris Hecker
	// for the Game developers magazine. I forced the whole thing into a class
	// and made the bitmap handling and rop abstract. (BPT)
	// ------------------------------------------------------------------------

	//
	//	TPerspectiveCorrectTextureMapTriangle
	//

	class TPerspectiveCorrectTextureMapTriangle {

	public:

		struct FVF {

			float X, Y, Z, U, V;

		};
	
	private:

		// --------------------------------------------------------------------

		struct gradients_fl_fl {

			float aOneOverZ[3];				// 1/z for each vertex
			float aUOverZ[3];				// u/z for each vertex
			float aVOverZ[3];				// v/z for each vertex
			float dOneOverZdX, dOneOverZdY;	// d(1/z)/dX, d(1/z)/dY
			float dUOverZdX, dUOverZdY;		// d(u/z)/dX, d(u/z)/dY
			float dVOverZdX, dVOverZdY;		// d(v/z)/dX, d(v/z)/dY

			gradients_fl_fl( FVF const *pVertices ) {

				int Counter;
				
				float OneOverdX = 1 / (((pVertices[1].X - pVertices[2].X) *
						(pVertices[0].Y - pVertices[2].Y)) -
						((pVertices[0].X - pVertices[2].X) *
						(pVertices[1].Y - pVertices[2].Y)));
			
				float OneOverdY = -OneOverdX;
			
				for(Counter = 0;Counter < 3;Counter++)
				{
					float const OneOverZ = 1/pVertices[Counter].Z;
					aOneOverZ[Counter] = OneOverZ;
					aUOverZ[Counter] = pVertices[Counter].U * OneOverZ;
					aVOverZ[Counter] = pVertices[Counter].V * OneOverZ;
				}
			
				dOneOverZdX = OneOverdX * (((aOneOverZ[1] - aOneOverZ[2]) *
						(pVertices[0].Y - pVertices[2].Y)) -
						((aOneOverZ[0] - aOneOverZ[2]) *
						(pVertices[1].Y - pVertices[2].Y)));
				dOneOverZdY = OneOverdY * (((aOneOverZ[1] - aOneOverZ[2]) *
						(pVertices[0].X - pVertices[2].X)) -
						((aOneOverZ[0] - aOneOverZ[2]) *
						(pVertices[1].X - pVertices[2].X)));
			
				dUOverZdX = OneOverdX * (((aUOverZ[1] - aUOverZ[2]) *
						(pVertices[0].Y - pVertices[2].Y)) -
						((aUOverZ[0] - aUOverZ[2]) *
						(pVertices[1].Y - pVertices[2].Y)));
				dUOverZdY = OneOverdY * (((aUOverZ[1] - aUOverZ[2]) *
						(pVertices[0].X - pVertices[2].X)) -
						((aUOverZ[0] - aUOverZ[2]) *
						(pVertices[1].X - pVertices[2].X)));
			
				dVOverZdX = OneOverdX * (((aVOverZ[1] - aVOverZ[2]) *
						(pVertices[0].Y - pVertices[2].Y)) -
						((aVOverZ[0] - aVOverZ[2]) *
						(pVertices[1].Y - pVertices[2].Y)));
				dVOverZdY = OneOverdY * (((aVOverZ[1] - aVOverZ[2]) *
						(pVertices[0].X - pVertices[2].X)) -
						((aVOverZ[0] - aVOverZ[2]) *
						(pVertices[1].X - pVertices[2].X)));

			}

		};

		// --------------------------------------------------------------------

		struct edge_fl_fl {

			float X, XStep;					// fractional x and dX/dY
			int Y, Height;					// current y and vertical count
			float OneOverZ, OneOverZStep;	// 1/z and step
			float UOverZ, UOverZStep;		// u/z and step
			float VOverZ, VOverZStep;		// v/z and step

			__forceinline int Step( void ) {

				X += XStep; Y++; Height--;
				UOverZ += UOverZStep; VOverZ += VOverZStep; OneOverZ += OneOverZStep;
				return Height;

			}
		
			edge_fl_fl(
				gradients_fl_fl const &Gradients, 
				FVF const *pVertices, int Top,
				int Bottom ) {

				Y = (int)ceil(pVertices[Top].Y);
				int YEnd = (int)ceil(pVertices[Bottom].Y);
				Height = YEnd - Y;
			
				float YPrestep = Y - pVertices[Top].Y;
				
				float RealHeight = pVertices[Bottom].Y - pVertices[Top].Y;
				float RealWidth = pVertices[Bottom].X - pVertices[Top].X;
			
				X = ((RealWidth * YPrestep)/RealHeight) + pVertices[Top].X;
				XStep = RealWidth/RealHeight;
				float XPrestep = X - pVertices[Top].X;
			
				OneOverZ = Gradients.aOneOverZ[Top] + YPrestep * Gradients.dOneOverZdY
								+ XPrestep * Gradients.dOneOverZdX;
				OneOverZStep = XStep * Gradients.dOneOverZdX + Gradients.dOneOverZdY;
			
				UOverZ = Gradients.aUOverZ[Top] + YPrestep * Gradients.dUOverZdY
								+ XPrestep * Gradients.dUOverZdX;
				UOverZStep = XStep * Gradients.dUOverZdX + Gradients.dUOverZdY;
			
				VOverZ = Gradients.aVOverZ[Top] + YPrestep * Gradients.dVOverZdY
								+ XPrestep * Gradients.dVOverZdX;
				VOverZStep = XStep * Gradients.dVOverZdX + Gradients.dVOverZdY;

			}

		};
		
		// --------------------------------------------------------------------

		template< class DST_SURFACE, class SRC_SURFACE, class TOP >
		void __forceinline DrawScanLine(
			DST_SURFACE &Dest, gradients_fl_fl const &Gradients,
			edge_fl_fl *pLeft, edge_fl_fl *pRight, SRC_SURFACE &Texture,
			TOP & op, const int leftEdge, const int rightEdge
		)
		{
			int XStart = (int)ceil(pLeft->X);
			int XEnd = (int)ceil(pRight->X);

			// clip the left edge

			if ( 0 > XStart ) {

				XStart = 0;

			}

			int lClip = XStart - leftEdge;

			if ( 0 > lClip ) {

				XStart -= lClip;

				if ( XStart >= XEnd ) {

					return /* nop */;

				}

			}

			// clip the right edge

			if ( rightEdge < XEnd) {

				XEnd = rightEdge;

			}

			int rClip = XEnd - rightEdge;

			if ( 0 < rClip ) {

				XEnd -= rClip;

				if ( XStart >= XEnd ) {

					return /* nop */;

				}

			}

			// calc the operation width and bail if empty

			int Width = (XEnd - XStart);

			if ( 0 >= Width ) {

				return /* nop */;

			}

			DST_SURFACE::pixel_iterator pDestBits = Dest.Iterator( XStart, pLeft->Y );
			SRC_SURFACE::pixel_iterator pTextureBits = Texture.Iterator( 0, 0 );

			long TextureDeltaScan = Texture.Pitch();
			float XPrestep = XStart - pLeft->X;
			float OneOverZ = pLeft->OneOverZ + XPrestep * Gradients.dOneOverZdX;
			float UOverZ = pLeft->UOverZ + XPrestep * Gradients.dUOverZdX;
			float VOverZ = pLeft->VOverZ + XPrestep * Gradients.dVOverZdX;
		
#if defined(STRICT_ENFORCEMENT_OF_TEXTURE_COORDINATES)

			int maxU = Texture.Width() - 1;
			int maxV = Texture.Height() - 1;

#endif

			while( Width-- > 0 ) {

				float Z = 1/OneOverZ;
				int U = (int)(UOverZ * Z);
				int V = (int)(VOverZ * Z);

#if defined(STRICT_ENFORCEMENT_OF_TEXTURE_COORDINATES)

				U = max( 0, min( U, maxU ) );
				V = max( 0, min( V, maxV ) );

#endif

				op( *pDestBits++, *(DST_SURFACE::pixel_iterator(
					(BYTE *)(pTextureBits + U) + (V * TextureDeltaScan)))
				);
		
				OneOverZ += Gradients.dOneOverZdX;
				UOverZ += Gradients.dUOverZdX;
				VOverZ += Gradients.dVOverZdX;

			}

		}

		// --------------------------------------------------------------------

	public:

		TPerspectiveCorrectTextureMapTriangle() { /* Empty */ }

		template< class DST_SURFACE, class SRC_SURFACE, class TOP >
		void operator()(
			DST_SURFACE &Dest, FVF const *pVertices,
			SRC_SURFACE &Texture, TOP op,
			const RECT * pClipRect = 0 )
		{

			// Setup the clipping rectangle
			// ----------------------------------------------------------------

			RECT bitmapLimits = Dest.Rect();

			RECT clippedRect;

			if ( pClipRect ) {
	
				if ( !IntersectRect( &clippedRect, &bitmapLimits, pClipRect ) ) {
		
					return /* NOP */;
		
				}

			} else {

				clippedRect = bitmapLimits;

			}

			// ----------------------------------------------------------------

			int Top, Middle, Bottom, MiddleForCompare, BottomForCompare;

			float Y0 = pVertices[0].Y, Y1 = pVertices[1].Y,
							Y2 = pVertices[2].Y;
		
			// sort vertices in y

			if(Y0 < Y1) {
				if(Y2 < Y0) {
					Top = 2; Middle = 0; Bottom = 1;
					MiddleForCompare = 0; BottomForCompare = 1;
				} else {
					Top = 0;
					if(Y1 < Y2) {
						Middle = 1; Bottom = 2;
						MiddleForCompare = 1; BottomForCompare = 2;
					} else {
						Middle = 2; Bottom = 1;
						MiddleForCompare = 2; BottomForCompare = 1;
					}
				}
			} else {
				if(Y2 < Y1) {
					Top = 2; Middle = 1; Bottom = 0;
					MiddleForCompare = 1; BottomForCompare = 0;
				} else {
					Top = 1;
					if(Y0 < Y2) {
						Middle = 0; Bottom = 2;
						MiddleForCompare = 3; BottomForCompare = 2;
					} else {
						Middle = 2; Bottom = 0;
						MiddleForCompare = 2; BottomForCompare = 3;
					}
				}
			}
		
			gradients_fl_fl Gradients(pVertices);
			edge_fl_fl TopToBottom(Gradients,pVertices,Top,Bottom);
			edge_fl_fl TopToMiddle(Gradients,pVertices,Top,Middle);
			edge_fl_fl MiddleToBottom(Gradients,pVertices,Middle,Bottom);
			edge_fl_fl *pLeft, *pRight;
			int MiddleIsLeft;
		
			// the triangle is clockwise, so if bottom > middle then middle is right

			if( BottomForCompare > MiddleForCompare ) {

				MiddleIsLeft = 0;
				pLeft = &TopToBottom; pRight = &TopToMiddle;

			} else {

				MiddleIsLeft = 1;
				pLeft = &TopToMiddle; pRight = &TopToBottom;

			}
		
			int Height = TopToMiddle.Height;
		
			while( Height-- ) {

				if ( (pLeft->Y >= clippedRect.top) && (pLeft->Y < clippedRect.bottom) ) {

					DrawScanLine(
						Dest,Gradients,pLeft,pRight,Texture, op,
						clippedRect.left, clippedRect.right
					);

				}

				TopToMiddle.Step(); TopToBottom.Step();

			}
		
			Height = MiddleToBottom.Height;
		
			if( MiddleIsLeft ) {

				pLeft = &MiddleToBottom; pRight = &TopToBottom;

			} else {

				pLeft = &TopToBottom; pRight = &MiddleToBottom;

			}
			
			while( Height-- ) {

				if ( (pLeft->Y >= clippedRect.top) && (pLeft->Y < clippedRect.bottom) ) {

					DrawScanLine(
						Dest,Gradients,pLeft,pRight,Texture, op,
						clippedRect.left, clippedRect.right
					);

				}

				MiddleToBottom.Step(); TopToBottom.Step();

			}

		}

	};

	// ------------------------------------------------------------------------
	
	//
	//	T_DrawSurfaceToQuad()
	//

	template< class DST_SURFACE, class SRC_SURFACE, class TOP >
	void T_DrawSurfaceToQuad(
		DST_SURFACE & dstSurface, const POINT * pList, 
		SRC_SURFACE & srcSurface, TOP op,
		const RECT * pOptionalClipRect = 0
	) {

		TPerspectiveCorrectTextureMapTriangle triangleRenderer;

		float z = 1.0f;
		float u0 = 0.0f;
		float v0 = 0.0f;

		float u1 = (float)srcSurface.Width();
		float v1 = (float)srcSurface.Height();

#if 1

		// Alternative vertex order.

		TPerspectiveCorrectTextureMapTriangle::FVF verts[ 6 ] = {
			{ (float)pList[3].x, (float)pList[3].y, z, u0, v1 },
			{ (float)pList[0].x, (float)pList[0].y, z, u0, v0 },
			{ (float)pList[1].x, (float)pList[1].y, z, u1, v0 },
			{ (float)pList[3].x, (float)pList[3].y, z, u0, v1 },
			{ (float)pList[1].x, (float)pList[1].y, z, u1, v0 },
			{ (float)pList[2].x, (float)pList[2].y, z, u1, v1 }
		};

		triangleRenderer(
			dstSurface, verts, srcSurface, op, pOptionalClipRect
		);

		triangleRenderer(
			dstSurface, verts + 3, srcSurface, op, pOptionalClipRect
		);

#else

		TPerspectiveCorrectTextureMapTriangle::FVF verts[ 5 ] = {
			{ (float)pList[0].x, (float)pList[0].y, z, u0, v0 },
			{ (float)pList[1].x, (float)pList[1].y, z, u1, v0 },
			{ (float)pList[2].x, (float)pList[2].y, z, u1, v1 },
			{ (float)pList[3].x, (float)pList[3].y, z, u0, v1 },
			{ (float)pList[0].x, (float)pList[0].y, z, u0, v0 }
		};

		triangleRenderer(
			dstSurface, verts, srcSurface, op, pOptionalClipRect
		);

		triangleRenderer(
			dstSurface, verts + 2, srcSurface, op, pOptionalClipRect
		);

#endif

	}

	// ------------------------------------------------------------------------

	//
	//	T_ScaleUpBlit
	//
	//	-- This routine could cause a memory 'leak' to be reported at the end
	//	-- of a program, basicly if the tables are indeed cached then there is
	//	-- no way to flush them.  If this was a class things would be different!
	//

	template< class DST_SURFACE, class SRC_SURFACE, class TOP >
	void T_ScaleUpBlit(
		DST_SURFACE & dstSurface
		,const int x
		,const int y
		,SRC_SURFACE & srcSurface
		,const int w
		,const int h
		,TOP op = TOP()
		,const BLITFX * blitFX = 0
		,const RECT * optionalDstClipRect = 0
		,const RECT * optionalSrcSubRect = 0
		,const bool deleteTablesAfterOperation = false
	) {

		// Define the 'type' of the scaling table
		// --------------------------------------------------------------------

		typedef unsigned char scale_entry_type;
		typedef scale_entry_type * scale_table_type;

		typedef TTableDrivenScaleUpOP<
			TOP::dst_type, TOP::src_type, scale_table_type, TOP
		> scale_rop_type;

		// Clip the optional clipping rect to the dest bitmap limits
		// --------------------------------------------------------------------

		RECT dstLimitsRect = dstSurface.Rect();

		RECT clippedDstRect;

		if ( optionalDstClipRect ) {

			if ( !IntersectRect( &clippedDstRect, &dstLimitsRect, optionalDstClipRect ) ) {

				return /* NOP */;

			}

		} else {

			clippedDstRect = dstLimitsRect;

		}

		// Perform a simple clipping operation to detect NOP
		// --------------------------------------------------------------------

		RECT dstOperation = { x, y, x + w, y + h };

		RECT clippedRect;
			
		if ( !IntersectRect( &clippedRect, &clippedDstRect, &dstOperation ) ) {

			return /* NOP */;

		}

		// Get the source operation size
		// --------------------------------------------------------------------

		RECT srcLimitsRect = srcSurface.Rect();

		RECT clippedSrcRect;

		if ( optionalSrcSubRect ) {

			if ( !IntersectRect( &clippedSrcRect, &srcLimitsRect, optionalSrcSubRect ) ) {

				return /* NOP */;

			}

		} else {

			clippedSrcRect = srcLimitsRect;

		}

		int sw = (clippedSrcRect.right - clippedSrcRect.left);
		int sh = (clippedSrcRect.bottom - clippedSrcRect.top);

		// Make sure we are scaling up!
		// --------------------------------------------------------------------

		if ( (w < sw) || (h < sh) ) {

			return /* NOP */;

		}

		// Calculate the sub pixel coordinates & the operation size
		// --------------------------------------------------------------------

		const int cx = clippedRect.right - clippedRect.left;
		const int cy = clippedRect.bottom - clippedRect.top;

		float hScale = (float)sw / (float)w;
		float vScale = (float)sh / (float)h;

		int a = (clippedRect.left - x);
		int b = (clippedRect.top - y);

		// Setup the scale table and the scale rop
		// --------------------------------------------------------------------

		static scale_table_type pXScaleTable = 0;

		static int cached_w = -1;

		if ( (!pXScaleTable) || (cached_w < w) ) { // BPT 10/3/02 (< was !=)

			if ( pXScaleTable ) {

				AtExitDeleteArray( pXScaleTable, true ); // BPT 10/3/02
				
				delete [] pXScaleTable;

			}

			pXScaleTable = new scale_entry_type [ w ];

			if ( !pXScaleTable ) {
	
				cached_w = -1;

				return /* error nop */;
	
			} else {

				AtExitDeleteArray( pXScaleTable ); // BPT 10/3/02

			}

			cached_w = w;

		}

		static scale_table_type pYScaleTable = 0;

		static int cached_h = -1;

		if ( (!pYScaleTable) || (cached_h < h) ) { // BPT 10/3/02 (< was !=)

			if ( pYScaleTable ) {
				
				AtExitDeleteArray( pYScaleTable, true ); // BPT 10/3/02

				delete [] pYScaleTable;

			}

			pYScaleTable = new scale_entry_type [ h ];

			if ( !pYScaleTable ) {

				cached_h = -1;
	
				return /* error nop */;
	
			} else {

				AtExitDeleteArray( pYScaleTable ); // BPT 10/3/02

			}

			cached_h = h;

		}

		// Build the scale table, which is nothing more than when to increase
		// the source pointer during the blit operation. 
		// --------------------------------------------------------------------

		float fx = 0.0f;

		int lx = 0;

		for ( int i = 0; i < w; i++ ) {

			int ix = (int)fx;

			pXScaleTable[ i ] = lx - ix;

			lx = ix;

			fx += hScale;

		}

		float fy = 0.0f;

		int ly = 0;

		for ( int j = 0; j < h; j++ ) {

			int iy = (int)fy;

			pYScaleTable[ j ] = ly - iy;

			ly = (int)fy;

			fy += vScale;

		}

		// Figure out the 'start' position
		// --------------------------------------------------------------------

		int sx = clippedSrcRect.left;

		for ( i = 0; i < a; i++ ) {

			if ( pXScaleTable[i] ) {

				++sx;

			}

		}

		int sy = clippedSrcRect.top;

		for ( j = 0; j < b; j++ ) {

			if ( pYScaleTable[j] ) {

				++sy;

			}

		}

		// Check for flipping and adjust the dest position and step amount.
		// --------------------------------------------------------------------

		int dx, dy, ddx, ddy;

		if ( blitFX ) {

			if ( BLITFX::HFLIP & blitFX->dwFlags ) {

				// use the scaling table to find the starting sub pixel

				a = w - (cx + a);

				sx = clippedSrcRect.left;
		
				for ( i = 0; i < a; i++ ) {
		
					if ( pXScaleTable[ i ] ) {
		
						++sx;
		
					}
		
				}

				dx = (clippedRect.right - 1);
				ddx = -1;

			} else {

				dx = clippedRect.left;
				ddx = 1;

			}

			if ( BLITFX::VFLIP & blitFX->dwFlags ) {

				// use the scaling table to find the starting sub pixel

				b = h - (cy + b);

				sy = clippedSrcRect.top;
		
				for ( j = 0; j < b; j++ ) {
		
					if ( pYScaleTable[j] ) {
		
						++sy;
		
					}
		
				}

				dy = (clippedRect.bottom - 1);
				ddy = -1;

			} else {

				dy = clippedRect.top;
				ddy = 1;

			}

		} else {

			dx = clippedRect.left;
			ddx = 1;

			dy = clippedRect.top;
			ddy = 1;

		}

		// Setup the scale raster operation
		// --------------------------------------------------------------------

		scale_rop_type scaleOp( pXScaleTable + a, op );

		// Do each vertical line in the clipped operation rectangle
		// The general ddx & ddy could be removed and the code might see a
		// slight improvement in speed.
		// --------------------------------------------------------------------

		scale_table_type pVStep = (pYScaleTable + b);

		if ( 1 == ddx ) {

			for ( int ly = 0; ly < cy; ly++ ) {

				DST_SURFACE::pixel_iterator dstIT = dstSurface.Iterator( dx, dy );
				SRC_SURFACE::pixel_iterator srcIT = srcSurface.Iterator( sx, sy );

				scaleOp.InputMinor( dstIT, dstIT + cx, srcIT );
		
				dy += ddy;

				if ( *pVStep++ ) {

					++sy;

				}
	
			}

		} else {

			for ( int ly = 0; ly < cy; ly++ ) {

				DST_SURFACE::reverse_iterator dstIT = dstSurface.rIterator( dx, dy );
				SRC_SURFACE::pixel_iterator srcIT = srcSurface.Iterator( sx, sy );

				scaleOp.InputMinor( dstIT, dstIT + cx, srcIT );

				dy += ddy;

				if ( *pVStep++ ) {

					++sy;

				}
	
			}

		}

		// Free up our scale tables
		// --------------------------------------------------------------------

		if ( deleteTablesAfterOperation ) {

			AtExitDeleteArray( pYScaleTable, true ); // BPT 10/3/02
			AtExitDeleteArray( pXScaleTable, true ); // BPT 10/3/02

			delete [] pYScaleTable; 
			delete [] pXScaleTable; 

			pXScaleTable = 0;
			cached_w = -1;

			pYScaleTable = 0;
			cached_h = -1;

		}

	}

}; // namespace BPT

#endif // !defined(AFX_BPTBLITTER_H__681A3A57_7AAF_4AD7_A5BA_BE2A3BD11191__INCLUDED_)
