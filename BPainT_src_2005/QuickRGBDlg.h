//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_QUICKRGBDLG_H__A66D0895_D1D8_48D4_93C5_4F2F5127CF1C__INCLUDED_)
#define AFX_QUICKRGBDLG_H__A66D0895_D1D8_48D4_93C5_4F2F5127CF1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QuickRGBDlg.h : header file
//

#include "bpaintDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CQuickRGBDlg dialog

class CQuickRGBDlg : public CDialog
{

// private data
private:

	CBpaintDoc * m_pDocument;

	CWnd * m_pCenterWnd;

	void UpdateColorPreview();

	bool m_bInit;

// Construction
public:
	CQuickRGBDlg(
		CWnd* pParent = NULL, 
		CWnd * pCenterWnd = 0, 
		CBpaintDoc *  pDocument = 0
	);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQuickRGBDlg)
	enum { IDD = IDD_QUICK_RGB_EDIT };
	CScrollBar	m_GScrollBar;
	CScrollBar	m_RScrollBar;
	CScrollBar	m_BScrollBar;
	int		m_rValue;
	int		m_gValue;
	int		m_bValue;
	BOOL	m_bLiveUpdates;
	//}}AFX_DATA

	int m_nSlot;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuickRGBDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuickRGBDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeREdit();
	afx_msg void OnChangeGEdit();
	afx_msg void OnChangeBEdit();
	afx_msg void OnLiveUpdates();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUICKRGBDLG_H__A66D0895_D1D8_48D4_93C5_4F2F5127CF1C__INCLUDED_)
