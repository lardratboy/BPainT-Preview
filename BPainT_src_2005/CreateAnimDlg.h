//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_CREATEANIMDLG_H__BB7016CE_C397_4406_8347_DCC71F1A06BF__INCLUDED_)
#define AFX_CREATEANIMDLG_H__BB7016CE_C397_4406_8347_DCC71F1A06BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreateAnimDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreateAnimDlg dialog

class CCreateAnimDlg : public CDialog
{

private:

	CString m_SettingsSection;

// Construction
public:

	CCreateAnimDlg(CWnd* pParent = NULL);   // standard constructor

	bool LoadSettings( CString & section );
	bool SaveSettings();

// Dialog Data
	//{{AFX_DATA(CCreateAnimDlg)
	enum { IDD = IDD_CREATE_ANIM };
	CString	m_AnimationName;
	int		m_nFrameCount;
	int		m_nLayerCount;
	int		m_nFrameRate;
	int		m_nLinkX;
	int		m_nLinkY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateAnimDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateAnimDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATEANIMDLG_H__BB7016CE_C397_4406_8347_DCC71F1A06BF__INCLUDED_)
