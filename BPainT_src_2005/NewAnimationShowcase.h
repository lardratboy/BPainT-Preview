// NewAnimationShowcase.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --

#if !defined(AFX_NEWANIMATIONSHOWCASE_H__24A02229_1093_4577_9673_5415DF79A44A__INCLUDED_)
#define AFX_NEWANIMATIONSHOWCASE_H__24A02229_1093_4577_9673_5415DF79A44A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxadv.h>

/////////////////////////////////////////////////////////////////////////////
// CNewAnimationShowcaseDlg dialog

class CNewAnimationShowcaseDlg : public CDialog
{

// data
private:

	CString m_SettingsSection;

// Construction
public:
	CNewAnimationShowcaseDlg(CWnd* pParent = NULL);   // standard constructor

	~CNewAnimationShowcaseDlg();

	bool LoadSettings( CString & section );
	bool SaveSettings();

	CSize m_CanvasSize;
	CSize m_PixelAspect;

	CRecentFileList * m_pRecentFileList;

	CMenu * m_pRecentFilesPopupMenu;

	int m_nMRUElement;

// Dialog Data
	//{{AFX_DATA(CNewAnimationShowcaseDlg)
	enum { IDD = IDD_NEW_SHOWCASE };
	CButton	m_RecentFilesButton;
	CString	m_AnimationName;
	int		m_nFrameCount;
	int		m_nLayerCount;
	CString	m_ShowcaseName;
	BOOL	m_bAnimCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewAnimationShowcaseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void EnableAnimationInfo( const BOOL bEnable );
	void SyncCanvasSizeText();
	void SyncPixelScaleText();

	// Generated message map functions
	//{{AFX_MSG(CNewAnimationShowcaseDlg)
	afx_msg void OnCreateAnimCheck();
	virtual BOOL OnInitDialog();
	afx_msg void OnCanvasSize();
	afx_msg void OnRecentFiles();
	//}}AFX_MSG
	afx_msg void OnMRUFile( UINT nID );
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSetPixelAspectRatio();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWANIMATIONSHOWCASE_H__24A02229_1093_4577_9673_5415DF79A44A__INCLUDED_)
