#if !defined(AFX_SIMPLESPLITFRAME_H__3C64B136_90FF_43F3_A686_E3AD7C4E6DEF__INCLUDED_)
#define AFX_SIMPLESPLITFRAME_H__3C64B136_90FF_43F3_A686_E3AD7C4E6DEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimpleSplitFrame.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited

/////////////////////////////////////////////////////////////////////////////
// CSimpleSplitFrame frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CSimpleSplitFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CSimpleSplitFrame)
protected:
	CSimpleSplitFrame();           // protected constructor used by dynamic creation

// data
private:

	CDocTemplate * m_pDocTemplate;

	// ------------------------------------------------------------------------

	int GetInternalSetting(
		const char * sectionName, const char * baseEntryName,
		const char * entry, const int value ) {
		CString finalEntryName;
	
		finalEntryName.Format( "%s.%s", baseEntryName, entry );
	
		return GLOBAL_GetSettingInt( finalEntryName, value, sectionName );
	}
	
	void PutInternalSetting(
		const char * sectionName, const char * baseEntryName, 
		const char * entry, const int value ) {
		CString finalEntryName;
	
		finalEntryName.Format( "%s.%s", baseEntryName, entry );
	
		GLOBAL_PutSettingInt( finalEntryName, value, sectionName );
	
	}

	// ------------------------------------------------------------------------

	bool StoreSplitterSettings(
		CSplitterWnd * pSplitter, const int number, 
		const char * sectionName, const char * baseEntryName
	);

	bool RestoreSplitterSettings(
		CSplitterWnd * pSplitter, const int number, 
		const char * sectionName, const char * baseEntryName
	);

	// ------------------------------------------------------------------------

// Attributes
protected:

	CSplitterWnd    m_wndSplitter1;
	CSplitterWnd	m_wndSplitter2;
	int				m_Layout;

public:

	enum {

		LAYOUT_1		= 1
		,LAYOUT_2		= 2
		,LAYOUT_3		= 3

	};

// Operations

	BOOL OnCreateClient1(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	BOOL OnCreateClient2(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	BOOL OnCreateClient3(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	CDocTemplate * GetDocTemplate() const {

		return m_pDocTemplate;

	}

	bool StoreInternalSettings( const char * sectionName, const char * baseEntryName );
	bool RestoreInternalSettings( const char * sectionName, const char * baseEntryName );

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleSplitFrame)
	public:
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSimpleSplitFrame();

	// Generated message map functions
	//{{AFX_MSG(CSimpleSplitFrame)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLESPLITFRAME_H__3C64B136_90FF_43F3_A686_E3AD7C4E6DEF__INCLUDED_)
