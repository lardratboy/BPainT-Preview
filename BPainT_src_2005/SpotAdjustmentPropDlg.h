#if !defined(AFX_SPOTADJUSTMENTPROPDLG_H__CBBA5ED0_A0A5_4497_B7A6_4A72296202BB__INCLUDED_)
#define AFX_SPOTADJUSTMENTPROPDLG_H__CBBA5ED0_A0A5_4497_B7A6_4A72296202BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpotAdjustmentPropDlg.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CSpotAdjustmentPropDlg dialog

class CSpotAdjustmentPropDlg : public CDialog
{
// Construction
public:
	CSpotAdjustmentPropDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpotAdjustmentPropDlg)
	enum { IDD = IDD_SPOT_ADJUSTMENT_PROPERTIES };
	int		m_nFrame;
	int		m_xPos;
	int		m_yPos;
	CString	m_Note;
	//}}AFX_DATA

	int m_nMinFrame;
	int m_nMaxFrame;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpotAdjustmentPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpotAdjustmentPropDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPOTADJUSTMENTPROPDLG_H__CBBA5ED0_A0A5_4497_B7A6_4A72296202BB__INCLUDED_)
