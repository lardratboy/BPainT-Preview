#if !defined(AFX_ANIMATIONSPOTVIEW_H__ED371EAC_C7F3_42D1_B5BE_36AD91FB90ED__INCLUDED_)
#define AFX_ANIMATIONSPOTVIEW_H__ED371EAC_C7F3_42D1_B5BE_36AD91FB90ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// AnimationSpotView.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////

#include "bpaintDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CAnimationSpotView view

class CAnimationSpotView : public CTreeView
{

private:

	class CNodeInfo {

	private:

		CNodeInfo(); // Hidden

	public:

		enum NODE_TYPE {

			ANIMATION			= 1
			,NAMED_SPOT			= 2
			,SPOT_ADJUSTMENT	= 3

		};

	public: // Data

		int m_nFrame;
		HTREEITEM m_hTreeItem;
		NODE_TYPE m_NodeType;
		BPT::CAnimation * m_pAnimation;
		BPT::CNamedSpot * m_pNamedSpot;
		BPT::CNamedSpotAdjustment * m_pNamedSpotAdjustment;

	public: // interface

		CNodeInfo(
			HTREEITEM hTreeItem
			,const NODE_TYPE nodeType
			,BPT::CAnimation * pAnimation
			,BPT::CNamedSpot * pNamedSpot
			,BPT::CNamedSpotAdjustment * pNamedSpotAdjustment
			,const int nFrame
			) {

			m_hTreeItem = hTreeItem;
			m_NodeType = nodeType;
			m_pAnimation = pAnimation;
			m_pNamedSpot = pNamedSpot;
			m_pNamedSpotAdjustment = pNamedSpotAdjustment;
			m_nFrame = nFrame;

		}

	};

	typedef std::list< CNodeInfo > node_collection_type;

	node_collection_type m_NodeInfoCollection;

	CNodeInfo * m_pContexMenuNode;

protected:

	CAnimationSpotView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CAnimationSpotView)

// Attributes
public:

// Operations
public:

	CBpaintDoc* GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimationSpotView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAnimationSpotView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void FillTreeWithAnimationInfoPrim(
		BPT::CAnimation * pAnimation,
		BPT::CNamedSpot * pSelectSpot = 0,
		BPT::CNamedSpotAdjustment * pSelectAdjustment = 0
	);

	void FillTreeWithAnimationInfo(
		BPT::CAnimation * pAnimation,
		BPT::CNamedSpot * pSelectSpot = 0,
		BPT::CNamedSpotAdjustment * pSelectAdjustment = 0,
		const bool bRefreshMode = false
	);

	CNodeInfo * GetNodeInfoPtrForItem( HTREEITEM hTreeItem );

	CNodeInfo * 
	GetNodeForInfo(
		BPT::CAnimation * pAnimation, 
		BPT::CNamedSpot * pSelectSpot,
		BPT::CNamedSpotAdjustment * pSelectAdjustment
	);

	CString BuildStringForAnimation( BPT::CAnimation * pAnimation );
	CString BuildStringForSpot( BPT::CNamedSpot * pSpot );
	CString BuildStringForAdjustment( BPT::CNamedSpotAdjustment * pAdjustment, const int nFrame  );

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnimationSpotView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNspotAdjustmentProperties();
	afx_msg void OnNspotCreateNew();
	afx_msg void OnNspotDeleteSpot();
	afx_msg void OnNspotNewSpotAdjustment();
	afx_msg void OnNspotSpotProperties();
	afx_msg void OnNspotDeleteAdjustment();
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditSpotProperties();
	afx_msg void OnUpdateEditSpotProperties(CCmdUI* pCmdUI);
	afx_msg void OnDeleteSpotOrAdjustment();
	afx_msg void OnUpdateDeleteSpotOrAdjustment(CCmdUI* pCmdUI);
	afx_msg void OnNewNamedSpot();
	afx_msg void OnUpdateNewNamedSpot(CCmdUI* pCmdUI);
	afx_msg void OnCreateSpotAdjustment();
	afx_msg void OnUpdateCreateSpotAdjustment(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AnimGridView.cpp
inline CBpaintDoc* CAnimationSpotView::GetDocument()
   { return (CBpaintDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATIONSPOTVIEW_H__ED371EAC_C7F3_42D1_B5BE_36AD91FB90ED__INCLUDED_)
