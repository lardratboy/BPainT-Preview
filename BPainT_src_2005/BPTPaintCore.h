// BPTPaintCore.h: interface for the BPTPaintCore class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	This file contains the lowest level of the paint engine, it's
//	responsible for tracking the changed areas and managing the undo
//	state for the paint system. 
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTPAINTCORE_H__2A22DB04_21B5_43BA_ADBD_08F2D6227561__INCLUDED_)
#define AFX_BPTPAINTCORE_H__2A22DB04_21B5_43BA_ADBD_08F2D6227561__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

// ----------------------------------------------------------------------------

#include "BPTBitmap.h"
#include "BPTBlitter.h"
#include "BPTUtility.h"

// ----------------------------------------------------------------------------

namespace BPT {

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Paint core
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class T, class S, class N >
	class TPaintCore {

	public: // Traits

		typedef TPaintCore<T,S,N> this_type;
		typedef this_type & reference;
		typedef this_type * pointer;
		typedef const this_type & const_reference;
		typedef const this_type * const_pointer;

		typedef S stencil_handler_type;
		typedef N change_notification_type;
		typedef T bitmap_type;
		typedef typename bitmap_type::pixel_type pixel_type;
		typedef typename bitmap_type::pointer bitmap_pointer;
		typedef typename bitmap_type::reference bitmap_reference;

		typedef TRectChangeMediator<this_type> dirty_rect_mediator_type;

	private: // Data

		// bitmaps
		// --------------------------------------------------------------------

		bitmap_pointer m_pEchoCanvas;
		bitmap_pointer m_pCurrentCanvas;
		bitmap_pointer m_pLastValidCanvas;
		bitmap_pointer m_pStencilRestoreCanvas;

		// misc info types
		// --------------------------------------------------------------------

		bool m_BoundingRectangleMode;
		SIZE m_Size;

		// Stencil handler
		// --------------------------------------------------------------------

		stencil_handler_type * m_pStencilHandler;

		// Dirty rectangles
		// --------------------------------------------------------------------

		change_notification_type * m_pChangeWatcher;

		typedef std::vector<RECT> dirty_rects_collection;

		dirty_rects_collection m_DirtyRects;

		dirty_rect_mediator_type * m_pDirtyRectHandler;

	private: // Methods

		// Create/destroy interal bitmaps
		// --------------------------------------------------------------------

		bool InternalCreateBitmap( bitmap_pointer & out, const SIZE & size ) {

			// Destroy any exisiting bitmap in the pointer
			// ----------------------------------------------------------------

			InternalDestroyBitmap( out );

			// Allocate the object
			// ----------------------------------------------------------------

			out = new bitmap_type;

			if ( !out ) {

				return false;

			}

			// ----------------------------------------------------------------

			if ( !out->Create( size ) ) {

				delete out;

				out = 0;

				return false;

			}

			// Prep the bitmap for future use
			// ----------------------------------------------------------------

			out->ClearBuffer( 0 );

			return true;

		}

		void InternalDestroyBitmap( bitmap_pointer & out ) {

			if ( out ) {

				delete out;

				out = 0;

			}

		}

		// More buffer related methods
		// --------------------------------------------------------------------

		__forceinline void InternalBlitter( typename T::pointer d, typename T::pointer s, const RECT * pRect = 0 ) {

			T_Blit( *d, 0, 0, *s, TCopyROP<typename T::pixel_type>(), 0, pRect, 0 );

		}

		// Stencil operation
		// --------------------------------------------------------------------

		void PerformStencilOpOnRect( const RECT & rect ) { 

			if ( m_pStencilHandler ) {

				(*m_pStencilHandler)( m_pCurrentCanvas, m_pStencilRestoreCanvas, &rect );

			}

		}

		// Client notification
		// --------------------------------------------------------------------

		void NotifyClient( dirty_rects_collection * pDirtyRects = 0 ) {

			if ( m_pChangeWatcher ) {

				if ( pDirtyRects ) {

					typename dirty_rects_collection::iterator it;
	
					for ( it = pDirtyRects->begin(); it != pDirtyRects->end(); it++ ) {
	
						m_pChangeWatcher->AddDirtyRect( *it );
	
					}

				} else {

					m_pChangeWatcher->AddDirtyRect( m_pCurrentCanvas->Rect() );

				}

			}

		}

	public:

		// construction / destruction
		// --------------------------------------------------------------------

		TPaintCore() {

			m_pDirtyRectHandler = 0;
			m_pStencilHandler = 0;
			m_pChangeWatcher = 0;
			m_BoundingRectangleMode = false;
			m_pCurrentCanvas = 0;
			m_pLastValidCanvas = 0;
			m_pStencilRestoreCanvas = 0;
			m_pEchoCanvas = 0;
			m_Size.cx = 0;
			m_Size.cy = 0;

		}

		~TPaintCore() {

			Destroy();

		}

		// --------------------------------------------------------------------

		void SetStencilRestoreImage() {

			InternalBlitter( m_pStencilRestoreCanvas, m_pEchoCanvas );

		}

		// Initial setup
		// --------------------------------------------------------------------

		bool Initialize() {

			m_DirtyRects.reserve( 8 );

			if ( 0 == m_pDirtyRectHandler ) {

				m_pDirtyRectHandler = new dirty_rect_mediator_type( this );

				if ( 0 == m_pDirtyRectHandler ) {

					return false;

				}

			}

			return true;

		}

		// Setup / shutdown
		// --------------------------------------------------------------------

		bool Create( const SIZE & size, S * pStencilHandler, N * pChangeWatcher ) {

			// Hookup the stencil handler
			// ----------------------------------------------------------------

			m_pStencilHandler = pStencilHandler;

			// Hook up the watcher
			// ----------------------------------------------------------------

			m_pChangeWatcher = pChangeWatcher;

			// Release any resources currently held
			// ----------------------------------------------------------------

			Destroy();

			// Try to create the various support bitmaps
			// ----------------------------------------------------------------

			if ( !InternalCreateBitmap( m_pCurrentCanvas, size) ) {

				goto FAILURE;

			}

			if ( !InternalCreateBitmap( m_pLastValidCanvas, size) ) {

				goto FAILURE;

			}

			if ( !InternalCreateBitmap( m_pEchoCanvas, size) ) {

				goto FAILURE;

			}

			if ( !InternalCreateBitmap( m_pStencilRestoreCanvas, size ) ) {

				goto FAILURE;

			}
			
			m_Size = size;

			return true;

			// Error handle in one place
			// ----------------------------------------------------------------

		FAILURE:

			Destroy();

			return false;

		}

		void Destroy() {

			// Destroy the various support bitmaps
			// ----------------------------------------------------------------

			InternalDestroyBitmap( m_pEchoCanvas );
			InternalDestroyBitmap( m_pCurrentCanvas );
			InternalDestroyBitmap( m_pLastValidCanvas );
			InternalDestroyBitmap( m_pStencilRestoreCanvas );

			// Reset the size to empty
			// ----------------------------------------------------------------

			m_Size.cx = 0;
			m_Size.cy = 0;

		}

		// Init
		// --------------------------------------------------------------------

		bool InitCanvas( typename bitmap_type::pointer pFrom, const pixel_type value = pixel_type( 0 ) ) {

			ClearDirtyRectagles();

			if ( m_pCurrentCanvas && m_pLastValidCanvas ) {

				if ( pFrom ) {

#if 1
					if ( CSize( pFrom->Size() ) != CSize( m_pEchoCanvas->Size() ) ) {
	
						m_pEchoCanvas->ClearBuffer( value );
						m_pCurrentCanvas->ClearBuffer( value );
						m_pLastValidCanvas->ClearBuffer( value );
						m_pStencilRestoreCanvas->ClearBuffer( value );

					}
#endif
	
					InternalBlitter( m_pEchoCanvas, pFrom );
					InternalBlitter( m_pCurrentCanvas, pFrom );
					InternalBlitter( m_pLastValidCanvas, pFrom );
					InternalBlitter( m_pStencilRestoreCanvas, pFrom );
				
				} else {
	
					m_pEchoCanvas->ClearBuffer( value );
					m_pCurrentCanvas->ClearBuffer( value );
					m_pLastValidCanvas->ClearBuffer( value );
					m_pStencilRestoreCanvas->ClearBuffer( value );
	
				}

				NotifyClient();

				return true;

			}

			return false;

		}

		// Session management
		// --------------------------------------------------------------------

		bool BeginSession(
			typename bitmap_type::pointer pCanvas, 
			const pixel_type value = pixel_type( 0 )
		) {

			// Init the various support elements
			// ----------------------------------------------------------------

			if ( !InitCanvas( pCanvas, value ) ) {

				return false;

			}

			return true;

		}

		void EndSession() {

			CommitChanges();

		}

		// Dirty rectangle
		// --------------------------------------------------------------------

		void AddDirtyRect( const RECT & area ) {

			if ( (area.left != area.right) && (area.top != area.bottom) ) {

				// Convert the dirty rectangles to a single bounding rect?
				// ------------------------------------------------------------

				if ( m_BoundingRectangleMode && (!m_DirtyRects.empty()) ) {

					RECT & dr = m_DirtyRects.back();

					dr.left = min( dr.left, area.left );
					dr.right = max( dr.right, area.right );
					dr.top = min( dr.top, area.top );
					dr.bottom = max( dr.bottom, area.bottom );

				} else {

					m_DirtyRects.push_back( area );

				}

			}

		}

		// Change management operation
		// --------------------------------------------------------------------

		void ClearDirtyRectagles() {

			m_DirtyRects.clear();

		}

		void EraseLastChanges() {

			if ( !m_DirtyRects.empty() ) {

				// Copy the last buffer forward to erase the last areas, do this
				// first so that if the client imediately updates it's views it
				// will be correct.
				// ----------------------------------------------------------------

				typename dirty_rects_collection::iterator it;

				for ( it = m_DirtyRects.begin(); it != m_DirtyRects.end(); it++ ) {

					InternalBlitter( m_pCurrentCanvas, m_pEchoCanvas, &(*it) );

				}

				// Tell the client to update the last changed area
				// ------------------------------------------------------------

				NotifyClient( &m_DirtyRects );

				// ------------------------------------------------------------

				ClearDirtyRectagles();

			}

		}

		void CommitChanges() {

			InternalBlitter( m_pLastValidCanvas, m_pEchoCanvas );

			ClearDirtyRectagles();

		}

		// Begin tool 
		// --------------------------------------------------------------------

		void SetBoundingRectangleMode( const bool boundingRectangleMode = true ) {

			m_BoundingRectangleMode = boundingRectangleMode;

		}

		bool BeginTool( const bool bCopyAllIfNotDirty = false ) {

			// Start by comiting active changes to the last buffer
			// ----------------------------------------------------------------

			CommitChanges();

			return true;

		}

		// Post process
		// --------------------------------------------------------------------

		void ToolRenderPostProcess() {

			// Perform the stencil operation as a post process
			// ----------------------------------------------------------------

			typename dirty_rects_collection::iterator it;

			RECT * pLastRect = 0;

			for ( it = m_DirtyRects.begin(); it != m_DirtyRects.end(); it++ ) {

				RECT * pRect = &(*it);

				if ( pLastRect ) {

					RECT overlap;

					if ( IntersectRect( &overlap, pLastRect, pRect ) ) {

						RECT results[ 4 ];

						int count = ReturnExternalClipRects(
							results, &overlap, pRect
						);

						for ( int i = 0; i < count; i++ ) {

							PerformStencilOpOnRect( results[ i ] );

						}

					} else {

						PerformStencilOpOnRect( *pRect );

					}

				} else {

					PerformStencilOpOnRect( *pRect );

				}

				pLastRect = pRect;

			}

			// Perform any other post tool needs
			// ----------------------------------------------------------------

			if ( !m_DirtyRects.empty() ) {

				NotifyClient( &m_DirtyRects );

			}

		}

		// Echo buffer management helper
		// --------------------------------------------------------------------

		void UpdateEcho( const bool bCopyAll = false ) {

			// Special case copy all
			// ----------------------------------------------------------------

			if ( bCopyAll ) {

				InternalBlitter( m_pEchoCanvas, m_pCurrentCanvas );
				return;

			}

			// Perform the stencil operation as a post process
			// ----------------------------------------------------------------

			typename dirty_rects_collection::iterator it;

			RECT * pLastRect = 0;

			for ( it = m_DirtyRects.begin(); it != m_DirtyRects.end(); it++ ) {

				RECT * pRect = &(*it);

				if ( pLastRect ) {

					RECT overlap;

					if ( IntersectRect( &overlap, pLastRect, pRect ) ) {

						RECT results[ 4 ];

						int count = ReturnExternalClipRects(
							results, &overlap, pRect
						);

						for ( int i = 0; i < count; i++ ) {

							InternalBlitter( m_pEchoCanvas, m_pCurrentCanvas, &results[ i ] );

						}

					} else {

						InternalBlitter( m_pEchoCanvas, m_pCurrentCanvas, pRect );

					}

				} else {

					InternalBlitter( m_pEchoCanvas, m_pCurrentCanvas, pRect );

				}

				pLastRect = pRect;

			}

		}

		// UNDO code
		// --------------------------------------------------------------------

		void Undo() {

			ClearDirtyRectagles();

			typename T::pointer temp = m_pLastValidCanvas;
			m_pLastValidCanvas = m_pEchoCanvas;
			m_pEchoCanvas = temp;

			InternalBlitter( m_pCurrentCanvas, m_pEchoCanvas );

			NotifyClient();

		}

		// Clear code?
		// --------------------------------------------------------------------

		void Clear( const pixel_type value ) {

			CommitChanges();

			m_pCurrentCanvas->ClearBuffer( value );

			AddDirtyRect( m_pCurrentCanvas->Rect() );

			ToolRenderPostProcess();

			UpdateEcho();

		}

		// Shift? (This could be done way faster!)
		// --------------------------------------------------------------------

		void Shift( const int dx, const int dy, const pixel_type fillValue ) {

			CommitChanges();

			m_pCurrentCanvas->ClearBuffer( fillValue );

			T_Blit( *m_pCurrentCanvas, dx, dy, *m_pEchoCanvas, TCopyROP<typename T::pixel_type>() );

			InternalBlitter( m_pEchoCanvas, m_pCurrentCanvas );

			AddDirtyRect( m_pCurrentCanvas->Rect() );

			ToolRenderPostProcess();

			UpdateEcho();

		}

		// Flip? (This could be done way faster!)
		// --------------------------------------------------------------------

		void HFlip() {

			CommitChanges();

			T_Blit( 
				*m_pCurrentCanvas, 0, 0, *m_pEchoCanvas, 
				TCopyROP<typename T::pixel_type>(), &BLITFX( BLITFX::HFLIP )
			);

			InternalBlitter( m_pEchoCanvas, m_pCurrentCanvas );

			AddDirtyRect( m_pCurrentCanvas->Rect() );

			ToolRenderPostProcess();

			UpdateEcho();

		}

		void VFlip() {

			CommitChanges();

			T_Blit( 
				*m_pCurrentCanvas, 0, 0, *m_pEchoCanvas, 
				TCopyROP<typename T::pixel_type>(), &BLITFX( BLITFX::VFLIP )
			);

			InternalBlitter( m_pEchoCanvas, m_pCurrentCanvas );

			AddDirtyRect( m_pCurrentCanvas->Rect() );

			ToolRenderPostProcess();

			UpdateEcho();

		}

		// External use (Only valid for instant use...)
		// --------------------------------------------------------------------

		bitmap_pointer LastValidBitmap() {

			return m_pLastValidCanvas;

		}

		bitmap_pointer CurrentBitmap() {

			return m_pCurrentCanvas;

		}

		template< class RT > bool SetupRenderTarget( RT & renderTarget ) {

			renderTarget.SetCanvas( CurrentBitmap() );

			renderTarget.SetDirtyRectHandler( m_pDirtyRectHandler );

			return true;

		}

	};

}; // namespace BPT

#endif // !defined(AFX_BPTPAINTCORE_H__2A22DB04_21B5_43BA_ADBD_08F2D6227561__INCLUDED_)
