#if !defined(AFX_REMEMBERSIZETOOLBAR_H__F4A987CE_1694_43A9_AEFD_192C360ADEE2__INCLUDED_)
#define AFX_REMEMBERSIZETOOLBAR_H__F4A987CE_1694_43A9_AEFD_192C360ADEE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// RememberSizeToolbar.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CRememberSizeToolbar window

class CRememberSizeToolbar : public CToolBar
{

// data

private:

	bool m_bRememberSizeWhenDocked;

// Construction
public:
	CRememberSizeToolbar();

// Attributes
public:

// Operations
public:

	bool DoesToolbarRememberSize() const {

		return m_bRememberSizeWhenDocked;

	}

	void SetToolbarRemember( const bool bRemember = true ) {

		m_bRememberSizeWhenDocked = bRemember;

	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRememberSizeToolbar)
	//}}AFX_VIRTUAL

	virtual CSize CalcDynamicLayout (int nLength, DWORD nMode);

 // Implementation
public:
	virtual ~CRememberSizeToolbar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRememberSizeToolbar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMEMBERSIZETOOLBAR_H__F4A987CE_1694_43A9_AEFD_192C360ADEE2__INCLUDED_)
