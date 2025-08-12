#if !defined(AFX_COMPOSITEFRAMEOPSDLG_H__98162A78_A56B_4D19_9FFE_1484924D001C__INCLUDED_)
#define AFX_COMPOSITEFRAMEOPSDLG_H__98162A78_A56B_4D19_9FFE_1484924D001C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CompositeFrameOpsDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "BPTAnimator.h"

/////////////////////////////////////////////////////////////////////////////
// CCompositeFrameOpsDlg dialog

class CCompositeFrameOpsDlg : public CDialog
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

	class COperation {

	private:

	public:

		typedef selected_layers_collection_type layer_collection_type;
		typedef SFrameRangeInfo SFrameRangeInfo;

		// --------------------------------------------------------------------

		virtual const char * GetName() const {

			return "Error";

		}

		virtual CString GetMsg() const {
	
			return CString("This operation has no message.");
	
		}

		// --------------------------------------------------------------------

		virtual bool Perform(
			class CBpaintDoc * pDocument, 
			BPT::CAnimation * pDstAnimation, 
			SFrameRangeInfo & dstRangeInfo, 
			layer_collection_type & dstLayersCollection,
			BPT::CAnimation * pSrcAnimation, 
			SFrameRangeInfo & srcRangeInfo, 
			layer_collection_type & srcLayersCollection,
			const int value
			) {

			return true;

		}

		// --------------------------------------------------------------------

		virtual bool SettingsDialog( CWnd * pParent ) {

			return true;

		}

		virtual bool HasSettingsDialog() {

			return false;

		}


	};

	typedef std::list<COperation *> operations_collection_type;

	// ------------------------------------------------------------------------

// Construction
public:

	CCompositeFrameOpsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCompositeFrameOpsDlg)
	enum { IDD = IDD_COMPOSITE_FRAMES };
	CComboBox	m_OperationsComboBox;
	CListBox	m_SrcLayersListBox;
	CListBox	m_DstLayersListBox;
	CComboBox	m_DstAnimCombo;
	CComboBox	m_SrcAnimCombo;
	int		m_nColor;
	int		m_nDstFrom;
	int		m_nDstTo;
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

	operations_collection_type m_Operations;

	COperation * m_pSelectedOperation;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompositeFrameOpsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// internal methods

private:

	bool CCompositeFrameOpsDlg::CheckFrameRangeInfo(
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

	void SelectOperation( COperation * pOperation );

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCompositeFrameOpsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeSrcAnimCombo();
	afx_msg void OnSelchangeDstAnimCombo();
	afx_msg void OnSelchangeOpCombo();
	afx_msg void OnOpSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bIndependentLayersMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPOSITEFRAMEOPSDLG_H__98162A78_A56B_4D19_9FFE_1484924D001C__INCLUDED_)
