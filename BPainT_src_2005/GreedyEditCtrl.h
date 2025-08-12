#if !defined(AFX_GREEDYEDITCTRL_H__41E5C763_5D40_4071_9D99_F5F5AA04FC23__INCLUDED_)
#define AFX_GREEDYEDITCTRL_H__41E5C763_5D40_4071_9D99_F5F5AA04FC23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GreedyEditCtrl.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CGreedyEditCtrl window

class CGreedyEditCtrl : public CEdit
{
// Construction
public:
	CGreedyEditCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGreedyEditCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGreedyEditCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGreedyEditCtrl)
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GREEDYEDITCTRL_H__41E5C763_5D40_4071_9D99_F5F5AA04FC23__INCLUDED_)
