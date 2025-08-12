// SelectAnimationDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_SELECTANIMATIONDLG_H__25651BD6_86AD_47A4_918A_3093FC9F8771__INCLUDED_)
#define AFX_SELECTANIMATIONDLG_H__25651BD6_86AD_47A4_918A_3093FC9F8771__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BPTAnimator.h"

/////////////////////////////////////////////////////////////////////////////
// CSelectAnimationDlg dialog

class CSelectAnimationDlg : public CDialog
{
// Construction
public:

	CSelectAnimationDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectAnimationDlg)
	enum { IDD = IDD_SELECT_ANIMATION };
	CListBox	m_AnimationListBox;
	BOOL	m_bRemoveUnused;
	//}}AFX_DATA

	class CBpaintDoc * m_pDoc;
	BPT::CAnimation * m_pAnimation;
	BPT::CAnimationShowcase * m_pShowcase;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectAnimationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectAnimationDlg)
	afx_msg void OnPrevFrame();
	afx_msg void OnNextFrame();
	afx_msg void OnSelchangeAnimationList();
	virtual BOOL OnInitDialog();
	afx_msg void OnPrevLayer();
	afx_msg void OnNextLayer();
	afx_msg void OnDblclkAnimationList();
	afx_msg void OnCopyAnim();
	afx_msg void OnDeleteAnim();
	afx_msg void OnProperties();
	afx_msg void OnMakeElements();
	afx_msg void OnRemoveUnused();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	// RONG: Added
	BOOL RefreshAnimList();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTANIMATIONDLG_H__25651BD6_86AD_47A4_918A_3093FC9F8771__INCLUDED_)
