#if !defined(AFX_LOADINGSOMETHINGDLG_H__E6BB8D44_19EA_4247_9809_8AE3556708F3__INCLUDED_)
#define AFX_LOADINGSOMETHINGDLG_H__E6BB8D44_19EA_4247_9809_8AE3556708F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// LoadingSomethingDlg.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CLoadingSomethingDlg dialog

class CLoadingSomethingDlg : public CDialog
{
// Construction
public:
	CLoadingSomethingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoadingSomethingDlg)
	enum { IDD = IDD_LOADING_SOMETHING };
	CProgressCtrl	m_ProgressCtrl;
	//}}AFX_DATA

	int m_nRangeMin;
	int m_nRangeMax;

	void UpdateStatus( 
		const int nCurrent, 
		const int nRangeMin, 
		const int nRangeMax
	);

	void ChangeMessage( const char * pText );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadingSomethingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoadingSomethingDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADINGSOMETHINGDLG_H__E6BB8D44_19EA_4247_9809_8AE3556708F3__INCLUDED_)
