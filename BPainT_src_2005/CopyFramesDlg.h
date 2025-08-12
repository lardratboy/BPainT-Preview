#if !defined(AFX_COPYFRAMESDLG_H__98162A78_A56B_4D19_9FFE_1484924D001C__INCLUDED_)
#define AFX_COPYFRAMESDLG_H__98162A78_A56B_4D19_9FFE_1484924D001C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CCopyFramesDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "BPTAnimator.h"

/////////////////////////////////////////////////////////////////////////////
// CCopyFramesDlg dialog

class CCopyFramesDlg : public CDialog
{

public: // traits

	// ------------------------------------------------------------------------

	typedef std::list<int> selected_layers_collection_type;

	// ------------------------------------------------------------------------

	struct SFrameRangeInfo {

		int m_nMin;
		int m_nMax;
		int m_nFrom;
		int m_nTo;

		SFrameRangeInfo() {

			m_nMin = 0;
			m_nMax = 0;
			m_nFrom = 0;
			m_nTo = 0;

		}

	};

	// ------------------------------------------------------------------------

// Construction
public:
	int m_CopyType;

	CCopyFramesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCopyFramesDlg)
	enum { IDD = IDD_COPY_FRAMES };
	CComboBox	m_CopyTypeCombo;
	CListBox	m_SrcLayersListBox;
	CListBox	m_DstLayersListBox;
	CComboBox	m_DstAnimCombo;
	CComboBox	m_SrcAnimCombo;
	int		m_nDstFrom;
	int		m_nSrcFrom;
	int		m_nSrcTo;
	//}}AFX_DATA

	bool m_bFirstTime;

	BPT::CAnimationShowcase * m_pShowcase;
	BPT::CAnimation * m_pSrcAnimation;
	BPT::CAnimation * m_pDstAnimation;

	selected_layers_collection_type m_SrcSelectedLayers;
	selected_layers_collection_type m_DstSelectedLayers;

	SFrameRangeInfo m_SrcRangeInfo;
	SFrameRangeInfo m_DstRangeInfo;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCopyFramesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// internal methods

private:

	bool CCopyFramesDlg::CheckFrameRangeInfo(
		const int fromEdit, const int toEdit, SFrameRangeInfo & rangeInfo
	);

	bool GetListBoxSelections(
		selected_layers_collection_type & collection, CListBox & listBox
	);

	bool FillComboBoxWithAnimationList(
		CComboBox & comboBox, BPT::CAnimation * pSelectedAnim
	);

	bool SetupInformation(
		BPT::CAnimation * pAnimation,
		const int fromEdit, const int fromSpin,
		const int toEdit, const int toSpin,
		CListBox & listBox, selected_layers_collection_type & collection,
		SFrameRangeInfo & rangeInfo, const int staticInfoID, const char * lpstrName
	);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCopyFramesDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeSrcAnimCombo();
	afx_msg void OnSelchangeDstAnimCombo();
	afx_msg void OnSelchangeOpCombo();
	afx_msg void OnOpSettings();
	afx_msg void OnSelchangeCopyType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COPYFRAMESDLG_H__98162A78_A56B_4D19_9FFE_1484924D001C__INCLUDED_)
