#if !defined(AFX_ANIMATIONFRAMENOTESDLG_H__B0020A7A_FA69_4B6F_9568_E7C29013393C__INCLUDED_)
#define AFX_ANIMATIONFRAMENOTESDLG_H__B0020A7A_FA69_4B6F_9568_E7C29013393C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimationFrameNotesDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "GreedyEditCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CAnimationFrameNotesDlg dialog

class CAnimationFrameNotesDlg : public CDialog
{
// Construction
public:
	CAnimationFrameNotesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnimationFrameNotesDlg)
	enum { IDD = IDD_ALL_ANIMATION_FRAME_NOTES };
	CGreedyEditCtrl	m_GreedyEditCtrl;
	//}}AFX_DATA
	CString	m_strCombinedNotes;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimationFrameNotesDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnimationFrameNotesDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATIONFRAMENOTESDLG_H__B0020A7A_FA69_4B6F_9568_E7C29013393C__INCLUDED_)
