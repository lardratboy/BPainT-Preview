// NullView.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited

#include "stdafx.h"
#include "bpaint.h"
#include "NullView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNullView

IMPLEMENT_DYNCREATE(CNullView, CView)

CNullView::CNullView()
{
}

CNullView::~CNullView()
{
}


BEGIN_MESSAGE_MAP(CNullView, CView)
	//{{AFX_MSG_MAP(CNullView)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNullView drawing

void CNullView::OnDraw(CDC* pDC)
{
	CRect clientRect;

	GetClientRect(&clientRect);

	pDC->PatBlt(0,0,clientRect.Width(),clientRect.Height(),WHITENESS);
}

/////////////////////////////////////////////////////////////////////////////
// CNullView diagnostics

#ifdef _DEBUG
void CNullView::AssertValid() const
{
	CView::AssertValid();
}

void CNullView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNullView message handlers

BOOL CNullView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
