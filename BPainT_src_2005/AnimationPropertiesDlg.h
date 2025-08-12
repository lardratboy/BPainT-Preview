#if !defined(AFX_ANIMATIONPROPERTIESDLG_H__3A70B6AA_700B_45EF_8A02_1D82B5CB8BA5__INCLUDED_)
#define AFX_ANIMATIONPROPERTIESDLG_H__3A70B6AA_700B_45EF_8A02_1D82B5CB8BA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimationPropertiesDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "GreedyEditCtrl.h"
#include "afxwin.h"
#include "BPTAnimator.h"

/////////////////////////////////////////////////////////////////////////////
// CAnimationPropertiesDlg dialog

class CAnimationPropertiesDlg : public CDialog
{
// Construction
public:
	CAnimationPropertiesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnimationPropertiesDlg)
	enum { IDD = IDD_ANIM_PROPERTIES };
	CGreedyEditCtrl	m_GreedyEditCtrl;
	int		m_nFrameRate;
	CString	m_strName;
	int		m_nLinkX;
	int		m_nLinkY;
	int		m_nOutlineA;
	int		m_nOutlineB;
	//}}AFX_DATA
	CString	m_strNote;
	class BPT::CAnnotatedPalette * m_pRequestedPalette;
	class BPT::CAnimationShowcase * m_pShowcase;
	class CBpaintDoc * m_pDoc;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimationPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:

	typedef BPT::CAnimationShowcase::palette_collection_type::iterator palette_iterator;
	bool AddPaletteToList( BPT::CAnnotatedPalette * pPalette );
	bool FillInPaletteComboBox();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnimationPropertiesDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_PaletteCombo;
	afx_msg void OnCbnSelchangePaletteCombo();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATIONPROPERTIESDLG_H__3A70B6AA_700B_45EF_8A02_1D82B5CB8BA5__INCLUDED_)
