// BPTPaintLogic.h: interface for the BPTPaintLogic class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	This file contains the middle level logic for the paint system
//	this probably contains too many classes.  A refactoring session
//	is in order after the project is done. The update rectangle idea
//	started to get a little complicated so I abandoned the idea half
//	way through the implementation, that is way this code only deals
//	with a single bounding rectangle while the lower level code has
//	the ability to keep a much greater level of detail on what has 
//	actually changed.  The bounding rectangle is just fine speed wise
//	and probably the lower level editor core should be made to use 
//	that technique as well.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTPAINTLOGIC_H__320BBE19_04AA_4A1D_955D_90100FFE7280__INCLUDED_)
#define AFX_BPTPAINTLOGIC_H__320BBE19_04AA_4A1D_955D_90100FFE7280__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

#include "BPTPaintCore.h"
#include "BPTDrawingTools.h"
#include "BPTRasterOps.h"

namespace BPT {

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	
	// Stencil handler
	//	
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class T >
	class T_TableDriven_StencilHandler {

	public: // traits

		enum {

			TOTAL_COLORS = 256		 // This is dependent on 8bpp mode!!!

		};

	private:

		typedef TTransparentTableSrcTransferROP<typename T::pixel_type,int *> transfer_type;

		int m_Table[ TOTAL_COLORS ];

		bool m_bEnabled;

	public:

		// --------------------------------------------------------------------

		T_TableDriven_StencilHandler() : m_bEnabled( false ) { /* Empty */ }

		// --------------------------------------------------------------------

		void operator()(
			T * pCurrent, T * pLast, const RECT * pRect
		) {

			if ( m_bEnabled ) {

				T_Blit( *pCurrent, 0, 0, *pLast, transfer_type( m_Table ), 0, pRect, 0 );

			}

		}

		// --------------------------------------------------------------------

		bool Enable( const bool bEnabled, const bool * pTable ) {

			m_bEnabled = bEnabled;

			if ( pTable ) {

				for ( int i = 0; i < TOTAL_COLORS; i++ ) {
	
					m_Table[ i ] = pTable[ i ] ? 1 : 0;
	
				}

			}

			return true;

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Editor mid level	logic
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class FEEDBACK_OWNER, class T, class CCNT, class TOOLIDTYPE = int >
	class TPaintLogic {

	public: // Traits

		typedef T bitmap_type;
		typedef TPaintLogic<FEEDBACK_OWNER,T,CCNT,TOOLIDTYPE> this_type;
		typedef TRectChangeMediator<this_type> rect_change_mediator;
		typedef T_TableDriven_StencilHandler<T> stencil_handler_type;
		typedef TPaintCore<T,stencil_handler_type,rect_change_mediator> paint_core_type;
		typedef TRenderTarget<T> render_target_type;
		typedef TDrawingPen<render_target_type> pen_base_type;
		typedef TDrawingTool<this_type,render_target_type> tool_base_type;
		typedef std::map<TOOLIDTYPE,typename tool_base_type::pointer> tool_map_collection_type;
		typedef CCNT client_change_notification_type;
		typedef typename tool_base_type::tool_msg_type tool_msg_type;
		typedef this_type & reference;
		typedef this_type * pointer;
		typedef const this_type & const_reference;
		typedef const this_type * const_pointer;

		enum FEEDBACK {

			UNKNOWN					= 0
			,TOOL_CHANGE			= 1
			,OVERLAY_CHANGE			= 2

		};

	private: // Data

		// Object the 'owns' feedback this drawing object.
		// --------------------------------------------------------------------

		FEEDBACK_OWNER * m_pFeedbackOwner;

		// Current tool info
		// --------------------------------------------------------------------

		typename tool_base_type::pointer m_pCurrentTool;

		pen_base_type * m_pCurrentPen;

		// Misc data
		// --------------------------------------------------------------------

		RECT m_BoundingUpdateRect;
		RECT m_WholeCanvasRect;
		SIZE m_Size;
		bool m_bTrimEdge;
		CGridInfo * m_pGridInfo;

		// Rectangle change mediator
		// --------------------------------------------------------------------

		rect_change_mediator * m_pRectChangeHandler;

		client_change_notification_type * m_pClientRectChangeHandler;

		// Stencil handler
		// --------------------------------------------------------------------

		stencil_handler_type m_StencilHandler;

		// Core editor
		// --------------------------------------------------------------------

		paint_core_type m_PaintCore;

		// Tool map
		// --------------------------------------------------------------------

		tool_map_collection_type m_ToolMap;

	private: // methods

		void Feedback( const FEEDBACK id, void * pData ) {

			if ( m_pFeedbackOwner ) {

				m_pFeedbackOwner->Feedback( this, id, pData );

			}

		}

	public: // methods/interface

		// Construction / destruction
		// --------------------------------------------------------------------

		TPaintLogic( FEEDBACK_OWNER * pFeedbackOwner = 0 ) : 
			m_pFeedbackOwner( pFeedbackOwner ) 
			,m_pRectChangeHandler( 0 ) 
			,m_pCurrentTool( 0 )
			,m_pCurrentPen( 0 )
			,m_bTrimEdge( false )
			,m_pGridInfo( 0 )
		{

			SetRectEmpty( &m_BoundingUpdateRect );

		}

		~TPaintLogic() {

			if ( m_pRectChangeHandler ) {

				delete m_pRectChangeHandler;

			}

#if 1

			tool_map_collection_type::iterator it;

			while ( (it = m_ToolMap.begin()) != m_ToolMap.end() ) {

				delete (*it).second;

				m_ToolMap.erase( it );

			}

#endif
		}

		// Initial setup
		// --------------------------------------------------------------------

		void SetFeedbackOwner( FEEDBACK_OWNER * pFeedbackOwner ) {

			m_pFeedbackOwner = pFeedbackOwner;

		}

		bool Initialize() {

			// Make sure the editor core has intialized itself
			// ----------------------------------------------------------------

			if ( !m_PaintCore.Initialize() ) {

				return false;

			}

			// Create the rectangle mediator
			// ----------------------------------------------------------------

			if ( 0 == m_pRectChangeHandler ) {

				m_pRectChangeHandler = new rect_change_mediator( this );

				if ( 0 == m_pRectChangeHandler ) {

					return false;

				}

			}

			return true;

		}

		// Setup management
		// --------------------------------------------------------------------

		bool Create( const SIZE & size, client_change_notification_type * pChangeWatcher ) {

			// Ask the editor to size itself and hook  up the various handlers
			// ----------------------------------------------------------------

			if ( !m_PaintCore.Create( size, &m_StencilHandler, m_pRectChangeHandler ) ) {

				return false;

			}

			// Turn on bounding rect update mode for the paint core.
			// ----------------------------------------------------------------

			m_PaintCore.SetBoundingRectangleMode( true ); 

			// Setup the misc helper member variables
			// ----------------------------------------------------------------

			m_Size = size;
			m_WholeCanvasRect.left = 0;
			m_WholeCanvasRect.top = 0;
			m_WholeCanvasRect.right = size.cx;
			m_WholeCanvasRect.bottom = size.cy;
			m_pClientRectChangeHandler = pChangeWatcher;

			// ----------------------------------------------------------------

			return true;

		}

		// Tool manager logic (should this be a class?)
		// --------------------------------------------------------------------

		bool SelectTool( typename tool_base_type::pointer pTool ) {

			// Same as current tool if so this is a NOP.
			// ----------------------------------------------------------------

			if ( pTool == m_pCurrentTool ) {

				return true;

			}

			// Ask the tool to transfer it's internal settings
			// ----------------------------------------------------------------

			if ( pTool && m_pCurrentTool ) {

				pTool->TakeSettingsFrom( *m_pCurrentTool );

			}

			// Select the tool
			// ----------------------------------------------------------------

			m_pCurrentTool = pTool;

			// Need to repaint...
			// ----------------------------------------------------------------

			Edit_OnRepaint();

			// Handle the feedback logic
			// ----------------------------------------------------------------

			TOOLIDTYPE id = GetIdForTool( pTool );

			Feedback( TOOL_CHANGE, &id );

#if 1

			// Force the overlay to none so that the existing tools
			// don't have to deal with this overlay hack!

			SetOverlay( OVERLAY_NONE, 0 );

#endif

			return true;

		}

		void PostToolHack() {

			if ( m_pCurrentTool ) {

				m_pCurrentTool->ClearSettings();

			}

		}

		bool SelectTool( const TOOLIDTYPE & id ) {

			return SelectTool( GetToolForId( id ) );

		}

		bool RegisterTool( const TOOLIDTYPE & id, typename tool_base_type::pointer pTool ) {

			if ( !pTool ) {

				return UnregisterTool( id );

			}

			m_ToolMap[ id ] = pTool;

			return true;

		}

		bool UnregisterTool( const TOOLIDTYPE & id ) {

			tool_map_collection_type::iterator found = m_ToolMap.find( id );

			if ( m_ToolMap.end() != found ) {

				m_ToolMap.erase( found );
				return true;

			}

			return false;

		}

		typename tool_base_type::pointer GetToolForId( const TOOLIDTYPE & id ) {

			tool_map_collection_type::iterator found = m_ToolMap.find( id );

			if ( m_ToolMap.end() != found ) {

				return (*found).second;

			}

			return typename tool_base_type::pointer( 0 );

		}

		TOOLIDTYPE GetIdForTool(
			const typename tool_base_type::pointer pTool,
			const TOOLIDTYPE defaultReturn = TOOLIDTYPE( 0 )
		) {

			tool_map_collection_type::iterator it = m_ToolMap.begin();

			for ( ; it != m_ToolMap.end(); it++ ) {

				if ( pTool == (*it).second ) {

					return (*it).first;

				}

			}

			return defaultReturn;

		}

		TOOLIDTYPE GetCurrentToolID() {

			return GetIdForTool( m_pCurrentTool, 0 );

		}

		typename tool_base_type::pointer GetCurrentToolPtr() {

			return m_pCurrentTool;

		}

		// Rectangle change handler
		// --------------------------------------------------------------------

		void AddDirtyRect( const RECT & rect ) {

			if ( !IsRectEmpty( &rect ) ) {

				if ( !IsRectEmpty( &m_BoundingUpdateRect ) ) {

					UnionRect( &m_BoundingUpdateRect, &m_BoundingUpdateRect, &rect );

				} else {

					m_BoundingUpdateRect = rect;

				}

			}

		}

		void NotifyClient() {

			if ( !IsRectEmpty( &m_BoundingUpdateRect ) ) {

				if ( m_pClientRectChangeHandler ) {

					m_pClientRectChangeHandler->AddDirtyRect(
						m_BoundingUpdateRect
					);

				}

				SetRectEmpty( &m_BoundingUpdateRect );

			}

		}

		// Set current pen
		// --------------------------------------------------------------------

		void SetPen( pen_base_type * pPen ) {

			m_pCurrentPen = pPen;

			Edit_OnRepaint();

		}

		pen_base_type * GetPen() {

			return m_pCurrentPen;

		}

		// Tool Message prep
		// --------------------------------------------------------------------

		void PrepToolMessage(
			tool_msg_type & msg
			,UINT nFlags = 0
			,CPoint * pPoint = 0
		) {

			msg.nFlags = msg.W32FlagsToInternal( nFlags );

			if ( m_bTrimEdge ) {

				msg.nFlags |= tool_msg_type::X_TRIM_EDGE;

			}

			msg.pPen = GetPen();

			msg.pOutterLogic = this;

			if ( pPoint ) {

				msg.at = *pPoint;

			}

			m_PaintCore.SetupRenderTarget( msg.renderTarget );

			msg.renderTarget.SetGridInfo( m_pGridInfo );

		}

		// Message distribution
		// --------------------------------------------------------------------

		void Edit_OnDown( UINT nFlags, CPoint point) {

			if ( m_pCurrentTool && m_pCurrentPen ) {

				tool_msg_type msg;

				PrepToolMessage( msg, nFlags, &point );

				m_pCurrentTool->InputDn( msg );

			}

			NotifyClient();

		}

		void Edit_OnUp( UINT nFlags, CPoint point ) {

			if ( m_pCurrentTool && m_pCurrentPen ) {

				tool_msg_type msg;

				PrepToolMessage( msg, nFlags, &point );

				m_pCurrentTool->InputUp( msg );

			}

			NotifyClient();

		}

		void Edit_OnMove( UINT nFlags, CPoint point ) {

			if ( m_pCurrentTool && m_pCurrentPen ) {

				tool_msg_type msg;

				PrepToolMessage( msg, nFlags, &point );

				m_pCurrentTool->InputMv( msg );

			}

			NotifyClient();

		}

		void Edit_OnCancel() {

			if ( m_pCurrentTool && m_pCurrentPen ) {

				tool_msg_type msg;

				PrepToolMessage( msg );

				m_pCurrentTool->Cancel( msg );

			}

			NotifyClient();

		}

		void Edit_OnFinish() {

			if ( m_pCurrentTool && m_pCurrentPen ) {

				tool_msg_type msg;

				PrepToolMessage( msg );

				m_pCurrentTool->Finish( msg );

			}

			NotifyClient();

		}

		void Edit_OnRepaint() {

			if ( m_pCurrentTool && m_pCurrentPen ) {

				tool_msg_type msg;

				PrepToolMessage( msg );

				m_pCurrentTool->Repaint( msg );

			}

			NotifyClient();

		}

		// command messages.
		// --------------------------------------------------------------------

		bool CanClear() {

			return true;

		}

		void Clear( const typename bitmap_type::pixel_type value ) {

			m_PaintCore.Clear( value );

			// Need to repaint but what pen should it use?

			Edit_OnRepaint();

		}

		bool CanUndo() {

			return true;

		}

		void Undo() {

			m_PaintCore.Undo();

			// Need to repaint but what pen should it use?

			Edit_OnRepaint();

		}

		// --------------------------------------------------------------------

		void Shift( const int dx, const int dy, const typename bitmap_type::pixel_type fillValue ) {

			m_PaintCore.Shift( dx, dy, fillValue );

			// Need to repaint but what pen should it use?

			Edit_OnRepaint();

		}

		// --------------------------------------------------------------------

		void HFlip( ) {

			m_PaintCore.HFlip();

			// Need to repaint but what pen should it use?

			Edit_OnRepaint();

		}

		void VFlip( ) {

			m_PaintCore.VFlip();

			// Need to repaint but what pen should it use?

			Edit_OnRepaint();

		}

		// --------------------------------------------------------------------

		void Stamp() {

			if ( m_pCurrentTool && m_pCurrentPen ) {

				tool_msg_type msg;

				PrepToolMessage( msg );

				m_pCurrentTool->Stamp( msg );

				Edit_OnRepaint();

			}

		}

		// Buffer setup / retrieval methods
		// --------------------------------------------------------------------

		typename bitmap_type::pointer GetCurentBitmapPtr() {

			return m_PaintCore.CurrentBitmap();

		}

		typename bitmap_type::pointer GetFinalBitmapPtr() {

			return m_PaintCore.LastValidBitmap();

		}

		paint_core_type * GetEditorCore() {

			return &m_PaintCore;

		}

		// --------------------------------------------------------------------

		bool GetTrimEdgeMode() const {

			return m_bTrimEdge;

		}

		void SetTrimEdgeMode( const bool bEnable ) {

			m_bTrimEdge = bEnable;

		}
	
		// --------------------------------------------------------------------

		bool BeginSession(
			typename bitmap_type::pointer pCanvas, 
			const typename bitmap_type::pixel_type value = pixel_type( 0 )
		) {

#if 1
			if ( !m_PaintCore.BeginSession( pCanvas, value ) ) {

				return false;

			}

			if ( m_pCurrentTool && m_pCurrentTool->IsButtonDown() ) {

				Edit_OnRepaint();

			}

			return true;

#else

			return m_PaintCore.BeginSession( pCanvas, value );

#endif

		}

		void EndSession() {

			m_PaintCore.EndSession();

		}

		// --------------------------------------------------------------------

		bool SetStencilMode( const bool bStencilEnabled, const bool * pStencilTable, const bool bBuild = true ) {

			if ( bBuild ) {

				m_PaintCore.SetStencilRestoreImage();

			}

			return m_StencilHandler.Enable(
				bStencilEnabled, pStencilTable
			);

		}

		// --------------------------------------------------------------------

		bool SetGridInfo( CGridInfo * pGridInfo ) {

			m_pGridInfo = pGridInfo;
			
			return true;

		}

		// --------------------------------------------------------------------
		// Overlay (HACK!)
		// --------------------------------------------------------------------

		enum {

			OVERLAY_NONE			= 0
			,OVERLAY_RECT			= 1
			,OVERLAY_CROSS_HAIRS	= 2

		};

		struct overlay_info {

			RECT rcRect;

			int nType;

			overlay_info() : nType( OVERLAY_NONE ) {

				SetRectEmpty( &rcRect );

			}

		};

		void SetOverlay( const int nType, const RECT * pRect ) {

			overlay_info info;

			if ( pRect ) {
			
				info.rcRect = *pRect;

			} else {

				SetRectEmpty( &info.rcRect );

			}

			info.nType = nType;

			Feedback( OVERLAY_CHANGE, &info );

		}

	};

}; // namespace BPT

#endif // !defined(AFX_BPTPAINTLOGIC_H__320BBE19_04AA_4A1D_955D_90100FFE7280__INCLUDED_)
