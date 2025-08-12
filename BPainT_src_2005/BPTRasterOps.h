// BPTRasterOps.h: interface for the BPTRasterOps class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	This file contains the low level pixel writting code for use
//	in the various rendering primitives.  The code relies heavily
//	on template specialization, the code would be MUCH easier to
//	understand if VC 6.0 had partial template specialization!!!
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTRASTEROPS_H__9484DA55_390E_43E2_8A1B_8E9CD9234DCA__INCLUDED_)
#define AFX_BPTRASTEROPS_H__9484DA55_390E_43E2_8A1B_8E9CD9234DCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------------

#include "BPTUtility.h"
#include "BPTBitmap.h"

// ----------------------------------------------------------------------------

namespace BPT {

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Blitter raster ops
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TTransparentSrcTransferROP
	//	

	template< class DST_TYPE, class SRC_TYPE = DST_TYPE >
	class TTransparentSrcTransferROP {

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

	private:

		src_type m_ColorKey;

	public:

		TTransparentSrcTransferROP( const src_type colorKey = SRC_TYPE(0) ) : 
			m_ColorKey( colorKey ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			if ( m_ColorKey != s ) {

				d = s;

			}

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				SRC_TYPE s = *first++;

				if ( m_ColorKey != s ) {

					*output = s;

				}

				++output;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			if ( m_ColorKey != s ) {

				while ( first != last ) {
	
					*first++ = s;
	
				}

			}

		}

		// --------------------------------------------------------------------

		void SetColorKey( const src_type & s ) {

			m_ColorKey = s;

		}

		src_type GetColorKey() const {

			return m_ColorKey;

		}

	};

	// ------------------------------------------------------------------------
	// Invert
	// ------------------------------------------------------------------------

	//
	//	TInvertDstTransferROP
	//	

	template< class DST_TYPE, class SRC_TYPE = DST_TYPE >
	class TInvertDstTransferROP {

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

	public:

		TInvertDstTransferROP() { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			d = ~d;

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				*output = ~(*output);

				++output;
				++first;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			while ( first != last ) {
	
				*first = ~(*first);

				++first;

			}

		}

	};

	// ------------------------------------------------------------------------
	//	'memset' helper
	// ------------------------------------------------------------------------

	template< class T >
	class TMemsetHelper {

	public: // traits

		template<int> struct Specialized_Memset {
			template< class OutputIt, class src_type > __forceinline
			void Action( OutputIt first, OutputIt last, const src_type & s ) const {
				/* Empty */
			}
		};

		template<> struct Specialized_Memset<1> {

			template< class OutputIt, class src_type > __forceinline
			void Action( OutputIt first, OutputIt last, const src_type & s ) const {

				memset( (void *)&(*first), s, last - first );

			}

		};

		template<> struct Specialized_Memset<2> {

			template< class OutputIt, class src_type > __forceinline
			void Action( OutputIt first, OutputIt last, const src_type & s ) const {

				// Do the odd value first if there is one
				// ----------------------------------------------------------------
	
				int count = last - first;
	
				if ( count & 1 ) {
	
					*first = s;
	
					count = count / 2;
	
					if ( !count ) {
	
						return;
	
					}
	
					++first;
	
				} else {
	
					count /= 2;
	
				}
	
				// Do the DWORD's
				// ----------------------------------------------------------------

				typedef unsigned U32;
	
				U32 combined = (U32)s | ((U32)s << 16);
	
				U32 * dst = reinterpret_cast<U32 *>( &(*first) );
				U32 * end = dst + count;
	
				while ( dst != end ) {
	
					*dst++ = combined;
	
				}

			}

		};

		template<> struct Specialized_Memset<4> {

			template< class OutputIt, class src_type > __forceinline
			void Action( OutputIt first, OutputIt last, const src_type & s ) const {

				while ( first != last ) {
		
					*first++ = s;
	
				}

			}

		};

		template<> struct Specialized_Memset<0> {

			template< class OutputIt, class src_type > __forceinline
			void Action( OutputIt first, OutputIt last, const src_type & s ) const {

				while ( first != last ) {
		
					*first++ = s;
	
				}

			}

		};

	public: // 'interface'

		template< class OutputIt, class src_type > __forceinline
		void op( OutputIt first, OutputIt last, const src_type & s ) {

			// Hack test to see if we have a forward iterator (could be horribly wrong)
			// Hack test to see if we have a forward iterator (could be horribly wrong)
			// Hack test to see if we have a forward iterator (could be horribly wrong)
			// ----------------------------------------------------------------

			BYTE * a = reinterpret_cast< BYTE *>( &(*first) );
			BYTE * b = reinterpret_cast< BYTE *>( &(*(first + 1)) );

			if ( 0 < (b - a) ) {

				Specialized_Memset<sizeof(src_type)> dummy_var;

				dummy_var.Action( first, last, s );

			} else {

				Specialized_Memset<0> dummy_var;

				dummy_var.Action( first, last, s );

			}


		}

	};

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------

	//
	//	TCopyROP
	//	

	template< class DST_TYPE, class SRC_TYPE = DST_TYPE >
	class TCopyROP {

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

	public:

		TCopyROP() { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			d = s;

			return d;

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			dst_type * a = &(*first);
			dst_type * b = &(*(first + 1));

			if ( (1 == (b - a)) && (sizeof(src_type) == sizeof(dst_type)) ) {

				// Need to find a memset type for this 'size'

				TMemsetHelper<dst_type> helper;
				
				helper.op( first, last, s );

			} else {

				while ( first != last ) {
		
					*first++ = s;
	
				}

			}

		}

		// "Specializations"
		// --------------------------------------------------------------------

	private:

		template<bool> struct Specialized_InputMajor {
			template< class ForwardIt, class OutputIt > __forceinline
			void Action( ForwardIt first, ForwardIt last, OutputIt output ) const {
				/* Empty */
			}
		};

		template<> struct Specialized_InputMajor<false> {

			template< class ForwardIt, class OutputIt > __forceinline
			void Action( ForwardIt first, ForwardIt last, OutputIt output ) const {

				while ( first != last ) {
	
					*output++ = *first++;
	
				}

			}

		};

		template<> struct Specialized_InputMajor<true> {

			template< class ForwardIt, class OutputIt > __forceinline
			void Action( ForwardIt _first, ForwardIt _last, OutputIt _output ) const {

				memcpy(
					reinterpret_cast<void *>( &(*_output) ),
					reinterpret_cast<void *>( &(*_first) ),
					(_last - _first) * sizeof(*_output)
				);

			}

		};

	public:

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			// Hack test to see if we have a forward iterator (could be horribly wrong)
			// Hack test to see if we have a forward iterator (could be horribly wrong)
			// Hack test to see if we have a forward iterator (could be horribly wrong)
			// ----------------------------------------------------------------

			BYTE * a = reinterpret_cast< BYTE *>( &(*output) );
			BYTE * b = reinterpret_cast< BYTE *>( &(*(output + 1)) );
			BYTE * c = reinterpret_cast< BYTE *>( &(*first) );
			BYTE * d = reinterpret_cast< BYTE *>( &(*(first + 1)) );

			// ----------------------------------------------------------------

			int bma = (b - a);
			int dmc = (d - c);

			// ----------------------------------------------------------------

#if 0

			if ( (0 < bma) && (0 < dmc) && (bma == dmc) ) {
				
				Specialized_InputMajor<true> dummy_var;

#else

			if ( (0 < bma) && (0 < dmc) ) {
				
				Specialized_InputMajor<(sizeof(src_type) == sizeof(dst_type))> dummy_var;

#endif

				dummy_var.Action( first, last, output );
	
			} else {
	
				Specialized_InputMajor<false> dummy_var;

				dummy_var.Action( first, last, output );
	
			}

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// CLUT raster ops
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TClutTransferROP
	//	

	template<
		class DST_TYPE
		,class SRC_TYPE = DST_TYPE
		,class CLUT_TYPE = TPow2Clut< DST_TYPE >       
	>
	class TClutTransferROP {

	private:

		CLUT_TYPE * m_pClut;

		TClutTransferROP(); // Hidden

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

		TClutTransferROP( CLUT_TYPE * pClut) : 
			m_pClut( pClut ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			d = (*m_pClut)[ s ];

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			CLUT_TYPE & clut = *m_pClut;

			while ( first != last ) {

				*output++ = clut[ *first++ ];

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			dst_type out = (*m_pClut)[ s ];

			while ( first != last ) {

				*first++ = out;

			}

		}

	};

	//
	//	TSrcPredicateClutTransfer
	//	

	template<
		class DST_TYPE
		,class PREDICATE 
		,class SRC_TYPE = DST_TYPE
		,class CLUT_TYPE = TPow2Clut< DST_TYPE >       
		,class TRANSFER_OP = TCopyROP< DST_TYPE >
	>
	class TSrcPredicateClutTransfer {

	private:

		TRANSFER_OP m_Op;
		PREDICATE m_Predicate;
		CLUT_TYPE * m_pClut;

		TSrcPredicateClutTransfer(); // Hidden

	public: // Traits

		typedef PREDICATE predicate_type;
		typedef TRANSFER_OP transfer_type;
		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

	public: // interface

		TSrcPredicateClutTransfer(
			CLUT_TYPE * pClut, PREDICATE predicate = PREDICATE(),
			TRANSFER_OP op = TRANSFER_OP() ) : 
			m_pClut( pClut ), m_Predicate( predicate ), m_Op( op ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			if ( m_Predicate( s ) ) {

				m_Op( d, (*m_pClut)[ s ] );

			}

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			CLUT_TYPE & clut = *m_pClut;

			while ( first != last ) {

				src_type s = *first++;

				if ( m_Predicate( s ) )  {

					m_Op( *output++, clut[ s ] );

				} else {

					++output;

				}

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			if ( m_Predicate( s ) ) {

				m_Op.OutputMajor( first, last, (*m_pClut)[ s ] );
	
			}

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TClut5050TransferROP
	//	

	template<
		class DST_TYPE
		,class SRC_TYPE = DST_TYPE
		,class CLUT_TYPE = TPow2Clut< DST_TYPE >       
		, const int LOWBITS = 0x3def
	>
	class TClut5050TransferROP {

	private:

		CLUT_TYPE * m_pClut;

		TClut5050TransferROP(); // Hidden

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

		TClut5050TransferROP( CLUT_TYPE * pClut) : 
			m_pClut( pClut ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			d = ((d >> 1) & LOWBITS) + (((*m_pClut)[ s ] >> 1) & LOWBITS);

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			CLUT_TYPE & clut = *m_pClut;

			while ( first != last ) {

				*output++ = ((*output >> 1) & LOWBITS) + ((clut[ *first++ ] >> 1) & LOWBITS);

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			dst_type out = (((*m_pClut)[ s ] >> 1) & LOWBITS);

			while ( first != last ) {

				*first++ = ((*first >> 1) & LOWBITS) + out;

			}

		}

	};

	// ------------------------------------------------------------------------

	//
	//	TTableDrivenScaleUpOP<>
	//	

	template<
		class DST_TYPE
		,class SRC_TYPE = DST_TYPE
		,class SCALE_TABLE_TYPE = BYTE * 
		,class TRANSFER_OP = TCopyROP< DST_TYPE >
	>
	class TTableDrivenScaleUpOP {

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

	private:

		SCALE_TABLE_TYPE m_ScaleTable;
		TRANSFER_OP m_Op;

		TTableDrivenScaleUpOP(); // Hidden

	public:

		TTableDrivenScaleUpOP(
			SCALE_TABLE_TYPE scaleTable, TRANSFER_OP op = TRANSFER_OP()
		) : m_ScaleTable( scaleTable), m_Op( op ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			m_Op( d, s );

			return d;

		}

		// [first, last) to output
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			SCALE_TABLE_TYPE pTable = m_ScaleTable;

			while ( first != last ) {

				m_Op( *output++, *first );

				if ( *pTable++ ) {

					++first;

				}

			}

		}

		// [first, last) write, read from input
		// --------------------------------------------------------------------

		template< class OutputIt, class ForwardIt >
		__forceinline void InputMinor( OutputIt first, OutputIt last, ForwardIt input ) const {

			SCALE_TABLE_TYPE pTable = m_ScaleTable;

			while ( first != last ) {

				m_Op( *first++, *input );

				if ( *pTable++ ) {

					++input;

				}

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			m_Op.OutputMajor( first, last, s );

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// BRUSH raster op
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TMultiModeBrushTransferROP
	//	

	template< class DST_TYPE, class SRC_TYPE = DST_TYPE >
	class TMultiModeBrushTransferROP {

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

		enum MODE {

			SOURCE	= 0
			,SINGLE	= 1

		};

	private:

		dst_type m_WriteColor;
		src_type m_ColorKey;
		MODE m_Mode;

	public:

		TMultiModeBrushTransferROP(
			const MODE mode = SOURCE, 
			const dst_type writeColor = DST_TYPE(0),
			const src_type colorKey = SRC_TYPE(0) ) : 
			m_Mode( mode ), m_WriteColor( writeColor), 
			m_ColorKey( colorKey ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			if ( SOURCE == m_Mode ) {

				if ( m_ColorKey != s ) {
	
					d = s;
	
				}

			} else {

				if ( m_ColorKey != s ) {
	
					d = m_WriteColor;
	
				}

			}

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			if ( SOURCE == m_Mode ) {

				while ( first != last ) {
	
					SRC_TYPE s = *first++;
	
					if ( m_ColorKey != s ) {
	
						*output = s;
	
					}
	
					++output;
	
				}

			} else {

				while ( first != last ) {
	
					if ( m_ColorKey != *first++ ) {
	
						*output = m_WriteColor;
	
					}
	
					++output;
	
				}

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			if ( m_ColorKey != s ) {

				if ( SOURCE == m_Mode ) {

					while ( first != last ) {
		
						*first++ = s;
		
					}

				} else {

					while ( first != last ) {
		
						*first++ = m_WriteColor;
		
					}

				}

			}

		}

		// --------------------------------------------------------------------

		void SetColorKey( const src_type & s ) {

			m_ColorKey = s;

		}

		src_type GetColorKey() const {

			return m_ColorKey;

		}

		void SetWriteColor( const dst_type & w ) {

			m_WriteColor = w;

		}

		dst_type GetWriteColor() const {

			return m_WriteColor;

		}

	};

	// ------------------------------------------------------------------------
	// 16bpp 50/50
	// ------------------------------------------------------------------------

	//
	//	T16bpp_5050_OP
	//	

	template< class T, const int LOWBITS >
	class T16bpp_5050_OP {

	public:

		typedef T dst_type;
		typedef T src_type;

	public:

		T16bpp_5050_OP() { /* Empty */ }

		__forceinline dst_type & operator()( dst_type & d, const src_type & s ) const {

			d = ((d >> 1) & LOWBITS) + ((s >> 1) & LOWBITS);

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				*output++ = ((*output >> 1) & LOWBITS) + ((*first++ >> 1) & LOWBITS);

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			dst_type t = (s >> 1) & LOWBITS;

			while ( first != last ) {
	
				*first = ((*first >> 1) & LOWBITS) + t;

				++first;

			}

		}

	};

	// ------------------------------------------------------------------------

	//
	//	TSrcChromakeyClutTransfer
	//	
	//	Note the use of int is just there to aid the compiler in doing it's 
	//	optimizations, this code isn't as nifty as it could be. (as if)
	//	

	template<
		class DST_TYPE
		,class SRC_TYPE = DST_TYPE
		,class CLUT_TYPE = TPow2Clut< DST_TYPE >       
	>
	class TSrcChromakeyClutTransfer {

	private:

		CLUT_TYPE & m_rClut;
		/* SRC_TYPE */ int m_Chromakey;

		TSrcChromakeyClutTransfer(); // Hidden

	public: // Traits

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

	public: // interface

		TSrcChromakeyClutTransfer(
			CLUT_TYPE & rClut, const SRC_TYPE chromakey
		) : m_rClut( rClut ), m_Chromakey( chromakey ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			if ( m_Chromakey != s ) {

				d = m_rClut[ s ];

			}

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				/* src_type */ int s = *first++;

				if ( m_Chromakey != s ) {

					*output++ = m_rClut[ s ];

				} else {

					++output;

				}

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			if ( m_Chromakey != s ) {

				dst_type d = m_rClut[ s ];

				while ( first != last ) {

					*first++ = d;

				}
	
			}

		}

	};

	// ========================================================================

	//
	//	TTransparentTableSrcTransferROP
	//	

	template< class DST_TYPE, class TABLE_TYPE, class SRC_TYPE = DST_TYPE >
	class TTransparentTableSrcTransferROP {

	public:

		typedef TABLE_TYPE table_type;
		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

	private:

		table_type m_ColorKeyTable;

		TTransparentTableSrcTransferROP(); // Hidden

	public:

		TTransparentTableSrcTransferROP( const table_type colorKeyTable ) : 
			m_ColorKeyTable( colorKeyTable ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			if ( m_ColorKeyTable[ s ] ) {

				d = s;

			}

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				SRC_TYPE s = *first++;

				if ( m_ColorKeyTable[ s ] ) {

					*output = s;

				}

				++output;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			if ( m_ColorKeyTable[ s ] ) {

				while ( first != last ) {
	
					*first++ = s;
	
				}

			}

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TSingleColorTransferROP
	//	

	template<
		class DST_TYPE
		,class SRC_TYPE = DST_TYPE
	>
	class TSingleColorTransferROP {

	private:

		DST_TYPE m_SingleColor;

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

		TSingleColorTransferROP( DST_TYPE singleColor = DST_TYPE(0) ) : 
			m_SingleColor( singleColor ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			d = m_SingleColor;

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				*output++ = m_SingleColor;

				++first;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			while ( first != last ) {

				*first++ = m_SingleColor;

			}

		}

		// --------------------------------------------------------------------

		void SetColor( const DST_TYPE value ) {

			m_SingleColor = value;

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Blitter raster ops
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TSingleColorTransparentSrcTransferROP
	//	

	template< class DST_TYPE, class SRC_TYPE = DST_TYPE >
	class TSingleColorTransparentSrcTransferROP {

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

	private:

		src_type m_ColorKey;
		dst_type m_OutputColor;

	public:

		TSingleColorTransparentSrcTransferROP(
			const dst_type outColor = DST_TYPE(~0),
			const src_type colorKey = SRC_TYPE(0)
		) : m_OutputColor( outColor ), m_ColorKey( colorKey ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			if ( m_ColorKey != s ) {

				d = m_OutputColor;

			}

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				if ( m_ColorKey != *first++ ) {

					*output = m_OutputColor;

				}

				++output;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			if ( m_ColorKey != s ) {

				while ( first != last ) {
	
					*first++ = m_OutputColor;
	
				}

			}

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Difference raster ops
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TChromakeyedDifferenceOP
	//	

	template<
		class PIXEL_TYPE, 
		class OP = TCopyROP<PIXEL_TYPE>,
		class PO = TInvertDstTransferROP<PIXEL_TYPE>
	>
	class TChromakeyedDifferenceOP {

	public:

		typedef PIXEL_TYPE DST_TYPE;
		typedef PIXEL_TYPE SRC_TYPE;
		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

	private:

		bool * m_pbWriteMode;
		PO m_PreviewOp;
		OP m_Op;
		bool m_bStoreResult;
		src_type m_ColorKey;
		dst_type m_OutputColor;

	public:

		TChromakeyedDifferenceOP(
			bool * pbWriteMode = 0
			,const bool bStoreResult = false
			,const dst_type outColor = dst_type(~0)
			,const src_type colorKey = src_type(0)
			,OP op = OP()
			,PO po = PO()
		) : m_pbWriteMode( pbWriteMode ), m_PreviewOp( po ), m_bStoreResult( bStoreResult ), m_Op( op ), m_OutputColor( outColor ), m_ColorKey( colorKey ) { /* Empty */ }

		__forceinline dst_type & operator()( dst_type & d, const src_type & s ) const {

			if ( m_ColorKey != s ) {

				if ( m_bStoreResult == (0 == (d - s)) ) {

					m_Op( d, m_OutputColor );

				} else if ( m_pbWriteMode ) {

					if ( !(*m_pbWriteMode) ) {

						m_PreviewOp( d, m_OutputColor );

					} else {

					}

				}

			}

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			bool writeMode;

			if ( m_pbWriteMode ) {

				writeMode = *m_pbWriteMode;

			} else {

				writeMode = true;

			}

			while ( first != last ) {

				src_type s = *first++;

				if ( m_ColorKey != s ) {

					if ( m_bStoreResult == (0 == (*output - s)) ) {

						m_Op( *output, m_OutputColor );

					} else {
						
						if ( !writeMode ) {

							m_PreviewOp( *output, m_OutputColor );

						} else {

						}

					}

				}

				++output;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			bool writeMode;

			if ( m_pbWriteMode ) {

				writeMode = *m_pbWriteMode;

			} else {

				writeMode = true;

			}

			if ( m_ColorKey != s ) {

				while ( first != last ) {
	
					if ( m_bStoreResult == (0 == (*first - s)) ) {

						m_Op( *first, m_OutputColor );

					} else {
						
						if ( !writeMode ) {

							m_PreviewOp( *first, m_OutputColor );

						} else {

						}

					}

					++first;
	
				}

			}

		}

		// Modification interface
		// --------------------------------------------------------------------

		void SetOutputColor( const dst_type value ) {

			m_OutputColor = value;

		}

		void SetChromakeyColor( const src_type value ) {

			m_ColorKey = value;

		}

		void SetStorageResult( const bool value ) {

			m_bStoreResult = value;

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Adaptor ROP's
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class T, class ROP >
	class TIgnoreConstructorAdaptorROP : public ROP {

	public:

		typedef typename ROP::dst_type dst_type;
		typedef typename ROP::src_type src_type;

		TIgnoreConstructorAdaptorROP() { /* empty */ }

		TIgnoreConstructorAdaptorROP( T param ) { /* empty */ }

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	TAddColorsTransferROP
	//	

	template<
		class DST_TYPE
		,class SRC_TYPE = DST_TYPE
	>
	class TAddColorsTransferROP {

	private:

		DST_TYPE m_AddColor;

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

		TAddColorsTransferROP( DST_TYPE addColor = DST_TYPE(1) ) : 
			m_AddColor( addColor ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			d += m_AddColor;

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				*output++ = (*first++) + m_AddColor;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			DST_TYPE result = s + m_AddColor;

			while ( first != last ) {

				*first++ = result;

			}

		}

		// --------------------------------------------------------------------

		void SetColor( const DST_TYPE value ) {

			m_AddColor = value;

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// DESTINATION ALPHA AWARE OPS
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//
	//	T_DestAlphaTransferROP<>
	//	
	//	-- destination alpha support
	//	

	template<
		class DST_TYPE
		,class SRC_TYPE = DST_TYPE
	>
	class T_DestAlphaTransferROP {

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

		T_DestAlphaTransferROP() { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

			// fetch the s & d values to check for shortcuts
			// ----------------------------------------------------------------

			unsigned s_alpha;

			if ( !(s_alpha = (s >> 24)) ) return d; // nop

			unsigned d_alpha;

			if ( !(d_alpha = (d >> 24)) ) { // simply the src

				d = s;

				return d;

			}

			// scale the alpha values to be 0 to 256 instead of 0 to 255
			// so that the combined alpha multiply will work.
			// ----------------------------------------------------------------

			s_alpha = (s_alpha << 8) + 128;
			s_alpha = (s_alpha + (s_alpha >> 8)) >> 8;

			d_alpha = (d_alpha << 8) + 128;
			d_alpha = (d_alpha + (d_alpha >> 8)) >> 8;

			// I used blinn's for how to calculate the resulting alpha value
			// ----------------------------------------------------------------

			unsigned gamma = (s_alpha + d_alpha) - ((s_alpha * d_alpha) >> 8);

			if ( 0 == gamma ) return d;

			unsigned alpha = (s_alpha << 8) / gamma; // this should be table driven!!!!

			// ----------------------------------------------------------------

			const unsigned dstrb = d & 0xFF00FF;
			const unsigned dstg = d & 0xFF00;

			const unsigned srcrb = s & 0xFF00FF;
			const unsigned srcg = s & 0xFF00;

			unsigned drb = srcrb - dstrb;
			unsigned dg = srcg - dstg;

			drb *= alpha;
			dg  *= alpha; 

			drb >>= 8;
			dg  >>= 8;

			unsigned rb = (drb + dstrb) & 0xFF00FF;
			unsigned g  = (dg  + dstg) & 0xFF00;

			d = rb | g | (((gamma * 255)>>8) << 24);

			return d;

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				(*this)( *output, *first );

				++first;

				++output;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			while ( first != last ) {

				(*this)( *first, s );

				++first;

			}

		}

	}; // T_DestAlphaTransferROP<>

	//
	//	T_DA_AlphaClutTransferROP<>
	//	
	//	-- destination alpha support
	//	

	template<
		class DST_TYPE
		,class SRC_TYPE = DST_TYPE
		,class CLUT_TYPE = TPow2Clut< DST_TYPE >       
	>
	class T_DA_AlphaClutTransferROP {

	private:

		CLUT_TYPE * m_pClut;

		T_DA_AlphaClutTransferROP(); // Hidden

		T_DestAlphaTransferROP<DST_TYPE, typename CLUT_TYPE::output_type> m_DstAlphaOp;

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

		T_DA_AlphaClutTransferROP( CLUT_TYPE * pClut) : 
			m_pClut( pClut ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & index ) const {

			return m_DstAlphaOp( d, (*m_pClut)[ index ] );

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				(*this)( *output, *first );

				++first;

				++output;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			while ( first != last ) {

				(*this)( *first, s );

				++first;

			}

		}

	}; // T_DA_AlphaClutTransferROP<>

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ALPHA raster op's
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#define ALWAYS_USE_DEST_ALPHA

	//
	//	TAlphaClutTransferROP<>
	//	

	template<
		class DST_TYPE
		,class SRC_TYPE = DST_TYPE
		,class CLUT_TYPE = TPow2Clut< DST_TYPE >       
	>
	class TAlphaClutTransferROP {

	private:

		CLUT_TYPE * m_pClut;

		TAlphaClutTransferROP(); // Hidden

#if defined(ALWAYS_USE_DEST_ALPHA)

		T_DestAlphaTransferROP<DST_TYPE, typename CLUT_TYPE::output_type> m_DstAlphaOp;

#endif

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

		TAlphaClutTransferROP( CLUT_TYPE * pClut) : 
			m_pClut( pClut ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & index ) const {

#if defined(ALWAYS_USE_DEST_ALPHA)

			return m_DstAlphaOp( d, (*m_pClut)[ index ] );

#else

			typename CLUT_TYPE::output_type s = (*m_pClut)[ index ];

			unsigned a;

			if ( !(a = (s >> 24)) ) return d;

			const unsigned dstrb = d & 0xFF00FF;
			const unsigned dstg  = d & 0xFF00;

			const unsigned srcrb = s & 0xFF00FF;
			const unsigned srcg  = s & 0xFF00;

			unsigned drb = srcrb - dstrb;
			unsigned dg  =  srcg - dstg;

			++a;

			drb *= a;
			dg  *= a; 

			drb >>= 8;
			dg  >>= 8;

			unsigned rb = (drb + dstrb) & 0xFF00FF;
			unsigned g  = (dg  + dstg) & 0xFF00;

			d = rb | g;

			return d;

#endif // defined(ALWAYS_USE_DEST_ALPHA)

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				(*this)( *output, *first );

				++first;

				++output;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			while ( first != last ) {

				(*this)( *first, s );

				++first;

			}

		}

	}; // TAlphaClutTransferROP<>

	// ------------------------------------------------------------------------

	//
	//	TAlpha32BppTransferROP<>
	//	

	template<
		class DST_TYPE
		,class SRC_TYPE = DST_TYPE
	>
	class TAlpha32BppTransferROP {

	private:

#if defined(ALWAYS_USE_DEST_ALPHA)

		T_DestAlphaTransferROP<DST_TYPE, SRC_TYPE> m_DstAlphaOp;

#endif

	public:

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

		TAlpha32BppTransferROP() { /* empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & s ) const {

#if defined(ALWAYS_USE_DEST_ALPHA)

			return m_DstAlphaOp( d, s );

#else

			unsigned a;

			if ( !(a = (s >> 24)) ) return d;

			const unsigned dstrb = d & 0xFF00FF;
			const unsigned dstg  = d & 0xFF00;

			const unsigned srcrb = s & 0xFF00FF;
			const unsigned srcg  = s & 0xFF00;

			unsigned drb = srcrb - dstrb;
			unsigned dg  =  srcg - dstg;

			++a;

			drb *= a;
			dg  *= a; 

			drb >>= 8;
			dg  >>= 8;

			unsigned rb = (drb + dstrb) & 0xFF00FF;
			unsigned g  = (dg  + dstg) & 0xFF00;

			d = rb | g;

			return d;
#endif // defined(ALWAYS_USE_DEST_ALPHA)

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				(*this)( *output, *first );

				++first;

				++output;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			while ( first != last ) {

				(*this)( *first, s );

				++first;

			}

		}

	}; // TAlpha32BppTransferROP<>

	//
	//	TSrcChromakeyAlphaClutTransfer
	//	
	//	Note the use of int is just there to aid the compiler in doing it's 
	//	optimizations, this code isn't as nifty as it could be. (as if)
	//	

	template<
		class DST_TYPE
		,class SRC_TYPE = DST_TYPE
		,class CLUT_TYPE = TPow2Clut< DST_TYPE >       
	>
	class TSrcChromakeyAlphaClutTransfer {

	private:

		CLUT_TYPE & m_rClut;
		/* SRC_TYPE */ unsigned m_Chromakey;

		TSrcChromakeyAlphaClutTransfer(); // Hidden

#if defined(ALWAYS_USE_DEST_ALPHA)

		T_DestAlphaTransferROP<DST_TYPE, typename CLUT_TYPE::output_type> m_DstAlphaOp;

#endif

	public: // Traits

		typedef DST_TYPE dst_type;
		typedef SRC_TYPE src_type;

	public: // interface

		TSrcChromakeyAlphaClutTransfer(
			CLUT_TYPE & rClut, const SRC_TYPE chromakey
		) : m_rClut( rClut ), m_Chromakey( chromakey ) { /* Empty */ }

		__forceinline DST_TYPE & operator()( dst_type & d, const src_type & index ) const {

			if ( m_Chromakey == index ) return d;

#if defined(ALWAYS_USE_DEST_ALPHA)

			return m_DstAlphaOp( d, m_rClut[ index ] );

#else

			typename CLUT_TYPE::output_type s = m_rClut[ index ];

			unsigned a;

			if ( !(a = (s >> 24)) ) return d;

			const unsigned dstrb = d & 0xFF00FF;
			const unsigned dstg  = d & 0xFF00;

			const unsigned srcrb = s & 0xFF00FF;
			const unsigned srcg  = s & 0xFF00;

			unsigned drb = srcrb - dstrb;
			unsigned dg  =  srcg - dstg;

			++a;

			drb *= a;
			dg  *= a; 

			drb >>= 8;
			dg  >>= 8;

			unsigned rb = (drb + dstrb) & 0xFF00FF;
			unsigned g  = (dg  + dstg) & 0xFF00;

			d = rb | g;

			return d;

#endif // defined(ALWAYS_USE_DEST_ALPHA)

		}

		// [first, last) to output (same semantics as std::copy())
		// --------------------------------------------------------------------

		template< class ForwardIt, class OutputIt >
		__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) const {

			while ( first != last ) {

				(*this)( *output, *first );

				++first;

				++output;

			}

		}

		// [first, last) write value 's'
		// --------------------------------------------------------------------

		template< class OutputIt >
		__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

			if ( m_Chromakey != s ) {

				(*this)( *first, s );

				++first;
	
			}

		}

	}; // TSrcChromakeyAlphaClutTransfer

}; // namespace BPT

#endif // !defined(AFX_BPTRASTEROPS_H__9484DA55_390E_43E2_8A1B_8E9CD9234DCA__INCLUDED_)
