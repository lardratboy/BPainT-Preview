#if !defined(AFX_LAYERPROPERTIESDLG_H__B6A9EC74_477C_406C_A8B2_5563E39459EB__INCLUDED_)
#define AFX_LAYERPROPERTIESDLG_H__B6A9EC74_477C_406C_A8B2_5563E39459EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// LayerPropertiesDlg.h : header file
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
// CLayerPropertiesDlg dialog

class CLayerPropertiesDlg : public CDialog
{
// Construction
public:
	CLayerPropertiesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLayerPropertiesDlg)
	enum { IDD = IDD_LAYER_PROPERTIES };
	CGreedyEditCtrl	m_GreedyEditCtrl;
	CString	m_strNote;
	BOOL	m_bVisible;
	int		m_nOutlineOption;
	//}}AFX_DATA
	CString	m_strName;

	class BPT::CAnnotatedPalette * m_pRequestedPalette;
	class BPT::CAnimationShowcase * m_pShowcase;
	class CBpaintDoc * m_pDoc;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerPropertiesDlg)
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
	//{{AFX_MSG(CLayerPropertiesDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_PaletteCombo;
	afx_msg void OnCbnSelchangePaletteCombo();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERPROPERTIESDLG_H__B6A9EC74_477C_406C_A8B2_5563E39459EB__INCLUDED_)
