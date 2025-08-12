#pragma once
#include "afxwin.h"

// CLayersStripView view
//
//	Copyright (c) 2002, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//

// ============================================================================
#pragma once



// CLayersStripView form view

class CLayersStripView : public CFormView
{
	DECLARE_DYNCREATE(CLayersStripView)

protected:
	CLayersStripView();           // protected constructor used by dynamic creation
	virtual ~CLayersStripView();

public:
	enum { IDD = IDD_LAYERSSTRIPVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnBnClickedPrevFrame();
	afx_msg void OnBnClickedNextFrame();
	afx_msg void OnBnClickedInsertLayer();
	afx_msg void OnBnClickedDeleteLayer();
	afx_msg void OnBnClickedMoveLayerUp();
	afx_msg void OnBnClickedMoveLayerDown();
	afx_msg void OnBnClickedLayerProperties();
	afx_msg void OnCbnSelchangeAnimationCombo();
	CListBox m_LayersListBox;
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLbnSelchangeLayersListbox();
	virtual void OnInitialUpdate();
};


