//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_MAKESTENCILDLG_H__385A8823_B086_41EE_867B_C962F3BF9E50__INCLUDED_)
#define AFX_MAKESTENCILDLG_H__385A8823_B086_41EE_867B_C962F3BF9E50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MakeStencilDlg.h : header file
//

#include "BPaintDoc.h"
#include "SimplePaletteCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMakeStencilDlg dialog

class CMakeStencilDlg : public CDialog
{
// Construction
public:
	CMakeStencilDlg( CBpaintDoc * pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMakeStencilDlg)
	enum { IDD = IDD_MAKE_STENCIL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMakeStencilDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// ------------------------------------------------------------------------

	typedef TPaletteReflectionMediator<CMakeStencilDlg> mediator_type;

private:

	bool m_bLButtonDown;
	DWORD m_dwActiveRemoveBits;
	DWORD m_dwActiveAddBits;

	// ------------------------------------------------------------------------

	mediator_type m_PalDataMediator;

	CSimplePaletteCtrl m_PaletteCtrl;

	int m_LastSlot;
	int m_LastCount;

private: // methods

public:

	// ------------------------------------------------------------------------

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

	void Palette_UpdateInfoMessage( const int slot );

	void Palette_ToggleSlotSelection( const int slot ) {

		DWORD dwFlags = m_PalDataMediator.GetColorStyleBits( slot );

		if ( mediator_type::SELECTED & dwFlags ) {

			m_PalDataMediator.ModifyColorStyleBits( slot, mediator_type::SELECTED, 0 );

		} else {

			m_PalDataMediator.ModifyColorStyleBits( slot, 0, mediator_type::SELECTED );

		}

		m_PaletteCtrl.InvalidateSlot( slot, true );

		Palette_UpdateInfoMessage( slot );

	}

	template< class T> void Palette_OnLButtonDown(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		m_bLButtonDown = true;

		DWORD dwFlags = pPal->GetColorStyleBits( slot );

		if ( T::SELECTED & dwFlags ) {

			pPal->ModifyColorStyleBits( slot, T::SELECTED, 0 );

			m_dwActiveRemoveBits = T::SELECTED;
			m_dwActiveAddBits = 0;

		} else {

			pPal->ModifyColorStyleBits( slot, 0, T::SELECTED );

			m_dwActiveRemoveBits = 0;
			m_dwActiveAddBits = T::SELECTED;

		}

		m_PaletteCtrl.InvalidateSlot( slot, true );

		Palette_UpdateInfoMessage( slot );

	}

	template< class T> void Palette_OnLButtonUp(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		m_bLButtonDown = false;

	}

	template< class T> void Palette_OnLButtonDblClk(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		Palette_OnLButtonDown( pPal, slot, bCapture, nFlags, point );
		Palette_OnLButtonUp( pPal, slot, bCapture, nFlags, point );

	}

	template< class T> void Palette_OnRButtonDown(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		int rowStart = (slot / 16) * 16;

		// Determine setting
		// --------------------------------------------------------------------

		DWORD dwRemove = 0;
		DWORD dwAdd = T::SELECTED;

		for ( int r = 0; r < 16; r++ ) {

			DWORD dwFlags = pPal->GetColorStyleBits( rowStart + r );

			if ( T::SELECTED & dwFlags ) {

				dwRemove = T::SELECTED;
				dwAdd = 0;

				break;

			}

		}

		// --------------------------------------------------------------------

		for ( r = 0; r < 16; r++ ) {

			pPal->ModifyColorStyleBits( (rowStart + r), dwRemove, dwAdd );

			m_PaletteCtrl.InvalidateSlot( (rowStart + r), (15 == r) );

		}

		Palette_UpdateInfoMessage( slot );

	}

	template< class T> void Palette_OnRButtonUp(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {
	}

	template< class T> void Palette_OnRButtonDblClk(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		Palette_OnRButtonDown( pPal, slot, bCapture, nFlags, point );
		Palette_OnRButtonUp( pPal, slot, bCapture, nFlags, point );

	}

	template< class T> void Palette_OnMouseMove(
		T * pPal,int slot, bool bCapture, UINT nFlags, CPoint point) {

		if ( m_bLButtonDown ) {

			pPal->ModifyColorStyleBits( 
				slot, m_dwActiveRemoveBits, m_dwActiveAddBits 
			);

			m_PaletteCtrl.InvalidateSlot( slot, true );

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
	//{{AFX_MSG(CMakeStencilDlg)
	afx_msg void OnClear();
	afx_msg void OnInvert();
	afx_msg void OnUsed();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAKESTENCILDLG_H__385A8823_B086_41EE_867B_C962F3BF9E50__INCLUDED_)
