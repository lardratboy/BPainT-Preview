// InsertXDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_INSERTXDLG_H__FF395B0F_23C0_4D75_8AEA_697C5D10EED3__INCLUDED_)
#define AFX_INSERTXDLG_H__FF395B0F_23C0_4D75_8AEA_697C5D10EED3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CInsertXDlg dialog

class CInsertXDlg : public CDialog
{

// traits

public:

	enum {

		INSERT_AFTER		= 0x0001
		,DUPLICATE_CURRENT	= 0x0002

	};

// data

	const char * m_pTitle;

	int m_nMin;
	int m_nMax;

private:

// Construction
public:
	CInsertXDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInsertXDlg)
	enum { IDD = IDD_INSERT_X_DIALOG };
	BOOL	m_bInsertAfter;
	BOOL	m_bDuplicate;
	int		m_count;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsertXDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInsertXDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

bool InsertXDialog(
	CWnd * pWnd, const char * title, 
	int & outCount, DWORD & outFlags,
	const int minCount, const int maxCount,
	const char * pSettingsStorage = 0, 
	const bool bLimitStorageWithIncomingData = true
);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSERTXDLG_H__FF395B0F_23C0_4D75_8AEA_697C5D10EED3__INCLUDED_)
