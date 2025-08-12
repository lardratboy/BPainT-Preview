#if !defined(AFX_POPFRAMEWND_H__3C87AD2A_E198_4CC1_AB25_DE14189E9385__INCLUDED_)
#define AFX_POPFRAMEWND_H__3C87AD2A_E198_4CC1_AB25_DE14189E9385__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PopFrameWnd.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited

#include "MainFrm.h"

/////////////////////////////////////////////////////////////////////////////
// CPopFrameWnd frame

#define POPUP_USE_MINI_FRAME

#if defined(POPUP_USE_MINI_FRAME)
class CPopFrameWnd : public CMiniFrameWnd
#else
class CPopFrameWnd : public CFrameWnd
#endif
{

private:

#if defined(POPUP_USE_MINI_FRAME)
	typedef CMiniFrameWnd base_class;
#else
	typedef CFrameWnd base_class;
#endif

	CToolBar m_ToolBar;
	bool m_bShyToolbar;
	CMainFrame * m_pMainFrame;
	int m_ToolbarResID;

	DECLARE_DYNCREATE(CPopFrameWnd)

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

public:

	CPopFrameWnd( const int tbResID = 0 );           // protected constructor used by dynamic creation

	CDocTemplate * GetDocTemplate() const {

		return m_pDocTemplate;

	}

	bool StoreInternalSettings( const char * sectionName, const char * baseEntryName );
	bool RestoreInternalSettings( const char * sectionName, const char * baseEntryName );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPopFrameWnd)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// private helper methods
private:

	void OnToolbarX( CToolBar * pToolbar );
	void OnUpdateToolbarX( CToolBar * pToolbar, CCmdUI* pCmdUI );
	void HandleShyToolbar( CToolBar * pToolbar, const int nActivateState );

// Implementation
protected:
	virtual ~CPopFrameWnd();

	// Generated message map functions
	//{{AFX_MSG(CPopFrameWnd)
	afx_msg void OnDestroy();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnVframeShowZoomToolbar();
	afx_msg void OnUpdateVframeShowZoomToolbar(CCmdUI* pCmdUI);
	afx_msg void OnVframeShyZoomToolbar();
	afx_msg void OnUpdateVframeShyZoomToolbar(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POPFRAMEWND_H__3C87AD2A_E198_4CC1_AB25_DE14189E9385__INCLUDED_)
