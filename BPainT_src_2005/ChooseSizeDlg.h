//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_CHOOSESIZEDLG_H__59D42EE2_D9B7_48B5_B5AF_434E2AE649CB__INCLUDED_)
#define AFX_CHOOSESIZEDLG_H__59D42EE2_D9B7_48B5_B5AF_434E2AE649CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseSizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChooseSizeDlg dialog

class CChooseSizeDlg : public CDialog
{
// Construction
public:

	const char * m_pTitle;

	int m_LastChanged;

	CSize m_min;
	CSize m_max;

	CChooseSizeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChooseSizeDlg)
	enum { IDD = IDD_CHOOSE_WIDTH_HEIGHT };
	int		m_height;
	int		m_width;
	BOOL	m_bSameSettings;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void EnsureSameSettings( const int id );

	// Generated message map functions
	//{{AFX_MSG(CChooseSizeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSameSettings();
	afx_msg void OnChangeHeight();
	afx_msg void OnChangeWidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

bool GetSizeDialog(
	CWnd * pWnd, const char * title, SIZE & size, const SIZE * pLimits = 0,
	const char * pSettingsStorage = 0, const bool bInitFromIncomingSize = false
);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSESIZEDLG_H__59D42EE2_D9B7_48B5_B5AF_434E2AE649CB__INCLUDED_)
