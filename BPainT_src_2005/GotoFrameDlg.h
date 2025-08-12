#if !defined(AFX_GOTOFRAMEDLG_H__1FC2E4A4_7B98_40A4_B18E_8DB29E824410__INCLUDED_)
#define AFX_GOTOFRAMEDLG_H__1FC2E4A4_7B98_40A4_B18E_8DB29E824410__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GotoFrameDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CGotoFrameDlg dialog

class CGotoFrameDlg : public CDialog
{
// Construction
public:
	CGotoFrameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGotoFrameDlg)
	enum { IDD = IDD_GOTO_FRAME };
	int		m_nFrame;
	CString	m_strFrameRange;
	//}}AFX_DATA

	int m_nFrameMin;
	int m_nFrameMax;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGotoFrameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGotoFrameDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOTOFRAMEDLG_H__1FC2E4A4_7B98_40A4_B18E_8DB29E824410__INCLUDED_)
