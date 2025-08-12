// BPTDib.h: interface for the BPTDib class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTDIB_H__4EE3AB65_FB91_4E19_9877_81FB6B6BDBFD__INCLUDED_)
#define AFX_BPTDIB_H__4EE3AB65_FB91_4E19_9877_81FB6B6BDBFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------------

//#define BPTDIB_USE_BOTTOM_UP_DIB

// ----------------------------------------------------------------------------

#if defined(_DEBUG)

#define BPTDIB_TRACE_DIB_CREATION

#define BPTDIB_DEBUG_CREATE( WW, HH, HHDC ) \
	Create( WW, HH, HHDC, THIS_FILE, __LINE__ )

#define BPTDIB_DEBUG_CREATE2( SSSS, HHDC ) \
	Create( SSSS, HHDC, THIS_FILE, __LINE__ )

#else

#define BPTDIB_DEBUG_CREATE( WW, HH, HHDC ) \
	Create( WW, HH, HHDC )

#define BPTDIB_DEBUG_CREATE2( SSSS, HHDC ) \
	Create( SSSS, HHDC )

#endif

// ----------------------------------------------------------------------------

#include "BPTBitmap.h"

// ----------------------------------------------------------------------------

namespace BPT {

#if defined(BPTDIB_TRACE_DIB_CREATION)

	extern int g_ExistingDibSections;

#endif

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	Abstract Dib base type
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class ADib {

		virtual void Destroy() = 0;

#if defined(_DEBUG)

		virtual bool Create(
			const int width, const int height, HDC hdc = static_cast<HDC>(0),
			const char * sourceFile = THIS_FILE, const int lineNumber = __LINE__
		) = 0;

		virtual bool Create(
			const SIZE & size, HDC hdc = static_cast<HDC>(0),
			const char * sourceFile = THIS_FILE, const int lineNumber = __LINE__
		) = 0;

#else

		virtual bool Create( const int width, const int height, HDC hdc = static_cast<HDC>(0) ) = 0;
		virtual bool Create( const SIZE & size, HDC hdc = static_cast<HDC>(0) ) = 0;

#endif

		virtual int BitDepth() const = 0;
		virtual RECT Rect() const = 0;
		virtual SIZE Size() const = 0;
		virtual int Width() const = 0;
		virtual int Height() const = 0;
		virtual int Pitch() const = 0;
		virtual HBITMAP GetHBitmap() const = 0;
		virtual const BITMAPINFO * GetBITMAPINFO() const = 0;
		virtual const void * GetBitmapBits() const = 0;

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// DIBSECTION wrapper (16, 24, or 32 top down dib's only!!!)
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class PIXELTYPE, const int ALIGNMENT = 4 >
	class TDIBSection : public ADib {

	public: // traits

		typedef TSimpleBitmap<PIXELTYPE> bitmap_type;
		typedef TDIBSection<PIXELTYPE> this_type;
		typedef this_type & reference;
		typedef this_type * pointer;
		typedef const this_type & const_reference;
		typedef const this_type * const_pointer;

	private: // data

		BITMAPINFO m_BitmapInfo;
		void * m_BitmapBits;
		HBITMAP m_hBitmap;
		bitmap_type m_FakeBitmap;
		SIZE m_Size;
		int m_Pitch;

#if defined(_DEBUG)

		const char * m_SourceFile;
		int m_LineNumber;

#endif

	private: // internal methods

		int CalcPitch( const int width ) {

			int rawPitch = width * sizeof( PIXELTYPE );

			int leftOver = (rawPitch % ALIGNMENT);

			if ( !leftOver ) {

				return rawPitch;

			}

			return rawPitch + (ALIGNMENT - leftOver);

		}

	public: // interface

		// --------------------------------------------------------------------

		TDIBSection() : m_hBitmap(0), m_Pitch(0) {

			::ZeroMemory( &m_BitmapInfo, sizeof(m_BitmapInfo) );

			m_Size.cx = 0;
			m_Size.cy = 0;
		
#if defined(_DEBUG)

			m_SourceFile = 0;
			m_LineNumber = 0;

#endif

		}

		virtual ~TDIBSection() {

			Destroy();

		}

		// --------------------------------------------------------------------

		virtual void Destroy() {

			m_FakeBitmap.Destroy();

			if ( m_hBitmap ) {

#if defined(BPTDIB_TRACE_DIB_CREATION)

				TRACE( "Attempting to destroy dib (%d x %d x %dbpp) (created by %s line %d)\n",
					Width(), Height(), BitDepth(), m_SourceFile, m_LineNumber
				);

#endif

				// Ask GDI to destroy it's concept of this bitmap

				if ( !::DeleteObject( (HGDIOBJ) m_hBitmap ) ) {

					TRACE( "TDIBSection::Destroy() failed to delete HBITMAP GDI object (%08x)\n", (DWORD)m_hBitmap );

				}

				// Release all of our information

				::ZeroMemory( &m_BitmapInfo, sizeof(m_BitmapInfo) );

				m_BitmapBits = 0;
				m_hBitmap = 0;

#if defined(BPTDIB_TRACE_DIB_CREATION)

				if ( --g_ExistingDibSections ) {

					TRACE( "%d Allocated dibs remain\n", g_ExistingDibSections );

				} else {

					TRACE( "ALL ALLOCATED DIB'S HAVE BEEN DESTROYED\n" );

				}

#endif

			}

			m_Size.cx = 0;
			m_Size.cy = 0;

#if defined(_DEBUG)

			m_SourceFile = 0;
			m_LineNumber = 0;

#endif

		}

		// --------------------------------------------------------------------

#if defined(_DEBUG)

		bool Create(
			const int width, const int height, HDC hdc = static_cast<HDC>(0),
			const char * sourceFile = THIS_FILE, const int lineNumber = __LINE__
		) {

#else

		bool Create( const int width, const int height, HDC hdc = static_cast<HDC>(0) ) {

#endif

			// Release any previous information
			// ----------------------------------------------------------------

			Destroy();

			// Fill in the bitmap info structure 
			// ----------------------------------------------------------------

			::ZeroMemory( &m_BitmapInfo, sizeof(m_BitmapInfo) );

			m_BitmapInfo.bmiHeader.biSize = sizeof(m_BitmapInfo.bmiHeader);
			m_BitmapInfo.bmiHeader.biBitCount = (sizeof(PIXELTYPE) * 8);
			m_BitmapInfo.bmiHeader.biClrUsed = 0;
			m_BitmapInfo.bmiHeader.biClrImportant = 0;
			m_BitmapInfo.bmiHeader.biCompression = BI_RGB;
			m_BitmapInfo.bmiHeader.biSizeImage = 0;
			m_BitmapInfo.bmiHeader.biPlanes = 1;
			m_BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
			m_BitmapInfo.bmiHeader.biYPelsPerMeter = 0; 
			m_BitmapInfo.bmiHeader.biWidth = width;

#if defined(BPTDIB_USE_BOTTOM_UP_DIB)
			m_BitmapInfo.bmiHeader.biHeight = height;
#else
			m_BitmapInfo.bmiHeader.biHeight = -height;
#endif

			// Figure out what hdc to use
			// ----------------------------------------------------------------

			HDC usehdc = hdc;

			if ( !hdc ) {

				usehdc = GetDC( NULL );

			}

			// Ask GDI to create a DIBSECTION
			// ----------------------------------------------------------------

			m_hBitmap = ::CreateDIBSection( 
				usehdc, &m_BitmapInfo, DIB_RGB_COLORS, 
				static_cast< void ** >( &m_BitmapBits ),
				0, 0
			);

			// If we created the hdc then destroy it.
			// ----------------------------------------------------------------

			if ( !hdc ) {

				ReleaseDC( NULL, usehdc );

			}

			// Failure?
			// ----------------------------------------------------------------

			if ( !m_hBitmap ) {

				return false;

			}

			m_Pitch = CalcPitch( width );

			m_Size.cx = width;
			m_Size.cy = height;

			// Deal with bottom up and top down dib mapping differences
			// ----------------------------------------------------------------

#if defined(BPTDIB_USE_BOTTOM_UP_DIB)

			BYTE * pFirstScanline = ((BYTE *)m_BitmapBits) + ((m_Size.cy - 1) * m_Pitch);

			if ( !m_FakeBitmap.Map(
				(BYTE *)pFirstScanline, m_Size, -m_Pitch, false, false) ) {

				Destroy();

				return false;

			}

#else

			if ( !m_FakeBitmap.Map(
				(BYTE *)m_BitmapBits, m_Size, m_Pitch, false, false) ) {

				Destroy();

				return false;

			}

#endif
			
#if defined(_DEBUG)

			m_SourceFile = sourceFile;
			m_LineNumber = lineNumber;

#endif

#if defined(BPTDIB_TRACE_DIB_CREATION)

			++g_ExistingDibSections;

			TRACE( 
				"Allocating dib number %d (%d x %d x %dbpp) (created by %s line %d)\n", 
				g_ExistingDibSections, Width(), Height(), BitDepth(), m_SourceFile, m_LineNumber
			);

#endif

			return true;

		}

#if defined(_DEBUG)

		bool Create(
			const SIZE & size, HDC hdc = static_cast<HDC>(0),
			const char * sourceFile = THIS_FILE, const int lineNumber = __LINE__
		) {

			return Create( size.cx, size.cy, hdc, sourceFile, lineNumber );

		}

#else

		bool Create( const SIZE & size, HDC hdc = static_cast<HDC>(0) ) {

			return Create( size.cx, size.cy, hdc );

		}

#endif

		// Query
		// --------------------------------------------------------------------

		int BitDepth() const {

			return m_BitmapInfo.bmiHeader.biBitCount;

		}

		int Pitch() const {

			return m_Pitch;

		}

		RECT Rect() const {

			RECT rect = { 0, 0, m_Size.cx, m_Size.cy };

			return rect;

		}

		SIZE Size() const {

			return m_Size;

		}

		int Width() const {

			return m_Size.cx;

		}

		int Height() const {

			return m_Size.cy;

		}

		// --------------------------------------------------------------------

		HBITMAP GetHBitmap() const {

			return m_hBitmap;

		}

		const BITMAPINFO * GetBITMAPINFO() const {

			return &m_BitmapInfo;

		}

		const void * GetBitmapBits() const {

			return m_BitmapBits;

		}

		// --------------------------------------------------------------------

		bitmap_type * GetBitmapPtr() {

			return &m_FakeBitmap;

		}

	};

	// ========================================================================

	//
	//	TDibDC<>
	//

	template< class T >
	class TDibDC : public CDC {

	public: // traits

		typedef TDibDC<T> this_type;

		typedef T dib_type;

	private:

		dib_type * m_pDib;

		CBitmap * m_pOldBitmap;

		bool m_bSetup;

	public:

		TDibDC( dib_type * pDib ) : m_pDib( pDib ), m_bSetup( false ), m_pOldBitmap( 0 ) {

			if ( pDib ) {

				if ( CreateCompatibleDC( NULL ) ) {

					m_pOldBitmap = SelectObject(
						CBitmap::FromHandle( pDib->GetHBitmap() )
					);

					m_bSetup = true;

				}

			}
		
		}

		~TDibDC() {

			if ( m_bSetup && m_pOldBitmap ) {

				SelectObject( m_pOldBitmap );

			}

		}

		bool IsSetup() const {

			return m_bSetup;

		}

	};

	// ========================================================================

	//
	//	DIBSECTION_BlitAt()
	//
	//	-- This should really call another primitive, but for now this
	//	-- will work just fine :-)
	//

	template< class DIBSECTION > void
	T_BlitAt(
		HDC hdc, const int x, const int y, DIBSECTION & dibSection,
		const float xScale = 1.0f, const float yScale = 1.0f,
		const RECT * pOptionalSrcSubRect = 0
	) {

		// Determine the operation size

		RECT srcLimitRect = dibSection.Rect();

		RECT operationRect;

		if ( pOptionalSrcSubRect ) {

			if ( !IntersectRect( &operationRect, pOptionalSrcSubRect, &srcLimitRect ) ) {

				return /* NOP */;

			}

		} else {

			operationRect = srcLimitRect;

		}

		// Get down to business

		HBITMAP hBitmap = dibSection.GetHBitmap();

		if ( hBitmap ) {

			int w = operationRect.right - operationRect.left;
			int h = operationRect.bottom - operationRect.top;

#if 0

			int oldMode = SetStretchBltMode( hdc, STRETCH_DELETESCANS );

			StretchDIBits(
				hdc, x, y,
				(int)(w * xScale + 0.5f),
				(int)(h * yScale + 0.5f),
				operationRect.left,
				operationRect.top,
				w,
				h,
				dibSection.GetBitmapBits(),
				dibSection.GetBITMAPINFO(),
				DIB_RGB_COLORS,
				SRCCOPY
			);

			SetStretchBltMode( hdc, oldMode );

#else

			HDC memDC = ::CreateCompatibleDC( hdc );

			if ( memDC ) {

				HBITMAP hbmOld = (HBITMAP)::SelectObject( memDC, hBitmap );

				if ( (1.0f != xScale) || (1.0f != yScale) ) {

					int oldMode = SetStretchBltMode( hdc, STRETCH_DELETESCANS );
	
					::StretchBlt(
						hdc, x, y,
						(int)(w * xScale + 0.5f),
						(int)(h * yScale + 0.5f),
						memDC,
						operationRect.left,
						operationRect.top,
						w,
						h,
						SRCCOPY
					);
	
					SetStretchBltMode( hdc, oldMode );

				} else {

					::BitBlt(
						hdc,
						x, y,
						w,
						h,
						memDC,
						operationRect.left,
						operationRect.top,
						SRCCOPY
					);

				}

				::SelectObject( memDC, hbmOld );
				::DeleteDC( memDC );

			}

#endif

		}

	}

}; // namespace BPT

#endif // !defined(AFX_BPTDIB_H__4EE3AB65_FB91_4E19_9877_81FB6B6BDBFD__INCLUDED_)
