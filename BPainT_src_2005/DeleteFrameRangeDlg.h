// DeleteFrameRangeDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_DELETEFRAMERANGEDLG_H__01A39268_08E3_4056_88DB_A6CE9CC2D181__INCLUDED_)
#define AFX_DELETEFRAMERANGEDLG_H__01A39268_08E3_4056_88DB_A6CE9CC2D181__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDeleteFrameRangeDlg dialog

class CDeleteFrameRangeDlg : public CDialog
{
// Construction
public:
	CDeleteFrameRangeDlg(CWnd* pParent = NULL);   // standard constructor

	int m_nFromMin;
	int m_nFromMax;
	int m_nToMin;
	int m_nToMax;

// Dialog Data
	//{{AFX_DATA(CDeleteFrameRangeDlg)
	enum { IDD = IDD_DELETE_FRAMES };
	int		m_nFrom;
	int		m_nTo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteFrameRangeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteFrameRangeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeFromEdit();
	afx_msg void OnChangeToEdit();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETEFRAMERANGEDLG_H__01A39268_08E3_4056_88DB_A6CE9CC2D181__INCLUDED_)
