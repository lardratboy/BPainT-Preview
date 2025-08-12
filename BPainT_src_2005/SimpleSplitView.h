#if !defined(AFX_SIMPLESPLITVIEW_H__93FB8CC5_19C1_40A8_8C0E_9EEF0ABE2B5B__INCLUDED_)
#define AFX_SIMPLESPLITVIEW_H__93FB8CC5_19C1_40A8_8C0E_9EEF0ABE2B5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimpleSplitView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimpleSplitView frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CSimpleSplitView : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CSimpleSplitView)
protected:
	CSimpleSplitView();           // protected constructor used by dynamic creation

// Attributes
protected:
	CSplitterWnd    m_wndSplitter;
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleSplitView)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSimpleSplitView();

	// Generated message map functions
	//{{AFX_MSG(CSimpleSplitView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLESPLITVIEW_H__93FB8CC5_19C1_40A8_8C0E_9EEF0ABE2B5B__INCLUDED_)
