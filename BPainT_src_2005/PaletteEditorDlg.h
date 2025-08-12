// PaletteEditorDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_PALETTEEDITORDLG_H__0234A1A2_2FF7_40E0_AB8B_3F7831962973__INCLUDED_)
#define AFX_PALETTEEDITORDLG_H__0234A1A2_2FF7_40E0_AB8B_3F7831962973__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BPaintDoc.h"
#include "SimplePaletteCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CPaletteEditorDlg dialog

class CPaletteEditorDlg : public CDialog
{
// Construction
public:
	CPaletteEditorDlg(CBpaintDoc * pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaletteEditorDlg)
	enum { IDD = IDD_PALETTE_EDITOR };
	int		m_R_value;
	int		m_G_value;
	int		m_B_value;
	BOOL	m_bLiveUpdates;
	//}}AFX_DATA

	BPT::CAnnotatedPalette m_OriginalPalette;
	BPT::CAnnotatedPalette m_ModifiedPalette;
	int m_nActiveSlot;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaletteEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// ------------------------------------------------------------------------

public:

	template< class T> void Palette_OnLButtonDown(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		SetActiveSlot( slot );
	}

	template< class T> void Palette_OnLButtonUp(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		SetActiveSlot( slot );

	}

	template< class T> void Palette_OnLButtonDblClk(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		SetActiveSlot( slot );
	
	}

	template< class T> void Palette_OnRButtonDown(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		SetActiveSlot( slot );
	
	}

	template< class T> void Palette_OnRButtonUp(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		SetActiveSlot( slot );
	
	}

	template< class T> void Palette_OnRButtonDblClk(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		SetActiveSlot( slot );
	
	}

	template< class T> void Palette_OnMouseMove(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		if ( bCapture ) {

			SetActiveSlot( slot );

		}

	}

	template< class T> void Palette_OnCancelMode(
		T * pPal,bool bCapture) {
	
	}

// Implementation
protected:

	// ------------------------------------------------------------------------

	enum {

		TOTAL_COLORS	= 256

	};

	// ------------------------------------------------------------------------

	typedef TPaletteReflectionMediator<CPaletteEditorDlg> mediator_type;

	mediator_type m_PalDataMediator;

	CSimplePaletteCtrl m_PaletteCtrl;

	CBpaintDoc * m_pDoc;

	bool m_bInternalUpdate;

	// ------------------------------------------------------------------------

	void SyncDialogToActiveSlot();
	void SetActiveSlot( const int nSlot );
	void HandleSlotUpdate( const int nSlot );
	void HandleLiveUpdate();

	// ------------------------------------------------------------------------

	// Generated message map functions
	//{{AFX_MSG(CPaletteEditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLiveUpdateCheck();
	afx_msg void OnChangeREdit();
	afx_msg void OnChangeGEdit();
	afx_msg void OnChangeBEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PALETTEEDITORDLG_H__0234A1A2_2FF7_40E0_AB8B_3F7831962973__INCLUDED_)
