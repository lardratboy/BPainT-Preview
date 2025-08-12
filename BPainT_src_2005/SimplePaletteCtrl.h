// SimplePaletteCtrl.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#if !defined(AFX_SIMPLEPALETTECTRL_H__663F0481_3988_45DE_BE9F_C900BA35FBBC__INCLUDED_)
#define AFX_SIMPLEPALETTECTRL_H__663F0481_3988_45DE_BE9F_C900BA35FBBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------------

#include "BpaintDoc.h"
#include "BPTDib.h"

// ----------------------------------------------------------------------------

//
//	CPaletteDataMediator
//

class CPaletteDataMediator {

public: // Traits / globals

	typedef SDisplayPixelType pixel_type;
	typedef BPT::TDIBSection<pixel_type> dib_type;

	enum {

		TOTAL_SLOTS		= 256

	};

	enum SLOT_FLAGS {

		SELECTED		= 0x0001
		,MARKED			= 0x0002

	};

protected:

	CBpaintDoc * m_pDoc;
	pixel_type m_SlotColorData[ TOTAL_SLOTS ];
	DWORD m_dwStyleFlags[ TOTAL_SLOTS ];

	CPaletteDataMediator(); // Hidden

public: // interface

	CPaletteDataMediator( CBpaintDoc * pDoc, const DWORD dwDefaultFlags = 0 ) : m_pDoc( pDoc ) {

		for ( int i = 0; i < TOTAL_SLOTS; i++ ) {

			if ( m_pDoc ) {

				m_SlotColorData[ i ] = m_pDoc->GetDisplayColorForPaletteSlot( i );

			} else {

				m_SlotColorData[ i ] = pixel_type( 0 );

			}

			m_dwStyleFlags[ i ] = dwDefaultFlags;

		}

	}

	// ------------------------------------------------------------------------

	pixel_type GetDisplayColorForPaletteSlot( const int slot ) {

		return m_SlotColorData[ slot ];

	}

	void SetDisplayColorForPaletteSlot( const int slot, const pixel_type value ) {

		m_SlotColorData[ slot ] = value;

	}

	// ------------------------------------------------------------------------

	DWORD GetColorStyleBits( const int slot ) {

		return m_dwStyleFlags[ slot ];

	}

	void ModifyColorStyleBits( 
		const int slot, const DWORD dwRemove, const DWORD dwAdd ) {

		m_dwStyleFlags[ slot ] &= ~dwRemove;
		m_dwStyleFlags[ slot ] |= dwAdd;

	}

	// ------------------------------------------------------------------------

	virtual void OnLButtonDown(int slot, bool bCapture, UINT nFlags, CPoint point) {}
	virtual void OnLButtonUp(int slot, bool bCapture, UINT nFlags, CPoint point) {}
	virtual void OnLButtonDblClk(int slot, bool bCapture, UINT nFlags, CPoint point) {}
	virtual void OnRButtonDown(int slot, bool bCapture, UINT nFlags, CPoint point) {}
	virtual void OnRButtonUp(int slot, bool bCapture, UINT nFlags, CPoint point) {}
	virtual void OnRButtonDblClk(int slot, bool bCapture, UINT nFlags, CPoint point) {}
	virtual void OnMouseMove(int slot, bool bCapture, UINT nFlags, CPoint point) {}
	virtual void OnCancelMode(bool bCapture) {}

};

// ----------------------------------------------------------------------------

//
//	TPaletteReflectionMediator<>
//

template< class T >
class TPaletteReflectionMediator : public CPaletteDataMediator {

private:

	T * m_pOwner;

public:

	typedef TPaletteReflectionMediator<T> this_type;
	typedef CPaletteDataMediator base_class;

public:

	TPaletteReflectionMediator( CBpaintDoc * pDoc ) : 
		CPaletteDataMediator( pDoc ), m_pOwner( 0 ) {

		// Anything???

	}

	void SetOwner( T * pOwner ) {

		m_pOwner = pOwner;

	}

	virtual void OnLButtonDown(int slot, bool bCapture, UINT nFlags, CPoint point) {

		if ( m_pOwner ) m_pOwner->Palette_OnLButtonDown( this, slot, bCapture, nFlags, point );

		base_class::OnLButtonDown( slot, bCapture, nFlags, point );

	}

	virtual void OnLButtonUp(int slot, bool bCapture, UINT nFlags, CPoint point) {

		if ( m_pOwner ) m_pOwner->Palette_OnLButtonUp( this, slot, bCapture, nFlags, point );

		base_class::OnLButtonUp( slot, bCapture, nFlags, point );

	}

	virtual void OnLButtonDblClk(int slot, bool bCapture, UINT nFlags, CPoint point) {

		if ( m_pOwner ) m_pOwner->Palette_OnLButtonDblClk( this, slot, bCapture, nFlags, point );

		base_class::OnLButtonDblClk( slot, bCapture, nFlags, point );

	}

	virtual void OnRButtonDown(int slot, bool bCapture, UINT nFlags, CPoint point) {

		if ( m_pOwner ) m_pOwner->Palette_OnRButtonDown( this, slot, bCapture, nFlags, point );

		base_class::OnRButtonDown( slot, bCapture, nFlags, point );

	}

	virtual void OnRButtonUp(int slot, bool bCapture, UINT nFlags, CPoint point) {

		if ( m_pOwner ) m_pOwner->Palette_OnRButtonUp( this, slot, bCapture, nFlags, point );

		base_class::OnRButtonUp( slot, bCapture, nFlags, point );

	}

	virtual void OnRButtonDblClk(int slot, bool bCapture, UINT nFlags, CPoint point) {

		if ( m_pOwner ) m_pOwner->Palette_OnRButtonDblClk( this, slot, bCapture, nFlags, point );

		base_class::OnRButtonDblClk( slot, bCapture, nFlags, point );

	}

	virtual void OnMouseMove(int slot, bool bCapture, UINT nFlags, CPoint point) {

		if ( m_pOwner ) m_pOwner->Palette_OnMouseMove( this, slot, bCapture, nFlags, point );

		base_class::OnMouseMove( slot, bCapture, nFlags, point );

	}

	virtual void OnCancelMode(bool bCapture) {

		if ( m_pOwner ) m_pOwner->Palette_OnCancelMode( this, bCapture );

		base_class::OnCancelMode( bCapture );

	}

};

// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CSimplePaletteCtrl window

class CSimplePaletteCtrl : public CWnd
{

	static char s_ClassName[];

// Traits / globals
public:

	typedef BPT::TDIBSection<SDisplayPixelType> dib_type;

private: // Data

	CPaletteDataMediator * m_pMediator;
	dib_type m_DisplayDib;
	CSize m_ElementSize;
	CRect m_PaletteArea;
	HCURSOR m_hCursor;

private: // methods

	void RedrawPaletteBitmap( const CRect * pClipRect );

// Construction
public:
	CSimplePaletteCtrl();

// Attributes
public:

// Operations
public:

	void SetMediator( CPaletteDataMediator * pMediator ) {

		m_pMediator = pMediator;

		RedrawPaletteBitmap( 0 );

	}

	CPaletteDataMediator * GetMediator() {

		return m_pMediator;

	}

	void InvalidateSlot( const int slot, const bool bUpdate );
	void InvalidateWholePalette( const bool bUpdate );

	void CalcSlotRect( CRect & rect, const int slot );
	int GetSlotFromPoint( CPoint point );

	BOOL Create(CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle = WS_VISIBLE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimplePaletteCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSimplePaletteCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSimplePaletteCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	BOOL RegisterWindowClass();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEPALETTECTRL_H__663F0481_3988_45DE_BE9F_C900BA35FBBC__INCLUDED_)
