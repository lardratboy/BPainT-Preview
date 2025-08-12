// WindowClipFrmWnd.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited

#if !defined(AFX_WINDOWCLIPFRMWND_H__0F0A4466_1DB2_43EC_B50E_9BCCBE5A4D00__INCLUDED_)
#define AFX_WINDOWCLIPFRMWND_H__0F0A4466_1DB2_43EC_B50E_9BCCBE5A4D00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWindowClipFrmWnd frame

class CWindowClipFrmWnd : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(CWindowClipFrmWnd)
protected:
	CWindowClipFrmWnd();           // protected constructor used by dynamic creation

// data
private:

	typedef CMiniFrameWnd base_class;

	// ------------------------------------------------------------------------

	CMainFrame * m_pMainFrame;

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
	//{{AFX_VIRTUAL(CWindowClipFrmWnd)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWindowClipFrmWnd();

	// Generated message map functions
	//{{AFX_MSG(CWindowClipFrmWnd)
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWCLIPFRMWND_H__0F0A4466_1DB2_43EC_B50E_9BCCBE5A4D00__INCLUDED_)
