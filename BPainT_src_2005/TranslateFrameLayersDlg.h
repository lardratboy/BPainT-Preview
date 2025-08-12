#if !defined(AFX_TRANSLATEFRAMELAYERSDLG_H__525660DC_26F2_488B_9313_8EB7742E1255__INCLUDED_)
#define AFX_TRANSLATEFRAMELAYERSDLG_H__525660DC_26F2_488B_9313_8EB7742E1255__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TranslateFrameLayersDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////

#include <list>
#include "BPTAnimator.h"

/////////////////////////////////////////////////////////////////////////////
// CTranslateFrameLayersDlg dialog

class CTranslateFrameLayersDlg : public CDialog
{
// Construction
public:
	CTranslateFrameLayersDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTranslateFrameLayersDlg)
	enum { IDD = IDD_TRANSLATE_FRAME_LAYERS };
	CListBox	m_SrcLayersListBox;
	int		m_nFrom;
	int		m_nTo;
	int		m_nXDelta;
	int		m_nYDelta;
	int		m_nDDX;
	int		m_nDDY;
	//}}AFX_DATA

	int		m_nMin;
	int		m_nMax;

	BPT::CAnimation * m_pAnimation;

	typedef std::list<int> selected_layers_collection_type;

	selected_layers_collection_type m_SrcSelectedLayers;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTranslateFrameLayersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTranslateFrameLayersDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSLATEFRAMELAYERSDLG_H__525660DC_26F2_488B_9313_8EB7742E1255__INCLUDED_)
