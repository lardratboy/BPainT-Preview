// BPTSRL.h: interface for the CBPTSRL class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTSRL_H__CCC1EB59_863C_4490_84CA_C1F171D48D87__INCLUDED_)
#define AFX_BPTSRL_H__CCC1EB59_863C_4490_84CA_C1F171D48D87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------------

//#define SRL_RESERVE_MAX_AMOUNTS
//#define SRL_USE_STD_COLLECTION

// Based on CObject for debug memory leak tracking 

// ----------------------------------------------------------------------------

#include <vector>
#include <utility>

#include "BPTBlitter.h"
#include "BPTFileio.h"

// ----------------------------------------------------------------------------

namespace BPT {

	// ========================================================================
	// 
	// SRL control BYTE format
	// 
	//	xxxxxxx1 == skip of (xxxxxxx + 1) 
	//	xxxxxx10 == run of (xxxxxx + 1) of the next value in the value stream
	//  xxxxxx00 == literal run (xxxxxx + 1) of next n values from the stream
	// 
	// The stream format varies depending on type that was compressed.
	// 
	// ========================================================================

	// ========================================================================
	// Compress data into code & value streams
	// ========================================================================

	//
	//	TSRLCompressor
	//
	
	template< class T >
	class TSRLCompressor /* : public CObject */ {

	public:

		typedef T VALUE_TYPE;
		typedef unsigned char CODE_ENTRY;
		typedef std::vector<VALUE_TYPE> VALUE_COLLECTION;
		typedef std::vector<CODE_ENTRY> CODE_COLLECTION;
		typedef std::pair< CODE_COLLECTION, VALUE_COLLECTION > INFO;

		enum {

			SKIP_FLAG		= 0x01
			,RUN_FLAG		= 0x02

		};

	private:
	
		enum RUNTYPE {

			SKIP, RUN, LITERAL

		};

		enum {

			MAX_SKIP_LENGTH			= 127
			,MAX_RUN_LENGTH			= 63
			,MAX_LITERAL_LENGTH		= 63

		};

		void EmitCode( CODE_COLLECTION & out, RUNTYPE type, const int length ) {

			if ( SKIP == type ) {

				out.push_back( ((length - 1) << 1) | SKIP_FLAG );

			} else if ( RUN == type ) {

				out.push_back( ((length - 1) << 2) | RUN_FLAG );

			} else {

				out.push_back( ((length - 1) << 2) );

			}

		}
	
	public:

		template< class InIt, class Pred >
		bool operator()( INFO & info, InIt first, InIt last, Pred shouldStore ) {

			// Try to improve the efficiency of this routine by reserving
			// additional space in the collections for the new info.
			// ----------------------------------------------------------------

#if defined( SRL_RESERVE_MAX_AMOUNTS )

			typename VALUE_COLLECTION::size_type valueReserveCount = 
				info.second.size() + (last - first);

			info.second.reserve( valueReserveCount );

			typename VALUE_COLLECTION::size_type codeReserveCount =
				info.first.size() + ((last - first) / 2);

			info.first.reserve( codeReserveCount );

#endif

			// Figure out what type of code to start with skip or literal
			// ----------------------------------------------------------------
	
			RUNTYPE type;

			InIt it = first;

			T value = *it++;

			int count = 1;

			bool bStore = shouldStore( value );

			if ( bStore ) {

				type = LITERAL;

				info.second.push_back( value );

			} else {

				type = SKIP;

			}
	
			// Run through the remaining values collecting runs
			// ----------------------------------------------------------------

			while ( it != last ) {

				bool bLastStore = bStore;

				T lastValue = value;

				value = *it++;

				bStore = shouldStore( value );

				if ( (value == lastValue) && (bLastStore == bStore) ) {

					if ( (LITERAL == type) && (1 != count) ) {

						EmitCode( info.first, type, count - 1 );

						count = 1;

					}

					++count;

					if ( bStore ) {

						if ( count > MAX_RUN_LENGTH ) {

							EmitCode( info.first, RUN, MAX_RUN_LENGTH );

							info.second.push_back( value );

							type = LITERAL;

							count = 1;

						} else {

							type = RUN;

						}

					} else {

						if ( count > MAX_SKIP_LENGTH ) {

							EmitCode( info.first, SKIP, MAX_SKIP_LENGTH );

							count = 1;

						}

					}

				} else {

					if ( (LITERAL == type) && bStore ) {

						info.second.push_back( value );

						++count;

						if ( count > MAX_LITERAL_LENGTH ) {

							EmitCode( info.first, LITERAL, MAX_LITERAL_LENGTH );

							count = 1;

						}

					} else {

						// Add the last 'code' to the codes collection

						EmitCode( info.first, type, count );

						// now determine what type of code to start

						count = 1;

						if ( bStore ) {

							info.second.push_back( value );

							type = LITERAL;

						} else {

							type = SKIP;

						}

					}

				}
	
			}

			// Dump the last code to the code collection

			EmitCode( info.first, type, count );

			return true;
	
		}
	
	};

	// ========================================================================
	// Decode
	// ========================================================================

	//
	//	TDecodeSRL<>
	//

	template< class SRL >
	class TDecodeSRL /* : public CObject */ {

	private:

	public:

		TDecodeSRL() { /* Empty */ }

		// --------------------------------------------------------------------

		template<
			class DST_IT,
			class CODE_IT,
			class VALUE_IT,
			class TOP
		>
		__forceinline void operator()(
			DST_IT dstIT, CODE_IT codes, VALUE_IT values,
			int skipCount, int writeCount, TOP top ) {

			// Handle any skipping
			// ----------------------------------------------------------------

			int runCount = 0;
			int code = 0;

			while ( 0 < skipCount ) {

				int sCode = static_cast<int>( *codes++ );

				if ( sCode & SRL::SKIP_FLAG ) {

					runCount = (sCode >> 1) + 1;

					skipCount -= runCount;

					if ( 0 > skipCount ) {

						writeCount += skipCount;
						dstIT -= skipCount;
						break;

					}

				} else if ( sCode & SRL::RUN_FLAG ) {

					runCount = (sCode >> 2) + 1;

					skipCount -= runCount;

					if ( 0 > skipCount ) {

						runCount = -skipCount;

						goto HANDLE_RUN;

					}

					++values;

				} else {

					runCount = (sCode >> 2) + 1;

					values += runCount;

					skipCount -= runCount;

					if ( 0 > skipCount ) {

						values += skipCount;
						runCount = -skipCount;
						goto HANDLE_LITERAL;

					}


				}

			}

			// Finally get down
			// ----------------------------------------------------------------

			while ( 0 < writeCount ) {

				code = static_cast<int>( *codes++ );

				if ( code & SRL::SKIP_FLAG ) {

					runCount = (code >> 1) + 1;
					writeCount -= runCount;
					dstIT += runCount;

				} else if ( code & SRL::RUN_FLAG ) {

					runCount = (code >> 2) + 1;

				HANDLE_RUN:

					typename SRL::VALUE_TYPE value = *values++;

					writeCount -= runCount;

					if ( 0 > writeCount ) {

						runCount += writeCount;

					}

					top.OutputMajor( dstIT, dstIT + runCount, value );
					dstIT += runCount;

				} else {

					runCount = (code >> 2) + 1;

				HANDLE_LITERAL:

					writeCount -= runCount;

					if ( 0 > writeCount ) {

						runCount += writeCount;

					}

					top.InputMajor( values, values + runCount, dstIT );

					values += runCount;
					dstIT += runCount;

				}

			}

		}

		// --------------------------------------------------------------------

		template<
			class CODE_IT,
			class VALUE_IT
		>
		__forceinline bool HitTest(
			CODE_IT codes, VALUE_IT values, int skipCount, 
			typename SRL::VALUE_TYPE * pOptionalOutValue = 0 ) {

			// Handle any 'skipping' to get to the correct value
			// ----------------------------------------------------------------

			while ( 0 < skipCount ) {

				int sCode = static_cast<int>( *codes++ );

				if ( sCode & SRL::SKIP_FLAG ) {

					int runCount = (sCode >> 1) + 1;

					skipCount -= runCount;

					if ( 0 > skipCount ) {

						return false;

					}

				} else if ( sCode & SRL::RUN_FLAG ) {

					int runCount = (sCode >> 2) + 1;

					skipCount -= runCount;

					if ( 0 > skipCount ) {

						goto HANDLE_RUN;

					}

					++values;

				} else {

					int runCount = (sCode >> 2) + 1;

					values += runCount;

					skipCount -= runCount;

					if ( 0 > skipCount ) {

						values += skipCount;

						goto HANDLE_LITERAL;

					}


				}

			}

			// ----------------------------------------------------------------

			if ( SRL::SKIP_FLAG & static_cast<int>( *codes++ ) ) {

				return false;

    		}

			HANDLE_RUN:
			HANDLE_LITERAL:

			if ( pOptionalOutValue ) {

				*pOptionalOutValue = *values;

			}

			return true;

		}

	};

	// ========================================================================
	// ========================================================================

	template< class SRL >
		class TSRLCompressedImage /* : public CObject */ {

	public: // Traits

		typedef typename SRL::VALUE_TYPE storage_type;
		typedef SRL srl_type;
		typedef TSRLCompressedImage<SRL> this_type;

	private: // Data

#if defined( SRL_USE_STD_COLLECTION )

		typedef std::vector< typename srl_type::INFO * > info_collection_type;

		info_collection_type m_CompressedInfo;

#else

		typename srl_type::INFO * m_CompressedInfo;

#endif

		SIZE m_Size;

		srl_type m_Compressor;

		TDecodeSRL<SRL> m_Decompressor;

		int m_CompressedSizeEstimate;

	public: // Interface

		TSRLCompressedImage() {

			m_CompressedSizeEstimate = 0;

			m_Size.cx = 0;
			m_Size.cy = 0;

#if !defined( SRL_USE_STD_COLLECTION )

			m_CompressedInfo = 0;

#endif

		};

		~TSRLCompressedImage() {

			Destroy();
		
		}

		// --------------------------------------------------------------------

		SIZE Size() const {

			return m_Size;

		}

		int CompressedSizeEstimate() const {

			return m_CompressedSizeEstimate;

		}

		bool HasData() const {

			return (0 != m_Size.cx) && (0 != m_Size.cy);

		}

		// --------------------------------------------------------------------

		void Destroy() {

#if defined( SRL_USE_STD_COLLECTION )

			while ( !m_CompressedInfo.empty() ) {

				delete m_CompressedInfo.back();

				m_CompressedInfo.pop_back();

			}

#else

			if ( m_CompressedInfo ) {

				delete [] m_CompressedInfo;

				m_CompressedInfo = 0;

			}

#endif

			m_Size.cx = 0;
			m_Size.cy = 0;

			m_CompressedSizeEstimate = 0;

		}

		// --------------------------------------------------------------------

		template< class SURFACE, class PREDICATE >
		bool Create( SURFACE & srcSurface, PREDICATE predicate, const RECT * pRect = 0 ) {

			// Destroy any existing information
			// ----------------------------------------------------------------

			Destroy();

			// Determine/limit the size of the operation and reserve elements
			// ----------------------------------------------------------------

			RECT surfaceRect = srcSurface.Rect();

			if ( pRect ) {

				if ( !IntersectRect( &surfaceRect, &surfaceRect, pRect ) ) {

					return false;

				}

			}

			int cx = surfaceRect.right - surfaceRect.left;
			int cy = surfaceRect.bottom - surfaceRect.top;

#if defined( SRL_USE_STD_COLLECTION )

			m_CompressedInfo.reserve( cy );

#else

			m_CompressedInfo = new typename srl_type::INFO [ cy ];

			if ( !m_CompressedInfo ) {

				return false;

			}

#endif

			// For each of the lines in the surface call the compressor
			// ----------------------------------------------------------------

			int codesCount = 0;
			int valuesCount = 0;

			for ( int y = 0; y < cy; y++ ) {

#if defined( SRL_USE_STD_COLLECTION )

				m_CompressedInfo[ y ] = new typename srl_type::INFO();

				if ( !m_CompressedInfo[ y ] ) {

					Destroy();

					return false;

				}

#endif

				typename SURFACE::pixel_iterator it = srcSurface.Iterator(
					surfaceRect.left, y + surfaceRect.top
				);

#if defined( SRL_USE_STD_COLLECTION )

				if ( !m_Compressor( *m_CompressedInfo[ y ], it, it + cx, predicate ) ) {

#else

				if ( !m_Compressor( m_CompressedInfo[ y ], it, it + cx, predicate ) ) {

#endif

					Destroy();

					return false;

				}

#if defined( SRL_USE_STD_COLLECTION )

				codesCount += m_CompressedInfo[ y ]->first.size();
				valuesCount += m_CompressedInfo[ y ]->second.size();

#else

				codesCount += m_CompressedInfo[ y ].first.size();
				valuesCount += m_CompressedInfo[ y ].second.size();

#endif


			}

			m_CompressedSizeEstimate =
				(sizeof( typename srl_type::INFO ) * cy) +
				(codesCount * sizeof( typename srl_type::CODE_ENTRY )) +
				(valuesCount * sizeof( typename srl_type::VALUE_TYPE ));

			m_Size.cx = cx;
			m_Size.cy = cy;

			return true;

		}

		// --------------------------------------------------------------------

		//
		//	BlitCore
		//
		//	-- This can accept other forms of the decompressor!
		//
	
		template< class DST_SURFACE, class TOP, class DECOMPRESSOR >
		void __forceinline BlitCore(
			DECOMPRESSOR & decompressor,
			DST_SURFACE & dstSurface
			,const int x
			,const int y
			,TOP op = TOP()
			,const BLITFX * blitFX = 0
			,const RECT * optionalDstClipRect = 0
			,const RECT * optionalSrcSubRect = 0
		) {

			// Check for an empty bitmap.
			// ----------------------------------------------------------------

			if ( (0 == m_Size.cx) || (0 == m_Size.cy) ) {

				return /* NOP */;

			}
	
			// Clip the optional clipping rect to the dest bitmap limits
			// ----------------------------------------------------------------
	
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
			// ----------------------------------------------------------------
	
			RECT srcLimitsRect = { 0, 0, m_Size.cx, m_Size.cy };
	
			RECT clippedSrcRect;
	
			if ( optionalSrcSubRect ) {
	
				if ( !IntersectRect( &clippedSrcRect, &srcLimitsRect, optionalSrcSubRect) ) {
	
					return /* NOP */;
	
				}
	
			} else {
	
				clippedSrcRect = srcLimitsRect;
	
			}
	
			// Perform a simple clipping operation to detect NOP
			// ----------------------------------------------------------------
	
			SIZE clippedSrcRectSize = SizeOfRect( &clippedSrcRect );
	
			RECT dstOperation = {
				x, y, x + clippedSrcRectSize.cx, y + clippedSrcRectSize.cy
			};
	
			RECT clippedRect;
				
			if ( !IntersectRect( &clippedRect, &clippedDstRect, &dstOperation) ) {
	
				return /* NOP */;
	
			}
	
			// Setup the general loop variables
			// ----------------------------------------------------------------
	
			int cx = clippedRect.right - clippedRect.left;
			int cy = clippedRect.bottom - clippedRect.top;
	
			int sx = ((clippedRect.left - x) + clippedSrcRect.left);
			int sy = ((clippedRect.top - y) + clippedSrcRect.top);
	
			// Check for flipping and adjust the dest position and step amount.
			// ----------------------------------------------------------------
	
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
			// ----------------------------------------------------------------

			if ( 1 == ddx ) {
	
				for ( int ly = 0; ly < cy; ly++ ) {
	
					typename DST_SURFACE::pixel_iterator dstIT = dstSurface.Iterator( dx, dy );

					// call the decompressor

#if defined( SRL_USE_STD_COLLECTION )

					typename srl_type::INFO * pInfo = m_CompressedInfo[ sy ];

#else

					typename srl_type::INFO * pInfo = &m_CompressedInfo[ sy ];

#endif

					decompressor(
						dstIT, 
						pInfo->first.begin(), pInfo->second.begin(),
						sx, cx, op
					);

					// advance the locations

					dy += ddy;
					++sy;
		
				}
	
			} else {
	
				for ( int ly = 0; ly < cy; ly++ ) {
	
					typename DST_SURFACE::reverse_iterator dstIT = dstSurface.rIterator( dx, dy );

					// call the decompressor

#if defined( SRL_USE_STD_COLLECTION )

					typename srl_type::INFO * pInfo = m_CompressedInfo[ sy ];

#else

					typename srl_type::INFO * pInfo = &m_CompressedInfo[ sy ];

#endif

					decompressor(
						dstIT, 
						pInfo->first.begin(), pInfo->second.begin(),
						sx, cx, op
					);

					// advance the locations
	
					dy += ddy;
					++sy;
		
				}
	
			}
	
		}

		// --------------------------------------------------------------------

		template< class DST_SURFACE, class TOP >
		void __forceinline Blit(
			DST_SURFACE & dstSurface
			,const int x
			,const int y
			,TOP op = TOP()
			,const BLITFX * blitFX = 0
			,const RECT * optionalDstClipRect = 0
			,const RECT * optionalSrcSubRect = 0
		) {

			BlitCore(
				m_Decompressor, dstSurface, x, y, op, blitFX, 
				optionalDstClipRect, optionalSrcSubRect
			);

		}

		// --------------------------------------------------------------------

		bool HitTest( const int x, const int y, storage_type * pOptionalOutValue = 0 ) {

			// clip
			// ----------------------------------------------------------------

			if ( (0 > x) || (0 > y) || (x >= m_Size.cx) || (y >= m_Size.cy) ) {

				return false;

			}

			// Handle the actual hit test
			// ----------------------------------------------------------------

#if defined( SRL_USE_STD_COLLECTION )

			typename srl_type::INFO * pInfo = m_CompressedInfo[ y ];

#else

			typename srl_type::INFO * pInfo = &m_CompressedInfo[ y ];

#endif

			return m_Decompressor.HitTest( 
				pInfo->first.begin(), pInfo->second.begin(), x,
				pOptionalOutValue
			);

		}

		// --------------------------------------------------------------------

#if 1 // BPT 5/22/01

		bool SameAs( const this_type * pOther ) const {

			// do the simple tests
			// ----------------------------------------------------------------

			if ( (m_Size.cx != pOther->m_Size.cx) || (m_Size.cy != pOther->m_Size.cy) ) {

#if 0
				TRACE(
					"CI: %p == %p? sizes (%dx%d vs. %dx%d)\n",
					this, pOther, 
					m_Size.cx, m_Size.cy,
					pOther->m_Size.cx, pOther->m_Size.cy
				);
#endif

				return false;

			}

			// Use the estimated compressed size as a checksum like value
			// and return that there isn't a match if the estimated sizes
			// are different. 
			// ----------------------------------------------------------------

			if ( pOther->CompressedSizeEstimate() && CompressedSizeEstimate() ) {
				
				if ( pOther->CompressedSizeEstimate() != CompressedSizeEstimate() ) {

#if 0
					TRACE( "CI: %p == %p? size estimation different (%d vs %d)\n",
						this, pOther, CompressedSizeEstimate(), pOther->CompressedSizeEstimate()
					);
#endif

					return false;

				}

			}

			// Now start comparing stored data
			// ----------------------------------------------------------------

			for ( int y = 0; y < m_Size.cy; y++ ) {

#if defined( SRL_USE_STD_COLLECTION )

				typename srl_type::INFO * pInfoA = m_CompressedInfo[ y ];
				typename srl_type::INFO * pInfoB = pOther->m_CompressedInfo[ y ];

#else

				typename srl_type::INFO * pInfoA = &m_CompressedInfo[ y ];
				typename srl_type::INFO * pInfoB = &pOther->m_CompressedInfo[ y ];

#endif

				// Check to see if the collections are the same size if not
				// there is no match ( early bail out gotta love that :)
				// ------------------------------------------------------------

				if ( (pInfoA->first.size() != pInfoB->first.size()) || 
					(pInfoA->second.size() != pInfoB->second.size()) ) {

					return false;

				}

				// compare the codes/control for both collections
				// ------------------------------------------------------------

				typename srl_type::INFO::first_type::iterator codesAIT = pInfoA->first.begin();

				typename srl_type::INFO::first_type::iterator codesBIT = pInfoB->first.begin();

				while ( codesAIT != pInfoA->first.end() ) {

					// --------------------------------------------------------

					if ( *codesAIT++ != *codesBIT++ ) {

						return false;

					}

				}

				// compare the data for both data collections
				// ------------------------------------------------------------

				typename srl_type::INFO::second_type::iterator dataAIT = pInfoA->second.begin();

				typename srl_type::INFO::second_type::iterator dataBIT = pInfoB->second.begin();

				while ( dataAIT != pInfoA->second.end() ) {

					// --------------------------------------------------------

					if ( *dataAIT++ != *dataBIT++ ) {

						return false;

					}

				}

			}

			// If we've gotten here then it must be the same
			// ----------------------------------------------------------------

			return true;

		}

#endif // BPT 5/22/01

		// --------------------------------------------------------------------

		bool SaveTo( class CShowcaseFileIO * io );
		bool LoadFrom( class CShowcaseFileIO * io );

	};

}; // namespace BPT

#endif // !defined(AFX_BPTSRL_H__CCC1EB59_863C_4490_84CA_C1F171D48D87__INCLUDED_)
