#if !defined(AFX_TEXTTOBRUSHDLG_H__3E1D2F02_E58C_405F_8907_D0D9FB82574D__INCLUDED_)
#define AFX_TEXTTOBRUSHDLG_H__3E1D2F02_E58C_405F_8907_D0D9FB82574D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextToBrushDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "GreedyEditCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTextToBrushDlg dialog

class CTextToBrushDlg : public CDialog
{
// Construction
public:

	CTextToBrushDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTextToBrushDlg)
	enum { IDD = IDD_TEXT_TO_BRUSH };
	CGreedyEditCtrl	m_GreedyEditCtrl;
	//}}AFX_DATA
	CString	m_strText;

	bool m_bUserSelectedAFont;
	LOGFONT m_SelectedLogicalFont;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextToBrushDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextToBrushDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFontButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTTOBRUSHDLG_H__3E1D2F02_E58C_405F_8907_D0D9FB82574D__INCLUDED_)
