#if !defined(AFX_FRAMEPROPERTIESDLG_H__80A58495_E22E_46C3_BB6E_C28C77B6FBA2__INCLUDED_)
#define AFX_FRAMEPROPERTIESDLG_H__80A58495_E22E_46C3_BB6E_C28C77B6FBA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FramePropertiesDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "GreedyEditCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CFramePropertiesDlg dialog

class CFramePropertiesDlg : public CDialog
{
// Construction
public:
	CFramePropertiesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFramePropertiesDlg)
	enum { IDD = IDD_FRAME_PROPERTIES };
	CGreedyEditCtrl	m_GreedyEditCtrl;
	int		m_nLink1X;
	int		m_nLink1Y;
	int		m_nLink2X;
	int		m_nLink2Y;
	int		m_nLink3X;
	int		m_nLink4X;
	int		m_nLink3Y;
	int		m_nLink4Y;
	//}}AFX_DATA
	CString	m_strNote;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFramePropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFramePropertiesDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEPROPERTIESDLG_H__80A58495_E22E_46C3_BB6E_C28C77B6FBA2__INCLUDED_)
