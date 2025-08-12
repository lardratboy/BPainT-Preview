// SimpleSplitView.cpp : implementation file
//

#include "stdafx.h"
#include "bpaint.h"
#include "SimpleSplitView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimpleSplitView

IMPLEMENT_DYNCREATE(CSimpleSplitView, CMDIChildWnd)

CSimpleSplitView::CSimpleSplitView()
{
}

CSimpleSplitView::~CSimpleSplitView()
{
}

BOOL CSimpleSplitView::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	return m_wndSplitter.Create(this,
		2, 2,       // TODO: adjust the number of rows, columns
		CSize(10, 10),  // TODO: adjust the minimum pane size
		pContext);
}


BEGIN_MESSAGE_MAP(CSimpleSplitView, CMDIChildWnd)
	//{{AFX_MSG_MAP(CSimpleSplitView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleSplitView message handlers
