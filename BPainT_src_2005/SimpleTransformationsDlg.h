#if !defined(AFX_SIMPLETRANSFORMATIONSDLG_H__0F129DFB_BA79_44D6_AD2A_3C2016D6DACC__INCLUDED_)
#define AFX_SIMPLETRANSFORMATIONSDLG_H__0F129DFB_BA79_44D6_AD2A_3C2016D6DACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimpleTransformationsDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CSimpleTransformationsDlg dialog

class CSimpleTransformationsDlg : public CDialog
{
// Construction
public:
	CSimpleTransformationsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSimpleTransformationsDlg)
	enum { IDD = IDD_SIMPLE_TRANSFORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleTransformationsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSimpleTransformationsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLETRANSFORMATIONSDLG_H__0F129DFB_BA79_44D6_AD2A_3C2016D6DACC__INCLUDED_)
