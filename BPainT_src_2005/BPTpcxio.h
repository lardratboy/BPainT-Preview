// BPTpcxio.h: interface for the BPTpcxio class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTPCXIO_H__F8C7E227_79F3_43DC_9AA5_4DDFF745C5C7__INCLUDED_)
#define AFX_BPTPCXIO_H__F8C7E227_79F3_43DC_9AA5_4DDFF745C5C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------------

#include "BPTFileio.h"
#include "BPTBitmap.h"

// ----------------------------------------------------------------------------

namespace BPT {

	// ------------------------------------------------------------------------

	C8bppPicture * Load8BppPictureFrom_DIBSECTION( HBITMAP hBmp );
	C8bppPicture * Load8BppPicture( const char * filename );
	C8bppPicture * Load8bppPictureFromClipboard();

	// ------------------------------------------------------------------------

	//
	//	T_SaveSurfaceAsBMP()
	//
	//	-- This template probably needs some work to be correct for non 8bpp 
	//	-- surfaces, once again schedule pressures come into play.
	//

	template< class SURFACE >
	bool T_SaveSurfaceAsBMP(
		const char * filename, SURFACE & surface, 
		const RGBQUAD * pPaletteData = 0, 
		const int nPaletteEntries = 0
	) {

		// --------------------------------------------------------------------

		CFileIO m_IO;

		if ( !m_IO.Open( filename, "wb" ) ) {

			return false;

		}

		// --------------------------------------------------------------------

		int paletteDataSize = (nPaletteEntries * sizeof(RGBQUAD));

		// Deal with the alignment issues...
		// --------------------------------------------------------------------

		int rawPixelBytes = sizeof(SURFACE::pixel_type) * surface.Width();

		int filePitch = ((rawPixelBytes + 3) / 4) * 4;

		int imageDataSize = filePitch * surface.Height();

		// Fill in/Write the bitmap header
		// --------------------------------------------------------------------

		BITMAPFILEHEADER hdr;
	
		hdr.bfType = 0x4D42;

		hdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 
			paletteDataSize + imageDataSize; 

		hdr.bfReserved1 = 0;
		hdr.bfReserved2 = 0;

		hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteDataSize;

		m_IO.WriteNBytes( (void *)&hdr, sizeof(hdr) );

		// Fill in/write the bitmap info header
		// --------------------------------------------------------------------

		BITMAPINFOHEADER bminfoheader;

		bminfoheader.biSize = sizeof( BITMAPINFOHEADER );
		bminfoheader.biWidth = surface.Width();
		bminfoheader.biHeight = surface.Height();
		bminfoheader.biPlanes = 1;
		bminfoheader.biBitCount = sizeof(SURFACE::pixel_type) * 8;
		bminfoheader.biCompression = BI_RGB;
		bminfoheader.biSizeImage = 0;
		bminfoheader.biXPelsPerMeter = 0;
		bminfoheader.biYPelsPerMeter = 0;
		bminfoheader.biClrUsed = nPaletteEntries;
		bminfoheader.biClrImportant = 0;

		m_IO.WriteNBytes( (void *)&bminfoheader, sizeof(bminfoheader) );

		// Write the color data
		// --------------------------------------------------------------------

		if ( paletteDataSize ) {

			m_IO.WriteNBytes( pPaletteData, paletteDataSize );

		}

		// Write the image data
		// --------------------------------------------------------------------

		int yCount = surface.Height();

		int yRead = yCount - 1;

		for ( int y = 0; y < yCount; y++ ) {

			// Write the actual pixel data
			// ----------------------------------------------------------------

			SURFACE::pixel_iterator it = surface.Iterator( 0, yRead );

			--yRead;

			m_IO.WriteNBytes( (void *)&(*it), rawPixelBytes );

			// Handle any alignment padding
			// ----------------------------------------------------------------

			for ( int pad = rawPixelBytes; pad < filePitch; pad++ ) {

				m_IO.WriteByte( 0 );

			}

		}

		m_IO.Close();

		return true;

	}

	// ------------------------------------------------------------------------

}; // namespace BPT

#endif // !defined(AFX_BPTPCXIO_H__F8C7E227_79F3_43DC_9AA5_4DDFF745C5C7__INCLUDED_)
