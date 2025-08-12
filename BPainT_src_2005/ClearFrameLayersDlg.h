//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_CLEARFRAMELAYERSDLG_H__41B87CC9_72FB_445D_9723_8F29D733DD56__INCLUDED_)
#define AFX_CLEARFRAMELAYERSDLG_H__41B87CC9_72FB_445D_9723_8F29D733DD56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClearFrameLayersDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include <list>
#include "BPTAnimator.h"

/////////////////////////////////////////////////////////////////////////////
// CClearFrameLayersDlg dialog

class CClearFrameLayersDlg : public CDialog
{
// Construction
public:
	CClearFrameLayersDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CClearFrameLayersDlg)
	enum { IDD = IDD_CLEAR_LAYERS };
	CListBox	m_SelectedLayersListBox;
	int		m_nFrom;
	int		m_nTo;
	BOOL	m_bImage;
	BOOL	m_bFrameNotes;
	BOOL	m_bLayerNotes;
	BOOL	m_bLink1;
	BOOL	m_bLink2;
	BOOL	m_bLink3;
	BOOL	m_bLink4;
	//}}AFX_DATA

	int		m_nMin;
	int		m_nMax;

	BPT::CAnimation * m_pAnimation;

	typedef std::list<int> selected_layers_collection_type;

	selected_layers_collection_type m_SelectedLayers;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClearFrameLayersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClearFrameLayersDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLEARFRAMELAYERSDLG_H__41B87CC9_72FB_445D_9723_8F29D733DD56__INCLUDED_)
