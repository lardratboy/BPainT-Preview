#include "afxwin.h"
#if !defined(AFX_BRUSHGBAEXPORTDLG_H__57AB728C_32DE_4274_B0C6_86025B977BB2__INCLUDED_)
#define AFX_BRUSHGBAEXPORTDLG_H__57AB728C_32DE_4274_B0C6_86025B977BB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// BrushGBAExportDlg.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CBrushGBAExportDlg dialog

class CBrushGBAExportDlg : public CDialog
{
// Construction
public:

	CBrushGBAExportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBrushGBAExportDlg)
	enum { IDD = IDD_BRUSH_GBA_EXPORT };
	CString	m_ArrayName;
	CString	m_Filename;
	BOOL	m_bHexMode;
	BOOL	m_bSingleDimMode;
	BOOL	m_bNibblePackedPixels;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrushGBAExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBrushGBAExportDlg)
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_nBitPacking;
	int m_nExportMode;
	BOOL m_bOutputPalette;
	BOOL m_bRGB555Output;
	BOOL m_bGenerateShadeTable;
	int m_nTileWidth;
	int m_nTileHeight;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRUSHGBAEXPORTDLG_H__57AB728C_32DE_4274_B0C6_86025B977BB2__INCLUDED_)
