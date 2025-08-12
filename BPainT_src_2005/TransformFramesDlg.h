#if !defined(AFX_TRANSFORMFRAMESDLG_H__35292AEE_BB3C_44A1_85DD_5B1438800F2F__INCLUDED_)
#define AFX_TRANSFORMFRAMESDLG_H__35292AEE_BB3C_44A1_85DD_5B1438800F2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TransformFramesDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CTransformFramesDlg dialog

class CTransformFramesDlg : public CDialog
{
// Construction
public:
	CTransformFramesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTransformFramesDlg)
	enum { IDD = IDD_TRANSFORM_FRAMES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransformFramesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransformFramesDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFORMFRAMESDLG_H__35292AEE_BB3C_44A1_85DD_5B1438800F2F__INCLUDED_)
