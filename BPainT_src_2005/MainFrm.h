// MainFrm.h : interface of the CMainFrame class
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__B8CD4ECE_49F6_4CEC_BB11_35B2767BD69D__INCLUDED_)
#define AFX_MAINFRM_H__B8CD4ECE_49F6_4CEC_BB11_35B2767BD69D__INCLUDED_

#include <list>

#define USE_REMEMBER_SIZE_TOOLBARS
#define FANCY_WINDOWS_MANAGE_DIALOG

#if defined(FANCY_WINDOWS_MANAGE_DIALOG)
#include "external-source\WindowManager.h"
#endif

#if defined(USE_REMEMBER_SIZE_TOOLBARS)
#include "RememberSizeToolbar.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CMDIFrameWnd
{

private:

	// ------------------------------------------------------------------------

#if defined(FANCY_WINDOWS_MANAGE_DIALOG)

	CMDIClient m_MDIClient;

#endif

	// ------------------------------------------------------------------------

#if defined(USE_REMEMBER_SIZE_TOOLBARS)

	typedef CRememberSizeToolbar toolbar_type;

#else

	typedef CToolBar toolbar_type;

#endif

	bool m_bRememberToolbarSizes;

	// ------------------------------------------------------------------------

	enum {

		PENDING_TIMER		= 1234
		,ANIMATION_TIMER	= 3456

	};

	UINT m_AnimationTimerID;
	UINT m_PendingTimerID;

	int m_nPlayAnimFrameDelay;

	bool m_bFirstTime;

	typedef std::list<CFrameWnd *> popup_frames_collection_type;
	popup_frames_collection_type m_PopupFrames;

	DECLARE_DYNAMIC(CMainFrame)

public:
	CMainFrame();

// Attributes
public:

	char m_InternalName[ _MAX_PATH ];

	void SetInternalName( const char * name ) {

		strcpy( m_InternalName, name );

	}

	void AddPopupFrame( CFrameWnd * pFrame );
	void RemovePopupFrame( CFrameWnd * pFrame );
	void EnablePopupFrames( BOOL bEnable );

	void DockControlBarLeftOf(toolbar_type* Bar, toolbar_type* LeftOf);

	void StopPlayingAnimation();
	void StartPlayingAnimation();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// private helper methods
private:

	void OnToolbarX( toolbar_type * pToolbar );
	void OnUpdateToolbarX( toolbar_type * pToolbar, CCmdUI* pCmdUI );

public:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	toolbar_type    m_drawingToolBar;
	toolbar_type	m_quickBrushToolBar;
	toolbar_type    m_wndToolBar;
	toolbar_type	m_viewSettingsToolBar;
	toolbar_type	m_zoomSettingsToolBar;
	toolbar_type	m_brushTransfromToolBar;
	toolbar_type	m_playbackToolBar;
	toolbar_type	m_layoutToolBar;
	toolbar_type	m_compositeToolBar;
	toolbar_type	m_specialToolBar;
	toolbar_type	m_drawSpecialToolBar;
	toolbar_type	m_ropToolBar;

	// ------------------------------------------------------------------------

	void SyncToolbarRememberStates() {

#if defined(USE_REMEMBER_SIZE_TOOLBARS)

		m_drawingToolBar.SetToolbarRemember( m_bRememberToolbarSizes );
		m_quickBrushToolBar.SetToolbarRemember( m_bRememberToolbarSizes );
		m_wndToolBar.SetToolbarRemember( m_bRememberToolbarSizes );
		m_viewSettingsToolBar.SetToolbarRemember( m_bRememberToolbarSizes );
		m_zoomSettingsToolBar.SetToolbarRemember( m_bRememberToolbarSizes );
		m_brushTransfromToolBar.SetToolbarRemember( m_bRememberToolbarSizes );
		m_playbackToolBar.SetToolbarRemember( m_bRememberToolbarSizes );
		m_layoutToolBar.SetToolbarRemember( m_bRememberToolbarSizes );
		m_compositeToolBar.SetToolbarRemember( m_bRememberToolbarSizes );
		m_specialToolBar.SetToolbarRemember( m_bRememberToolbarSizes );
		m_drawSpecialToolBar.SetToolbarRemember( m_bRememberToolbarSizes );
		m_ropToolBar.SetToolbarRemember( m_bRememberToolbarSizes );

#endif

	}

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewDrawingTools();
	afx_msg void OnUpdateViewDrawingTools(CCmdUI* pCmdUI);
	afx_msg void OnViewQuickBrushes();
	afx_msg void OnUpdateViewQuickBrushes(CCmdUI* pCmdUI);
	afx_msg void OnViewViewSettings();
	afx_msg void OnUpdateViewViewSettings(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomSettings();
	afx_msg void OnUpdateViewZoomSettings(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnNewPopupView();
	afx_msg void OnUpdateNewPopupView(CCmdUI* pCmdUI);
	afx_msg void OnViewBrushXform();
	afx_msg void OnUpdateViewBrushXform(CCmdUI* pCmdUI);
	afx_msg void OnViewPlaybackToolbar();
	afx_msg void OnUpdateViewPlaybackToolbar(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnViewLayoutToolbar();
	afx_msg void OnUpdateViewLayoutToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewCompositeToolbar();
	afx_msg void OnUpdateViewCompositeToolbar(CCmdUI* pCmdUI);
	afx_msg void OnRequestPendingMode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowSpecialToolbar();
	afx_msg void OnUpdateShowSpecialToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewDrawSpecialToolbar();
	afx_msg void OnUpdateViewDrawSpecialToolbar(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnAnimPlay();
	afx_msg void OnAnimStop();
	afx_msg void OnUpdateAnimStop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnimPlay(CCmdUI* pCmdUI);
	afx_msg void OnViewRopToolbar();
	afx_msg void OnUpdateViewRopToolbar(CCmdUI* pCmdUI);
	afx_msg void OnRememberToolbarSizes();
	afx_msg void OnUpdateRememberToolbarSizes(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnEnable( BOOL bEnable );
#if defined(FANCY_WINDOWS_MANAGE_DIALOG)
	afx_msg void OnWindowManage();
#endif
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__B8CD4ECE_49F6_4CEC_BB11_35B2767BD69D__INCLUDED_)
