// BPTDrawingTools.h: interface for the BPTDrawingTools class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTDRAWINGTOOLS_H__0EB9A019_2ABB_4FE5_9874_481DFE24469D__INCLUDED_)
#define AFX_BPTDRAWINGTOOLS_H__0EB9A019_2ABB_4FE5_9874_481DFE24469D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------------

#include "BPTDrawingPen.h"

// ----------------------------------------------------------------------------

namespace BPT {

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Tool abstract
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class PLOGIC, class RT >
	class TDrawingTool : public TISupportProperties<> {

	protected:

		POINT m_LastPos;
		POINT m_CurrentPos;
		POINT m_DownPos;
		POINT m_UpPos;
		bool m_ButtonDown;

		struct {
			int stage;
			UINT buttonDownFlags;
		} m_InternalState;

	public: // Traits

		// normal traits
		// --------------------------------------------------------------------

		typedef TISupportProperties<> base_type;
		typedef PLOGIC paint_logic_type;
		typedef RT render_target_type;
		typedef TDrawingTool<PLOGIC,RT> this_type;
		typedef this_type & reference;
		typedef this_type * pointer;
		typedef const this_type & const_reference;
		typedef const this_type * const_pointer;
		typedef typename base_type::property_value_type property_value_type;

		// Message passing info.
		// --------------------------------------------------------------------

		struct tool_msg_type {

// DESIGN!!!
// DESIGN!!!
// DESIGN!!!

			// ----------------------------------------------------------------

			typedef PLOGIC paint_logic_type;
			typedef RT render_target_type;
			typedef TDrawingPen<RT> pen_type;

			PLOGIC * pOutterLogic;
			TDrawingPen<RT> * pPen;
			RT renderTarget;
			POINT at;
			UINT nFlags;

			// ----------------------------------------------------------------

			enum {
			
				X_CONTROL		= 0x0001
				,X_LBUTTON		= 0x0002
				,X_MBUTTON		= 0x0004
				,X_RBUTTON		= 0x0008
				,X_SHIFT		= 0x0010
				,X_ALT			= 0x0020
				,X_TRIM_EDGE	= 0x0040

			};
			
			UINT W32FlagsToInternal( const UINT nFlags ) {

				UINT nConverted = 0;

				if ( MK_CONTROL & nFlags ) nConverted |= X_CONTROL;
				if ( MK_LBUTTON & nFlags ) nConverted |= X_LBUTTON;
				if ( MK_MBUTTON & nFlags ) nConverted |= X_MBUTTON;
				if ( MK_RBUTTON & nFlags ) nConverted |= X_RBUTTON;
				if ( MK_ALT & nFlags ) nConverted |= X_ALT; // BPT 10/9/02
				if ( MK_SHIFT & nFlags ) nConverted |= X_SHIFT;

				return nConverted;

			}

// DESIGN!!!
// DESIGN!!!
// DESIGN!!!

		};

	public: // Interface

		TDrawingTool() {

			typename this_type::ClearSettings();

		}

		virtual ~TDrawingTool() { /* empty */ }

		virtual void InputDn( tool_msg_type & info ) {

			m_ButtonDown = true;
			m_DownPos = info.at;
			m_CurrentPos = info.at;
			m_UpPos = info.at;
			m_LastPos = info.at;
			m_InternalState.buttonDownFlags = info.nFlags;
		
		}

		virtual void InputUp( tool_msg_type & info ) {
		
			m_LastPos = m_CurrentPos;
			m_ButtonDown = false;
			m_CurrentPos = info.at;
			m_UpPos = info.at;

		}

		virtual void InputMv( tool_msg_type & info ) {

			m_LastPos = m_CurrentPos;
			m_CurrentPos = info.at;

			m_UpPos = info.at;

			if ( !m_ButtonDown ) {

				m_DownPos = info.at;

			}
		
		}

		virtual void Finish( tool_msg_type & info ) {
		
			if ( m_ButtonDown ) {

				CommitChanges( info );

			} else {

				EraseLastChanges( info );

			}
		
		}

		virtual void Repaint( tool_msg_type & info ) {}

		virtual void Cancel( tool_msg_type & info ) {

			m_ButtonDown = false;
		
		}

		virtual void Stamp( tool_msg_type & info ) {

			EraseLastChanges( info );
			CommitChanges( info );
			UpdateEcho( info );

			info.pPen->Stamp( info.renderTarget );
			ToolRenderPostProcess( info );
			UpdateEcho( info );
		
		}

		// the engage function is how one tool can transfer ownership
		// of it's operating information.

		virtual void TakeSettingsFrom( this_type & from ) {
		
			m_LastPos = from.m_LastPos;
			m_CurrentPos = from.m_CurrentPos;
			m_DownPos = from.m_DownPos;
			m_UpPos = from.m_UpPos;
			m_ButtonDown = from.m_ButtonDown;
			m_InternalState = from.m_InternalState;
			m_InternalState.stage = 0;
		
		}

		// Clear settings
		// --------------------------------------------------------------------

		virtual void ClearSettings() {

			m_InternalState.stage = 0;
			m_InternalState.buttonDownFlags = 0;

			m_ButtonDown = false;
		
			m_UpPos.x = 0;
			m_UpPos.y = 0;

			m_DownPos.x = 0;
			m_DownPos.y = 0;

			m_CurrentPos.x = 0;
			m_CurrentPos.y = 0;

		}

		// --------------------------------------------------------------------

		virtual bool IsButtonDown() {

			return m_ButtonDown;

		}

		// Helpers
		// --------------------------------------------------------------------

		void EraseLastChanges( tool_msg_type & info ) {

			typename PLOGIC::paint_core_type * pPaintCore = info.pOutterLogic->GetEditorCore();

			if ( pPaintCore ) {

				pPaintCore->EraseLastChanges();

			}

		}

		void CommitChanges( tool_msg_type & info ) {

			typename PLOGIC::paint_core_type * pPaintCore = info.pOutterLogic->GetEditorCore();

			if ( pPaintCore ) {

				pPaintCore->CommitChanges();

			}

		}

		void BeginTool( tool_msg_type & info ) {

			typename PLOGIC::paint_core_type * pPaintCore = info.pOutterLogic->GetEditorCore();

			if ( pPaintCore ) {

				pPaintCore->BeginTool();

			}

		}

		void ToolRenderPostProcess( tool_msg_type & info ) {

			typename PLOGIC::paint_core_type * pPaintCore = info.pOutterLogic->GetEditorCore();

			if ( pPaintCore ) {

				pPaintCore->ToolRenderPostProcess();

			}

		}

		void UpdateEcho( tool_msg_type & info, const bool bCopyAll = false ) {

			typename PLOGIC::paint_core_type * pPaintCore = info.pOutterLogic->GetEditorCore();

			if ( pPaintCore ) {

				pPaintCore->UpdateEcho( bCopyAll );

			}

		}

		void ClearDirtyRectagles( tool_msg_type & info ) {

			typename PLOGIC::paint_core_type * pPaintCore = info.pOutterLogic->GetEditorCore();

			if ( pPaintCore ) {

				pPaintCore->ClearDirtyRectagles();

			}

		}

		// --------------------------------------------------------------------

		void SetOverlay( tool_msg_type & info, const int nType, const RECT * pRect ) {

			info.pOutterLogic->SetOverlay( nType, pRect );

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Rubberband tool base class
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class PL >
	class TRubberbandToolBase : public PL::tool_base_type {

	public: // Traits

		typedef typename PL::tool_base_type base_type;
		typedef TRubberbandToolBase<PL> this_type;
		typedef this_type & reference;
		typedef this_type * pointer;
		typedef const this_type & const_reference;
		typedef const this_type * const_pointer;
		typedef typename base_type::tool_msg_type tool_msg_type;
		typedef typename base_type::property_value_type property_value_type;

	public: // Interface

		virtual void InputDn( tool_msg_type & info ) {

			CommitChanges( info );

			typename base_type::InputDn( info );

			Repaint( info );

		}

		virtual void InputUp( tool_msg_type & info ) {
		
			typename base_type::InputUp( info );

			Repaint( info );

			UpdateEcho( info );

		}

		virtual void InputMv( tool_msg_type & info ) {

			typename base_type::InputMv( info );

			Repaint( info );

		}

		// property experiment

#if 1

		virtual void CalcRectFromUpAndDownPos( RECT & rect ) {

			int x1 = min( m_DownPos.x, m_UpPos.x );
			int y1 = min( m_DownPos.y, m_UpPos.y );
			int x2 = max( m_DownPos.x, m_UpPos.x );
			int y2 = max( m_DownPos.y, m_UpPos.y );

			int z = ( typename tool_msg_type::X_TRIM_EDGE & m_InternalState.buttonDownFlags ) ? 0 : 1;

			rect.left = x1;
			rect.top = y1;
			rect.right = x2 + z;
			rect.bottom = y2 + z;

		}

		virtual int GetPropertyCount() {

			if ( !m_ButtonDown ) {

				return 0;
			}
			
			return 2;
		
		}

		virtual int GetPropertyType( const int index ) {

			if ( GetPropertyCount() ) {

				switch ( index ) {
	
				case 0:
					return ISupportPropertyTypes::WIDTH;
					break;
	
				case 1:
					return ISupportPropertyTypes::HEIGHT;
					break;
	
				}

			}

			return ISupportPropertyTypes::UNKNOWN;

		}

		virtual bool GetPropertyValue( const int index, property_value_type & out ) {

			RECT rect;

			CalcRectFromUpAndDownPos( rect );
			
			if ( GetPropertyCount() ) {

				switch ( index ) {
	
				case 0:
					out = rect.right - rect.left;
					return true;
					break;
	
				case 1:
					out = rect.bottom - rect.top;
					return true;
					break;
	
				}

			}

			return false;
		
		}

#endif

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Freehand tool base class
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class PL >
	class TFreehandToolBase : public PL::tool_base_type {

	public: // Traits

		typedef typename PL::tool_base_type base_type;
		typedef TFreehandToolBase<PL> this_type;
		typedef this_type & reference;
		typedef this_type * pointer;
		typedef const this_type & const_reference;
		typedef const this_type * const_pointer;
		typedef typename base_type::tool_msg_type tool_msg_type;
		typedef typename base_type::property_value_type property_value_type;

	public: // Interface

		virtual void InputDn( tool_msg_type & info ) {

			CommitChanges( info );

			typename base_type::InputDn( info );

			Repaint( info );

			UpdateEcho( info );
			ClearDirtyRectagles( info );

		}

		virtual void InputUp( tool_msg_type & info ) {
		
			typename base_type::InputUp( info );

			Repaint( info );

			UpdateEcho( info );
			ClearDirtyRectagles( info );

		}

		virtual void InputMv( tool_msg_type & info ) {

			typename base_type::InputMv( info );

			Repaint( info );

			if ( m_ButtonDown ) {

				UpdateEcho( info );
				ClearDirtyRectagles( info );

			}

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	Instant tool base class
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class PL >
	class TInstantToolBase : public PL::tool_base_type {

	public: // Traits

		typedef typename PL::tool_base_type base_type;
		typedef TInstantToolBase<PL> this_type;
		typedef this_type & reference;
		typedef this_type * pointer;
		typedef const this_type & const_reference;
		typedef const this_type * const_pointer;
		typedef typename base_type::tool_msg_type tool_msg_type;
		typedef typename base_type::property_value_type property_value_type;

	public: // Interface

		virtual void InputDn( tool_msg_type & info ) {

			CommitChanges( info );

			typename base_type::InputDn( info );

		}

		virtual void InputUp( tool_msg_type & info ) {
		
			typename base_type::InputUp( info );

			InstantPaint( info );

			UpdateEcho( info );

		}

		virtual void InputMv( tool_msg_type & info ) {

			typename base_type::InputMv( info );

			EraseLastChanges( info );

		}

		// --------------------------------------------------------------------

		virtual void InstantPaint( tool_msg_type & info ) {

			// Called in the pen up handler, so that tools can
			// be switched (bad idea?)

		}

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	Multiple stage tool base class
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class PL >
	class TMultiStageToolBase : public PL::tool_base_type {

	public: // Traits

		typedef typename PL::tool_base_type base_type;
		typedef TMultiStageToolBase<PL> this_type;
		typedef this_type & reference;
		typedef this_type * pointer;
		typedef const this_type & const_reference;
		typedef const this_type * const_pointer;
		typedef typename base_type::tool_msg_type tool_msg_type;
		typedef typename base_type::property_value_type property_value_type;

	public: // Interface

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	Preview pen tool base class
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	template< class PL >
	class TPreviewToolBase : public PL::tool_base_type {

	public: // Traits

		typedef typename PL::tool_base_type base_type;
		typedef TPreviewToolBase<PL> this_type;
		typedef this_type & reference;
		typedef this_type * pointer;
		typedef const this_type & const_reference;
		typedef const this_type * const_pointer;
		typedef typename base_type::tool_msg_type tool_msg_type;
		typedef typename base_type::property_value_type property_value_type;

	public: // Interface

	};

}; // namespace BPT

#endif // !defined(AFX_BPTDRAWINGTOOLS_H__0EB9A019_2ABB_4FE5_9874_481DFE24469D__INCLUDED_)
