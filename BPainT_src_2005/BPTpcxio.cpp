// BPTpcxio.cpp: implementation of the BPTpcxio class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//
//	This code really needs to client out to a some kind of DLL or
//	library that can load & save more bitmap file formats.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bpaint.h"
#include "BPTpcxio.h"
#include "IJLBitmap.H"
#include "BPTAnimator.h"
#include "BPTDib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace BPT {

	// ------------------------------------------------------------------------

	typedef struct {

		BYTE	manufacturer;
		BYTE	version;
		BYTE	encoding;
		BYTE	bitsPerPixel;
		WORD	xmin;
		WORD	ymin;
		WORD	xmax;
		WORD	ymax;
		WORD	hres;
		WORD	vres;
		BYTE	colorMap16[48];
		BYTE	reserved;
		BYTE	nPlanes;
		WORD	rowBytes;
		WORD	paletteInfo;
		WORD	screenWidth;
		WORD	screenHeight;
		BYTE	pad[54];

	} PCX_HEADER;

	// ------------------------------------------------------------------------

	//
	//	Load32bppDibFrom_HBITMAP()
	//

	TDIBSection<DWORD> * Load32bppDibFrom_HBITMAP( HBITMAP hBitmap ) {

		// map the HBITMAP into a CBitmap
		// --------------------------------------------------------------------

		CBitmap * pBitmap = CBitmap::FromHandle( hBitmap );

		if ( !pBitmap ) return 0;

		BITMAP bitmap;

		if ( !pBitmap->GetBitmap( &bitmap ) ) return 0;

		// create a DIBSECTION to hold the results
		// --------------------------------------------------------------------

		TDIBSection<DWORD> * pDib = new TDIBSection<DWORD>();

		if ( !pDib ) return 0;

		if ( !pDib->Create( bitmap.bmWidth, bitmap.bmHeight ) ) {

			delete pDib;

			return 0;

		}

		// okay use GDI to render the bitmap into our dib
		// --------------------------------------------------------------------

		TDibDC< TDIBSection<DWORD> > dc( pDib );

		if ( !dc.IsSetup() ) {

			delete pDib;

			return 0;

		}
			
		CDC memDC;

		if ( FALSE == memDC.CreateCompatibleDC( &dc ) ) {

			delete pDib;

			return 0;

		}

		CBitmap * pOldBitmap = memDC.SelectObject( pBitmap );

		dc.BitBlt( 0, 0, bitmap.bmWidth, bitmap.bmHeight, &memDC, 0, 0, SRCCOPY );

		memDC.SelectObject( pOldBitmap );

		// finally
		// --------------------------------------------------------------------

		return pDib;

	}

	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------

	//
	//	Load8BppPictureFrom_DIBSECTION()
	//

	C8bppPicture * Load8BppPictureFrom_DIBSECTION( HBITMAP hBmp ) {

		// --------------------------------------------------------------------

		bool bMakeRandomPalette = true;

		C8bppPicture * pPicture = 0;

		HDC screenDC = 0;

		int lPitch = 0;

		// --------------------------------------------------------------------

		DIBSECTION dibInfo;

		if ( !GetObject( (HGDIOBJ)hBmp, sizeof(DIBSECTION), (LPVOID)&dibInfo ) ) {

			TRACE( "GetBitmap() method failed?\n" );

			goto FAILURE;

		}

		if ( !((1 == dibInfo.dsBm.bmPlanes) && (8 == dibInfo.dsBm.bmBitsPixel)) ) {

			TRACE( "Bitmap isn't 8bpp color depth\n" );

			goto FAILURE;

		}

		lPitch = 2 * ((dibInfo.dsBm.bmWidth * dibInfo.dsBm.bmBitsPixel + 15) / 16);
//		lPitch = 4 * ((dibInfo.dsBm.bmWidth * dibInfo.dsBm.bmBitsPixel + 31) / 32);

		// --------------------------------------------------------------------

		pPicture = new C8bppPicture;

		if ( !pPicture ) {

			TRACE( "Failed to create C8bppPicture object.\n" );
			goto FAILURE;

		} else {

			SIZE size = { dibInfo.dsBm.bmWidth, dibInfo.dsBm.bmHeight };
	
			if ( !pPicture->Create( size, lPitch ) ) {
	
				TRACE( "Unable to create %dx%d bitmap.\n", size.cx, size.cy );

				goto FAILURE;
	
			}

		}

		// Get the bitmap bits
		// --------------------------------------------------------------------

		GetBitmapBits( hBmp, lPitch * dibInfo.dsBm.bmHeight, (LPVOID)pPicture->Iterator( 0, 0 ) );

		// Get the palette info
		// --------------------------------------------------------------------

		screenDC = GetDC( NULL );

		if ( screenDC ) {

			HDC memDC = CreateCompatibleDC( screenDC );

			if ( memDC ) {

				HGDIOBJ hOld = SelectObject( memDC, (HGDIOBJ) hBmp );

				bMakeRandomPalette = false;

				GetDIBColorTable( memDC, 0, 256, pPicture->m_PaletteData );

				SelectObject( memDC, hOld );
				DeleteObject( memDC );

			}

			ReleaseDC( NULL, screenDC );

		}

		DeleteObject( (HGDIOBJ) hBmp );

		// --------------------------------------------------------------------

		if ( bMakeRandomPalette ) {

			for ( int i = 0; i < C8bppPicture::PALETTE_ENTRIES; i++ ) {

				pPicture->m_PaletteData[ i ].rgbRed = rand() & 255;
				pPicture->m_PaletteData[ i ].rgbGreen = rand() & 255;
				pPicture->m_PaletteData[ i ].rgbBlue = rand() & 255;
				pPicture->m_PaletteData[ i ].rgbReserved = 0;

			}

		}

		// --------------------------------------------------------------------

		return pPicture;

		// --------------------------------------------------------------------

	FAILURE:

		if ( pPicture ) {

			delete pPicture;

		}

		return 0;

	}

	// ------------------------------------------------------------------------

	//
	//	Load8BppPictureFrom_BITMAPINFO()
	//

	C8bppPicture * Load8BppPictureFrom_BITMAPINFO( PBITMAPINFO pBitmapInfo, HPALETTE hPAL ) {

		// future...

		return 0;

	}

	// ------------------------------------------------------------------------

	//
	//	Load8bppPictureFromClipboard()
	//

	C8bppPicture * Load8bppPictureFromClipboard() {

		CWnd * pMainWnd = AfxGetMainWnd();

		if ( !pMainWnd ) return 0;

		if ( FALSE == pMainWnd->OpenClipboard() ) return 0;

		// Try a DIB first
		// --------------------------------------------------------------------

#if 1

		HPALETTE hPAL = (HPALETTE)GetClipboardData( CF_PALETTE );

		HANDLE hDIB = (HBITMAP)GetClipboardData( CF_DIB );

		if ( hDIB ) {

			PBITMAPINFO pBitmapInfo = (PBITMAPINFO)GlobalLock( hDIB );

			if ( pBitmapInfo ) {

				C8bppPicture * pPicture = Load8BppPictureFrom_BITMAPINFO( pBitmapInfo, hPAL );

				GlobalUnlock( hDIB );

				if ( pPicture ) {

					CloseClipboard();

					return pPicture;

				}

			}

		}

#endif

		// Try a simple bitmap (hope it's a DIB)
		// --------------------------------------------------------------------

		HBITMAP hBitmap = (HBITMAP)GetClipboardData( CF_BITMAP );

		if ( hBitmap ) {

			C8bppPicture * pPicture = Load8BppPictureFrom_DIBSECTION( hBitmap );

			if ( pPicture ) {

				CloseClipboard();

				return pPicture;

			}

		}

		// try owner draw?
		// --------------------------------------------------------------------

		// nothing we support :(
		// --------------------------------------------------------------------

		CloseClipboard();

		return 0;

	}

	// ------------------------------------------------------------------------
	// Load Bmp
	// ------------------------------------------------------------------------

	C8bppPicture * Load8BppBMP( const char * filename ) {

		HBITMAP hBmp = (HBITMAP)::LoadImage(
			NULL, filename, IMAGE_BITMAP, 0, 0,
			LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE
		);

		if ( !hBmp ) {

			TRACE( "LoadImage() failed to load bitmap.\n" );

			return 0;

		}

		C8bppPicture * pPicture = Load8BppPictureFrom_DIBSECTION( hBmp );

		DeleteObject( (HGDIOBJ) hBmp );

		return pPicture;

	}

	// ------------------------------------------------------------------------

	//
	//	Load8BppBMPViaCImage()
	//

	C8bppPicture * Load8BppBMPViaCImage( const char * filename ) {

		C8bppPicture * pPicture = 0;

		CImage loadedImage;

		HRESULT hr = loadedImage.Load( filename );

		if ( FAILED( hr ) ) {

			TRACE( "CImage::Load() failed to load bitmap.\n" );

			return 0;

		}

		if ( 8 != loadedImage.GetBPP() ) {

			TRACE( "CImage::Load() invalid bpp (%d bpp).\n", loadedImage.GetBPP() );

			return 0;

		}

		if ( !loadedImage.IsDIBSection() ) {

			TRACE( "Loaded CImage is not a dibsection.\n" );

			return 0;

		}

		// --------------------------------------------------------------------

		int width = loadedImage.GetWidth();

		int height = loadedImage.GetHeight();

		int lPitch = loadedImage.GetPitch();

//		lPitch = 2 * ((dibInfo.dsBm.bmWidth * dibInfo.dsBm.bmBitsPixel + 15) / 16);
//		lPitch = 4 * ((dibInfo.dsBm.bmWidth * dibInfo.dsBm.bmBitsPixel + 31) / 32);

TRACE( "%4dx%-4d (%4d) \"%s\"\n", width, height, lPitch, filename );

		// --------------------------------------------------------------------

		pPicture = new C8bppPicture;

		if ( !pPicture ) {

			TRACE( "Failed to create C8bppPicture object.\n" );
			goto FAILURE;

		} else {

			if ( !pPicture->Create( width, height ) ) {
	
				TRACE( "Unable to create %dx%d bitmap.\n", width, height );

				goto FAILURE;
	
			}

		}

		// Get the bitmap bits
		// --------------------------------------------------------------------

		for ( int y = 0; y < height; y++ ) {

			memcpy(
				(void *)pPicture->Iterator( 0, y ),
				(void *)loadedImage.GetPixelAddress( 0, y ),
				width
			);

		}

		// Get the palette info
		// --------------------------------------------------------------------

		int nMaxColors = loadedImage.GetMaxColorTableEntries();

		if ( 0 < nMaxColors ) {

			loadedImage.GetColorTable( 0, min( C8bppPicture::PALETTE_ENTRIES, nMaxColors ), pPicture->m_PaletteData );

		} else {

			for ( int i = 0; i < C8bppPicture::PALETTE_ENTRIES; i++ ) {

				pPicture->m_PaletteData[ i ].rgbRed = rand() & 255;
				pPicture->m_PaletteData[ i ].rgbGreen = rand() & 255;
				pPicture->m_PaletteData[ i ].rgbBlue = rand() & 255;
				pPicture->m_PaletteData[ i ].rgbReserved = 0;

			}

		}

		return pPicture;

		// --------------------------------------------------------------------

	FAILURE:

		if ( pPicture ) {

			delete pPicture;

		}

		return 0;
	}

	// ------------------------------------------------------------------------
	// General OLE load picture wrapper (JPG, BMP, others?)
	// ------------------------------------------------------------------------

	C8bppPicture * Load8BppPicture( const char * filename ) {

#if 0 // BPT 6/20/01 

		return Load8BppBMP( filename );

#else

		C8bppPicture * pPicture = 0;

		// --------------------------------------------------------------------

		pPicture = Load8BppBMPViaCImage( filename );

		if ( pPicture ) {

			return pPicture;

		}

		// Try the BMP loader if it likes it return the picture otherwise
		// drop down to the OLE picture loader.
		// --------------------------------------------------------------------

		pPicture = Load8BppBMP( filename );

		if ( pPicture ) {

			return pPicture;

		}

		// Ask the helper code to load 
		// --------------------------------------------------------------------

		CIJLBitmap loader;

		if ( !loader.Load( filename ) ) {

			TRACE( "Ole picture load wrapper failed to load.\n" );

			return 0;

		}

		// Check to make sure it's a 8bpp picture
		// --------------------------------------------------------------------

		BITMAP bitmapInfo;

		if ( !loader.GetBitmap( &bitmapInfo ) ) {

			TRACE( "GetBitmap() method failed?\n" );
			return 0;

		}

		if ( !((1 == bitmapInfo.bmPlanes) && (8 == bitmapInfo.bmBitsPixel)) ) {

			TRACE( "Bitmap isn't 8bpp color depth\n" );
			return 0;

		}

		int lPitch = 2 * ((bitmapInfo.bmWidth * bitmapInfo.bmBitsPixel + 15) / 16);

		// --------------------------------------------------------------------

		/* C8bppPicture * */ pPicture = new C8bppPicture;

		if ( !pPicture ) {

			TRACE( "Failed to create C8bppPicture object.\n" );
			return 0;

		}

		SIZE size = loader.Size();

		if ( !pPicture->Create( size, lPitch ) ) {

			delete pPicture;
			TRACE( "Unable to create %dx%d bitmap.\n", size.cx, size.cy );
			return 0;

		}

		// Copy the palette from the loaded bitmap
		// --------------------------------------------------------------------

		CPalette * pPal = &loader.Palette();

		PALETTEENTRY palEntries[ C8bppPicture::PALETTE_ENTRIES ];

		if ( NULL != (HPALETTE)*pPal ) {

			int nColors = max( 0, min( C8bppPicture::PALETTE_ENTRIES, pPal->GetEntryCount() ));
	
			if ( nColors ) {
	
				if ( nColors != pPal->GetPaletteEntries( 0, nColors, palEntries ) ) {
	
					TRACE( "Unable to get palette entries from the CPalette object?\n" );
	
				}
	
			}
			
		} else {

			for ( int i = 0; i < C8bppPicture::PALETTE_ENTRIES; i++ ) {

				palEntries[ i ].peRed = rand() & 255;
				palEntries[ i ].peGreen = rand() & 255;
				palEntries[ i ].peBlue = rand() & 255;
				palEntries[ i ].peFlags = 0;

			}

		}
		
		// Convert over our palette information
		// --------------------------------------------------------------------

		for ( int i = 0; i < C8bppPicture::PALETTE_ENTRIES; i++ ) {

			pPicture->m_PaletteData[ i ].rgbRed = palEntries[ i ].peRed;
			pPicture->m_PaletteData[ i ].rgbGreen = palEntries[ i ].peGreen;
			pPicture->m_PaletteData[ i ].rgbBlue = palEntries[ i ].peBlue;
			pPicture->m_PaletteData[ i ].rgbReserved = 0;

		}

		// Get the pixel data from the loaded bitmap
		// --------------------------------------------------------------------

		loader.GetBitmapBits(
			lPitch * size.cy, (LPVOID)pPicture->Iterator( 0, 0 )
		);

		// Finally return out bitmap
		// --------------------------------------------------------------------

		return pPicture;

#endif

	}

	// ------------------------------------------------------------------------

	//
	//	CAnimationBackdrop::LoadFromFilename()
	//

	bool CAnimationBackdrop::LoadFromFilename( const char * filename, const SIZE & canvasSize )
	{

		// Destroy the existing backdrop if any.
		// --------------------------------------------------------------------

		Destroy();

		// Create and clear the display bitmap
		// --------------------------------------------------------------------

		BPT::TDIBSection<SDisplayPixelType> temporaryDibSection;

		if ( !temporaryDibSection.BPTDIB_DEBUG_CREATE( canvasSize.cx, canvasSize.cy, 0 ) ) {

			TRACE( "Unable to create temporary bitmap %dx%dx%dbpp\n",
				canvasSize.cx, canvasSize.cy, sizeof( SDisplayPixelType ) * 8
			);

			return false;

		}

		BPT::TDIBSection<SDisplayPixelType>::bitmap_type * 
			pTemporaryBitmap = temporaryDibSection.GetBitmapPtr();

		if ( !pTemporaryBitmap ) {

			return false;

		}

#if 0

		pTemporaryBitmap->ClearBuffer( 0 );

#else

		// this needs to be a function!!!
	
		SIZE bk = { 8, 8 };
	
		for ( int bky = 0; bky < canvasSize.cy; bky += bk.cy ) {
	
			int oddCounter = bky / bk.cy;
	
			for ( int bkx = 0; bkx < canvasSize.cx; bkx += bk.cx ) {
	
				RECT rect = { bkx, bky, bkx + bk.cx, bky + bk.cy };
	
				if ( ++oddCounter & 1 ) {
			
					BPT::T_SolidRectPrim(
						*pTemporaryBitmap, rect, 0x739c, 
						BPT::TCopyROP<SDisplayPixelType>()
					);
	
				} else {
	
					BPT::T_SolidRectPrim(
						*pTemporaryBitmap, rect, ~0, BPT::TCopyROP<SDisplayPixelType>()
					);
	
				}
	
			}
	
	  	}

#endif

		// --------------------------------------------------------------------

		C8bppPicture * pPicture = 0;

		// --------------------------------------------------------------------

#if 1

			// Ask the helper code to load 
			// ----------------------------------------------------------------
	
			CIJLBitmap loader;
	
			if ( !loader.Load( filename ) ) {
	
				TRACE( "Ole picture load wrapper failed to load.\n" );
	
				return false;
	
			}

			// Render our loaded image into the dib for later conversion.
			// ----------------------------------------------------------------

			{
				CDC memDC;

				if ( memDC.CreateCompatibleDC(NULL) ) {

					// Select our dibsection as the render target for this dc
					// --------------------------------------------------------

					CBitmap* pOldBitmap = memDC.SelectObject(
						CBitmap::FromHandle( temporaryDibSection.GetHBitmap() )
					);

					// Ask the CIJLBitmap class to render
					// --------------------------------------------------------

					loader.Draw( &memDC );

					// Put the DC back so we can safely destroy our dib
					// --------------------------------------------------------

					if ( pOldBitmap ) {

						memDC.SelectObject( pOldBitmap );

					}

				}

			}

#else

		// --------------------------------------------------------------------

		pPicture = Load8BppPicture( filename );

		if ( !pPicture ) {

			TRACE( "8bpp picture loaded didn't like \"%s\"\n", filename );

			return false;

		}

		// Build the 'clut'
		// --------------------------------------------------------------------

		TPow2Clut<SDisplayPixelType> clut;

		for ( int i = 0; i < C8bppPicture::PALETTE_ENTRIES; i++ ) {

			clut[ i ] = MakeDisplayPixelType(
				pPicture->m_PaletteData[ i ].rgbRed
				,pPicture->m_PaletteData[ i ].rgbGreen
				,pPicture->m_PaletteData[ i ].rgbBlue
			);

		}

		// Blit the loaded picture to the display bitmap
		// --------------------------------------------------------------------

		TClutTransferROP<
			SDisplayPixelType
			,C8bppPicture::pixel_type
			,TPow2Clut<SDisplayPixelType>
		> top( &clut );

		BPT::T_Blit( 
			*pTemporaryBitmap, 0, 0, *pPicture, top
		);

#endif

		// Create the visual element object to store the compressed data
		// --------------------------------------------------------------------

		m_pVisualElement = new CVisualElement( 0 );

		if ( !m_pVisualElement ) {

			if ( pPicture ) delete pPicture;

			return false;

		}

		if ( !m_pVisualElement->CreateFrom( *pTemporaryBitmap, 
			TAlwaysXPredicate<SDisplayPixelType,true>(),0,0) ) {

			delete m_pVisualElement;

			m_pVisualElement = 0;

			if ( pPicture ) delete pPicture;

			return false;

		}

		// --------------------------------------------------------------------

		if ( pPicture ) delete pPicture;

		return true;
	}

}; // namespace BPT

