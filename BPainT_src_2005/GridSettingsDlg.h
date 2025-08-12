#if !defined(AFX_GRIDSETTINGSDLG_H__D79C71FC_0DA6_4B60_A0DB_0504046240DC__INCLUDED_)
#define AFX_GRIDSETTINGSDLG_H__D79C71FC_0DA6_4B60_A0DB_0504046240DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GridSettingsDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CGridSettingsDlg dialog

class CGridSettingsDlg : public CDialog
{
// Construction
public:
	CGridSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGridSettingsDlg)
	enum { IDD = IDD_GRID_INFO };
	int		m_grid_cy;
	int		m_grid_cx;
	int		m_offset_x;
	int		m_offset_y;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGridSettingsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDSETTINGSDLG_H__D79C71FC_0DA6_4B60_A0DB_0504046240DC__INCLUDED_)
