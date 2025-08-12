//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_EDITPALETTEDLG_H__385A8823_B086_41EE_867B_C962F3BF9E50__INCLUDED_)
#define AFX_EDITPALETTEDLG_H__385A8823_B086_41EE_867B_C962F3BF9E50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditPaletteDlg.h : header file
//

#include "BPaintDoc.h"
#include "SimplePaletteCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CEditPaletteDlg dialog

class CEditPaletteDlg : public CDialog
{
// Construction
public:
	CEditPaletteDlg( CBpaintDoc * pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditPaletteDlg)
	enum { IDD = IDD_EDIT_PALETTE };
	CScrollBar	m_GScrollBar;
	CScrollBar	m_RScrollBar;
	CScrollBar	m_BScrollBar;
	int		m_rValue;
	int		m_gValue;
	int		m_bValue;
	BOOL	m_bLiveUpdates;
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CString m_WindowTitle;

	BPT::CAnnotatedPalette m_OriginalPalette;
	BPT::CAnnotatedPalette m_InternalPalette1;
	BPT::CAnnotatedPalette m_InternalPalette2;
	BPT::CAnnotatedPalette * m_pEditedPalette;

	int m_HeadSlot;
	int m_TailSlot;

	bool m_bEnableRepaint;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditPaletteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// ------------------------------------------------------------------------

	typedef TPaletteReflectionMediator<CEditPaletteDlg> mediator_type;

private:

	CBpaintDoc * m_pDocument;

	bool m_bInit;

	bool m_bLButtonDown;
	bool m_selectMode;

//	DWORD m_dwActiveRemoveBits;
//	DWORD m_dwActiveAddBits;

	// ------------------------------------------------------------------------

	mediator_type m_PalDataMediator;

	CSimplePaletteCtrl m_PaletteCtrl;

	int m_LastSlot;
	int m_LastCount;

private: // methods

	void BeginUndo() {

		if ( &m_InternalPalette1 == m_pEditedPalette ) {

			m_InternalPalette2.CopyColorsFrom( m_pEditedPalette, true );

			m_pEditedPalette = &m_InternalPalette2; 

		} else {

			m_InternalPalette1.CopyColorsFrom( m_pEditedPalette, true );

			m_pEditedPalette = &m_InternalPalette1; 

		}

	}

	void HandleUndo() {

		if ( &m_InternalPalette1 == m_pEditedPalette ) {

			m_pEditedPalette = &m_InternalPalette2; 

		} else {

			m_pEditedPalette = &m_InternalPalette1; 

		}

		// ------------------------------------------------

		for ( int i = 0; i < 256; i++ ) {

			m_PalDataMediator.SetDisplayColorForPaletteSlot(
				i, m_pEditedPalette->GetCLUTValue( i )
			);

		}

		m_PaletteCtrl.InvalidateWholePalette( true );

		UpdateScrollBars();

	}

public:

	void UpdateColorPreview();
	void UpdateScrollBars();
	void Palette_UpdateInfoMessage( const int slot );
	void HandleLiveUpdate();

	// ------------------------------------------------------------------------

	void SelectSlotRange(int start, int end, const bool bSelect, const bool bUpdate = false) {

		if (end < start) {

			int temp = start; start = end; end = temp;

		}
		
		for ( int i = start; i <= end; i++ ) {

			SelectSlot( i, bSelect, false );

			if ( bUpdate ) {

				m_PaletteCtrl.InvalidateSlot( i, false );

			}
		}

		if ( bUpdate ) {
			
			m_PaletteCtrl.UpdateWindow();

		}

	}

	bool IsSlotSelected( const int slot ) {

		return (0 != (mediator_type::SELECTED & m_PalDataMediator.GetColorStyleBits( slot )));

	}

	void SelectSlot( const int slot, const bool bSelect, const bool bUpdate = false ) {

		if ( bSelect ) {

			m_PalDataMediator.ModifyColorStyleBits( slot, 0, mediator_type::SELECTED );

		} else {

			m_PalDataMediator.ModifyColorStyleBits( slot, mediator_type::SELECTED, 0 );

		}

		if ( bUpdate ) {

			m_PaletteCtrl.InvalidateSlot( slot, true );

		}

	}

	// ------------------------------------------------------------------------

	template< class T> void Palette_OnLButtonDown(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		m_bLButtonDown = true;

		if ( ::GetAsyncKeyState(VK_SHIFT) & 0x8000) {

			SelectSlotRange(0, 255, false, false);
			
			m_TailSlot = slot;

			SelectSlotRange(m_HeadSlot, m_TailSlot, true, false);

			m_PaletteCtrl.InvalidateWholePalette( true );

		} else if ( ::GetAsyncKeyState(VK_CONTROL) & 0x8000) {

			if ( IsSlotSelected( slot ) ) {

				SelectSlot( slot, false, true);

				m_selectMode = false;

			} else {

				SelectSlot( slot, true, true);

				m_selectMode = true;

			}

			m_HeadSlot = min( m_HeadSlot, slot );
			m_TailSlot = max( m_TailSlot, slot );

		} else {
			
			SelectSlotRange(0, 255, false, true);

			pPal->ModifyColorStyleBits( slot, 0, T::SELECTED );

			m_PaletteCtrl.InvalidateSlot( slot, true );

			Palette_UpdateInfoMessage( slot );

			m_HeadSlot = slot;
			m_TailSlot = slot;

		}

		UpdateScrollBars();

	}

	template< class T> void Palette_OnLButtonUp(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		m_bLButtonDown = false;

		if (m_HeadSlot > m_TailSlot) {

			int temp = m_HeadSlot;
			m_HeadSlot = m_TailSlot;
			m_TailSlot = temp;

			UpdateScrollBars();

		}

		BeginUndo();

	}

	template< class T> void Palette_OnLButtonDblClk(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		Palette_OnLButtonDown( pPal, slot, bCapture, nFlags, point );
		Palette_OnLButtonUp( pPal, slot, bCapture, nFlags, point );

	}

	template< class T> void Palette_OnRButtonDown(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {
	}

	template< class T> void Palette_OnRButtonUp(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {
	}

	template< class T> void Palette_OnRButtonDblClk(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {
	}

	template< class T> void Palette_OnMouseMove(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {
		if ( m_bLButtonDown ) {

			if ( ::GetAsyncKeyState(VK_CONTROL) & 0x8000) {

				SelectSlot( slot, m_selectMode, true);
				
				m_HeadSlot = min( m_HeadSlot, slot );
				m_TailSlot = max( m_TailSlot, slot );

			} else {

				if (slot != m_TailSlot) {

					SelectSlotRange(0, 255, false, false);
					
					m_TailSlot = slot;
					SelectSlotRange(m_HeadSlot, m_TailSlot, true, false);

				}

			}

			m_PaletteCtrl.InvalidateWholePalette( true );

		}

		Palette_UpdateInfoMessage( slot );
	}

	template< class T> void Palette_OnCancelMode(
		T * pPal,bool bCapture) {
	
		m_bLButtonDown = false;

	}

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditPaletteDlg)
	afx_msg void OnClear();
	afx_msg void OnInvert();
	afx_msg void OnUsed();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeREdit();
	afx_msg void OnChangeGEdit();
	afx_msg void OnChangeBEdit();
	afx_msg void OnLiveUpdates();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFill();
	afx_msg void OnSpread();
	afx_msg void OnPaste();
	afx_msg void OnCopy();
	afx_msg void OnFromParent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedUndo();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITPALETTEDLG_H__385A8823_B086_41EE_867B_C962F3BF9E50__INCLUDED_)
