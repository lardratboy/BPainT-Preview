#if !defined(AFX_VISUALELEMENTINFOVIEW_H__64487FC2_4527_45E9_9FBA_E5FA2075773B__INCLUDED_)
#define AFX_VISUALELEMENTINFOVIEW_H__64487FC2_4527_45E9_9FBA_E5FA2075773B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VisualElementInfoView.h : header file
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
// CVisualElementInfoView view

class CVisualElementInfoView : public CEditView
{
protected:
	CVisualElementInfoView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CVisualElementInfoView)

	bool HasActiveLayer();

// Attributes
public:

	void SyncTextToCurrentVisualElement();

// Operations
public:

	CBpaintDoc* GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualElementInfoView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CVisualElementInfoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CVisualElementInfoView)
	afx_msg void OnVemodeReplaceAsk();
	afx_msg void OnUpdateVemodeReplaceAsk(CCmdUI* pCmdUI);
	afx_msg void OnVemodeReplaceDontAsk();
	afx_msg void OnUpdateVemodeReplaceDontAsk(CCmdUI* pCmdUI);
	afx_msg void OnVemodeSimple();
	afx_msg void OnUpdateVemodeSimple(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AnimGridView.cpp
inline CBpaintDoc* CVisualElementInfoView::GetDocument()
   { return (CBpaintDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISUALELEMENTINFOVIEW_H__64487FC2_4527_45E9_9FBA_E5FA2075773B__INCLUDED_)
