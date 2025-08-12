#ifndef __WINDOWMANAGER_H__
#define __WINDOWMANAGER_H__

/////////////////////////////////////////////////////////////////////////////
// CWindowDlg dialog

class CWindowDlg : public CDialog
{
// Construction
public:
	CWindowDlg(CMDIFrameWnd *pMDIFrame,CWnd * pParentWnd);   
	CWindowDlg(CMDIFrameWnd *pMDIFrame);   

// Dialog Data
	//{{AFX_DATA(CWindowDlg)
	enum { IDD = IDD_WINDOW_MANAGE };
	CListBox	m_wndList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void			MDIMessage(UINT uMsg,WPARAM flag);
	void			FillWindowList(void);
	void			SelActive(void);
	void			UpdateButtons(void);

	CMDIFrameWnd 	       *m_pMDIFrame;

	// Generated message map functions
	//{{AFX_MSG(CWindowDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSelChange();
	afx_msg void OnSave();
	afx_msg void OnActivate();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnUpdateUI(CCmdUI * pCmdUI);
	afx_msg void OnTileHorz();
	afx_msg void OnMinimize();
	afx_msg void OnTileVert();
	afx_msg void OnCascade();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMDIClient window

class CMDIClient : public CWnd
{
// Construction
public:
	CMDIClient();

// Attributes
public:

protected:
	CString m_strWindows;
	HMENU	m_hMenuWindow;

// Operations
public:
	void ManageWindows(CMDIFrameWnd *pMDIFrame);
	void ManageWindows(CMDIFrameWnd *pMDIFrame, CWnd * pParentWnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDIClient)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CMDIClient();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMDIClient)
	afx_msg LRESULT OnRefreshMenu(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetMenu(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif