// BPTTools.h: interface for the BPTTools class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTTOOLS_H__BBEB0FA3_2D46_4FA1_BC29_D9B4BE22C6E4__INCLUDED_)
#define AFX_BPTTOOLS_H__BBEB0FA3_2D46_4FA1_BC29_D9B4BE22C6E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ============================================================================

#include "BPTBrushTools.h"

// ============================================================================

//
//	CBrushLineTool
//

class CBrushLineTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CBrushLineTool() { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {
	
		EraseLastChanges( info );

		BPT::T_LineDDA( info.renderTarget, m_DownPos, m_UpPos, *info.pPen );

		ToolRenderPostProcess( info );

	}

};

// ============================================================================

//
//	CBrushRectTool
//

class CBrushRectTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CBrushRectTool() { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {

		EraseLastChanges( info );
	
		RECT rect = { m_DownPos.x, m_DownPos.y, m_UpPos.x + 1, m_UpPos.y + 1 };

		BPT::T_EmptyRect( info.renderTarget, rect, *info.pPen );

		base_type::Repaint( info );

		ToolRenderPostProcess( info );
	}

};

//
//	CSolidRectTool
//

class CSolidRectTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CSolidRectTool() { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {

		EraseLastChanges( info );

		// calculate the rect.

		RECT rect;

		CalcRectFromUpAndDownPos( rect );

		// okay get to work.

		BPT::T_SolidRect( info.renderTarget, rect, *info.pPen );

		if ( m_ButtonDown ) {

			SetOverlay( info, tool_msg_type::paint_logic_type::OVERLAY_NONE, 0 );

		} else {

			RECT hair = { m_CurrentPos.x, m_CurrentPos.y, m_CurrentPos.x + 1, m_CurrentPos.y + 1 };

			SetOverlay( info, tool_msg_type::paint_logic_type::OVERLAY_CROSS_HAIRS, &hair );

		}

		base_type::Repaint( info );

		ToolRenderPostProcess( info );
	}

};

//
//	CCropRectTool
//

class CCropRectTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

	bool m_bForceRender;

public: // Interface

	CCropRectTool() : m_bForceRender( false ) { /* Empty */ }

	virtual void InputUp( tool_msg_type & info ) {

		m_bForceRender = true;
		
		base_type::InputUp( info );

		Repaint( info );

		m_bForceRender = false;

		UpdateEcho( info );

	}

	virtual void Repaint( tool_msg_type & info ) {

		EraseLastChanges( info );

		if ( m_ButtonDown || m_bForceRender ) {

			// calculate the rect.

			RECT rect;

			CalcRectFromUpAndDownPos( rect );

			// okay get to work.

			if ( m_ButtonDown ) {

				SetOverlay( info, tool_msg_type::paint_logic_type::OVERLAY_RECT, &rect );

			} else {

				SetOverlay( info, tool_msg_type::paint_logic_type::OVERLAY_NONE, 0 );

				BPT::T_CropRect( info.renderTarget, rect, *info.pPen );

			}
	
			base_type::Repaint( info );
	
			ToolRenderPostProcess( info );

		} else {

			RECT rect = { m_CurrentPos.x, m_CurrentPos.y, m_CurrentPos.x + 1, m_CurrentPos.y + 1 };

			SetOverlay( info, tool_msg_type::paint_logic_type::OVERLAY_CROSS_HAIRS, &rect );

		}

	}

};

// ============================================================================

//
//	CBrushEllipseTool
//

class CBrushEllipseTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CBrushEllipseTool() { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {

		EraseLastChanges( info );

		POINT center = m_DownPos;
	
		int a = abs( m_UpPos.x - m_DownPos.x ) / 2;
		int b = abs( m_UpPos.y - m_DownPos.y ) / 2;

		center.x += ( m_UpPos.x > m_DownPos.x ) ? a : -a;
		center.y += ( m_UpPos.y > m_DownPos.y ) ? b : -b;
	
		BPT::T_Ellipse(
			info.renderTarget,
			center, a, b,
			*info.pPen
		);

		base_type::Repaint( info );

		ToolRenderPostProcess( info );
	}

};

//
//	CSolidEllipseTool
//

class CSolidEllipseTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CSolidEllipseTool() { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {

		EraseLastChanges( info );

		POINT center = m_DownPos;
	
		int a = abs( m_UpPos.x - m_DownPos.x ) / 2;
		int b = abs( m_UpPos.y - m_DownPos.y ) / 2;

		center.x += ( m_UpPos.x > m_DownPos.x ) ? a : -a;
		center.y += ( m_UpPos.y > m_DownPos.y ) ? b : -b;
	
		BPT::T_SolidEllipse(
			info.renderTarget,
			center, a, b,
			*info.pPen
		);

		base_type::Repaint( info );

		ToolRenderPostProcess( info );
	}

};

// ============================================================================

//
//	CBrushCircleTool
//

class CBrushCircleTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CBrushCircleTool() { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {

		EraseLastChanges( info );

		float dx = (float)(m_UpPos.x - m_DownPos.x);
		float dy = (float)(m_UpPos.y - m_DownPos.y);
		int a = (int)(sqrtf( dx * dx + dy * dy ) + 0.5f);

		BPT::T_Ellipse(
			info.renderTarget,
			m_DownPos, a, a,
			*info.pPen
		);

		base_type::Repaint( info );

		ToolRenderPostProcess( info );
	}

};

//
//	CSolidCircleTool
//

class CSolidCircleTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CSolidCircleTool() { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {

		EraseLastChanges( info );

		float dx = (float)(m_UpPos.x - m_DownPos.x);
		float dy = (float)(m_UpPos.y - m_DownPos.y);
		int a = (int)(sqrtf( dx * dx + dy * dy ) + 0.5f);

		BPT::T_SolidEllipse(
			info.renderTarget,
			m_DownPos, a, a,
			*info.pPen
		);

		base_type::Repaint( info );

		ToolRenderPostProcess( info );
	}

};

// ============================================================================

//
//	CBrushSquareTool
//

class CBrushSquareTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CBrushSquareTool() { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {

		EraseLastChanges( info );

		int dx = abs( m_UpPos.x - m_DownPos.x );
		int dy = abs( m_UpPos.y - m_DownPos.y );
		int a = max( dx, dy );

		RECT rect = { m_DownPos.x - a, m_DownPos.y - a, m_DownPos.x + a + 1, m_DownPos.y + a + 1 };

		BPT::T_EmptyRect( info.renderTarget, rect, *info.pPen );

		base_type::Repaint( info );

		ToolRenderPostProcess( info );
	}

};

//
//	CSolidSquareTool
//

class CSolidSquareTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CSolidSquareTool() { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {

		EraseLastChanges( info );

		int dx = abs( m_UpPos.x - m_DownPos.x );
		int dy = abs( m_UpPos.y - m_DownPos.y );
		int a = max( dx, dy );

		RECT rect = { m_DownPos.x - a, m_DownPos.y - a, m_DownPos.x + a + 1, m_DownPos.y + a + 1 };

		BPT::T_SolidRect( info.renderTarget, rect, *info.pPen );

		if ( m_ButtonDown ) {

			SetOverlay( info, tool_msg_type::paint_logic_type::OVERLAY_NONE, 0 );

		} else {

			RECT hair = { m_CurrentPos.x, m_CurrentPos.y, m_CurrentPos.x + 1, m_CurrentPos.y + 1 };

			SetOverlay( info, tool_msg_type::paint_logic_type::OVERLAY_CROSS_HAIRS, &hair );

		}

		base_type::Repaint( info );

		ToolRenderPostProcess( info );
	}

};

// ============================================================================

//
//	CBrushCurveTool
//

class CBrushCurveTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CBrushCurveTool() { /* Empty */ }

	virtual void InputDn( tool_msg_type & info ) {

		if ( 0 == m_InternalState.stage ) {

			CommitChanges( info );

			base_type::InputDn( info );

			Repaint( info );

			return;

		}

		m_ButtonDown = true;

		m_CurrentPos = info.at;

		Repaint( info );

	}

	virtual void InputUp( tool_msg_type & info ) {
		
		if ( 1 == ++m_InternalState.stage ) {

			m_UpPos = info.at;

		}

		m_ButtonDown = false;
		m_LastPos = m_CurrentPos;
		m_CurrentPos = info.at;

		Repaint( info );

		if ( 2 == m_InternalState.stage ) {

			UpdateEcho( info );

			m_InternalState.stage = 0;
			m_CurrentPos = info.at;
			m_UpPos = info.at;
			m_DownPos = info.at;

		}

	}

	virtual void InputMv( tool_msg_type & info ) {

		if ( 0 == m_InternalState.stage ) {

			base_type::InputMv( info );

			Repaint( info );

			return;

		}

		m_CurrentPos = info.at;

		Repaint( info );

	}

	virtual void Repaint( tool_msg_type & info ) {

		EraseLastChanges( info );

		if ( 0 == m_InternalState.stage ) {

			BPT::T_LineDDA( info.renderTarget, m_DownPos, m_UpPos, *info.pPen );

		} else {

			float dx = 
				(float)(((m_DownPos.x - m_CurrentPos.x) * (m_DownPos.x - m_CurrentPos.x)) +
				((m_UpPos.x - m_CurrentPos.x) * (m_UpPos.x - m_CurrentPos.x)));

			float dy = 
				(float)(((m_DownPos.y - m_CurrentPos.y) * (m_DownPos.y - m_CurrentPos.y)) +
				((m_UpPos.y - m_CurrentPos.y) * (m_UpPos.y - m_CurrentPos.y)));

			int segments = (int)((sqrtf( dx + dy ) / 3.14f) + 0.5f);

			BPT::T_dPaintCurve(
				info.renderTarget 
				,m_DownPos
				,m_CurrentPos
				,m_UpPos
				,max( 1, segments )
				,*info.pPen
			);

		}

		ToolRenderPostProcess( info );

	}

};

// ============================================================================

//
//	CFloodFillTool
//

class CFloodFillTool : public CBpaintDoc::instant_tool_base_type {

public: // Traits

	typedef CBpaintDoc::instant_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CFloodFillTool() { /* Empty */ }

	// --------------------------------------------------------------------

	virtual void InstantPaint( tool_msg_type & info ) {

		EraseLastChanges( info );

		BPT::T_SimpleFloodFill( info.renderTarget, m_UpPos, *info.pPen );

		ToolRenderPostProcess( info );

	}

};

// ============================================================================

//
//	CFreehandBrushLineTool
//

class CFreehandBrushLineTool : public CBpaintDoc::freehand_tool_base_type {

public: // Traits

	typedef CBpaintDoc::freehand_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CFreehandBrushLineTool() { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {

		base_type::Repaint( info );

		if ( !m_ButtonDown ) {

			EraseLastChanges( info );

			BPT::T_LineDDA( info.renderTarget, m_CurrentPos, m_CurrentPos, *info.pPen );

		} else {

			BPT::T_LineDDA( info.renderTarget, m_LastPos, m_CurrentPos, *info.pPen );

		}

		ToolRenderPostProcess( info );

	}

};

// ============================================================================

//
//	CFreehandBrushSpottyTool
//

class CFreehandBrushSpottyTool : public CBpaintDoc::freehand_tool_base_type {

public: // Traits

	typedef CBpaintDoc::freehand_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CFreehandBrushSpottyTool() { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {

		base_type::Repaint( info );

		if ( !m_ButtonDown ) {

			EraseLastChanges( info );

			BPT::T_LineDDA( info.renderTarget, m_CurrentPos, m_CurrentPos, *info.pPen );

		} else {

			BPT::T_LineDDA( info.renderTarget, m_CurrentPos, m_CurrentPos, *info.pPen );

		}

		ToolRenderPostProcess( info );

	}

};

// ============================================================================

//
//	CFreehandBrushSingleTool
//

class CFreehandBrushSingleTool : public CBpaintDoc::freehand_tool_base_type {

public: // Traits

	typedef CBpaintDoc::freehand_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

public: // Interface

	CFreehandBrushSingleTool() { /* Empty */ }

	virtual void InputUp( tool_msg_type & info ) {
		
		base_type::base_type::InputUp( info );

//		Repaint( info );

//		UpdateEcho( info );
//		ClearDirtyRectagles( info );

	}

	virtual void Repaint( tool_msg_type & info ) {

		base_type::Repaint( info );

		if ( !m_ButtonDown ) {

			EraseLastChanges( info );

			BPT::T_LineDDA( info.renderTarget, m_CurrentPos, m_CurrentPos, *info.pPen );

		} else {

			BPT::T_LineDDA( info.renderTarget, m_DownPos, m_DownPos, *info.pPen );

		}

		ToolRenderPostProcess( info );

	}

};

// ============================================================================

//
//	CEyedropperTool
//

class CEyedropperTool : public CBpaintDoc::rubberband_tool_base_type {

public: // Traits

	typedef CBpaintDoc::rubberband_tool_base_type base_type;
	typedef base_type::tool_msg_type tool_msg_type;

private: // Data

	CBpaintDoc * m_pDoc;

	CEyedropperTool(); // Hidden

	bool m_bRecursionStopper;

public: // Interface

	CEyedropperTool( CBpaintDoc * pDoc ) : m_pDoc( pDoc ), m_bRecursionStopper( false ) { /* Empty */ }

	virtual void Repaint( tool_msg_type & info ) {

		if ( m_bRecursionStopper ) {

			return;

		}

		m_bRecursionStopper = true;
	
		EraseLastChanges( info );

		if ( m_ButtonDown ) {

			CBpaintDoc::editor_pixel_type color;

			CPoint loc = m_pDoc->GetLastRawInputCoordinate();

			if ( m_pDoc->PickupColor( loc.x, loc.y, &color ) ) {

				if ( tool_msg_type::X_RBUTTON & m_InternalState.buttonDownFlags ) {

					m_pDoc->Request_R_ColorChange( color );

				} else {

					m_pDoc->Request_L_ColorChange( color );

				}

			}

		}

		ToolRenderPostProcess( info );

		m_bRecursionStopper = false;

	}

	virtual void InputUp( tool_msg_type & info ) {
		
		base_type::InputUp( info );

		m_pDoc->Request_PreviousDrawingTool();

	}

};

#endif // !defined(AFX_BPTTOOLS_H__BBEB0FA3_2D46_4FA1_BC29_D9B4BE22C6E4__INCLUDED_)
