// PaletteManager.h : header file
//
//	Copyright (c) 2002, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#pragma once
#include "afxwin.h"

// (INCLUDES) -----------------------------------------------------------------

#include "bpaint.h"
#include "BpaintDoc.h"

// ----------------------------------------------------------------------------

// CPaletteManager dialog

class CPaletteManager : public CDialog
{
	DECLARE_DYNAMIC(CPaletteManager)

public:
	CPaletteManager(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaletteManager();

// Dialog Data
	enum { IDD = IDD_PALETTE_MANAGER };

private:

	typedef BPT::TDIBSection<SDisplayPixelType> dib_type;

	typedef BPT::CAnimationShowcase::palette_collection_type::iterator palette_iterator;

	// ------------------------------------------------------------------------

	dib_type m_PalettePreviewDib;

	// ------------------------------------------------------------------------

	bool AddPaletteToList( BPT::CAnnotatedPalette * pPalette );

	bool FillPaletteListbox();

	void SyncButtonsForSelectedPalette();

	void EnableDlgItem( const int nID, const bool bEnable = true );

	void GeneratePalettePreview();

	void UpdatePalettePreview();

	void SetModifiedFlag();
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	class CBpaintDoc * m_pDoc;
	BPT::CAnimationShowcase * m_pShowcase;
	BPT::CAnnotatedPalette * m_pDefaultPalette;
	BPT::CAnnotatedPalette * m_pSelectedPalette;
	CListBox m_PaletteList;
	afx_msg void OnBnClickedEditPalette();
	afx_msg void OnBnClickedNewPalette();
	afx_msg void OnBnClickedCopyPalette();
	afx_msg void OnBnClickedDeletePalette();
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangePaletteList();
	afx_msg void OnLbnDblclkPaletteList();
	afx_msg void OnBnClickedRenamePalette();
	afx_msg void OnBnClickedLoadPalette();
};
