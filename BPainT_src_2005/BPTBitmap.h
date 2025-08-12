// BPTBitmap.h: interface for the BPTBitmap class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	This file contains the surface interface template and simplistic
//	pixel types.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTBITMAP_H__B21246CA_A04D_44AE_BD3B_AD720ED5ABCC__INCLUDED_)
#define AFX_BPTBITMAP_H__B21246CA_A04D_44AE_BD3B_AD720ED5ABCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iterator>

namespace BPT {

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Pixel type
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	typedef unsigned long PIXEL_RGB32;
	typedef unsigned short PIXEL_RGB16;
	typedef unsigned char PIXEL_8;

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// iterator replacement
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#if !defined(BPT_USE_STL)

	//
	//	TMYRIterator
	//

	template<
		class _RI,
		class _Ty,
		class _Rt = _Ty &,
		class _Pt = _Ty *,
		class _D = size_t
	> struct TMYRIterator {

	// traits

	typedef TMYRIterator<_RI,_Ty,_Rt,_Pt,_D> _Myt;

	// construction

	TMYRIterator() {}

	explicit TMYRIterator(_RI _X) : current(_X) {}

	// make it work like a pointer

	__forceinline _Rt operator*() const { return (*(current - 1)); }
	__forceinline _Pt operator->() const { return (&**this); }
	__forceinline _Myt& operator++() { --current; return (*this); }
	__forceinline _Myt operator++(int) { _Myt _Tmp = *this; --current; return (_Tmp); }
	__forceinline _Myt& operator--() { ++current; return (*this); }
	__forceinline _Myt operator--(int) { _Myt _Tmp = *this; ++current; return (_Tmp); }
	__forceinline _Myt& operator+=(_D _N) { current -= _N; return (*this); }
	__forceinline _Myt operator+(_D _N) const { return (_Myt(current - _N)); }
	__forceinline _Myt& operator-=(_D _N) { current += _N ; return (*this); }
	__forceinline _Myt operator-(_D _N) const { return (_Myt(current + _N)); }
	__forceinline _Rt operator[](_D _N) const { return (*(*this + _N)); }
	__forceinline bool operator==( const _Myt & rhs ) const { return (current == rhs.current); }
	__forceinline bool operator!=( const _Myt & rhs ) const { return (current != rhs.current); }
	__forceinline int operator-( const _Myt & rhs ) const { return rhs.current - current; }

	protected:

		_RI current;

	};

#endif

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Bitmap type
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class T > class TSimpleBitmap {

	private:

		SIZE m_Size;
		BYTE * m_Data;
		int m_DataSize;
		int m_Pitch;
		bool m_OwnsData;

	public:

		// Traits
		// --------------------------------------------------------------------

		typedef TSimpleBitmap<T> this_type;
		typedef this_type & reference;
		typedef this_type * pointer;
		typedef const this_type & const_reference;
		typedef const this_type * const_pointer;

		// Iterators etc.
		// --------------------------------------------------------------------

		typedef T pixel_type;
		typedef pixel_type * pixel_iterator;

#if defined(BPT_USE_STL)

		typedef std::reverse_iterator<
			pixel_iterator, pixel_type, pixel_type &, pixel_type *, ptrdiff_t
		> reverse_iterator;

#else

		typedef TMYRIterator< pixel_iterator, pixel_type > reverse_iterator;

#endif

		// construction / destruction
		// --------------------------------------------------------------------

		TSimpleBitmap() {

			m_DataSize = 0;
			m_Pitch = 0;
			m_OwnsData = false;
			m_Data = 0;
			m_Size.cx = 0;
			m_Size.cy = 0;

		}

		~TSimpleBitmap() {

			Destroy();

		}

		// Setup / shutdown
		// --------------------------------------------------------------------

		bool Map(
			BYTE * data, 
			const SIZE & size, 
			const int pitch, 
			const bool ownData = false,
			const bool callDestroy = true
		) {

			if ( callDestroy ) {

				Destroy();

			}

			m_OwnsData = ownData;
			m_Data = data;
			m_Size = size;
			m_Pitch = pitch;
			m_DataSize = (pitch * size.cy);

			return true;

		}

		bool Create( const SIZE & size, const int forcePitchValue = 0 ) {

			Destroy();

			int pitch = (size.cx * sizeof(T));

			if ( forcePitchValue ) {

				if ( pitch > forcePitchValue ) {

					return false;

				}

				pitch = forcePitchValue;

			}

			BYTE * pData = new BYTE [ pitch * size.cy ];

			if ( !pData ) {

				return false;

			}

			return Map( pData, size, pitch, true, false );

		}

		bool Create( const int w, const int h, const int forcePitchValue = 0 ) {

			SIZE size = { w, h };

			return Create( size, forcePitchValue );

		}

		void Destroy() {

			if ( m_Data && m_OwnsData ) {

				delete [] m_Data;

				m_OwnsData = false;

			}

			m_Data = 0;
			m_Pitch = 0;
			m_DataSize = 0;
			m_Size.cx = 0;
			m_Size.cy = 0;

		}

		// Misc operations
		// --------------------------------------------------------------------

		void ClearBuffer( const T value ) {

			if ( m_Data ) {
	
				for ( int y = 0; y < m_Size.cy; y++ ) {

					pixel_iterator it = Iterator( 0, y );

					for ( int x = 0; x < m_Size.cx; x++ ) {

						*it++ = value;

					}

				}

			}

		}

		// Query
		// --------------------------------------------------------------------

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

		int Pitch() const {

			return m_Pitch;

		}

		// Simplistic iterator concept.
		// --------------------------------------------------------------------

		pixel_iterator Iterator( const int x, const int y ) {

			if ( m_Data ) {

				return pixel_iterator(
					reinterpret_cast<T *>(
						m_Data + ((x * sizeof(T)) + (y * m_Pitch))
					)
				);

			}

			return pixel_iterator( 0 );

		}

		reverse_iterator rIterator( const int x, const int y ) {

			return reverse_iterator( Iterator( x + 1, y ) );

		}

		// --------------------------------------------------------------------

		void LineAdvanceIterator( pixel_iterator & it, const int multiplier = 1 ) {

			it = reinterpret_cast<T *>(
				(reinterpret_cast< BYTE * >(it) + (m_Pitch * multiplier) )
			);

		}

		// --------------------------------------------------------------------

		template< class SURFACE >
		bool Clone( SURFACE & what ) {

			if ( !Create( what.Width(), what.Height(), what.Pitch() ) ) {

				return false;

			}

			T_Blit( *this, 0, 0, what, TCopyROP<pixel_type,SURFACE::pixel_type>() );

			return true;

		}

		// --------------------------------------------------------------------

		template< class SURFACE >
		bool CreateFromRect( SURFACE & from, RECT & rect ) {

			if ( this == &from ) return false;

			if ( IsRectEmpty( &rect ) ) return false;

			if ( !Create( rect.right - rect.left, rect.bottom - rect.top ) ) {

				return false;

			}

			T_Blit( *this, -rect.left, -rect.top, from, TCopyROP<pixel_type,SURFACE::pixel_type>() );

			return true;

		}
	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	CLUT table
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TPow2Clut
	//

	template< class O, const int POW2 = 8 >
	class TPow2Clut {

	public: // traits

		typedef O output_type;

		enum {

			TABLE_SIZE = (1 << POW2)
			,INDEX_MASK = (1 << POW2) - 1

		};

	private: // data

		output_type m_Table[ TABLE_SIZE ];

	public: // interface

		output_type & RawLookup( const int i ) {

			return m_Table[ i ];

		}

		output_type & SafeLookup( const int i ) {

			return RawLookup( i & INDEX_MASK );

		}

		output_type & operator[]( const int i ) {

#ifdef _DEBUG

			return SafeLookup( i );

#else

			return RawLookup( i );

#endif

		}

		bool Included( const output_type & value ) const {

			for ( int i = 0; i < TABLE_SIZE; i++ ) {

				if ( value == m_Table[ i ] ) {

					return true;

				}

			}

			return false;

		}

		int FindIndex( const output_type & value, const int defValue ) const {
			
			for ( int i = 0; i < TABLE_SIZE; i++ ) {

				if ( value == m_Table[ i ] ) {

					return i;

				}

			}

			return defValue;

		}

		// Set
		// --------------------------------------------------------------------

		void Set( const int index, const output_type & output ) {

			m_Table[ (index & INDEX_MASK) ] = output;

		}

		void SetN( const int startIndex, const int count, const output_type & output ) {

			int index = startIndex;

			for ( int i = 0; i < count; i++ ) {

				Set( index++, output );

			}

		}

		// --------------------------------------------------------------------

		output_type * GetLowLevelTablePtr() {

			return m_Table;

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	256 color picture class
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class C8bppPicture : public TSimpleBitmap<PIXEL_8> {

	public: // Traits

		typedef C8bppPicture this_type;
		typedef TSimpleBitmap<PIXEL_8> base_type;
		typedef base_type::pixel_type pixel_type;
		typedef base_type::pixel_iterator pixel_iterator;
		typedef base_type::reverse_iterator reverse_iterator;
		typedef RGBQUAD color_value_type;

		enum {

			PALETTE_ENTRIES	= 256

		};
			
	private: // Data

	public: // interface

		color_value_type m_PaletteData[ PALETTE_ENTRIES ];

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Misc creation helpers
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	T_Create90DegreeRotatedBitmap()
	//

	template< class T >
	T * T_Create90DegreeRotatedBitmap( T & src ) {

		// Create and size the output bitmap
		// --------------------------------------------------------------------

		T * pNew = new T;

		if ( !pNew ) {

			return 0;

		}

		int src_w = src.Width();
		int src_h = src.Height();

		if ( !pNew->Create( src_h, src_w ) ) {

			delete pNew;

			return 0;

		}

		// Now go through the process of transfering the pixels...
		// --------------------------------------------------------------------

		for ( int y = 0; y < src_w; y++ ) {

			T::pixel_iterator dstIT = pNew->Iterator( 0, y );

			T::pixel_iterator srcIT = src.Iterator( y, src_h - 1 );

			for ( int x = 0; x < src_h; x++ ) {

				*dstIT++ = *srcIT;

				src.LineAdvanceIterator( srcIT, -1 );

			}

		}

		// --------------------------------------------------------------------

		return pNew;

	}

	// ------------------------------------------------------------------------

	//
	//	T_CreateScaledBitmap()
	//
	//	-- This is really cheesy!
	//

	template< class T, const int SHIFT >
	T * T_CreateScaledBitmap( T & src, const int w, const int h ) {

		// Validate
		// --------------------------------------------------------------------

		if ( (0 >= w) || (0 >= h) ) {

			return 0;

		}

		// Create and size the output bitmap
		// --------------------------------------------------------------------

		T * pNew = new T;

		if ( !pNew ) {

			return 0;

		}

		if ( !pNew->Create( w, h ) ) {

			delete pNew;

			return 0;

		}

		// Setup the fractional step values
		// --------------------------------------------------------------------

		int wd = (1 != w) ? w - 1 : 1;
		int xd = ((src.Width() - 1) << SHIFT) / wd;

		int hd = (1 != h) ? h - 1 : 1;
		int yd = ((src.Height() - 1) << SHIFT) / hd;

		// Now go through the process of transfering the pixels...
		// This assumes that the pixel_iterator type is random access!
		// --------------------------------------------------------------------

//		int xBias = (w > src.Width()) ? ((1 << (SHIFT - 1))) : 0;
//		int yBias = (h > src.Height()) ? ((1 << (SHIFT - 1))) : 0;

		// --------------------------------------------------------------------

		int yf = 0; // yBias;

		for ( int y = 0; y < h; y++ ) {

			T::pixel_iterator dstIT = pNew->Iterator( 0, y );

			T::pixel_iterator srcIT = src.Iterator( 0, ((yf + (1 << (SHIFT - 1))) >> SHIFT) );
//			T::pixel_iterator srcIT = src.Iterator( 0, ((yf + yBias) >> SHIFT) );

			yf += yd;

			int xf = 0; // xBias;

			for ( int x = 0; x < w; x++ ) {

				*dstIT++ = *(srcIT + ((xf + (1 << (SHIFT - 1))) >> SHIFT));
//				*dstIT++ = *(srcIT + ((xf + xBias) >> SHIFT));

				xf += xd;

			}

		}

		// --------------------------------------------------------------------

		return pNew;

	}

	// ------------------------------------------------------------------------

	//
	//	T_CreateOutlinedBitmap<>
	//

	template< class T >
	T * T_CreateOutlinedBitmap(
		T & src, const typename T::pixel_type chromakey, const typename T::pixel_type outlineColor
	) {

		// Create and size the output bitmap
		// --------------------------------------------------------------------

		T * pNew = new T;

		if ( !pNew ) {

			return 0;

		}

		if ( !pNew->Create( src.Width() + 2, src.Height() + 2 ) ) {

			delete pNew;

			return 0;

		}

		// --------------------------------------------------------------------

		pNew->ClearBuffer( chromakey );

		typedef BPT::TMultiModeBrushTransferROP< T::pixel_type > helper_type;

		// top

		BPT::T_Blit( 
			*pNew, 1, 0, src, helper_type(
				helper_type::SINGLE, outlineColor, chromakey
			 )
		);

		// left

		BPT::T_Blit( 
			*pNew, 0, 1, src, helper_type(
				helper_type::SINGLE, outlineColor, chromakey
			 )
		);

		// right

		BPT::T_Blit( 
			*pNew, 2, 1, src, helper_type(
				helper_type::SINGLE, outlineColor, chromakey
			 )
		);

		// bottom

		BPT::T_Blit( 
			*pNew, 1, 2, src, helper_type(
				helper_type::SINGLE, outlineColor, chromakey
			 )
		);

		// put the original back down

		BPT::T_Blit( 
			*pNew, 1, 1, src, helper_type(
				helper_type::SOURCE, outlineColor, chromakey
			 )
		);

		// we're done!

		return pNew;

	}

	// ------------------------------------------------------------------------

	//
	//	T_TransparentAwareCreateScaledBitmap()
	//

	template< class T, const int SHIFT, class TOP >
	T * T_TransparentAwareCreateScaledBitmap(
		T & src, const int w, const int h, 
		const typename T::pixel_type chromaKey, TOP op ) {

		// Validate
		// --------------------------------------------------------------------

		if ( (0 >= w) || (0 >= h) ) {

			return 0;

		}

		// Create and size the output bitmap
		// --------------------------------------------------------------------

		T * pNew = new T;

		if ( !pNew ) {

			return 0;

		}

		if ( !pNew->Create( w, h ) ) {

			delete pNew;

			return 0;

		}

		pNew->ClearBuffer( chromaKey );

		// Setup the fractional step values
		// --------------------------------------------------------------------

		int sw = src.Width();
		int sh = src.Height();

		int wd = (1 != sw) ? sw - 1 : 1;
		int xd = ((w - 1) << SHIFT) / wd;

		int hd = (1 != sh) ? sh - 1 : 1;
		int yd = ((h - 1) << SHIFT) / hd;

		// Now go through the process of transfering the pixels...
		// This assumes that the pixel_iterator type is random access!
		// --------------------------------------------------------------------

		int yf = 0;

		for ( int y = 0; y < sh; y++ ) {

			//T::pixel_iterator dstIT = pNew->Iterator( 0, (yf >> SHIFT) );
			T::pixel_iterator dstIT = pNew->Iterator( 0, ((yf + (1 << (SHIFT - 1))) >> SHIFT) );
			
			T::pixel_iterator srcIT = src.Iterator( 0, y );

			yf += yd;

			int xf = 0;

			for ( int x = 0; x < sw; x++ ) {

				T::pixel_type t = *srcIT++;

				if ( chromaKey != t ) {

					//int offset = (xf >> SHIFT);
					int offset = ((xf + (1 << (SHIFT - 1))) >> SHIFT);
					

					if ( chromaKey != *(dstIT + offset) ) {

						op( *(dstIT + offset), t );

					} else {

						*(dstIT + offset) = t;

					}

				}

				xf += xd;

			}

		}

		// --------------------------------------------------------------------

		return pNew;

	}

}; // namespace BPT

#endif // !defined(AFX_BPTBITMAP_H__B21246CA_A04D_44AE_BD3B_AD720ED5ABCC__INCLUDED_)
