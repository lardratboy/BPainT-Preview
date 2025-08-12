//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_BRUSHDEFORMDLG_H__63F71099_CC9B_4C85_9215_6B0A055641F3__INCLUDED_)
#define AFX_BRUSHDEFORMDLG_H__63F71099_CC9B_4C85_9215_6B0A055641F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BrushDeformDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBrushDeformDlg dialog

class CBrushDeformDlg : public CDialog
{

// methods & data
private:

	POINT m_CurrentQuad[ 4 ];
	POINT * m_pQuad;

	void RestoreSettingsFromQuad();
	void SyncStatusMessages();
	void BuildMsgFromQuad( CString & msg, const char * prefix, const POINT * pQuad );
	void RefreshQuadFromValues();

// Construction
public:
	CBrushDeformDlg(CWnd* pParent = NULL, POINT * pQuad = 0);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBrushDeformDlg)
	enum { IDD = IDD_BRUSH_DEFORMATION };
	int		m_x1;
	int		m_x2;
	int		m_x3;
	int		m_x4;
	int		m_y3;
	int		m_y1;
	int		m_y2;
	int		m_y4;
	CString	m_originalSizeMsg;
	CString	m_outputSizeMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrushDeformDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBrushDeformDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeX();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

bool DeformationDialog(
	CWnd * pWnd, const char * title, POINT * pQuad,
	const char * pSettingsStorage = 0
);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRUSHDEFORMDLG_H__63F71099_CC9B_4C85_9215_6B0A055641F3__INCLUDED_)
