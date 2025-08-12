#if !defined(AFX_SIMPLEPALETTEVIEW_H__CFBA3AA7_308F_47A1_A094_A06CFB41A9E0__INCLUDED_)
#define AFX_SIMPLEPALETTEVIEW_H__CFBA3AA7_308F_47A1_A094_A06CFB41A9E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SimplePaletteView.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//

// ----------------------------------------------------------------------------

#include "BPTDib.h"
#include "BPTBlitter.h"

////////////////////////////////////////////////////////////////////////
// CSimplePaletteView view

class CSimplePaletteView : public CView
{

public:

	typedef BPT::TDIBSection<SDisplayPixelType> dib_type;

	enum BUTTON_MODE {

		NONE		= 0
		,LEFT		= 1
		,RIGHT		= 2

	};
private: // data

	BUTTON_MODE m_ButtonMode;

	HCURSOR m_hGenericPaletteCrsr;
	HCURSOR m_hChoose_L_Crsr;
	HCURSOR m_hChoose_R_Crsr;

	dib_type m_DisplayDib;

	CSize m_ElementSize;

	int m_Overhead;
	int m_Last_L_Color;
	int m_Last_R_Color;

	CRect m_PaletteArea;
	CRect m_StatusArea;
	CRect m_LStatusRect;
	CRect m_RStatusRect;

private: // methods

	bool SetElementSize( const CSize size, const int overhead, const bool adjustFrameWnd = true );
	void RedrawPaletteBitmap( const CRect * pRect );
	void InvalidateColor( const int slot, const bool bUpdate = false );
	void CalcColorRect( CRect & rect, const int slot );
	void DrawSelectionForSlot( dib_type::bitmap_type * pBits, const int slot, const CRect * pClipRect = 0);
	void HandleColorSelection( UINT nFlags, CPoint point );
	int GetColorFromPoint( CPoint point );

	dib_type::bitmap_type::pixel_type GetColorForSlot( const int slot );

protected:

	CSimplePaletteView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSimplePaletteView)

// Attributes
public:

	CBpaintDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimplePaletteView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSimplePaletteView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CSimplePaletteView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SimplePaletteView.cpp
inline CBpaintDoc* CSimplePaletteView::GetDocument()
   { return (CBpaintDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEPALETTEVIEW_H__CFBA3AA7_308F_47A1_A094_A06CFB41A9E0__INCLUDED_)
