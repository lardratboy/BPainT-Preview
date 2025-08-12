// ChildFrm.h : interface of the CChildFrame class
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__0F747474_3E50_4B2B_924A_2BD7EFE010BB__INCLUDED_)
#define AFX_CHILDFRM_H__0F747474_3E50_4B2B_924A_2BD7EFE010BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChildFrame : public CMDIChildWnd
{

private:

	CDocTemplate * m_pDocTemplate;

	CToolBar m_zoomSettingsToolBar;
	bool m_bShyZoomToolbar;
	bool m_bFirstTime;

	DECLARE_DYNCREATE(CChildFrame)

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

	CChildFrame();

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
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// private helper methods
private:

	void OnToolbarX( CToolBar * pToolbar );
	void OnUpdateToolbarX( CToolBar * pToolbar, CCmdUI* pCmdUI );
	void HandleShyToolbar( CToolBar * pToolbar, const int nActivateState );

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVframeShowZoomToolbar();
	afx_msg void OnUpdateVframeShowZoomToolbar(CCmdUI* pCmdUI);
	afx_msg void OnVframeShyZoomToolbar();
	afx_msg void OnUpdateVframeShyZoomToolbar(CCmdUI* pCmdUI);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnViewExpand();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__0F747474_3E50_4B2B_924A_2BD7EFE010BB__INCLUDED_)
