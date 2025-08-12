#if !defined(AFX_NAMEDSPOTPROPDLG_H__2C6CA90B_781D_421D_A836_111AE35736BA__INCLUDED_)
#define AFX_NAMEDSPOTPROPDLG_H__2C6CA90B_781D_421D_A836_111AE35736BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NamedSpotPropDlg.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CNamedSpotPropDlg dialog

class CNamedSpotPropDlg : public CDialog
{
// Construction
public:
	CNamedSpotPropDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNamedSpotPropDlg)
	enum { IDD = IDD_NEW_NAMED_SPOT };
	CString	m_Name;
	CString	m_Note;
	int		m_xPos;
	int		m_yPos;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNamedSpotPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNamedSpotPropDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAMEDSPOTPROPDLG_H__2C6CA90B_781D_421D_A836_111AE35736BA__INCLUDED_)
