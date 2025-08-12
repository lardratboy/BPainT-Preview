// BrushGBAExportDlg.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "BrushGBAExportDlg.h"
#include "BpaintDoc.h"
#include "BPTTools.h"
#include "ChooseSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ****************************************************************************
// ****************************************************************************

namespace BPT {

	// ========================================================================

	//
	//	TStoreSkipEncoder
	//
	
	class TStoreSkipEncoder {

	public:

		template< class CONTEXT, class EMITTER, class ITERATOR, class PREDICATE >
		bool operator()( CONTEXT & context, EMITTER & emitter, ITERATOR first, ITERATOR last, PREDICATE shouldStore ) {

			// Figure out what type of code to start with skip or store
			// ----------------------------------------------------------------
	
			EMITTER::RUNTYPE type;

			ITERATOR runStartIt;
			ITERATOR it = first;

			bool bShouldStore = shouldStore( *it++ );

			int count = 1;

			if ( bShouldStore ) {

				runStartIt = (it - 1);
				type = EMITTER::STORE;

			} else {

				runStartIt = last;
				type = EMITTER::SKIP;

			}
	
			// Run through the remaining values collecting runs
			// ----------------------------------------------------------------

			while ( it != last ) {

				bool bLastStoreValue = bShouldStore;

				bShouldStore = shouldStore( *it++ );

				if ( bShouldStore == bLastStoreValue ) {

					++count;

				} else {

					// Dump the last code and start a new one
					// --------------------------------------------------------

					if ( !emitter.EmitCode( context, type, count, runStartIt ) ) {

						return false;

					}

					// Start a new run 
					// --------------------------------------------------------

					if ( bShouldStore ) {

						runStartIt = (it - 1);
						type = EMITTER::STORE;
						count = 1;

					} else {

						runStartIt = last;
						type = EMITTER::SKIP;
						count = 1;

					}

				}
	
			}

			// Dump the last code to the code collection

			return emitter.EmitCode( context, type, count, runStartIt );
	
		}
	
	}; // class TStoreSkipEncoder

	// ========================================================================

	//
	//	TZeroRunSkipRunLiteralEmitter
	//
	//	-- 0 is the skip value, the decoder uses this to determine if it should
	//	-- write a value or not, 
	//

	template< class OUTPUT >
	class TZeroRunSkipRunLiteralEmitter {

	public:

		enum RUNTYPE { SKIP, STORE };

	private:

		OUTPUT * m_pOutput;

	public:

		TZeroRunSkipRunLiteralEmitter( OUTPUT * pOutput ) : m_pOutput( pOutput ) {}

		// --------------------------------------------------------------------

		template< class CONTEXT, class ITERATOR > bool
		EmitCode( CONTEXT & context, const RUNTYPE type, const int count, ITERATOR incommingIt ) {

			if ( SKIP == type ) {

				return m_pOutput->EmitRun( context, 0, count );

			} else if ( STORE == type ) {

				ITERATOR it = incommingIt;
				ITERATOR end = incommingIt + count;

				while ( it != end ) {

					ITERATOR start = it;

					switch ( end - it ) {

					case 1:
						return m_pOutput->EmitRun( context, *start, 1 );
						break;

					case 2:
						if ( *it == *(it + 1) ) {

							return m_pOutput->EmitRun( context, *start, 2 );

						}
						return m_pOutput->EmitLiteral( context, start, 2 );
						break;

					default:

						// try to collect a run
						// ----------------------------------------------------

						if ( (*it == *(it + 1)) && (*(it + 1) == *(it + 2)) ) {

							it += 3; // there's at least 3

							while ( it != end ) {

								if ( *it != *(it - 1) ) break;

								++it;

							}

							if ( !m_pOutput->EmitRun( context, *start, it - start ) ) {
								
								return false;

							}

							if ( end == it ) return true;

							break;

						}

						// try to collect a literal
						// ----------------------------------------------------

						while ( (*it != *(it + 1)) || (*(it + 1) != *(it + 2)) ) {

							it += 3;

							if ( 3 > (end - it) ) {

								// collect the remaining entries into the literal

								it = end;

								break;

							}

						}

						if ( !m_pOutput->EmitLiteral( context, start, it - start ) ) {
							
							return false;

						}

						if ( end == it ) return true;

						break;

					}

				}

			}

			// ----------------------------------------------------------------
			
			ASSERT( false ); // should never get here...

			return false;

		}

		// --------------------------------------------------------------------

	}; // class TZeroRunSkipRunLiteralEmitter<>

	// ========================================================================

	// write stream helpers...

	//
	//	TWriteBytes<>
	//

	template< class ITERATOR, class VALUE, const bool bLITTLE_ENDIAN = true >
	struct TWriteBytes {

		ITERATOR & operator()( ITERATOR & it, const VALUE v ) const {

			if ( bLITTLE_ENDIAN ) {

				// Little ENDIAN

				VALUE t = v;

				for ( int i = 0; i < sizeof(VALUE); i++ ) {

					*it++ = t & 0xff;

					t >>= 8;

				}

				return it;

			}

			// Big ENDIAN

			int shift = (sizeof(VALUE) - 1) * 8;

			for ( int i = 0; i < sizeof(VALUE); i++ ) {

				*it++ = (v >> shift) & 0xff;

				shift -= 8;

			}

			return it;

		}

	}; // TWriteBytes<>

	// ========================================================================

	//
	//	TRLU16AlignedU8Encoder
	//

	template< class SIZE_WORD = WORD, const int CONTROL_BITS = 8 >
	class TRLU16AlignedU8Encoder {

	public:

		typedef TRLU16AlignedU8Encoder<SIZE_WORD,CONTROL_BITS> this_type;

		enum {

			control_word_bit_count	= CONTROL_BITS
			,max_run_count			= (1 << (CONTROL_BITS - 1)) - 1

		};

	private:

		typedef TZeroRunSkipRunLiteralEmitter<this_type> run_literal_emitter_type;

	public:

		TRLU16AlignedU8Encoder() {}

		// --------------------------------------------------------------------

		template< class STORAGE_ITERATOR, class ITERATOR, class PREDICATE >
		bool Encode( STORAGE_ITERATOR & storage, ITERATOR first, ITERATOR last, PREDICATE shouldStore ) {

			run_literal_emitter_type runLiteralEmitter( this );

			TStoreSkipEncoder storeSkipEncoder;

			if ( !storeSkipEncoder( storage, runLiteralEmitter, first, last, shouldStore ) ) {

				return false;

			}

			return true;

		}

		// --------------------------------------------------------------------

		template< class STORAGE_ITERATOR, class ITERATOR >
		bool Encode( STORAGE_ITERATOR & storage, ITERATOR first, ITERATOR last ) {

			run_literal_emitter_type runLiteralEmitter( this );

			if ( !runLiteralEmitter.EmitCode( 
				storage, run_literal_emitter_type::STORE, last - first, first 
				) ) {

				return false;

			}

			return true;

		}

		// --------------------------------------------------------------------

		template< class STORAGE_ITERATOR, class T >
		bool EmitRun( STORAGE_ITERATOR & storage, const T value, const int count ) {

			for ( int left = count; 0 < left; ) {

				int currentLength = left;

				if ( currentLength > max_run_count ) currentLength = max_run_count;

				*storage++ = (currentLength << 1);
				*storage++ = value;

				left -= currentLength;

			}

			return true;

		}

		template< class STORAGE_ITERATOR, class T >
		bool EmitLiteral( STORAGE_ITERATOR & storage, T iterator, const int count ) {

			for ( int left = count; 0 < left; ) {

				if ( 1 == left ) {

					return EmitRun( storage, *iterator, 1 );

				}

				int currentLength = left;

				if ( currentLength > max_run_count ) currentLength = max_run_count;

				*storage++ = (currentLength << 1) | 1;

				if ( !(currentLength & 1) ) {

					*storage++ = 0; // pad

				}

				for ( int i = 0; i < currentLength; i++ ) {

					*storage++ = *iterator++;

				}

				left -= currentLength;

			}

			return true;

		}

		// --------------------------------------------------------------------

		//
		//	EncodeSurface
		//

		template<
			class STORAGE_ITERATOR
			,class SURFACE
			,class PREDICATE
		>
		bool EncodeSurface(
			STORAGE_ITERATOR & /* out */ storage
			,RECT & /* out */ encodedRect
			,SURFACE & srcSurface
			,PREDICATE predicate
			,const RECT * pRect = 0
		) {

			// Determine/limit the size of the operation and reserve elements
			// ----------------------------------------------------------------

			RECT surfaceRect = srcSurface.Rect();

			if ( pRect ) {

				if ( !IntersectRect( &surfaceRect, &surfaceRect, pRect ) ) {

					return false;

				}

			}

			// should it use a bounding rect to reduce the size of the encoded
			// area further?

			encodedRect = surfaceRect;

			SIZE size;

			size.cx = surfaceRect.right - surfaceRect.left;
			size.cy = surfaceRect.bottom - surfaceRect.top;

			// For each of the lines in the surface rect call the compressor
			// ----------------------------------------------------------------

			TWriteBytes<STORAGE_ITERATOR,SIZE_WORD> sizeWriter;

			for ( int y = 0; y < size.cy; y++ ) {

				SURFACE::pixel_iterator it = srcSurface.Iterator(
					surfaceRect.left, y + surfaceRect.top
				);

				// handle size prefix for this scanline

				STORAGE_ITERATOR fixup = storage;

				sizeWriter( storage, 0 );

				// encode the scanline

				STORAGE_ITERATOR start = storage;

				if ( !Encode( storage, it, it + size.cx, predicate ) ) {

					return false;
					
				}

				// fixup the size for this scanline

				sizeWriter( fixup, storage - start );

			}

			return true;

		}

	}; // class TRLU16AlignedU8Encoder

	// ------------------------------------------------------------------------

	//
	//	TSizeEstimationIterator<>
	//
	//	-- this gives the interface of an iterator but only refers to one location
	//

	template< class T > struct TSizeEstimationIterator {

	public: // traits

		typedef TSizeEstimationIterator<T> _Myt;
		typedef T & _Rt;
		typedef T * _Pt;
		typedef int _D;

	protected:

		_D m_nPosition;

		mutable T m_Storage;

	public:

		// construction

		TSizeEstimationIterator( const size_t nPosition = 0 ) : m_Storage(0), m_nPosition(nPosition) {}

		// make it work like a pointer

		inline _Rt operator*() const { return m_Storage; }
		inline _Pt operator->() const { return &m_Storage; }

		// comparison

		inline bool operator==( const _Myt & rhs ) const {

			return m_nPosition == rhs.m_nPosition;

		}

		inline bool operator!=( const _Myt & rhs ) const {

			return m_nPosition != rhs.m_nPosition;

		}

		// handle the simple pre/post increment & decrement operators.

		inline _Myt& operator++() { ++m_nPosition; return (*this); }
		inline _Myt operator++(int) { _Myt _Tmp = *this; ++m_nPosition; return (_Tmp); }
		inline _Myt& operator--() { --m_nPosition; return (*this); }
		inline _Myt operator--(int) { _Myt _Tmp = *this; --m_nPosition; return (_Tmp); }
		inline _Myt& operator+=(_D _N) { m_nPosition += _N; return (*this); }
		inline _Myt operator+(_D _N) const { return _Myt(m_nPosition + _N); }
		inline _Myt& operator-=(_D _N) { m_nPosition -= _N; return (*this); }
		inline _Myt operator-(_D _N) const { return _Myt(m_nPosition - _N); }
		inline _Rt operator[](_D _N) const { return m_Storage; }

		// handle the difference type

		_D operator-( const _Myt & rhs ) const {

			return m_nPosition - rhs.m_nPosition;

		}

	}; // struct TSizeEstimationIterator<>

	// ------------------------------------------------------------------------

	//
	//	TEncodedSurface
	//

	template< class STORAGE_TYPE = unsigned char >
	class TEncodedSurface {

	public:

		typedef STORAGE_TYPE storage_type;

		typedef storage_type * storage_iterator;

	private:

		storage_type * m_pData;

		bool m_bOwnsData;

		SIZE m_Size;

		int m_DataSize;

	public:

		TEncodedSurface() : m_pData(0), m_bOwnsData(false), m_DataSize(0) {

			m_Size.cx = 0;
			m_Size.cy = 0;
		
		}

		~TEncodedSurface() {

			Destroy();

		}

		void Destroy() {

			if ( m_bOwnsData && m_pData ) {

				delete [] m_pData;

			}

			m_bOwnsData = false;
			m_pData = 0;
			m_Size.cx = 0;
			m_Size.cy = 0;

		}

		bool Map( void * pData, const int cx, const int cy, const int dataSize, const bool bOwnData ) {

			Destroy();

			m_pData = reinterpret_cast<storage_type *>( pData );
			m_bOwnsData = bOwnData;
			m_DataSize = dataSize;
			m_Size.cx = cx;
			m_Size.cy = cy;

			return true;
		}

		bool Create( const int cx, const int cy, const int dataSize ) {

			Destroy();

			storage_type * pData = new storage_type [ dataSize ];

			if ( !pData ) return false;

			if ( Map( pData, cx, cy, dataSize, true ) ) {

				return true;

			}

			delete [] pData;

			return false;

		}

		storage_iterator begin() {

			return m_pData;

		}

		storage_iterator end() {

			return begin() + m_DataSize;

		}

	}; // class CEncodedSurface

	// ------------------------------------------------------------------------

	//
	//	TEncodedSurface<>
	//

	template<
		class XSURFACE
		,class ENCODER
		,class SURFACE
		,class PREDICATE
	>
	bool EncodeSurface(
		XSURFACE & encodedSurface
		,ENCODER & encoder
		,SURFACE & srcSurface
		,PREDICATE predicate
		,RECT * pRect
	) {

		RECT encodedRect;

		// ask the encoder to encode using an estimator iterator for storage
		// --------------------------------------------------------------------

		TSizeEstimationIterator<BYTE> sizeEstimator;

		if ( !encoder.EncodeSurface(
			sizeEstimator, encodedRect, srcSurface, predicate, pRect ) ) {

			return false;

		}

		int dataSize = sizeEstimator - TSizeEstimationIterator<BYTE>();

		// Okay prepare the XSURFACE
		// --------------------------------------------------------------------

		if ( !encodedSurface.Create(
			encodedRect.right - encodedRect.left,
			encodedRect.bottom - encodedRect.top,
			dataSize ) ) {

			return false;

		}

		// now call the encoder to actually store the data
		// --------------------------------------------------------------------

		RECT validateRect;

		XSURFACE::storage_iterator storage = encodedSurface.begin();

		if ( !encoder.EncodeSurface(
			storage, validateRect, srcSurface, predicate, &encodedRect ) ) {

			return false;

		}

		if ( storage != encodedSurface.end() ) {

			encodedSurface.Destroy();

			return false;

		}

		return true;

	}

	// ------------------------------------------------------------------------

	//
	//	THexDump()
	//

	template< class I >
	void THexDump(
		FILE * outFile
		,I first
		,I last
		,const char * pFmt
		,const int group
		,const int modulo
		,const int bitsPerElement
	) {

		int counter = 0;

		unsigned mask = (1 << bitsPerElement) - 1;

		fprintf( outFile, "\t" );

		while ( first != last ) {

			unsigned gValue = (*first++) & mask;

			for ( int i = 1; i < group; i++ ) {

				if ( first == last ) break;

				gValue |= ((*first++) & mask) << bitsPerElement;

			}

			fprintf( outFile, pFmt, gValue );

			if ( first != last ) {

				fprintf( outFile, ", " );

				if ( !(++counter % modulo) ) {

					fprintf( outFile, "\n\t" );

				}

			}

		}

		fprintf( outFile, "\n" );

	}

	//
	//	TGBA_DumpRLU16Encode8bppSurface()
	//

	template< class T > bool TGBA_DumpRLU16Encode8bppSurface(
		FILE * outFile
		,T & bitmap
		,const char * pszArrayName
		,const bool bHex
	) {

		// --------------------------------------------------------------------

		TRLU16AlignedU8Encoder<> encoder;

		TEncodedSurface<> encodedSurface;

		if ( !EncodeSurface(
			encodedSurface
			,encoder
			,bitmap
			,BPT::TIsNotValue<T::pixel_type>( 0 )
			,0
		) ) {

			return false;

		}

		// --------------------------------------------------------------------

		const char * pszFormat;

		if ( bHex ) {

			pszFormat = "0x%04x";

		} else {

			pszFormat = "%5d";

		}

		// --------------------------------------------------------------------

		fprintf(
			outFile, "const u16 %s[] = { // bitmap size (%d x %d) {%d vs %d}\n\t%d,%d,\n",
			pszArrayName,
			bitmap.Width(), bitmap.Height(),
			encodedSurface.end() - encodedSurface.begin(),
			bitmap.Width() * bitmap.Height(),
			bitmap.Width(), bitmap.Height()
		);

		THexDump( outFile, encodedSurface.begin(), encodedSurface.end(), pszFormat, 2, 8, 8 );

		// array footer
		// ------------------------------------------------------------------------

		fprintf( outFile, "\n};\n" );

		return true;

	}

	// ----------------------------------------------------------------------------

}; // namespace BPT

// ****************************************************************************
// ****************************************************************************


/////////////////////////////////////////////////////////////////////////////
// CBrushGBAExportDlg dialog


CBrushGBAExportDlg::CBrushGBAExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBrushGBAExportDlg::IDD, pParent)
	, m_nExportMode(0)
	, m_nBitPacking(0)
	, m_bRGB555Output(FALSE)
	, m_bGenerateShadeTable(FALSE)
	, m_nTileWidth(0)
	, m_nTileHeight(0)
{
	//{{AFX_DATA_INIT(CBrushGBAExportDlg)
	m_ArrayName = _T("g_GraphicData_");
	m_Filename = _T("");
	m_bHexMode = TRUE;
	m_bSingleDimMode = TRUE;
	m_bNibblePackedPixels = FALSE;
	m_bOutputPalette = TRUE;
	//}}AFX_DATA_INIT
}


void CBrushGBAExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrushGBAExportDlg)
	DDX_Text(pDX, IDC_ARRAY_NAME, m_ArrayName);
	DDX_Text(pDX, IDC_FILENAME, m_Filename);
	DDX_Check(pDX, IDC_HEX_CHECK, m_bHexMode);
	DDX_Check(pDX, IDC_SINGLE_DIM_CHECK, m_bSingleDimMode);
	DDX_Check(pDX, IDC_4BPP_EXPORT, m_bNibblePackedPixels);
	DDX_Radio(pDX, IDC_EXPORT_MODE1, m_nExportMode);
	DDX_Check(pDX, IDC_OUTPUT_PALETTE_CHECK, m_bOutputPalette);
	DDX_Radio(pDX, IDC_8BIT_PACKING, m_nBitPacking );
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_RGB555_OUTPUT, m_bRGB555Output);
	DDX_Check(pDX, IDC_GENERATE_SHADE_TABLE, m_bGenerateShadeTable);
	DDX_Text(pDX, IDC_TILE_W_EDIT, m_nTileWidth);
	DDX_Text(pDX, IDC_TILE_H_EDIT, m_nTileHeight);
}


BEGIN_MESSAGE_MAP(CBrushGBAExportDlg, CDialog)
	//{{AFX_MSG_MAP(CBrushGBAExportDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrushGBAExportDlg message handlers

//
//	TGBA_OutputTile
//

#if 0

template< class T > void TGBA_OutputTile(
	FILE * outFile, T & bitmap, const bool bHex, const bool bNibblePack
) {

	int h = bitmap.Height();
	int w = bitmap.Width();
	int total = (w * h);
	int next = 0;

	for ( int y = 0; y < h; ) {

		T::pixel_iterator it = bitmap.Iterator( 0, y );

		fprintf( outFile, "\t" );

		for ( int x = 0; x < w; x++ ) {

			int value = *it++;

			if ( bNibblePack ) {

				value = (value & 0x0f);

				if ( ++x < w ) {

					value |= ((*it++) & 0x0f) << 4;

				}

			}

			if ( bHex ) {

				fprintf( outFile, "0x%02x", value );

			} else {

				fprintf( outFile, "%3d", value );

			}

			if ( ++next != total ) {

				fprintf( outFile, "," );

			}

		}

		if ( ++y != h ) {

			fprintf( outFile, "\n" );

		}

	}

}

#endif

//
//	TGBA_PackedOutput8bppSurface()
//
//	-- pads each line with 0's if not enough pixels to flesh out word size
//

template< class T > void TGBA_PackedOutput8bppSurface(
	FILE * outFile,
	T & bitmap,
	const bool bHex,
	const int nBitsPerValue,
	const int nBitsPerPackedOutput
) {

	// ------------------------------------------------------------------------

	int h = bitmap.Height();
	int w = bitmap.Width();
	int total = ((w * h) * nBitsPerValue) / nBitsPerPackedOutput;
	int next = 0;

	// ------------------------------------------------------------------------

	const char * pszFormat;

	int extraCount = 0;

	if ( 8 == nBitsPerPackedOutput ) {

		if ( bHex ) {

			pszFormat = "0x%02x";

		} else {

			pszFormat = "%3d";

		}

	} else if ( 16 == nBitsPerPackedOutput ) {

		extraCount = 1;

		if ( bHex ) {

			pszFormat = "0x%04x";

		} else {

			pszFormat = "%5d";

		}

	} else {

		extraCount = 3;
		
		if ( bHex ) {

			pszFormat = "0x%08x";

		} else {

			pszFormat = "%10d";

		}

	}

	int nPixelsPerByte = 8 / nBitsPerValue;

	int subBytePixelMask = (1 << nBitsPerValue) - 1;

	// ------------------------------------------------------------------------

	for ( int y = 0; y < h; ) {

		fprintf( outFile, "\t" );

		T::pixel_iterator it = bitmap.Iterator( 0, y );

		T::pixel_iterator end = it + w;

		while ( it != end ) {

			// ----------------------------------------------------------------

			DWORD value = (*it++) & subBytePixelMask;

			{
				for ( int jj = 1; jj < nPixelsPerByte; jj++ ) {

					if ( it == end ) break;

					value |= ((*it++) & subBytePixelMask) << (jj * nBitsPerValue);

				}
			}

			for ( int extra = 1; extra <= extraCount; extra++ ) {

				if ( it == end ) break;

				DWORD packed = (*it++) & subBytePixelMask;

				for ( int kk = 1; kk < nPixelsPerByte; kk++ ) {

					if ( it == end ) break;

					packed |= ((*it++) & subBytePixelMask) << (kk * nBitsPerValue);

				}

				value |= (packed << (8 * extra));

			}

			// ----------------------------------------------------------------

			if ( bHex ) {

				fprintf( outFile, pszFormat, value );

			} else {

				fprintf( outFile, pszFormat, value );

			}

			if ( ++next != total ) {

				fprintf( outFile, "," );

			}

		}

		if ( ++y != h ) {

			fprintf( outFile, "\n" );

		}

	}

}

//
//	TGBA_SubTilePackedOutput8bppSurface()
//
//	-- pads each line with 0's if not enough pixels to flesh out word size
//

template< class T > bool TGBA_SubTilePackedOutput8bppSurface(
	FILE * outFile
	,T & bitmap
	,const bool bHex
	,const int nBitsPerValue
	,const int nBitsPerPackedOutput
	,CSize & tileSize
) {

	T tileBitmap;

	if ( !tileBitmap.Create( tileSize.cx, tileSize.cy ) ) {

		return false;

	}

	BPT::TCopyROP< T::pixel_type > top;

	int tilesWide = (bitmap.Width() + (tileSize.cx - 1)) / tileSize.cx;

	int tilesTall = (bitmap.Height() + (tileSize.cy - 1)) / tileSize.cy;

	int totalTiles = tilesWide * tilesTall;

	int currentTile = 0;

	for ( int y = 0; y < tilesTall; y++ ) {

		for ( int x = 0; x < tilesWide; x++ ) {

			// get the tile pixels
			// ----------------------------------------------------------------

			tileBitmap.ClearBuffer( 0 );

			BPT::T_Blit(
				tileBitmap, (x * -tileSize.cx), (y * -tileSize.cy), 
				bitmap, top
			);

			// output the sub tile
			// ----------------------------------------------------------------

			TGBA_PackedOutput8bppSurface(
				outFile
				,tileBitmap
				,bHex
				,nBitsPerValue
				,nBitsPerPackedOutput
			);

			if ( ++currentTile != totalTiles ) {

				fprintf( outFile, ",\n\n" );

			}

		}

	}

	return true;

}

// ----------------------------------------------------------------------------

//
//	TWriteMaskedDWORDStream()
//

template< class T > void
TWriteMaskedDWORDStream(
	FILE * outFile
	,T & bitmap
	,const char * pszArrayName
	,const bool bHex
)
{

#if 1

	if ( !BPT::TGBA_DumpRLU16Encode8bppSurface( outFile, bitmap, pszArrayName, bHex ) ) {

		fprintf( outFile, "// ERROR during encode?\n" );

	}

#else


	// Array header
	// ------------------------------------------------------------------------

	fprintf(
		outFile, "const u32 %s[] = { // bitmap size (%d x %d)\n\t%d,%d,\n",
		pszArrayName, bitmap.Width(), bitmap.Height(),
		bitmap.Width(), bitmap.Height()
	);

	// Okay 
	// ------------------------------------------------------------------------

	int h = bitmap.Height();
	int w = bitmap.Width();

	for ( int y = 0; y < h; ) {

		T::pixel_iterator it = bitmap.Iterator( 0, y );

		fprintf( outFile, "\t" );

		// need to process the bitmap in batches of 4
		// every 32 pixels it dumps a new bitmask

		for ( int x = 0; x < w; ) {

			// build mask if this x position is a multiple of 32 (or 0)
			// ----------------------------------------------------------------

			if ( !(x % 32) ) {

				DWORD mask = 0;

				int vx = x;

				T::pixel_iterator iscan = it;

				for ( int m = 0; m < 32; m++ ) {

					if ( *iscan++ ) {

						mask |= (1 << m);

					}

					if ( ++vx >= w ) {

						break;

					}

				}

				if ( bHex ) {

					fprintf( outFile, "0x%08x, ", mask );

				} else {

					fprintf( outFile, "%10d, ", mask );

				}

			}

			// collect up to 4 pixels
			// ----------------------------------------------------------------

			DWORD pixelQuad = 0;

			for ( int p = 0; p < 4; p++ ) {

				pixelQuad |= (*it++) << (p * 8); // little endian...

				if ( ++x >= w ) {

					break;

				}
			}

			// output the pixel group
			// ----------------------------------------------------------------

			if ( bHex ) {

				fprintf( outFile, "0x%08x", pixelQuad );

			} else {

				fprintf( outFile, "%10d", pixelQuad );

			}

			if ( (x < w) ) {

				fprintf( outFile, "," );

			}

		}

		if ( ++y != h ) {

			fprintf( outFile, ",\n" );

		}

	}

	// array footer
	// ------------------------------------------------------------------------

	fprintf( outFile, "\n};\n" );

#endif

}

// ----------------------------------------------------------------------------

//
//	FindIndexForClosestRGB()
//

int
FindIndexForClosestRGB(
	BPT::CAnnotatedPalette * pPal
	,const int R
	,const int G
	,const int B
)
{

	// this really should have a search slot mask, to protect palette areas!

	int colorsInPalette = pPal->TotalColors();

	__int64 closestDistanceSquared = __int64(255 * 255) * __int64(255 * 255) * __int64(255 * 255);

	int closestIndex = 0;

	for ( int i = 0; i < colorsInPalette; i++ ) {

		// calculate the distance from this slot to seach color
		// --------------------------------------------------------------------------

		int dr = pPal->GetSlot_R( i ) - R;
		int dg = pPal->GetSlot_G( i ) - G;
		int db = pPal->GetSlot_B( i ) - B;

		__int64 distSquared = __int64(dr * dr) + __int64(dg * dg) + __int64(db * db);

		if ( 0 == distSquared ) return i;

		// if closer to a match keep this one
		// --------------------------------------------------------------------------

		if ( distSquared < closestDistanceSquared ) {

			closestDistanceSquared = distSquared;

			closestIndex = i;

		}

	}

	return closestIndex;

}

//
//	Output8bppShadeTable()
//

void 
Output8bppShadeTable(
	FILE * outFile
	,const char * pszArrayName
	,BPT::CAnnotatedPalette * pPal
	,const int nShades
	,const bool bOutput16BppColors
)
{
	int colorsInPalette = pPal->TotalColors();

	if ( bOutput16BppColors ) {

		fprintf( outFile, "const u16 %s_ShadeTable[ %d * %d ] = {\n\t", pszArrayName, colorsInPalette, nShades );

	} else {

		fprintf( outFile, "const u8 %s_ShadeTable[ %d * %d ] = {\n\t", pszArrayName, colorsInPalette, nShades );

	}

	int shadeToR = 232;
	int shadeToG = 232;
	int shadeToB = 255;

	for ( int c = 0; c < colorsInPalette; c++ ) {

		// figure out the distance shade
		// --------------------------------------------------------------------

		int r = pPal->GetSlot_R( c );
		int g = pPal->GetSlot_G( c );
		int b = pPal->GetSlot_B( c );

		int brighten = 0; // 48; // 32; // 0; //32;

		r = (r * (256 + brighten)) / 256;
		g = (g * (256 + brighten)) / 256;
		b = (b * (256 + brighten)) / 256;

		r = min( 255, r );
		g = min( 255, g );
		b = min( 255, b );

		// makeup shade color

#if 1

		int dr = ((shadeToR - r) << 16) / (nShades - 1);
		int dg = ((shadeToG - g) << 16) / (nShades - 1);
		int db = ((shadeToB - b) << 16) / (nShades - 1);

#else

		int dr = (((r/8) - r) << 16) / (nShades - 1);
		int dg = (((g/8) - g) << 16) / (nShades - 1);
		int db = (((b/8) - b) << 16) / (nShades - 1);

#endif

		// how to shade?

		int sr = r << 16;
		int sg = g << 16;
		int sb = b << 16;

		int r16 = r << 16;
		int g16 = g << 16;
		int b16 = b << 16;

		// okay now build the table
		// --------------------------------------------------------------------

		for ( int s = 0; s < nShades; s++ ) {

#if 1
			// non-linear blend
			// ----------------------------------------------------------------

			int alphaBlend = (int)(256.0f * powf( (float)s/(nShades - 1), 3.0f ));

			sr = ((((shadeToR - r) * alphaBlend) / 256) + r) << 16;
			sg = ((((shadeToG - g) * alphaBlend) / 256) + g) << 16;
			sb = ((((shadeToB - b) * alphaBlend) / 256) + b) << 16;

#endif

			// find or calculate the shade
			// ----------------------------------------------------------------

			if ( bOutput16BppColors ) {

#if 0
				fprintf(
					outFile, "%c0x%04x", (c + s) ? ',' : ' ',
						(((sr >> (16 + 3)) & 0x1f) << 11)
					|	(((sg >> (16 + 2)) & 0x3f) << 5)
					|	(((sb >> (16 + 3)) & 0x1f) << 0)
				);
#elif 0
				// RGB555
				fprintf(
					outFile, "%c0x%04x", (c + s) ? ',' : ' ',
						(((sr >> (16 + 3)) & 0x1f) << 10)
					|	(((sg >> (16 + 3)) & 0x1f) << 5)
					|	(((sb >> (16 + 3)) & 0x1f) << 0)
				);
#else			
				// BGR555 (GBA)
				fprintf(
					outFile, "%c0x%04x", (c + s) ? ',' : ' ',
						(((sr >> (16 + 3)) & 0x1f) << 0)
					|	(((sg >> (16 + 3)) & 0x1f) << 5)
					|	(((sb >> (16 + 3)) & 0x1f) << 10)
				);
#endif

			} else {

				int found = FindIndexForClosestRGB( pPal, sr >> 16, sg >> 16, sb >> 16 );

				fprintf( outFile, "%c%3d", (c + s) ? ',' : ' ', found );

				// find the dither pair shade :)
				// ------------------------------------------------------------

				// future 8]

			}

			sr += dr;
			sg += dg;
			sb += db;

		}

		fprintf( outFile, "\n%c", ((c + 1) < colorsInPalette) ? '\t' : '\n' );

	}

	fprintf( outFile, "};\n\n" );
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::OnBrushExportGbaData()
//

void CBpaintDoc::OnBrushExportGbaData() 
{

	// basic prep / error checking
	// ------------------------------------------------------------------------

	CBpaintDoc::editor_bitmap_type::pointer pBrushBitmap = 
		m_pPickupBrushMediator->GetBitmapPtr();

	if ( !pBrushBitmap ) {

		return;

	}

	// setup dlg
	// ------------------------------------------------------------------------

	CBrushGBAExportDlg dlg( AfxGetMainWnd() );

	// Retrieve the settings
	// ------------------------------------------------------------------------

	dlg.m_nTileWidth = 8;
	dlg.m_nTileHeight = 8;
	dlg.m_nBitPacking = 0;

	dlg.m_nTileWidth = GLOBAL_GetSettingIntHelper( "CBrushGBAExportDlg", "m_nTileWidth", dlg.m_nTileWidth );
	dlg.m_nTileHeight = GLOBAL_GetSettingIntHelper( "CBrushGBAExportDlg", "m_nTileHeight", dlg.m_nTileHeight );
	dlg.m_nBitPacking = GLOBAL_GetSettingIntHelper( "CBrushGBAExportDlg", "m_nBitPacking", dlg.m_nBitPacking );

	// Handle the dialog
	// ------------------------------------------------------------------------

	if ( IDOK == dlg.DoModal() ) {

		// Store the settings
		// --------------------------------------------------------------------

		GLOBAL_PutSettingIntHelper( "CBrushGBAExportDlg", "m_nTileWidth", dlg.m_nTileWidth );
		GLOBAL_PutSettingIntHelper( "CBrushGBAExportDlg", "m_nTileHeight", dlg.m_nTileHeight );
		GLOBAL_PutSettingIntHelper( "CBrushGBAExportDlg", "m_nBitPacking", dlg.m_nBitPacking );

		// process
		// --------------------------------------------------------------------

		CBpaintDoc::editor_bitmap_type tileBitmap;

		SIZE tileSize;

		tileSize.cx = dlg.m_nTileWidth;
		tileSize.cy = dlg.m_nTileHeight;

		if ( !tileBitmap.Create( tileSize.cx, tileSize.cy ) ) {

			AfxMessageBox( "Unable to create working bitmap, you should try to save and quit NOW!!!", MB_ICONERROR | MB_OK );

			return; // this is an error

		}

		// Begin the file output
		// --------------------------------------------------------------------

		FILE * outFile = fopen( dlg.m_Filename, "wt" );

		if ( NULL != outFile ) {

			// Header
			// ----------------------------------------------------------------

			fprintf( outFile, "\n// File \"%s\" exported by Bpaint\n\n", dlg.m_Filename );

			// should the palette be more complex than this? (does this work
			// with all the multiple palette stuff?)
			// ----------------------------------------------------------------

			BPT::CAnnotatedPalette * pPal = GetCurrentPalette();

			if ( pPal && dlg.m_bOutputPalette ) {

				int nTotalColors = pPal->TotalColors();

				fprintf( outFile, "const u16 %s_Palette[ %d ] = {\n\t", dlg.m_ArrayName, nTotalColors );

				for ( int i = 0; i < nTotalColors; i++ ) {

					fprintf( outFile, "0x%04x",
						((pPal->GetSlot_B(i) >> 3) << 10) |
						((pPal->GetSlot_G(i) >> 3) <<  5) |
						((pPal->GetSlot_R(i) >> 3) <<  0)
					);

					if ( (nTotalColors - 1) != i ) {

						fprintf( outFile, "," );

					}

					if ( (0 == ((i + 1) % 16)) ) {

						fprintf( outFile, "\n" );

						if ( (nTotalColors - 1) != i ) {

							fprintf( outFile, "\t" );

						}

					}

				}

				fprintf( outFile, "};\n\n" );

				// output shade table
				// ------------------------------------------------------------

				if ( dlg.m_bGenerateShadeTable ) {

					Output8bppShadeTable( outFile, dlg.m_ArrayName, pPal, 32, true );

				}

			}

			// ----------------------------------------------------------------

			const char * pszArrayType;

			int nPackOutputIntoNBits;

			switch ( dlg.m_nBitPacking ) {

			default:
			case 0:
				pszArrayType = "u8";
				nPackOutputIntoNBits = 8;
				break;

			case 1:
				pszArrayType = "u16";
				nPackOutputIntoNBits = 16;
				break;

			case 2:
				pszArrayType = "u32";
				nPackOutputIntoNBits = 32;
				break;

			}

			int bytesPerOutputPackage = (nPackOutputIntoNBits + 7) / 8;
//			int bytesPerOutputMask = ~(bytesPerOutputPackage - 1);

			// ----------------------------------------------------------------

			int nBpp = 8;

			if ( TRUE == dlg.m_bNibblePackedPixels ) {

				nBpp = 4;

			}

			// ----------------------------------------------------------------

			if ( 2 == dlg.m_nExportMode ) { // simple stream

				int packedWidth = pBrushBitmap->Width();

//				if ( FALSE != dlg.m_bNibblePackedPixels ) packedWidth = (packedWidth + 1) / 2;

				int elementsPerByte = 8 / nBpp;

				packedWidth = 
					(packedWidth + bytesPerOutputPackage - 1) / bytesPerOutputPackage;

				packedWidth /= elementsPerByte;

				fprintf(
					outFile, "const %s %s[ %d * %d ] = { // bitmap size (%d x %d)\n\n",
					pszArrayType,
					dlg.m_ArrayName,
					packedWidth,
					pBrushBitmap->Height(),
					pBrushBitmap->Width(),
					pBrushBitmap->Height()
				);

				TGBA_PackedOutput8bppSurface(
					outFile,
					*pBrushBitmap,
					(TRUE == dlg.m_bHexMode),
					nBpp,
					nPackOutputIntoNBits
				);

				// array footer
				// ----------------------------------------------------------------

				fprintf( outFile, "\n};\n" );

			} else if ( 1 == dlg.m_nExportMode ) { // DWORD Stream

				TWriteMaskedDWORDStream(
					outFile, *pBrushBitmap, dlg.m_ArrayName, (TRUE == dlg.m_bHexMode)
				);

			} else if ( 0 == dlg.m_nExportMode ) { // GBA TWxTH tiles

				// Output each tile
				// ----------------------------------------------------------------

				int tilesWide = (pBrushBitmap->Width() + (tileSize.cx - 1)) / tileSize.cx;
				int tilesTall = (pBrushBitmap->Height() + (tileSize.cy - 1)) / tileSize.cy;
				int totalTiles = (tilesWide * tilesTall);

				CBpaintDoc::paint_brush_transfer_op_type top; 

				top.SetColorKey( m_BrushChromaKey );

				// array header
				// ----------------------------------------------------------------

				int packedWidth = tileSize.cx; // pBrushBitmap->Width();

//				if ( FALSE != dlg.m_bNibblePackedPixels ) packedWidth = (packedWidth + 1) / 2;

				int elementsPerByte = 8 / nBpp;

				packedWidth = 
					(packedWidth + bytesPerOutputPackage - 1) / bytesPerOutputPackage;

				packedWidth /= elementsPerByte;

				// ----------------------------------------------------------------

				if ( FALSE == dlg.m_bSingleDimMode ) {

					fprintf(
						outFile, "const %s %s[ %d ][ %d * %d ] = { // bitmap size (%d x %d)\n\n",
						pszArrayType,
						dlg.m_ArrayName,
						totalTiles,
						packedWidth,
						tileSize.cy, 
						pBrushBitmap->Width(),
						pBrushBitmap->Height()
					);

				} else {

					fprintf(
						outFile, "const %s %s[ %d * (%d * %d) ] = { // bitmap size (%d x %d)\n\n",
						pszArrayType,
						dlg.m_ArrayName,
						totalTiles,
						packedWidth,
						tileSize.cy,
						pBrushBitmap->Width(),
						pBrushBitmap->Height()
					);

				}

				// do the tiles
				// ----------------------------------------------------------------

#if 1 // SUB TILES BPT 4/14/03
				CSize subTileSize = tileBitmap.Size();

				if ( !GetSizeDialog(
					AfxGetMainWnd(), "Choose subtile size", subTileSize, &CSize( tileBitmap.Width(), tileBitmap.Height() ), "SizeSubTileDlg", false ) ) {

					subTileSize = tileBitmap.Size();

				}
#endif

				int tileCounter = 0;

				for ( int v = 0; v < tilesTall; v++ ) {

					for ( int h = 0; h < tilesWide; h++ ) {

						// Fill the tile bitmap with tile data
						// --------------------------------------------------------

						tileBitmap.ClearBuffer( 0 );

						BPT::T_Blit(
							tileBitmap, (h * -tileSize.cx), (v * -tileSize.cy), 
							*pBrushBitmap, top
						);

						// Tile footer
						// --------------------------------------------------------

						if ( FALSE == dlg.m_bSingleDimMode ) {

							fprintf( outFile, "{ // Tile %d\n", tileCounter );

						} else {

							fprintf( outFile, "// Tile %d\n", tileCounter );

						}

						// Do the simple output
						// --------------------------------------------------------

#if 1 // SUB TILES BPT 4/14/03

						if ( !TGBA_SubTilePackedOutput8bppSurface(
							outFile,
							tileBitmap,
							(TRUE == dlg.m_bHexMode),
							nBpp,
							nPackOutputIntoNBits,
							subTileSize
							) ) {

							fprintf( outFile, "\n\n<<<Error during tile?>>>\n\n" );

						}

#else

						TGBA_PackedOutput8bppSurface(
							outFile,
							tileBitmap,
							(TRUE == dlg.m_bHexMode),
							nBpp,
							nPackOutputIntoNBits
						);

#endif

						// Tile footer
						// --------------------------------------------------------

						if ( FALSE == dlg.m_bSingleDimMode ) {

							fprintf( outFile, "\n}" );

						}

						// --------------------------------------------------------

						++tileCounter;

						if ( tileCounter != totalTiles ) {

							fprintf( outFile, ",\n" );

							if ( TRUE == dlg.m_bSingleDimMode ) {

								fprintf( outFile, "\n" );

							} else {

							}

						} else {

							fprintf( outFile, "\n" );

						}

					}

				}

				// array footer
				// ----------------------------------------------------------------

				fprintf( outFile, "\n};\n" );

			}

			// close up shop 'cause we're done!
			// ----------------------------------------------------------------

			fclose( outFile );

		}
	}
	
}

//
//	CBpaintDoc::OnUpdateBrushExportGbaData()
//

void CBpaintDoc::OnUpdateBrushExportGbaData(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (0 != m_pPickupBrushMediator->GetBitmapPtr()) ? TRUE : FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CBrushGBAExportDlg message handlers

void CBrushGBAExportDlg::OnBrowse() 
{
	CFileDialog opf( 
		TRUE, _T("*.c"), NULL, OFN_LONGNAMES, 
		_T(
			"All Supported Formats (*.cpp,*.h,*.c)|*.cpp;*.h;*.c|"
			"CPP Files (*.cpp)|*.cpp|"
			"H Files (*.h)|*.h|"
			"All Files (*.*)|*.*|"
			"|"
		)
		,this
	);

	if ( IDOK == opf.DoModal() ) {

		SetDlgItemText( IDC_FILENAME,  opf.GetPathName() );

	}
	
}

// ============================================================================

void CBpaintDoc::OnAnimSpecialExportToCode()
{
	// Choose the filename
	// ------------------------------------------------------------------------

	CFileDialog opf(
		TRUE, _T("*.c"), NULL, OFN_LONGNAMES, 
		_T(
			"All Supported Formats (*.cpp,*.h,*.c)|*.cpp;*.h;*.c|"
			"CPP Files (*.cpp)|*.cpp|"
			"H Files (*.h)|*.h|"
			"All Files (*.*)|*.*|"
			"|"
		)
		,AfxGetMainWnd()
	);

	if ( IDOK != opf.DoModal() ) {

		return;

	}

	// Determine the name of the array
	// ------------------------------------------------------------------------

	// Okay let's open up the file and start appending data
	// ------------------------------------------------------------------------

}

void CBpaintDoc::OnUpdateAnimSpecialExportToCode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_pAnimation ? TRUE : FALSE );
}
