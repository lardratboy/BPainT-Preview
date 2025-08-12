// NullView.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited

#if !defined(AFX_NULLVIEW_H__65AC2C56_15A4_40D9_B64F_6CED9EAE1FF2__INCLUDED_)
#define AFX_NULLVIEW_H__65AC2C56_15A4_40D9_B64F_6CED9EAE1FF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNullView view

class CNullView : public CView
{
protected:
	CNullView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNullView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNullView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CNullView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CNullView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NULLVIEW_H__65AC2C56_15A4_40D9_B64F_6CED9EAE1FF2__INCLUDED_)
