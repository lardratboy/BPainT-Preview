#if !defined(AFX_ANIMGRIDVIEW_H__3DA77CDA_4596_4E1D_81A9_CE3E4C8A7294__INCLUDED_)
#define AFX_ANIMGRIDVIEW_H__3DA77CDA_4596_4E1D_81A9_CE3E4C8A7294__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimGridView.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////

#include "bpaintDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CAnimGridView view

class CAnimGridView : public CScrollView
{
public:

	typedef CBpaintDoc::dib_type dib_type;
	typedef dib_type::bitmap_type bitmap_type;

	// ------------------------------------------------------------------------

	struct GridCursorInfo {

		enum Type {

			UNKNOWN		= 0
			,CEL		= 1
			,COL		= 2
			,ROW		= 3

		};

		Type type;

		POINT info;

	};

	// ------------------------------------------------------------------------

protected:

	CAnimGridView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CAnimGridView)

// data
private:

	BPT::CAnimation * m_pAnimation;

	CSize m_ElementFrameSize;
	CSize m_ThumbnailSize;
	CSize m_ElementSize;
	CSize m_ElementBorderSize;
	CSize m_ElementWithBorderSize;
	CSize m_GridSize;
	CSize m_HeaderSize;

	CRect m_ElementSrcRect;
	CRect m_DisplaySrcLimitRect;
	CRect m_RealDisplaySrcLimitRect;
	CRect m_RowHeaderRect;
	CRect m_ColHeaderRect;
	CRect m_CelsRect;
	CRect m_CornerHeaderRect;
	CRect m_CelsClipRect;

	CRect m_HScrollClipRect;
	CRect m_VScrollClipRect;

	GridCursorInfo m_ButtonDownInfo;

	//CBitmap m_HasCelsIndicator;

#if 1 // BPT 6/18/01

	CBitmap m_HasPaletteIndicator;
	CSize m_PalIndicatorSize;
	CPoint m_PalIndicatorOffset;

#endif

#if 1 // BPT 6/18/01

	CBitmap m_ReusedCelIndicator;
	CSize m_ReusedCelIndicatorSize;

#endif

	HCURSOR m_hCrossCrsr;
	CRect m_ClipRect;
	dib_type m_DisplayDib;

	bitmap_type::pixel_type m_BorderColor;
	bitmap_type::pixel_type m_EmptyCelColor;

	int m_StandardHeaderLevel;
	int m_CornerHeaderLevel;

	bitmap_type::pixel_type m_HeaderHColor;
	bitmap_type::pixel_type m_HeaderBColor;
	bitmap_type::pixel_type m_HeaderSColor;

	bitmap_type::pixel_type m_SelectedHeaderHColor;
	bitmap_type::pixel_type m_SelectedHeaderBColor;
	bitmap_type::pixel_type m_SelectedHeaderSColor;

	bitmap_type::pixel_type m_MultiSelectionHColor;
	bitmap_type::pixel_type m_MultiSelectionBColor;
	bitmap_type::pixel_type m_MultiSelectionSColor;

	UINT m_nScrollControlTimer;

#if 1 // BPT 5/25/01

	int m_lastBeginFrame;
	int m_lastEndFrame;
	int m_lastBeginLayer;
	int m_lastEndLayer;

#endif

#if 1 // HACK!

	bitmap_type m_HackElement;

	bitmap_type m_TemporaryCanvasBitmap;

#endif

	bool m_bShowVisualElementIDs;

// internal methods
private:

	void RenderThumbnail(
		CBpaintDoc* pDoc, const int nFrame, 
		const int nLayerStart, const int nLayerEnd,
		bitmap_type * pBitmap, const RECT & location, 
		const RECT * pClip
	);

	void RenderRowHeader(
		bitmap_type * pBitmap, const int x, const int y,
		const int row, const RECT * pClip,
		CBpaintDoc* pDoc, CDC * pDC
	);

	void RenderColHeader(
		bitmap_type * pBitmap, const int x, const int y,
		const int col, const RECT * pClip,
		CBpaintDoc* pDoc, CDC * pDC
	);

	void RenderCel(
		bitmap_type * pBitmap, const int x, const int y,
		const int row, const int col, const RECT * pClip,
		CBpaintDoc* pDoc, CDC * pDC
	);

	void RenderGrid( const RECT * pOptionalClip = 0 );
	void EnsureCanvasRect( const CRect * pLPRect, const CRect *pDPRect );
	void SyncInternals();

	void ClientCoordsToGridInfo(
		GridCursorInfo & outInfo, const CPoint pt,
		const GridCursorInfo * pOverrideInfo 
	);

	void InvalidateLogicalRect(
		const RECT & rc, const BOOL bErase, const BOOL bUpdate,
		const bool bHScrollAdjust = true, const bool bVScrollAdjust = true
	);

	void InvalidateColHeader( const int col, const BOOL bUpdate );
	void InvalidateRowHeader( const int row, const BOOL bUpdate );
	void InvalidateCel( const int row, const int col, const BOOL bUpdate );
	void ChangeSelection( const POINT pNew, const POINT pOld, const BOOL bUpdate );
	void CalcCelRect( CRect & out, const int row, const int col );
	void CalcCelRangeRect( CRect & out, const int brow, const int bcol, const int erow, const int ecol );
	void BrindCelIntoFullView( const int row, const int col );

	void InternalSetCapture();
	void InternalReleaseCapture();

// Attributes
public:
	CBpaintDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimGridView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAnimGridView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnimGridView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnGridShowUids();
	afx_msg void OnUpdateGridShowUids(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AnimGridView.cpp
inline CBpaintDoc* CAnimGridView::GetDocument()
   { return (CBpaintDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMGRIDVIEW_H__3DA77CDA_4596_4E1D_81A9_CE3E4C8A7294__INCLUDED_)
