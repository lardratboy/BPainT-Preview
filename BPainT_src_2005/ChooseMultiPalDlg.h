#if !defined(AFX_CHOOSEMULTIPALDLG_H__8674BD5B_ED0F_464B_962C_235B71671213__INCLUDED_)
#define AFX_CHOOSEMULTIPALDLG_H__8674BD5B_ED0F_464B_962C_235B71671213__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ChooseMultiPalDlg.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CChooseMultiPalDlg dialog

class CChooseMultiPalDlg : public CDialog
{
// Construction
public:
	CChooseMultiPalDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChooseMultiPalDlg)
	enum { IDD = IDD_MULTIPAL_QUESTION };
	int		m_nSelection;
	//}}AFX_DATA

	bool m_bUserCanCancel;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseMultiPalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChooseMultiPalDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSEMULTIPALDLG_H__8674BD5B_ED0F_464B_962C_235B71671213__INCLUDED_)
