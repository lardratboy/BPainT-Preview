#if !defined(AFX_GENERICMDICHILDFRAME_H__712BE808_EFEF_4B8F_8E86_FCF8D5095DAB__INCLUDED_)
#define AFX_GENERICMDICHILDFRAME_H__712BE808_EFEF_4B8F_8E86_FCF8D5095DAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GenericMDIChildFrame.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited

/////////////////////////////////////////////////////////////////////////////
// CGenericMDIChildFrame frame

class CGenericMDIChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CGenericMDIChildFrame)
protected:
	CGenericMDIChildFrame();           // protected constructor used by dynamic creation

// data
private:

	CDocTemplate * m_pDocTemplate;
	
	// ------------------------------------------------------------------------

	int GetInternalSetting(
		const char * sectionName, const char * baseEntryName,
		const char * entry, const int value ) {
		CString finalEntryName;
	
		finalEntryName.Format( "%s.%s", baseEntryName, entry );
	
		return GLOBAL_GetSettingInt( finalEntryName, value, sectionName );
	}
	
	void PutInternalSetting(
		const char * sectionName, const char * baseEntryName, 
		const char * entry, const int value ) {
		CString finalEntryName;
	
		finalEntryName.Format( "%s.%s", baseEntryName, entry );
	
		GLOBAL_PutSettingInt( finalEntryName, value, sectionName );
	
	}

	// ------------------------------------------------------------------------

// Attributes
public:

// Operations
public:

	CDocTemplate * GetDocTemplate() const {

		return m_pDocTemplate;

	}

	bool StoreInternalSettings( const char * sectionName, const char * baseEntryName );
	bool RestoreInternalSettings( const char * sectionName, const char * baseEntryName );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenericMDIChildFrame)
	public:
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGenericMDIChildFrame();

	// Generated message map functions
	//{{AFX_MSG(CGenericMDIChildFrame)
	afx_msg void OnViewExpand();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERICMDICHILDFRAME_H__712BE808_EFEF_4B8F_8E86_FCF8D5095DAB__INCLUDED_)
