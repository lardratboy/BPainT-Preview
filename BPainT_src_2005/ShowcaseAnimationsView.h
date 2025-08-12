#if !defined(AFX_SHOWCASEANIMATIONSVIEW_H__9A968E1D_8271_4F63_8E34_C89B4F16A758__INCLUDED_)
#define AFX_SHOWCASEANIMATIONSVIEW_H__9A968E1D_8271_4F63_8E34_C89B4F16A758__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ShowcaseAnimationsView.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////

#include "bpaintDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CShowcaseAnimationsView view

class CShowcaseAnimationsView : public CListView
{
protected:
	CShowcaseAnimationsView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CShowcaseAnimationsView)

// Attributes
public:

private:

	bool FillListWithAnimationInfoCore( BPT::CAnimationShowcase * pShowcase );

	void FillListWithAnimationInfo( BPT::CAnimationShowcase * pShowcase );

// Operations
public:

	CBpaintDoc* GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowcaseAnimationsView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CShowcaseAnimationsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CShowcaseAnimationsView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AnimGridView.cpp
inline CBpaintDoc* CShowcaseAnimationsView::GetDocument()
   { return (CBpaintDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWCASEANIMATIONSVIEW_H__9A968E1D_8271_4F63_8E34_C89B4F16A758__INCLUDED_)
