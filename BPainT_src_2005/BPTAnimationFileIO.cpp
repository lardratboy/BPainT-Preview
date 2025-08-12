// BPTAnimationFileIO.cpp: implementation of the BPTAnimationFileIO class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	This file was a huge rush job, so it has lots of places where the
//	process of loading / writing could be distilled into primitives
//	which would make the code easier to understand and in general better!
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bpaint.h"
#include "BPTAnimationFileIO.h"
#include "BPTBitmap.h"
#include "BPTDib.h"
#include "IJLBitmap.H"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// ----------------------------------------------------------------------------

namespace BPT {

	// ========================================================================

	char * HELPER_BlockText( CShowcaseFileIO::id_type id ) {

		static char text[ 5 ];

		text[ 3 ] = (char)((id >> 24) & 0xff);
		text[ 2 ] = (char)((id >> 16) & 0xff);
		text[ 1 ] = (char)((id >>  8) & 0xff);
		text[ 0 ] = (char)((id >>  0) & 0xff);
		text[ 4 ] = '\0';

		return text;

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Annotated palette saver
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	HELPER_PaletteFromFile()
	//	

	bool HELPER_PaletteFromFile(
		CAnnotatedPalette * pPal, const char * pFilename, CShowcaseFileIO * io
	) {

		// Ask the picture loader to do it's job
		// --------------------------------------------------------------------

		BPT::C8bppPicture * p8bppPicture = BPT::Load8BppPicture( pFilename );

		if ( !p8bppPicture ) {

			return false;

		}

		// Get our palette data
		// --------------------------------------------------------------------

		int nEntries = min( BPT::C8bppPicture::PALETTE_ENTRIES, pPal->TotalColors() );

		for ( int i = 0; i < nEntries; i++ ) {

			pPal->SetSlot(
				i
				,p8bppPicture->m_PaletteData[ i ].rgbRed
				,p8bppPicture->m_PaletteData[ i ].rgbGreen
				,p8bppPicture->m_PaletteData[ i ].rgbBlue
			);

		}

		for ( ; i < pPal->TotalColors(); i++ ) {

			pPal->SetSlot( i, 255, 255, 255 );

		}

		// Clean up and begone
		// --------------------------------------------------------------------

		delete p8bppPicture;

		return true;

	}

	//
	//	HELPER_PaletteToFile()
	//	

	bool HELPER_PaletteToFile(
		CAnnotatedPalette * pPal, const char * pFilename, CShowcaseFileIO * io
	) {

		// Determine the output canvas size & offset if needed...
		// ----------------------------------------------------------------

		RGBQUAD temporaryPalette[ BPT::C8bppPicture::PALETTE_ENTRIES ];

		int nPaletteEntries = min( BPT::C8bppPicture::PALETTE_ENTRIES, pPal->TotalColors() );

		// Copy the palette data over
		// ----------------------------------------------------------------

		for ( int i = 0; i < nPaletteEntries; i++ ) {

			temporaryPalette[ i ].rgbRed = pPal->GetSlot_R( i );
			temporaryPalette[ i ].rgbGreen = pPal->GetSlot_G( i );
			temporaryPalette[ i ].rgbBlue = pPal->GetSlot_B( i );
			temporaryPalette[ i ].rgbReserved = 0;

		}

		for ( ; i < BPT::C8bppPicture::PALETTE_ENTRIES; i++ ) {

			temporaryPalette[ i ].rgbRed = 255;
			temporaryPalette[ i ].rgbGreen = 255;
			temporaryPalette[ i ].rgbBlue = 255;
			temporaryPalette[ i ].rgbReserved = 0;

		}

		// Create the temporary canvase (1x1)
		// ----------------------------------------------------------------

		SIZE canvasSize = { 1, 1 };

		BPT::C8bppPicture temporaryPicture;

		if ( !temporaryPicture.Create( canvasSize ) ) {

			TRACE( 
				"Unable to create temp bitmap %dx%dx%dbpp\n", 
				canvasSize.cx, canvasSize.cy, ( sizeof( BPT::C8bppPicture::pixel_type ) * 8 )
			);

			return false;

		}

		// Ask the bitmap saver to do it's job and be done with it
		// ----------------------------------------------------------------

		return BPT::T_SaveSurfaceAsBMP(
			pFilename, temporaryPicture, temporaryPalette, nPaletteEntries
		);

	}
	
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Visual Element IMPORT/EXPORT helpers...
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	HELPER_VisualElementFromFile()
	//

	bool HELPER_VisualElementFromFile( 
		CVisualElement * pVisualElement, const char * pFilename, CShowcaseFileIO * io
	) {

		BPT::C8bppPicture * p8bppPicture = BPT::Load8BppPicture( pFilename );

		// If the 8BPP loader didn't like it try it as a high color image.
		// ----------------------------------------------------------------

		if ( !p8bppPicture ) {

			// Ask the helper code to load 
			// ----------------------------------------------------------------
	
			CIJLBitmap loader;
	
			if ( !loader.Load( pFilename ) ) {
	
				TRACE( "Ole picture load wrapper failed to load \"%s\".\n", pFilename );
	
				return false;
	
			}

			// Determine the canvas size and make sure the loaded bitmap
			// is large enough
			// ----------------------------------------------------------------

			SIZE canvasSize = loader.Size();

			bool bClearCanvas = false;

			CAnimationShowcase * pShowcase = io->CurrentShowcase();

			if ( pShowcase ) {

				SIZE showcaseCanvas = pShowcase->CanvasSize();

				if ( (canvasSize.cx < showcaseCanvas.cx) ||
					(canvasSize.cy < showcaseCanvas.cy) ) {

					bClearCanvas = true;

				}

			}

			// Create our dib
			// ----------------------------------------------------------------

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

			if ( bClearCanvas ) {

				pTemporaryBitmap->ClearBuffer( 0 );

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

			// Try to create the compressed image from the loaded file
			// ----------------------------------------------------------------

			return pVisualElement->CreateFrom( *pTemporaryBitmap, 
				BPT::TAlwaysXPredicate<SDisplayPixelType,true>(),0,0
			);

		}

		// Okay process an 8bpp image...
		// ----------------------------------------------------------------

		BPT::C8bppPicture::pixel_type chromaKey = 0; // where does this come from????

		BPT::TIsNotValue<BPT::C8bppPicture::pixel_type> chromaKeyPredicate( chromaKey );

		// Finally create a visual element
		// ----------------------------------------------------------------

		CRect captureRect;
		
		BPT::T_FindBoundingRect(
			captureRect, *p8bppPicture, chromaKeyPredicate, 0
		);
		
		if ( captureRect.IsRectEmpty() ) {

			captureRect.SetRect( 0, 0, 1, 1 );

		}

		// Okay lets have some fun!
		// ----------------------------------------------------------------

		bool bResult = pVisualElement->CreateFrom( 
			*p8bppPicture, chromaKeyPredicate, &captureRect, 0
		);

		// Blindly take the palette from this file (should ask the user...)
		// ----------------------------------------------------------------

		CAnimationShowcase * pShowcase = io->CurrentShowcase();

		if ( pShowcase ) {

#if 0 // BPT 6/15/01 (delt with differently now)

			BPT::CAnnotatedPalette * pPal = pShowcase->GetDefaultPalette();

			if ( pPal ) {

				int nEntries = min( BPT::C8bppPicture::PALETTE_ENTRIES, pPal->TotalColors() );

				for ( int i = 0; i < nEntries; i++ ) {

					pPal->SetSlot(
						i
						,p8bppPicture->m_PaletteData[ i ].rgbRed
						,p8bppPicture->m_PaletteData[ i ].rgbGreen
						,p8bppPicture->m_PaletteData[ i ].rgbBlue
					);

				}

			}

#endif // BPT 6/15/01

		}

		// Clean up & return where we came from
		// ----------------------------------------------------------------

		delete p8bppPicture;

		return bResult;

	}

	//
	//	THELPER_XVisualeElementToFile<>()
	//

	template< class T >
	bool THELPER_XVisualeElementToFile(
		T & temporaryPicture,
		CVisualElement * pVisualElement, 
		const char * pFilename, 
		CShowcaseFileIO * io,
		const bool bSavePalette = false,
		const typename T::pixel_type chromaKey = static_cast<T::pixel_type>( 0 )
	) {

		// ----------------------------------------------------------------

		SIZE canvasSize;

		POINT outputOffset = { 0, 0 };

		// Determine the output canvas size & offset if needed...
		// ----------------------------------------------------------------

		CAnimationShowcase * pShowcase = io->CurrentShowcase();

		RGBQUAD temporaryPalette[ BPT::C8bppPicture::PALETTE_ENTRIES ];

		RGBQUAD * pUsePalette = 0;

		int nPaletteEntries = 0;

		// ----------------------------------------------------------------

		if ( pShowcase ) {

			canvasSize = pShowcase->CanvasSize();

			// Copy the palette data over
			// ----------------------------------------------------------------

			BPT::CAnnotatedPalette * pPal = io->CurrentPalette();

			if ( pPal && bSavePalette ) {

				pUsePalette = temporaryPalette;

				nPaletteEntries = min( BPT::C8bppPicture::PALETTE_ENTRIES, pPal->TotalColors() );

				for ( int i = 0; i < nPaletteEntries; i++ ) {

					temporaryPalette[ i ].rgbRed = pPal->GetSlot_R( i );
					temporaryPalette[ i ].rgbGreen = pPal->GetSlot_G( i );
					temporaryPalette[ i ].rgbBlue = pPal->GetSlot_B( i );
					temporaryPalette[ i ].rgbReserved = 0;

				}

				for ( ; i < BPT::C8bppPicture::PALETTE_ENTRIES; i++ ) {
		
					temporaryPalette[ i ].rgbRed = 255;
					temporaryPalette[ i ].rgbGreen = 255;
					temporaryPalette[ i ].rgbBlue = 255;
					temporaryPalette[ i ].rgbReserved = 0;
		
				}

			}

		} else {

			canvasSize = pVisualElement->Size();

			RECT captureRect = pVisualElement->BoundingRect();

			outputOffset.x = -captureRect.left;
			outputOffset.y = -captureRect.top;

			// Build a simple palette since we couldn't determine the showcase?
			// ----------------------------------------------------------------

			if ( bSavePalette ) {

				pUsePalette = temporaryPalette;
	
				nPaletteEntries = BPT::C8bppPicture::PALETTE_ENTRIES;
	
				for ( int i = 0; i < nPaletteEntries; i++ ) {
	
					temporaryPalette[ i ].rgbRed = i;
					temporaryPalette[ i ].rgbGreen = i;
					temporaryPalette[ i ].rgbBlue = i;
					temporaryPalette[ i ].rgbReserved = 0;
	
				}

			}
	
		}

		// Finally let's generate the output bitmap
		// ----------------------------------------------------------------

		if ( !temporaryPicture.Create( canvasSize ) ) {

			TRACE( 
				"Unable to create temp bitmap %dx%dx%dbpp\n", 
				canvasSize.cx, canvasSize.cy, ( sizeof( typename T::pixel_type ) * 8 )
			);

			return false;

		}

		temporaryPicture.ClearBuffer( chromaKey );

		// Render the visual element image 
		// ----------------------------------------------------------------

		typedef BPT::TCopyROP<
			typename T::pixel_type, 
			BPT::CVisualElement::storage_type
		> visual_to_X_copy_type;

		pVisualElement->Render(
			temporaryPicture, outputOffset.x, outputOffset.y,
			visual_to_X_copy_type()
		);

		// Ask the bitmap saver to do it's job and be done with it
		// ----------------------------------------------------------------

		return BPT::T_SaveSurfaceAsBMP<T>(
			pFilename, temporaryPicture, pUsePalette, nPaletteEntries
		);

	}

	//
	//	HELPER_VisualElementToFile()
	//

	bool HELPER_VisualElementToFile( 
		CVisualElement * pVisualElement, const char * pFilename, CShowcaseFileIO * io
	) {

// need to determine chromakey value?
// need to determine chromakey value?
// need to determine chromakey value?

		// Save out a high color depth image.
		// ----------------------------------------------------------------

		if ( 4 == pVisualElement->GetStoredPixelSize() ) {

			BPT::TSimpleBitmap<BPT::PIXEL_RGB32> temporaryPicture;

			return THELPER_XVisualeElementToFile(
				temporaryPicture, pVisualElement, pFilename, io, false
			);

		} else if ( 2 == pVisualElement->GetStoredPixelSize() ) {

			BPT::TSimpleBitmap<BPT::PIXEL_RGB16> temporaryPicture;

			return THELPER_XVisualeElementToFile(
				temporaryPicture, pVisualElement, pFilename, io, false
			);

		} else if ( 1 == pVisualElement->GetStoredPixelSize() ) {

			BPT::TSimpleBitmap<BPT::PIXEL_8> temporaryPicture;

			return THELPER_XVisualeElementToFile(
				temporaryPicture, pVisualElement, pFilename, io, true
			);

		}

		TRACE(
			"Unsupported bitmap depth %dbpp for visual element\n",
			pVisualElement->GetStoredPixelSize()
		);

		// bad

		return false;
		
	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Load/Save helpers
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	T_BlockSaveTemplate
	//

	template< class T > bool T_BlockSaveTemplate( 
		T iFirst, T iLast, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO;

		if ( !(pIO = io->BeginBlock( id )) ) {

			return false;

		}

		for ( ; iFirst != iLast; iFirst++ ) {
	
			if ( !(*iFirst)->SaveTo( io ) ) {
	
				return false;
	
			}
	
		}

		if ( !io->FinishBlock( id ) ) {

			return false;

		}

		return true;

	}

	// ------------------------------------------------------------------------

	//
	//	T_SaveCollectionAsBlock()
	//

	template< class T > bool T_SaveCollectionAsBlock(
		T & collection, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO;

		if ( !(pIO = io->BeginBlock( id )) ) {

			return false;

		}

		// write out the size followed by the data

		pIO->Write_32( collection.size() );

		// write each element in the collection

		T::iterator iFirst = collection.begin();
		T::iterator iLast = collection.end();

		for ( ; iFirst != iLast; iFirst++ ) {

			pIO->T_Write( *iFirst );
	
		}

		if ( !io->FinishBlock( id ) ) {

			return false;

		}

		return true;

	}

	//
	//	T_LoadCollectionFromBlock()
	//

	template< class T > bool T_LoadCollectionFromBlock(
		T & collection, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO = io->BeginChunkDataRead();

		if ( !pIO ) {

			return false;

		}

		// read the element count
		// --------------------------------------------------------------------

		int count = pIO->Read_32();

		// Attempt to load each value (this could be done MUCH faster!)
		// --------------------------------------------------------------------

#if 1 

		int dataSize = sizeof(T::value_type) * count;

		if ( dataSize ) {

			BYTE * ptr = pIO->LoadData( dataSize );

			if ( ptr ) {

				const T::value_type * pData = (const T::value_type *)ptr;

				collection.insert( collection.end(), pData, pData + count );

				pIO->UnloadData( ptr );

			} else {

				for ( int i = 0; i < count; i++ ) {
		
					T::value_type value;
		
					pIO->T_Read( value );
		
					collection.push_back( value );
		
				}

			}

		}

#else

		for ( int i = 0; i < count; i++ ) {

			T::value_type value;

			pIO->T_Read( value );

			collection.push_back( value );

		}

#endif

		return true;

	}

	// ------------------------------------------------------------------------

	//
	//	HELPER_SaveToBlock()
	//

	bool HELPER_SaveToBlock(
		const char * pzStr, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO;

		if ( !(pIO = io->BeginBlock( id )) ) {

			return false;

		}

		// Write the data

		pIO->WriteString( pzStr );

		// finish the block

		if ( !io->FinishBlock( id ) ) {

			return false;

		}

		return true;
	}

	// ------------------------------------------------------------------------

	//
	//	HELPER_SaveToBlock()
	//

	bool HELPER_SaveToBlock(
		const int value, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO;

		if ( !(pIO = io->BeginBlock( id )) ) {

			return false;

		}

		// Write the data

		pIO->Write_32( value );

		// finish the block

		if ( !io->FinishBlock( id ) ) {

			return false;

		}

		return true;
	}

	//
	//	HELPER_LoadFromBlock()
	//

	bool HELPER_LoadFromBlock(
		int & value, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO = io->BeginChunkDataRead();

		if ( !pIO ) {

			return false;

		}

		// read the data

		value = pIO->Read_32();

		TRACE( "INT: (%d)\n", value);

		return true;
	}

	// ------------------------------------------------------------------------

	//
	//	HELPER_SaveToBlock()
	//

	bool HELPER_SaveToBlock(
		const DWORD value, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO;

		if ( !(pIO = io->BeginBlock( id )) ) {

			return false;

		}

		// Write the data

		pIO->Write_32( value );

		// finish the block

		if ( !io->FinishBlock( id ) ) {

			return false;

		}

		return true;
	}

	//
	//	HELPER_LoadFromBlock()
	//

	bool HELPER_LoadFromBlock(
		DWORD & value, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO = io->BeginChunkDataRead();

		if ( !pIO ) {

			return false;

		}

		// read the data

		value = pIO->Read_32();

		TRACE( "DWORD: (0x%08x == %d)\n", value, value );

		return true;
	}

	// ------------------------------------------------------------------------

	//
	//	HELPER_SaveToBlock()
	//

	bool HELPER_SaveToBlock(
		const POINT & pt, CShowcaseFileIO * io, CShowcaseFileIO::id_type id,
		const bool bDontSaveEmptyPoint = true
	) {

#if 1 // BPT 6/6/01

		if ( bDontSaveEmptyPoint ) {

			if ( (0 == pt.x) && (0 == pt.y) ) {

				return true;

			}

		}

#endif

		BPT::CFileIO * pIO;

		if ( !(pIO = io->BeginBlock( id )) ) {

			return false;

		}

		// Write the data

		pIO->Write_32( pt.x );
		pIO->Write_32( pt.y );

		// finish the block

		if ( !io->FinishBlock( id ) ) {

			return false;

		}

		return true;
	}

	//
	//	HELPER_LoadFromBlock()
	//

	bool HELPER_LoadFromBlock(
		POINT & pt, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO = io->BeginChunkDataRead();

		if ( !pIO ) {

			return false;

		}

		// read the data

		pt.x = pIO->Read_32();
		pt.y = pIO->Read_32();

		TRACE( "POINT: (%d, %d)\n", pt.x, pt.y );

		return true;
	}

	// ------------------------------------------------------------------------

	//
	//	HELPER_SaveToBlock()
	//

	bool HELPER_SaveToBlock(
		const SIZE & size, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO;

		if ( !(pIO = io->BeginBlock( id )) ) {

			return false;

		}

		// Write the data

		pIO->Write_32( size.cx );
		pIO->Write_32( size.cy );

		// finish the block

		if ( !io->FinishBlock( id ) ) {

			return false;

		}

		return true;
	}

	//
	//	HELPER_LoadFromBlock()
	//

	bool HELPER_LoadFromBlock(
		SIZE & size, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO = io->BeginChunkDataRead();

		if ( !pIO ) {

			return false;

		}

		// read the data

		size.cx = pIO->Read_32();
		size.cy = pIO->Read_32();

		TRACE( "SIZE: (%d, %d)\n", size.cx, size.cy );

		return true;
	}

	// ------------------------------------------------------------------------

	//
	//	HELPER_SaveToBlock()
	//

	bool HELPER_SaveToBlock(
		const RECT & rect, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO;

		if ( !(pIO = io->BeginBlock( id )) ) {

			return false;

		}

		// Write the data

		pIO->Write_32( rect.left );
		pIO->Write_32( rect.top );
		pIO->Write_32( rect.right );
		pIO->Write_32( rect.bottom );

		// finish the block

		if ( !io->FinishBlock( id ) ) {

			return false;

		}

		return true;
	}

	//
	//	HELPER_LoadFromBlock()
	//

	bool HELPER_LoadFromBlock(
		RECT & rect, CShowcaseFileIO * io, CShowcaseFileIO::id_type id
	) {

		BPT::CFileIO * pIO = io->BeginChunkDataRead();

		if ( !pIO ) {

			return false;

		}

		// read the data

		rect.left = pIO->Read_32();
		rect.top = pIO->Read_32();
		rect.right = pIO->Read_32();
		rect.bottom = pIO->Read_32();

		TRACE( "RECT: (%d, %d, %d, %d)\n", rect.left, rect.top, rect.right, rect.bottom );

		return true;
	}

	// ========================================================================

	bool HELPER_LoadCStringFromBlocK( CString & str, CShowcaseFileIO * io ) {

		BPT::CFileIO * pIO = io->BeginChunkDataRead();

		if ( !pIO ) {

			return false;

		}

		char * ptr = pIO->ReadString();

		if ( !ptr ) {

			return false;

		}

		str = ptr;

		pIO->UnloadData( (BYTE *)ptr );

		return true;

	}

	// ========================================================================

	//
	//	HELPER_SaveNestedPalette()
	//

	bool HELPER_SaveNestedPalette( CAnnotatedPalette * pPalette, CShowcaseFileIO * io ) {

		// save the palette reference id for moderen readers

		CShowcaseFileIO::io_internal_id_type id = io->SaveIDForPointer( pPalette );
		
		if ( !HELPER_SaveToBlock( id, io, ID_aPLI ) ) {
		
			return false;
		
		}

		// Save the whole palette to maintain backwards compatibility with old file readers

		if ( !pPalette->SaveTo( io ) ) {

			return false;

		}

		return true;

	}

	//
	//	HELPER_LoadNestedPalette()
	//

	bool HELPER_LoadNestedPalette( CAnnotatedPalette ** ppPalette, CShowcaseFileIO * io ) {

		if ( !ppPalette ) {

			return false;

		}

		// if there is already a palette in this pointer bail!

		if ( 0 != (*ppPalette) ) {

			return true;

		}

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		CAnimationShowcase * pShowcase = io->CurrentShowcase();

		if ( !pShowcase ) {

			TRACE( "Unable to determine showcase?\n" );

			return false;

		}

		*ppPalette = pShowcase->CreatePalette();

		if ( !(*ppPalette) ) {

			return false;

		}

#else

		*ppPalette = new CAnnotatedPalette;

		if ( !*ppPalette ) {

			return false;

		}

#endif

		// finally just load the data!

		return (*ppPalette)->LoadFrom( io );

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	Load .BPT file
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	bool LoadBPTFilePrim( CShowcaseFileIO * io, CAnimationShowcase ** out ) {

		*out = 0;

		// ------------------------------------------------------------------------
	
		CAnimationShowcase * pNew = new BPT::CAnimationShowcase();
	
		if ( !pNew ) {
	
			TRACE( "Unable to create animation showcase.\n" );
	
			return false;
	
		}

		if ( pNew->LoadFrom( io ) ) {

			*out = pNew;

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

			pNew->OptimizeVisualElements();

#endif // BPT 5/22/01

			return true;

		}

		delete pNew;

		return false;

	}
	
	bool LoadBPTFile( const char * filename, CAnimationShowcase ** out ) {

#if 0

CTime inTime( CTime::GetCurrentTime() );

#endif

		CShowcaseFileIO io;

		// Determine read mode XML+TEXT or BINARY (could be smarter!)
		// --------------------------------------------------------------------

		FILE * inFile = fopen( filename, "rt" );

		bool bXMLIO = false;

		char * pAccess = "rb";

		if ( NULL == inFile ) {

			return false;

		}

		if ( '<' == fgetc( inFile ) ) {

			if ( isalnum( fgetc( inFile ) ) ) {

				bXMLIO = true;

				pAccess = "rt";

			}

		}

		fclose( inFile );

		// --------------------------------------------------------------------

		if ( !io.Open( filename, pAccess, true, bXMLIO ) ) {

			return false;

		}

		bool rValue = LoadBPTFilePrim( &io, out );


#if 0

CTime outTime( CTime::GetCurrentTime() );

CString text;

CTimeSpan  diffTime = inTime - outTime;

text.Format( 
	"Operation took %d seconds", 
	diffTime.GetTotalSeconds()
);

AfxMessageBox( text, MB_OK );

#endif

		return rValue;

	}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

	// ========================================================================
	// ========================================================================
	// ========================================================================

	bool CSimpleName::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CSimpleName(%p)::LoadFrom\n", this );

		BPT::CFileIO * pIO = io->BeginChunkDataRead();

		if ( !pIO ) {

			return false;

		}

		BYTE * ptr = reinterpret_cast<BYTE *>( pIO->ReadString() );

		if ( !ptr ) {

			return false;

		}

		SetName( (const char *)ptr );

		pIO->UnloadData( ptr );

		return true;

	}

	// ========================================================================

	bool CAnnotatedObject::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CAnnotatedObject(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_nTXT != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_nTXT, HELPER_BlockText( ID_nTXT )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_TEXT:
				if ( !HELPER_LoadCStringFromBlocK( m_Str, io ) ) {
					return false;
				}
				break;

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	bool CAnimationBackdrop::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CAnimationBackdrop(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_aBKD != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_aBKD, HELPER_BlockText( ID_aBKD )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_aVEL:

				Destroy();

				m_pVisualElement = new CVisualElement( 0 );

				if ( !m_pVisualElement ) {

					return false;

				}

				if ( !m_pVisualElement->LoadFrom( io ) ) {

					delete m_pVisualElement;

					return false;

				}
				break;

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	bool CAnnotatedPalette::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CAnnotatedPalette(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_nPAL != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_nPAL, HELPER_BlockText( ID_nPAL )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

			case ID_NAME:
				if ( !m_Name.LoadFrom( io ) ) {
					return false;
				}
				break;

			case ID_aPLI: {

					CShowcaseFileIO::io_internal_id_type id;

					if ( !HELPER_LoadFromBlock( id, io, ID_aPLI ) ) {

						return false;

					}

					if ( !io->AssociatePointerWithID( (void *) this, id ) ) {

						TRACE( "Associate pointer %p to id 0x%08x failed?\n", (void *)this, id );

						return false;

					}

				}
				break;

#endif	// BPT 6/5/02

			case ID_RGBs: {
					BPT::CFileIO * pIO = io->BeginChunkDataRead();

					if ( !pIO ) {

						return false;

					}

					for ( int i = 0; i < TotalColors(); i++ ) {

						m_Colors[ i ] = (COLORREF)(pIO->Read_32());

					}

					RebuildDisplayCLUT(); // BPT 6/15/01

				}
				break;

			case ID_PFGs: {

					BPT::CFileIO * pIO = io->BeginChunkDataRead();

					if ( !pIO ) {

						return false;

					}

					for ( int i = 0; i < TotalColors(); i++ ) {

						m_Flags[ i ] = (DWORD)(pIO->Read_32());

					}

				}
				break;

			case ID_FILE: { // BPT 6/20/01

					CString filename;

					if ( !HELPER_LoadCStringFromBlocK( filename, io ) ) {

						return false;

					}

					if ( !HELPER_PaletteFromFile( this, filename, io ) ) {

						return false;

					}

				}
				break;

			case ID_SNTs: {

					CShowcaseFileIO::chunk_reference slotNotesChunk = io->GetNestedChunkReference( &innerChunk );

					int currentSlot = 0;

					while ( io->IsChunkReferenceIDValid( &slotNotesChunk ) ) {

						TRACE( "\t(2) BLOCK 0x%08x [%s] size %d offset %d\n", slotNotesChunk.id, HELPER_BlockText( slotNotesChunk.id ), slotNotesChunk.size, slotNotesChunk.fileOffset );

						if ( ID_TEXT == slotNotesChunk.id ) {

							if ( !HELPER_LoadCStringFromBlocK( m_Notes[ currentSlot ], io ) ) {

								return false;

							}

							if ( 0 == m_Notes[ currentSlot ].GetLength() ) {

								m_Notes[ currentSlot ].Empty();

							}

							if ( TotalColors() == ++currentSlot ) {

								break;

							}

						} else /* other data chunks */ {

							TRACE( "\t(2) Skipping unknown block (0x%08x [%s])\n", slotNotesChunk.id, HELPER_BlockText( slotNotesChunk.id ) );

						}

						// Move on to the next chunk or end the load loop
						// ----------------------------------------------------
			
						if ( !io->SkipChunk( &slotNotesChunk ) ) {
			
							TRACE( "End 2\n" );

							break;
			
						}

						slotNotesChunk = io->GetCurrentChunkReference( slotNotesChunk.pOutter );

					}

				}
				break;

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	//
	//	TSRLCompressedImage::LoadFrom()
	//
	//	-- I'm actually amazed that VC was able to compile this code :)
	//
	//	THIS BLOCK READ CODE REQUIRES THE SUB CHUNKS TO BE IN A SPECIFIC ORDER
	//	[ID_aSCD] must come first then [ID_aSVD] otherwise it will be broken!
	//
	//

	bool TSRLCompressedImage<CVisualElement::compressor_type>::LoadFrom(
		CShowcaseFileIO * io ) {

		TRACE( "TSRLCompressedImage(%p)::LoadFrom\n", this );

		// Clear out the internal state of this compressed image
		// --------------------------------------------------------------------

		Destroy();

		// determine the block id
		// --------------------------------------------------------------------

		int ia = '0' + ((sizeof( storage_type ) * 8) / 10);
		int ib = '0' + ((sizeof( storage_type ) * 8) % 10);

		CShowcaseFileIO::id_type blockID = MAKE_CHUNK_ID( 'a', 'S', ia, ib );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (blockID != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				blockID, HELPER_BlockText( blockID )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_WWHH:
				if ( !HELPER_LoadFromBlock( m_Size, io, ID_WWHH ) ) {
					return false;
				}
				break;

#if 1 // BPT 5/22/01

			case ID_ESIZ:
				if ( !HELPER_LoadFromBlock( m_CompressedSizeEstimate, io, ID_ESIZ ) ) {
					return false;
				}
				break;

#endif // BPT 5/22/01

			case ID_nSXD: {

					// Check to see if we had a size before we got here!
					// --------------------------------------------------------

					if ( 0 >= m_Size.cy ) {

						return false;

					}

					// Create the info block
					// --------------------------------------------------------

#if defined( SRL_USE_STD_COLLECTION )

					m_CompressedInfo.reserve( m_Size.cy );

					for ( int y = 0; y < m_Size.cy; y++ ) {

						m_CompressedInfo[ y ] = new srl_type::INFO();
		
						if ( !m_CompressedInfo[ y ] ) {
		
							Destroy();
		
							return false;
		
						}

					}

#else

					m_CompressedInfo = new srl_type::INFO [ m_Size.cy ];
		
					if ( !m_CompressedInfo ) {
		
						return false;
		
					}

#endif

					// Finally get down to business and load something!
					// --------------------------------------------------------

					CShowcaseFileIO::chunk_reference lineChunk = io->GetNestedChunkReference( &innerChunk );
	
					int lineCounter = 0;

					while ( io->IsChunkReferenceIDValid( &lineChunk ) ) {
	
						TRACE( "\t(2) BLOCK 0x%08x [%s] size %d offset %d\n", lineChunk.id, HELPER_BlockText( lineChunk.id ), lineChunk.size, lineChunk.fileOffset );

						bool bBailOut = false;
	
						// Get the scan line object
						// ----------------------------------------------------

#if defined( SRL_USE_STD_COLLECTION )

						srl_type::INFO * pInfo = m_CompressedInfo[ lineCounter ];

#else

						srl_type::INFO * pInfo = &m_CompressedInfo[ lineCounter ];

#endif

						// Parse the sub chunks
						// ----------------------------------------------------

						switch ( lineChunk.id ) {
	
						default:
							TRACE( "\t(2) Skipping unknown block (0x%08x [%s])\n", lineChunk.id, HELPER_BlockText( lineChunk.id ) );
							break;
	
						case ID_aSCD:
							if ( !T_LoadCollectionFromBlock( pInfo->first, io, ID_aSCD ) ) {
								return false;
							}
							break;
							
						case ID_aSVD:
							if ( !T_LoadCollectionFromBlock( pInfo->second, io, ID_aSVD ) ) {
								return false;
							}
							if ( lineCounter < m_Size.cy ) {

								++lineCounter;

							} else {

								bBailOut = true;

							}
							break;
	
						}
	
						// Move on to the next chunk or end the load loop
						// ----------------------------------------------------
				
						if ( !io->SkipChunk( &lineChunk ) ) {
			
							TRACE( "End 2\n" );
	
							break;
				
						}

						if ( bBailOut ) {

							break;

						}
	
						lineChunk = io->GetCurrentChunkReference( lineChunk.pOutter );
	
					}
	
				}
				break;

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	bool CVisualElement::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CVisualElement(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_aVEL != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_aVEL, HELPER_BlockText( ID_aVEL )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_DPTH:
				if ( !HELPER_LoadFromBlock( m_StoredPixelSize, io, ID_DPTH ) ) {
					return false;
				}
				break;

			case ID_RECT:
				if ( !HELPER_LoadFromBlock( m_CaptureRect, io, ID_RECT ) ) {
					return false;
				}
				break;

			case ID_FILE: { // BPT 6/5/01

					CString filename;

					if ( !HELPER_LoadCStringFromBlocK( filename, io ) ) {

						return false;

					}

					if ( !HELPER_VisualElementFromFile( this, filename, io ) ) {

						return false;

					}

				}
				break;

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

			case ID_SCID:
				if ( !HELPER_LoadFromBlock( m_ID, io, ID_SCID ) ) {
					return false;
				}
				break;

			case ID_aVEI: {

					CShowcaseFileIO::io_internal_id_type id;

					if ( !HELPER_LoadFromBlock( id, io, ID_aVEI ) ) {

						return false;

					}

					if ( !io->AssociatePointerWithID( (void *) this, id ) ) {

						TRACE( "Associate pointer %p to id 0x%08x failed?\n", (void *)this, id );

						return false;

					}

				}
				break;

#endif // BPT 5/22/01

			case ID_aS08:
			case ID_aS16:
			case ID_aS32:

				if ( !m_CompressedImage.LoadFrom( io ) ) {

					m_CompressedImage.Destroy();

					return false;

				}

				break;

#if 1 // BPT 10/8/02

			case ID_nVRS: { // load nested visual references

				TRACE( "START: Loading nested visual references\n" );

					CShowcaseFileIO::chunk_reference visualRefChunk = io->GetNestedChunkReference( &innerChunk );

					while ( io->IsChunkReferenceIDValid( &visualRefChunk ) ) {

						TRACE( "\t(2) BLOCK 0x%08x [%s] size %d offset %d\n", visualRefChunk.id, HELPER_BlockText( visualRefChunk.id ), visualRefChunk.size, visualRefChunk.fileOffset );

						if ( ID_aVRF == visualRefChunk.id ) {

							CVisualElementReference * pVisualReference = new CVisualElementReference;

							if ( !pVisualReference ) {

								TRACE( "Unable to get visual reference\n" );

								return false;

							}

							if ( !pVisualReference->LoadFrom( io ) ) {

								return false;

							}

							// add to nexted visuals collection :)

							m_NestedVisuals.push_back( pVisualReference );

						} else {

							TRACE( "\t(2) Skipping unknown block (0x%08x [%s])\n", visualRefChunk.id, HELPER_BlockText( visualRefChunk.id ) );

						}

						// Move on to the next chunk or end the load loop
						// ----------------------------------------------------
			
						if ( !io->SkipChunk( &visualRefChunk ) ) {
			
							TRACE( "End 2\n" );

							break;
			
						}

						visualRefChunk = io->GetCurrentChunkReference( visualRefChunk.pOutter );

					}

				TRACE( "FINISH: Loading nested visual references\n" );

				}
				break;

#endif // BPT 10/8/02

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	bool CVisualElementReference::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CVisualElementReference(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_aVRF != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_aVRF, HELPER_BlockText( ID_aVRF )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_XXYY:
				if ( !HELPER_LoadFromBlock( m_At, io, ID_XXYY ) ) {
					return false;
				}
				break;

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

			case ID_aVEI: {

					CShowcaseFileIO::io_internal_id_type id;

					if ( !HELPER_LoadFromBlock( id, io, ID_aVEI ) ) {

						return false;

					}

					CVisualElement * pVisualElement = (CVisualElement *)io->PointerFromSaveID( id );

					if ( 0 == pVisualElement ) {

						TRACE( "Unable to find visual element for reference %d\n", id );

						return false;

					}

					SetVisualElement( pVisualElement );

				}
				break;

#endif // BPT 5/22/01

			case ID_aVEL:

				CVisualElement * pVisualElement = new CVisualElement( 0 );

				if ( !pVisualElement ) {

					return false;

				}

				if ( !pVisualElement->LoadFrom( io ) ) {

					delete pVisualElement;

					return false;

				}

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01
				
				// Attach the visual element to the showcase for management

				{
					CAnimationShowcase * pShowcase = io->CurrentShowcase();

					if ( !pShowcase ) {

						TRACE( "No current showcase set in IO structure?\n" );

						delete pVisualElement;

						return false;

					}
					
					pShowcase->AdoptVisualElement( pVisualElement );

				}

#endif

				SetVisualElement( pVisualElement );

				break;

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	bool CNamedSpot::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CNamedSpot(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_aNSP != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_aNSP, HELPER_BlockText( ID_aNSP )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_aNSI: {

					CShowcaseFileIO::io_internal_id_type id;

					if ( !HELPER_LoadFromBlock( id, io, ID_aNSI ) ) {

						return false;

					}

					if ( !io->AssociatePointerWithID( (void *) this, id ) ) {

						TRACE( "Associate pointer %p to id 0x%08x failed?\n", (void *)this, id );

						return false;

					}

				}
				break;

			case ID_NAME:
				if ( !m_Name.LoadFrom(io) ) {

					return false;

				}
				break;

			case ID_XXYY:
				if ( !HELPER_LoadFromBlock( m_Location, io, ID_XXYY ) ) {

					return false;

				}
				break;

			case ID_nTXT:
				if ( !CAnnotatedObject::LoadFrom( io ) ) {
					return false;
				}
				break;

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	bool CNamedSpotAdjustment::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CNamedSpotAdjustment(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_aNSA != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_aNSA, HELPER_BlockText( ID_aNSA )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_aNSI: {

					CShowcaseFileIO::io_internal_id_type id;

					if ( !HELPER_LoadFromBlock( id, io, ID_aNSI ) ) {

						return false;

					}

					m_pNamedSpot = (CNamedSpot *)io->PointerFromSaveID( id );

					if ( 0 == m_pNamedSpot ) {

						return false;

					}

				}
				break;

			case ID_DXDY:
				if ( !HELPER_LoadFromBlock( m_Delta, io, ID_DXDY ) ) {

					return false;

				}
				break;

			case ID_nTXT:
				if ( !CAnnotatedObject::LoadFrom( io ) ) {
					return false;
				}
				break;

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	bool CLayer::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CLayer(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_aLYR != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_aLYR, HELPER_BlockText( ID_aLYR )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_aVRF:
				if ( !m_VisualReference.LoadFrom( io ) ) {
		
					return false;
		
				}
				break;

			case ID_nTXT:
				if ( !CAnnotatedObject::LoadFrom( io ) ) {
					return false;
				}
				break;

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	bool CLayerInfo::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CLayerInfo(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_aLII != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_aLII, HELPER_BlockText( ID_aLII )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_NAME:
				if ( !m_Name.LoadFrom( io ) ) {
		
					return false;
		
				}
				break;

			case ID_FLGS:
				if ( !HELPER_LoadFromBlock( m_dwFlags, io, ID_FLGS ) ) {
					return false;
				}
				break;

			case ID_nTXT:
				if ( !CAnnotatedObject::LoadFrom( io ) ) {
					return false;
				}
				break;

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

			case ID_aPLI: {

					CShowcaseFileIO::io_internal_id_type id;

					if ( !HELPER_LoadFromBlock( id, io, ID_aPLI ) ) {

						return false;

					}

					CAnnotatedPalette * pPalette = (CAnnotatedPalette *)io->PointerFromSaveID( id );

					if ( 0 == pPalette ) {

						TRACE( "Unable to find palette for reference %d\n", id );

						return false;

					}

					if ( !AdoptPalette( pPalette ) ) {

						TRACE( "Unable to adopt palette.\n" );

						return false;

					}

				}
				break;

#endif // BPT 6/5/02

			case ID_nPAL: // BPT 6/15/01
				if ( !HELPER_LoadNestedPalette( &m_pPalette, io ) ) { // BPT 6/5/02

					return false;

				}
				break;

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	bool CFrame::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CFrame(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_aFRM != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_aFRM, HELPER_BlockText( ID_aFRM )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_aFHD: {
			
					BPT::CFileIO * pIO = io->BeginChunkDataRead();
			
					if ( !pIO ) {
			
						return false;
			
					}

					int layerCount = pIO->Read_32();

					// Need to create the frames & layers...

					if ( layerCount != LayerCount() ) {

						TRACE( "Mismatch layerCount stored %d need %d\n", layerCount, LayerCount() );

						return false;

					}

				}
				break;

			case ID_LNKS:
				// Need to load the link points (future)
				break;

			case ID_LNK1:
				if ( !HELPER_LoadFromBlock( m_Link1, io, ID_LNK1 ) ) {
					return false;
				}
				break;

			case ID_LNK2:
				if ( !HELPER_LoadFromBlock( m_Link2, io, ID_LNK2 ) ) {
					return false;
				}
				break;

			case ID_LNK3:
				if ( !HELPER_LoadFromBlock( m_Link3, io, ID_LNK3 ) ) {
					return false;
				}
				break;

			case ID_LNK4:
				if ( !HELPER_LoadFromBlock( m_Link4, io, ID_LNK4 ) ) {
					return false;
				}
				break;

			case ID_nTXT:
				if ( !CAnnotatedObject::LoadFrom( io ) ) {
					return false;
				}
				break;

			case ID_nLRS: {

					CShowcaseFileIO::chunk_reference layerChunk = io->GetNestedChunkReference( &innerChunk );

					int layerCounter = 0;

					while ( io->IsChunkReferenceIDValid( &layerChunk ) ) {

						TRACE( "\t(2) BLOCK 0x%08x [%s] size %d offset %d\n", layerChunk.id, HELPER_BlockText( layerChunk.id ), layerChunk.size, layerChunk.fileOffset );

						if ( ID_aLYR == layerChunk.id ) {

							CLayer * pLayer = GetLayerPtr( layerCounter );

							if ( !pLayer ) {

								TRACE( "Unable to get pointer for layer %d\n", layerCounter );

								return false;

							}

							if ( !pLayer->LoadFrom( io ) ) {

								return false;

							}

							++layerCounter;

						} else {

							TRACE( "\t(2) Skipping unknown block (0x%08x [%s])\n", layerChunk.id, HELPER_BlockText( layerChunk.id ) );

						}

						// Move on to the next chunk or end the load loop
						// ----------------------------------------------------
			
						if ( !io->SkipChunk( &layerChunk ) ) {
			
							TRACE( "End 2\n" );

							break;
			
						}

						layerChunk = io->GetCurrentChunkReference( layerChunk.pOutter );

					}

				}
				break;

			case ID_nNSA: {

					CShowcaseFileIO::chunk_reference spotAdjustmentChunk = io->GetNestedChunkReference( &innerChunk );

					while ( io->IsChunkReferenceIDValid( &spotAdjustmentChunk ) ) {

						TRACE( "\t(2) BLOCK 0x%08x [%s] size %d offset %d\n", spotAdjustmentChunk.id, HELPER_BlockText( spotAdjustmentChunk.id ), spotAdjustmentChunk.size, spotAdjustmentChunk.fileOffset );

						if ( ID_aNSA == spotAdjustmentChunk.id ) {

							CNamedSpotAdjustment * pNamedSpotAdjustment = new CNamedSpotAdjustment;

							if ( !pNamedSpotAdjustment ) {

								TRACE( "Unable to get create new named spot adjustment\n" );

								return false;

							}

							if ( !pNamedSpotAdjustment->LoadFrom( io ) ) {

								return false;

							}

							AttachNamedSpotAdjustment( pNamedSpotAdjustment );

						} else {

							TRACE( "\t(2) Skipping unknown block (0x%08x [%s])\n", spotAdjustmentChunk.id, HELPER_BlockText( spotAdjustmentChunk.id ) );

						}

						// Move on to the next chunk or end the load loop
						// ----------------------------------------------------
			
						if ( !io->SkipChunk( &spotAdjustmentChunk ) ) {
			
							TRACE( "End 2\n" );

							break;
			
						}

						spotAdjustmentChunk = io->GetCurrentChunkReference( spotAdjustmentChunk.pOutter );

					}

				}
				break;

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	//
	//	CAnimation::LoadFrom()
	//

	bool CAnimation::LoadFrom( CShowcaseFileIO * io ) {

		TRACE( "CAnimation(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_aANM != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_aANM, HELPER_BlockText( ID_aANM )
			);

			return false;

		}

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_aAHD: {
			
					BPT::CFileIO * pIO = io->BeginChunkDataRead();
			
					if ( !pIO ) {
			
						return false;
			
					}

					int frameCount = pIO->Read_32();
					int layerCount = pIO->Read_32();

					// Need to create the frames & layers...

					if ( !InsertNewLayers( layerCount, 0 ) ) {

						TRACE( "Failed to create %d layers\n", layerCount );

						return false;

					}

					if ( !InsertNewFramesBefore( frameCount ) ) {

						TRACE( "Failed to create %d frames\n", frameCount );

						return false;

					}

				}
				break;

			case ID_NAME:
				if ( !m_Name.LoadFrom( io ) ) {
					return false;
				}
				break;

			case ID_aBKD:
				if ( !m_Backdrop.LoadFrom( io ) ) {
					return false;
				}
				break;

			case ID_LNKS:
				// Need to load the link points
				break;

			case ID_LNK1:
				if ( !HELPER_LoadFromBlock( m_Link1, io, ID_LNK1 ) ) {
					return false;
				}
				break;

			case ID_nTXT:
				if ( !CAnnotatedObject::LoadFrom( io ) ) {
					return false;
				}
				break;

			case ID_RATE:
				if ( !HELPER_LoadFromBlock( m_PlaybackDelay, io, ID_RATE ) ) {
					return false;
				}
				break;

			case ID_OUTA: // BPT 6/27/01
				if ( !HELPER_LoadFromBlock( m_nOutlineColorA, io, ID_OUTA ) ) {
					return false;
				}
				break;

			case ID_OUTB: // BPT 6/27/01
				if ( !HELPER_LoadFromBlock( m_nOutlineColorB, io, ID_OUTB ) ) {
					return false;
				}
				break;

			case ID_aPO1: // BPT 6/17/01
				if ( !HELPER_LoadFromBlock( m_Policies, io, ID_aPO1 ) ) {
					return false;
				}
				break;

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

			case ID_aPLI: {

					CShowcaseFileIO::io_internal_id_type id;

					if ( !HELPER_LoadFromBlock( id, io, ID_aPLI ) ) {

						return false;

					}

					CAnnotatedPalette * pPalette = (CAnnotatedPalette *)io->PointerFromSaveID( id );

					if ( 0 == pPalette ) {

						TRACE( "Unable to find palette for reference %d\n", id );

						return false;

					}

					if ( !AdoptPalette( pPalette ) ) {

						TRACE( "Unable to adopt palette.\n" );

						return false;

					}

				}
				break;

#endif // BPT 6/5/02

			case ID_nPAL: // BPT 6/15/01
				if ( !HELPER_LoadNestedPalette( &m_pPalette, io ) ) { // BPT 6/5/02

					return false;

				}
				break;

			case ID_nLIS: {
				
					CShowcaseFileIO::chunk_reference infoChunk = io->GetNestedChunkReference( &innerChunk );

					int infoCounter = 0;

					while ( io->IsChunkReferenceIDValid( &infoChunk ) ) {

						TRACE( "\t(3) BLOCK 0x%08x [%s] size %d offset %d\n", infoChunk.id, HELPER_BlockText( infoChunk.id ), infoChunk.size, infoChunk.fileOffset );

						if ( ID_aLII == infoChunk.id ) {

							CLayerInfo * pLayerInfo = GetLayerInfo( infoCounter );

							if ( !pLayerInfo ) {

								TRACE( "Unable to get pointer for layerInfo %d\n", infoCounter );

								return false;

							}

							if ( !pLayerInfo->LoadFrom( io ) ) {

								return false;

							}

							++infoCounter;

						} else {

							TRACE( "\t(3) Skipping unknown block (0x%08x [%s])\n", infoChunk.id, HELPER_BlockText( infoChunk.id ) );

						}

						// Move on to the next chunk or end the load loop
						// ----------------------------------------------------
			
						if ( !io->SkipChunk( &infoChunk ) ) {
			
							TRACE( "End 3\n" );

							break;
			
						}

						infoChunk = io->GetCurrentChunkReference( infoChunk.pOutter );

					}

				}
				break;

			case ID_nFMS: {

					CShowcaseFileIO::chunk_reference frameChunk = io->GetNestedChunkReference( &innerChunk );

					int frameCounter = 0;

					while ( io->IsChunkReferenceIDValid( &frameChunk ) ) {

						TRACE( "\t(2) BLOCK 0x%08x [%s] size %d offset %d\n", frameChunk.id, HELPER_BlockText( frameChunk.id ), frameChunk.size, frameChunk.fileOffset );

						if ( ID_aFRM == frameChunk.id ) {

							CFrame * pFrame = GetFramePtr( frameCounter );

							if ( !pFrame ) {

								TRACE( "Unable to get pointer for frame %d\n", frameCounter );

								return false;

							}

							if ( !pFrame->LoadFrom( io ) ) {

								return false;

							}

							++frameCounter;

						} else {

							TRACE( "\t(2) Skipping unknown block (0x%08x [%s])\n", frameChunk.id, HELPER_BlockText( frameChunk.id ) );

						}

						// Move on to the next chunk or end the load loop
						// ----------------------------------------------------
			
						if ( !io->SkipChunk( &frameChunk ) ) {
			
							TRACE( "End 2\n" );

							break;
			
						}

						frameChunk = io->GetCurrentChunkReference( frameChunk.pOutter );

					}

				}
				break;

			case ID_nNSD: {

					CShowcaseFileIO::chunk_reference namedSpotChunk = io->GetNestedChunkReference( &innerChunk );

					while ( io->IsChunkReferenceIDValid( &namedSpotChunk ) ) {

						TRACE( "\t(2) BLOCK 0x%08x [%s] size %d offset %d\n", namedSpotChunk.id, HELPER_BlockText( namedSpotChunk.id ), namedSpotChunk.size, namedSpotChunk.fileOffset );

						if ( ID_aNSP == namedSpotChunk.id ) {

							CNamedSpot * pNamedSpot = new CNamedSpot;

							if ( !pNamedSpot ) {

								TRACE( "Unable to get create new named spot\n" );

								return false;

							}

							if ( !pNamedSpot->LoadFrom( io ) ) {

								return false;

							}

							AttachNamedSpot( pNamedSpot );

						} else {

							TRACE( "\t(2) Skipping unknown block (0x%08x [%s])\n", namedSpotChunk.id, HELPER_BlockText( namedSpotChunk.id ) );

						}

						// Move on to the next chunk or end the load loop
						// ----------------------------------------------------
			
						if ( !io->SkipChunk( &namedSpotChunk ) ) {
			
							TRACE( "End 2\n" );

							break;
			
						}

						namedSpotChunk = io->GetCurrentChunkReference( namedSpotChunk.pOutter );

					}

				}
				break;

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return true;

	}

	// ========================================================================

	//
	//	CAnimationShowcase::LoadFrom()
	//

	bool CAnimationShowcase::LoadFrom( CShowcaseFileIO * io )
	{
		TRACE( "CAnimationShowcase(%p)::LoadFrom\n", this );

		// Get the root block and validate we're trying to load a showcase
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference rootChunk = io->GetCurrentChunkReference( 0 );

		if ( (ID_aBPT != rootChunk.id) || (!io->IsChunkReferenceIDValid( &rootChunk )) ) {

			TRACE( "Block isn't valid for load 0x%08x [ %s ] need 0x%08x [ %s ]\n",
				rootChunk.id, HELPER_BlockText( rootChunk.id ),
				ID_aBPT, HELPER_BlockText( ID_aBPT )
			);

			return FALSE;

		}

#if 1 // 5/22/01 BPT

		io->SetCurrentShowcase( this );

#endif

		// Start reading nested elements
		// --------------------------------------------------------------------

		CShowcaseFileIO::chunk_reference innerChunk = io->GetNestedChunkReference( &rootChunk );

		bool bError = true;

		while ( io->IsChunkReferenceIDValid( &innerChunk ) ) {

			TRACE( "\t(1) BLOCK 0x%08x [%s] size %d offset %d\n", innerChunk.id, HELPER_BlockText( innerChunk.id ), innerChunk.size, innerChunk.fileOffset );

			switch ( innerChunk.id ) {

			default:
				TRACE( "\t(1) Skipping unknown block (0x%08x [%s])\n", innerChunk.id, HELPER_BlockText( innerChunk.id ) );
				break;

			case ID_CANV:
				if ( !HELPER_LoadFromBlock( m_CanvasSize, io, ID_CANV ) ) {

					TRACE( "End 3\n" );

					return false;

				}
				bError = false;
				break;

			case ID_PRAT:
				if ( !HELPER_LoadFromBlock( m_PixelAspectRatio, io, ID_PRAT ) ) {

					TRACE( "End 3b\n" );

					return false;

				}
				bError = false;
				break;

			case ID_NAME:
				if ( !m_Name.LoadFrom( io ) ) {
					return false;
				}
				bError = false;
				break;

			case ID_nPAL:
				// Load the default palette
#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02
				{
					CAnnotatedPalette * pDefaultPal = GetDefaultPalette();

					if ( pDefaultPal ) {

						if ( !pDefaultPal->LoadFrom( io ) ) {

							return false;

						}

					}

				}
#else
				if ( !m_DefaultPalette.LoadFrom( io ) ) {

					return false;

				}
#endif
				break;

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // 5/22/01 BPT

			case ID_NXID:

				if ( !HELPER_LoadFromBlock( m_NextID, io, ID_NXID ) ) {

					TRACE( "End 4\n" );

					return false;

				}
				bError = false;
				break;

			case ID_nVLS: {

					CShowcaseFileIO::chunk_reference visualElementChunk = io->GetNestedChunkReference( &innerChunk );

					while ( io->IsChunkReferenceIDValid( &visualElementChunk ) ) {

						TRACE( "\t(3) BLOCK 0x%08x [%s] size %d offset %d\n", visualElementChunk.id, HELPER_BlockText( visualElementChunk.id ), visualElementChunk.size, visualElementChunk.fileOffset );

						if ( ID_aVEL == visualElementChunk.id ) {

							// create the visual element to load into.

							CVisualElement * pNew = new CVisualElement( 0 );

							if ( !pNew ) {

								return false;

							}

							if ( !pNew->LoadFrom( io ) ) {

								delete pNew;

								return false;

							}

							pNew->AddRef();

							m_VisualElementsCollection.push_back( pNew );
	
							bError = false;

						} else {

							TRACE( "\t(3) Skipping unknown block (0x%08x [%s])\n", visualElementChunk.id, HELPER_BlockText( visualElementChunk.id ) );

						}

						// Move on to the next chunk or end the load loop
						// ----------------------------------------------------
			
						if ( !io->SkipChunk( &visualElementChunk ) ) {
			
							TRACE( "End 5\n" );

							break;
			
						}

						visualElementChunk = io->GetCurrentChunkReference( visualElementChunk.pOutter );

					}

				}
				break;

#endif // 5/22/01 BPT

			case ID_nAMS: {

					CShowcaseFileIO::chunk_reference animChunk = io->GetNestedChunkReference( &innerChunk );

					while ( io->IsChunkReferenceIDValid( &animChunk ) ) {

						TRACE( "\t(2) BLOCK 0x%08x [%s] size %d offset %d\n", animChunk.id, HELPER_BlockText( animChunk.id ), animChunk.size, animChunk.fileOffset );

						if ( ID_aANM == animChunk.id ) {

							// create the animation to load into.

							CAnimation * pNew = new CAnimation();

							if ( !pNew ) {

								return false;

							}

							if ( !pNew->LoadFrom( io ) ) {

								delete pNew;

								return false;

							}

							AttachAnimation( pNew );
	
							bError = false;

						} else {

							TRACE( "\t(2) Skipping unknown block (0x%08x [%s])\n", animChunk.id, HELPER_BlockText( animChunk.id ) );

						}

						// Move on to the next chunk or end the load loop
						// ----------------------------------------------------
			
						if ( !io->SkipChunk( &animChunk ) ) {
			
							TRACE( "End 2\n" );

							break;
			
						}

						animChunk = io->GetCurrentChunkReference( animChunk.pOutter );

					}

				}
				break;

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02
			case ID_nPLS: {

					CShowcaseFileIO::chunk_reference palChunk = io->GetNestedChunkReference( &innerChunk );

					while ( io->IsChunkReferenceIDValid( &palChunk ) ) {

						TRACE( "\t(3) BLOCK 0x%08x [%s] size %d offset %d\n", palChunk.id, HELPER_BlockText( palChunk.id ), palChunk.size, palChunk.fileOffset );

						if ( ID_nPAL == palChunk.id ) {

							CAnnotatedPalette * pPalette = CreatePalette( false );

							if ( pPalette ) {

								return false;

							}

							if ( !pPalette->LoadFrom( io ) ) {

								return false;

							}

							bError = false;

						} else {

							TRACE( "\t(3) Skipping unknown block (0x%08x [%s])\n", palChunk.id, HELPER_BlockText( palChunk.id ) );

						}

						// Move on to the next chunk or end the load loop
						// ----------------------------------------------------
			
						if ( !io->SkipChunk( &palChunk ) ) {
			
							TRACE( "End 3\n" );

							break;
			
						}

						palChunk = io->GetCurrentChunkReference( palChunk.pOutter );

					}

				}
				break;
#endif // defined(BPT_SHOWCASE_MANAGED_PALETTES) BPT 6/5/02

			}

			// Move on to the next chunk or end the load loop
			// ----------------------------------------------------------------

			if ( !io->SkipChunk( &innerChunk ) ) {

				TRACE( "End 1\n" );

				break;

			}

			// ----------------------------------------------------------------

			innerChunk = io->GetCurrentChunkReference( innerChunk.pOutter );

		}

		TRACE( "Finished with 0x%08x [ %s ]\n", rootChunk.id, HELPER_BlockText( rootChunk.id ) );

		return !bError;
	}

	// ========================================================================
	// ========================================================================
	// ========================================================================

	// ========================================================================
	// ========================================================================
	// ========================================================================

	bool CSimpleName::SaveTo( CShowcaseFileIO * io ) {

		if ( m_pzName ) {

			return HELPER_SaveToBlock( m_pzName, io, ID_NAME );

		}

		return true;

	}

	// ========================================================================

	bool CAnnotatedObject::SaveTo( CShowcaseFileIO * io ) {

		// Check to see if there is an annotation
		// ---------------------------------------------------------------------

		if ( !strlen( m_Str ) ) {

			return true;

		}

		// ---------------------------------------------------------------------

		BPT::CFileIO * pIO = io->BeginBlock( ID_nTXT );

		if ( !pIO ) {

			return false;

		}

		// Check to see if there is a m_Str
		// ---------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_Str, io, ID_TEXT ) ) {

			return false;

		}

		return io->FinishBlock( ID_nTXT );

	}

	// ========================================================================

	bool CAnimationBackdrop::SaveTo( CShowcaseFileIO * io ) {

		BPT::CFileIO * pIO = io->BeginBlock( ID_aBKD );

		if ( !pIO ) {

			return false;

		}

		// Save off the visual element reference
		// ---------------------------------------------------------------------

		if ( m_pVisualElement ) {

			if ( !m_pVisualElement->SaveTo( io ) ) {

				return false;

			}

		}

		return io->FinishBlock( ID_aBKD );

	}

	// ========================================================================

	bool CAnnotatedPalette::SaveTo( CShowcaseFileIO * io ) {

		BPT::CFileIO * pIO = io->BeginBlock( ID_nPAL );

		if ( !pIO ) {

			return false;

		}

		// Write the managed palette fun
		// --------------------------------------------------------------------

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		CShowcaseFileIO::io_internal_id_type id = io->SaveIDForPointer( (void *) this );
		
		if ( !HELPER_SaveToBlock( id, io, ID_aPLI ) ) {
		
			return false;
		
		}

		if ( !m_Name.SaveTo( io ) ) {

			return false;

		}

#endif // BPT 6/5/02

		// Write the RGB values
		// --------------------------------------------------------------------

		// Should write the 'count' of the colors to indicate a non-default
		// palette size if there is one.
		// --------------------------------------------------------------------

		int nTotalColors = TotalColors();

		int i;

#if 1 // BPT 6/20/01

		if ( io->StoreVisualElementsExternally() ) { // should be a different condition

			// Get the save id to generate a unique filename
			// ----------------------------------------------------------------

			CShowcaseFileIO::io_internal_id_type id = io->SaveIDForPointer( (void *)this );

			// Okay now build and output the file :)
			// ---------------------------------------------------------------------

			CString postfix;

			postfix.Format( "-(%05d-palette)", id );

			CString filename = io->MakeSubordinateFilename(
				CString( "" ), postfix, CString( ".bmp" ), false
			);

			CString fullFilename = io->MakeSubordinateFilename(
				CString( "" ), postfix, CString( ".bmp" ), false
			);

			// ---------------------------------------------------------------------

			if ( !HELPER_PaletteToFile( this, fullFilename, io ) ) {

				return false;

			}

			// ---------------------------------------------------------------------

			if ( !HELPER_SaveToBlock( filename, io, ID_FILE ) ) {

				return false;

			}

		} else {

			pIO = io->BeginBlock( ID_RGBs );
		
			if ( !pIO ) {
		
				return false;
		
			}

			for ( i = 0; i < nTotalColors; i++ ) {
	
				pIO->Write_32( m_Colors[ i ] );
	
			}
	
			if ( !io->FinishBlock( ID_RGBs ) ) {
	
				return false;
	
			}

		}

#else // BPT 6/20/01

		pIO = io->BeginBlock( ID_RGBs );

		if ( !pIO ) {

			return false;

		}

		for ( i = 0; i < nTotalColors; i++ ) {

			pIO->Write_32( m_Colors[ i ] );

		}

		if ( !io->FinishBlock( ID_RGBs ) ) {

			return false;

		}

#endif // BPT 6/20/01

		// Write the flags
		// --------------------------------------------------------------------

		pIO = io->BeginBlock( ID_PFGs );

		if ( !pIO ) {

			return false;

		}

		for ( i = 0; i < nTotalColors; i++ ) {

			pIO->Write_32( m_Flags[ i ] );

		}

		if ( !io->FinishBlock( ID_PFGs ) ) {

			return false;

		}

		// Write the notes
		// --------------------------------------------------------------------

		pIO = io->BeginBlock( ID_SNTs );

		if ( !pIO ) {

			return false;

		}

		for ( i = 0; i < nTotalColors; i++ ) {

			if ( m_Notes[ i ].IsEmpty() ) {

				HELPER_SaveToBlock( "", io, ID_TEXT );

			} else {

				HELPER_SaveToBlock( m_Notes[ i ], io, ID_TEXT );

			}

		}

		if ( !io->FinishBlock( ID_SNTs ) ) {

			return false;

		}

		// write other things (FUTURE)
		// --------------------------------------------------------------------

		// finish the root block
		// --------------------------------------------------------------------

		return io->FinishBlock( ID_nPAL );

	}

	// ========================================================================

	//
	//	TSRLCompressedImage::SaveTo()
	//
	//	-- I'm actually amazed that VC was able to compile this code :)
	//

	bool TSRLCompressedImage<CVisualElement::compressor_type>::SaveTo(
		CShowcaseFileIO * io ) {

		// determine the block id
		// --------------------------------------------------------------------

		int ia = '0' + ((sizeof( storage_type ) * 8) / 10);
		int ib = '0' + ((sizeof( storage_type ) * 8) % 10);

		CShowcaseFileIO::id_type blockID = MAKE_CHUNK_ID( 'a', 'S', ia, ib );

		// --------------------------------------------------------------------

		BPT::CFileIO * pIO = io->BeginBlock( blockID );

		if ( !pIO ) {

			return false;

		}

		// Write out the size block
		// --------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_Size, io, ID_WWHH ) ) {

			return false;

		}

#if 1 // BPT 5/22/01

		// Write out the estimated size? 
		// WARNING: If the sizes of the types change between builds then 
		// this check is going to be wrong which might cause the quick check
		// for image simlarity to fail when it's not supposed to...
		// --------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_CompressedSizeEstimate, io, ID_ESIZ ) ) {

			return false;

		}

#endif // BPT 5/22/01

		// Wrap all of the lines in a nSXD block
		// --------------------------------------------------------------------

		pIO = io->BeginBlock( ID_nSXD );

		if ( !pIO ) {

			return false;

		}

		// Save each INFO block
		// --------------------------------------------------------------------
	
		for ( int y = 0; y < m_Size.cy; y++ ) {

#if defined( SRL_USE_STD_COLLECTION )

			srl_type::INFO * pInfo = m_CompressedInfo[ y ];

#else

			srl_type::INFO * pInfo = &m_CompressedInfo[ y ];

#endif

			// Write out the control data block
			// --------------------------------------------------------------------
	
			if ( !T_SaveCollectionAsBlock( pInfo->first, io, ID_aSCD ) ) {
	
				return false;
	
			}
	
			// Write out the value data block
			// --------------------------------------------------------------------
	
			if ( !T_SaveCollectionAsBlock( pInfo->second, io, ID_aSVD ) ) {
	
				return false;
	
			}

		}

		if ( !io->FinishBlock( ID_nSXD ) ) {

			return false;

		}

		// finish the root block
		// --------------------------------------------------------------------

		return io->FinishBlock( blockID );

	}

	// ========================================================================

	bool CVisualElement::SaveTo( CShowcaseFileIO * io ) {

		// save nested objects first
		// --------------------------------------------------------------------

#if 1 // BPT 10/8/02

		if ( io->PointerHasID( (void *)this ) ) {

			TRACE( "CVisualElement %p already saved skipping.\n", this );

			return true;

		}

		CShowcaseFileIO::io_internal_id_type id = io->SaveIDForPointer( (void *)this );

		if ( HasNestedVisualReferences() ) {

			TRACE( "Dumping nested visuals for %p\n", this );

			nested_collection_type::iterator it = m_NestedVisuals.begin();

			for ( ; it != m_NestedVisuals.end(); it++ ) {

				CVisualElement * pVisualElement = (*it)->GetVisualElement();

				if ( pVisualElement ) {

					// save the visual element if it hasn't been saved yet

					if ( !io->PointerHasID(pVisualElement) ) {

						if ( !pVisualElement->SaveTo( io ) ) {

							pVisualElement->Release();

							return false;

						}

					}

					pVisualElement->Release();

				}

			}

			TRACE( "done dumping nested visuals %p\n", this );

		}

#endif // BPT 10/8/02

		// --------------------------------------------------------------------

		BPT::CFileIO * pIO = io->BeginBlock( ID_aVEL );

		if ( !pIO ) {

			return false;

		}

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01 (Moved 10/8/02)
	
		// Save off the internal id for this element
		// ----------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_ID, io, ID_SCID ) ) {

			return false;

		}

		// Save off the storage id for this item.
		// ----------------------------------------------------------------

		if ( !HELPER_SaveToBlock( id, io, ID_aVEI ) ) {

			return false;

		}

#endif // BPT 5/22/01

		// Save any nested visual references
		// --------------------------------------------------------------------

#if 1 // BPT 10/8/02

		if ( HasNestedVisualReferences() ) {

			if ( !HELPER_SaveToBlock( m_CaptureRect, io, ID_RECT ) ) {

				return false;

			}

			TRACE( "Writing nested visual element references for %p\n", this );

			if ( !T_BlockSaveTemplate( m_NestedVisuals.begin(), m_NestedVisuals.end(), io, ID_nVRS ) ) {

				return false;

			}

			TRACE( "Done with riting nested visual element references for %p\n", this );

		}

#endif // BPT 10/8/02

		// save off the image
		// --------------------------------------------------------------------

		if ( io->StoreVisualElementsExternally() ) { // should be a different condition

			if ( m_CompressedImage.HasData() ) {

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

				// (Moved 10/8/02)
	
#endif // BPT 5/22/01

				// Okay now build and output the file :)
				// ---------------------------------------------------------------------

				CString postfix;

				postfix.Format( "-(%05d)", id );

				CString filename = io->MakeSubordinateFilename(
					CString( "" ), postfix, CString( ".bmp" ), false
				);

				CString fullFilename = io->MakeSubordinateFilename(
					CString( "" ), postfix, CString( ".bmp" ), false
				);

				// ---------------------------------------------------------------------

				if ( !HELPER_VisualElementToFile( this, fullFilename, io ) ) {

					return false;

				}

				// ---------------------------------------------------------------------

				if ( !HELPER_SaveToBlock( filename, io, ID_FILE ) ) {

					return false;

				}

			}

		} else if ( m_CompressedImage.HasData() ) {

			TRACE( "Writing compressed image for %p\n", this );

			// Store off the represented pixel size
			// ----------------------------------------------------------------

			if ( !HELPER_SaveToBlock( m_StoredPixelSize, io, ID_DPTH ) ) {

				return false;

			}

			// Store off the 'capture' rect
			// ----------------------------------------------------------------
			
			if ( !HELPER_SaveToBlock( m_CaptureRect, io, ID_RECT ) ) {

				return false;

			}

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

			// (Moved 10/8/02)
	
#endif // BPT 5/22/01

			// Save the actual compressed data
			// ----------------------------------------------------------------

			if ( !m_CompressedImage.SaveTo( io ) ) {

				return false;

			}

			TRACE( "done writing compressed image for %p\n", this );

		}

		return io->FinishBlock( ID_aVEL );

	}

	// ========================================================================

	bool CVisualElementReference::SaveTo( CShowcaseFileIO * io ) {

		// For now this stores the visual element inside eventually it will
		// contain a true reference to the element which means there will
		// need to be a different save context...

		BPT::CFileIO * pIO = io->BeginBlock( ID_aVRF );

		if ( !pIO ) {

			return false;

		}

		// Save off the visual element reference
		// ---------------------------------------------------------------------

		CVisualElement * pVisualElement = GetVisualElement();

		if ( pVisualElement ) {

			TRACE( "Writing reference to %p (was saved %c)\n", pVisualElement, io->PointerHasID(pVisualElement) ? 'Y' : 'N' );

			// Save off the position.
			// ------------------------------------------------------------------

			if ( !HELPER_SaveToBlock( m_At, io, ID_XXYY ) ) {

				return false;

			}

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

			// Match the visual element pointer to a save id, and save that -- this
			// of course assumes that the visual elements block has been written
			// before this can be valid unless the loading is made multi pass....
			// ---------------------------------------------------------------------
	
			CShowcaseFileIO::io_internal_id_type id = io->SaveIDForPointer( pVisualElement );
	
			pVisualElement->Release();

			if ( !HELPER_SaveToBlock( id, io, ID_aVEI ) ) {
	
				return false;
	
			}

#else

			// save the visual element right here, this will eventually be
			// replaced with a real reference to the visual element.
			// ------------------------------------------------------------------


			bool bResult = pVisualElement->SaveTo( io );

			pVisualElement->Release();

			if ( !bResult ) {

				return false;

			}

#endif

		}

		return io->FinishBlock( ID_aVRF );

	}


	// ========================================================================

	bool CNamedSpot::SaveTo( CShowcaseFileIO * io ) {

		BPT::CFileIO * pIO = io->BeginBlock( ID_aNSP );

		if ( !pIO ) {

			return false;

		}

		// Save off the annotations
		// ---------------------------------------------------------------------

		// Save off the storage id for this item.
		// ---------------------------------------------------------------------

		CShowcaseFileIO::io_internal_id_type id = io->SaveIDForPointer( (void *)this );

		if ( !HELPER_SaveToBlock( id, io, ID_aNSI ) ) {

			return false;

		}

		// Save off the name
		// ---------------------------------------------------------------------

		if ( !m_Name.SaveTo( io ) ) {

			return false;

		}

		// Save off the location
		// ---------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_Location, io, ID_XXYY ) ) {

			return false;

		}

		// Save off the annotations
		// ---------------------------------------------------------------------

		if ( !CAnnotatedObject::SaveTo( io ) ) {

			return false;

		}

		return io->FinishBlock( ID_aNSP );

	}
	
	// ========================================================================

	bool CNamedSpotAdjustment::SaveTo( CShowcaseFileIO * io ) {

		BPT::CFileIO * pIO = io->BeginBlock( ID_aNSA );

		if ( !pIO ) {

			return false;

		}

		// Save off the annotations
		// ---------------------------------------------------------------------

		// Match the named spot pointer to a save id, and save that
		// ---------------------------------------------------------------------

		CShowcaseFileIO::io_internal_id_type id = io->SaveIDForPointer( m_pNamedSpot );

		if ( !HELPER_SaveToBlock( id, io, ID_aNSI ) ) {

			return false;

		}

		// Save off the delta offset
		// ---------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_Delta, io, ID_DXDY ) ) {

			return false;

		}

		// Save off the annotations
		// ---------------------------------------------------------------------

		if ( !CAnnotatedObject::SaveTo( io ) ) {

			return false;

		}

		return io->FinishBlock( ID_aNSA );

	}
	// ========================================================================

	bool CLayer::SaveTo( CShowcaseFileIO * io ) {

		BPT::CFileIO * pIO = io->BeginBlock( ID_aLYR );

		if ( !pIO ) {

			return false;

		}

		// Save off the annotations
		// ---------------------------------------------------------------------

		// notes

		// Save off the annotations
		// ---------------------------------------------------------------------

		if ( !CAnnotatedObject::SaveTo( io ) ) {

			return false;

		}

		// Save off the visual element reference
		// ---------------------------------------------------------------------

		if ( !m_VisualReference.SaveTo( io ) ) {

			return false;

		}

		return io->FinishBlock( ID_aLYR );

	}

	// ========================================================================

	bool CLayerInfo::SaveTo( CShowcaseFileIO * io ) {

		BPT::CFileIO * pIO = io->BeginBlock( ID_aLII );

		if ( !pIO ) {

			return false;

		}

		// Write the name
		// ---------------------------------------------------------------------

		if ( !m_Name.SaveTo( io ) ) {

			return false;

		}

		// flags

		if ( !HELPER_SaveToBlock( m_dwFlags, io, ID_FLGS ) ) {

			return false;

		}

		// Save off the annotations
		// ---------------------------------------------------------------------

		if ( !CAnnotatedObject::SaveTo( io ) ) {

			return false;

		}

		// Write the [OPTIONAL] palette 
		// ---------------------------------------------------------------------

		if ( m_pPalette ) { // BPT 6/15/01

			if ( !HELPER_SaveNestedPalette( m_pPalette, io ) ) { // 6/5/02

				return false;

			}

		}

		// ??
		// ---------------------------------------------------------------------

		return io->FinishBlock( ID_aLII );

	}

	// ========================================================================

	bool CFrame::SaveTo( CShowcaseFileIO * io ) {

		BPT::CFileIO * pIO = io->BeginBlock( ID_aFRM );

		if ( !pIO ) {

			return false;

		}

		// Write the header
		// ---------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( LayerCount(), io, ID_aFHD ) ) {

			return false;

		}

		// Save off the named spot adjustments
		// ---------------------------------------------------------------------

		if ( m_NamedSpotAdjustments.size() ) {

			if ( !T_BlockSaveTemplate( 
				m_NamedSpotAdjustments.begin(), m_NamedSpotAdjustments.end(), io, ID_nNSA ) ) {

				return false;
	
			}

		}

		// Save off the annotations
		// ---------------------------------------------------------------------

		// link points (ID_LNKS)

		// notes

		// Special case link points
		// ---------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_Link1, io, ID_LNK1 ) ) {

			return false;

		}

		if ( !HELPER_SaveToBlock( m_Link2, io, ID_LNK2 ) ) {

			return false;

		}

		if ( !HELPER_SaveToBlock( m_Link3, io, ID_LNK3 ) ) {

			return false;

		}

		if ( !HELPER_SaveToBlock( m_Link4, io, ID_LNK4 ) ) {

			return false;

		}

		// Save off the annotations
		// ---------------------------------------------------------------------

		if ( !CAnnotatedObject::SaveTo( io ) ) {

			return false;

		}

		// Save off each frame
		// ---------------------------------------------------------------------

		if ( !T_BlockSaveTemplate( m_Layers.begin(), m_Layers.end(), io, ID_nLRS ) ) {

			return false;

		}

		return io->FinishBlock( ID_aFRM );

	}

	// ========================================================================

	//
	//	CAnimation::SaveTo()
	//

	bool CAnimation::SaveTo( CShowcaseFileIO * io ) {

		BPT::CFileIO * pIO = io->BeginBlock( ID_aANM );

		if ( !pIO ) {

			return false;

		}

		// Write the header
		// ---------------------------------------------------------------------

		if ( pIO = io->BeginBlock( ID_aAHD ) ) {

			// Write the data

			pIO->Write_32( FrameCount() );
			pIO->Write_32( LayerCount() );

			// finish the block

			if ( !io->FinishBlock( ID_aAHD ) ) {

				return false;

			}

		} else {

			return false;

		}

		// Save off the backdrop (ID_aBKD)
		// ---------------------------------------------------------------------

		if ( !m_Backdrop.SaveTo( io ) ) {

			return false;

		}

		// Write the name
		// ---------------------------------------------------------------------

		if ( !m_Name.SaveTo( io ) ) {

			return false;

		}

		// Save off the policy flags if there are some
		// --------------------------------------------------------------------

		if ( m_Policies ) { // BPT 6/17/01

			if ( !HELPER_SaveToBlock( m_Policies, io, ID_aPO1 ) ) {

				return false;

			}

		}

		// Save off the named spots
		// ---------------------------------------------------------------------

		if ( m_NamedSpots.size() ) {

			if ( !T_BlockSaveTemplate( 
				m_NamedSpots.begin(), m_NamedSpots.end(), io, ID_nNSD ) ) {

				return false;
	
			}

		}

		// Save off the annotations
		// ---------------------------------------------------------------------

		// link points

		// notes

		if ( !CAnnotatedObject::SaveTo( io ) ) {

			return false;

		}

		// Special case link points
		// ---------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_Link1, io, ID_LNK1 ) ) {

			return false;

		}

		// Save off the delay
		// ---------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_PlaybackDelay, io, ID_RATE ) ) {

			return false;

		}

		// Save off the outline colors
		// ---------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_nOutlineColorA, io, ID_OUTA ) ) {

			return false;

		}

		if ( !HELPER_SaveToBlock( m_nOutlineColorB, io, ID_OUTB ) ) {

			return false;

		}

		// Write the [OPTIONAL] palette 
		// ---------------------------------------------------------------------

		if ( m_pPalette ) { // BPT 6/15/01

			if ( !HELPER_SaveNestedPalette( m_pPalette, io ) ) { // 6/5/02

				return false;

			}

		}

		// Save off the general layer information
		// ---------------------------------------------------------------------

		if ( !T_BlockSaveTemplate( m_LayerInfo.begin(), m_LayerInfo.end(), io, ID_nLIS ) ) {

			return false;

		}

		// Save off each frame
		// ---------------------------------------------------------------------

		if ( !T_BlockSaveTemplate( m_Frames.begin(), m_Frames.end(), io, ID_nFMS ) ) {

			return false;

		}

		return io->FinishBlock( ID_aANM );

	}

	// ========================================================================

	//
	//	CAnimationShowcase::SaveTo()
	//

	bool CAnimationShowcase::SaveTo( CShowcaseFileIO * io )
	{

#if 1 // 5/22/01 BPT

		io->SetCurrentShowcase( this );

#endif

		// write the root block
		// ---------------------------------------------------------------------

		BPT::CFileIO * pIO = io->BeginBlock( ID_aBPT );

		if ( !pIO ) {

			return false;

		}

		// Write the canvas size
		// ---------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_CanvasSize, io, ID_CANV ) ) {

			return false;

		}

		// Write the pixel aspect ratio
		// ---------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_PixelAspectRatio, io, ID_PRAT ) ) {

			return false;

		}

		// Write the name
		// ---------------------------------------------------------------------

		if ( !m_Name.SaveTo( io ) ) {

			return false;

		}

		// Write the palette(s)
		// ---------------------------------------------------------------------

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		// save the default palette

		{
			CAnnotatedPalette * pDefaultPal = GetDefaultPalette();

			if ( pDefaultPal ) {

				if ( !pDefaultPal->SaveTo( io ) ) {

					return false;

				}

			}

		}

		// save the managed palettes

		if ( PaletteCount() ) {

			if ( !T_BlockSaveTemplate(
				m_PaletteCollection.begin(), m_PaletteCollection.end(), io, ID_nPLS ) ) {

				return false;

			}

		}

#else

		if ( !m_DefaultPalette.SaveTo( io ) ) {

			return false;

		}

#endif

		// write the notes
		// ---------------------------------------------------------------------

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

		// Write the next id
		// ---------------------------------------------------------------------

		if ( !HELPER_SaveToBlock( m_NextID, io, ID_NXID ) ) {

			return false;

		}

		// Save off each visual element
		// ---------------------------------------------------------------------

		if ( !T_BlockSaveTemplate(
			m_VisualElementsCollection.begin(), m_VisualElementsCollection.end(), 
			io, ID_nVLS ) ) {

			return false;

		}

#endif // BPT 5/22/01

		// Save off each animation
		// ---------------------------------------------------------------------

		if ( !T_BlockSaveTemplate(
			m_AnimationCollection.begin(), m_AnimationCollection.end(), io, ID_nAMS ) ) {

			return false;

		}

		// Finish the root block
		// ---------------------------------------------------------------------

		return io->FinishBlock( ID_aBPT );

	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	Save .BPT file
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	bool SaveBPTFilePrim( CShowcaseFileIO * io, CAnimationShowcase * pShowCase ) {

#if defined( DEMO_SAVE_DISABLED ) // BPT 7/1/01

		AfxMessageBox( "Save is disabled in this DEMO build", MB_OK );

		return true;

#else

		return pShowCase->SaveTo( io );

#endif

	}

	//
	//	SaveBPTFile()
	//
	
	bool SaveBPTFile(
		const char * filename, 
		CAnimationShowcase * pShowCase, 
		const bool bExternalVisualElements,
		const bool bXMLIOMode,
		const bool bBackup
	) {

#if defined( DEMO_SAVE_DISABLED ) // BPT 7/1/01

		AfxMessageBox( "Save is disabled in this DEMO build", MB_OK );

		return true;

#else

		// --------------------------------------------------------------------

		if ( bBackup ) {

			FILE * i = fopen( filename, "rb" );

			if ( i ) {

				fclose( i );

				char newFilename[ _MAX_PATH ];

				sprintf( newFilename, "%s-backup", filename );

				unlink( newFilename );

				rename( filename, newFilename );

			}

		}

		// --------------------------------------------------------------------

		CShowcaseFileIO io;

		char * pAccess;

		if ( bXMLIOMode ) {

			pAccess = "wt";

		} else {

			pAccess = "wb";

		}

		if ( !io.Open( filename, pAccess, bExternalVisualElements, bXMLIOMode ) ) {

			return false;

		}

		bool bResult = SaveBPTFilePrim( &io, pShowCase );

		io.Close();

		return bResult;

#endif

	}

}; // namespace BPT

