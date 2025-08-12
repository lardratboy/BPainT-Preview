// FramesStripView.cpp : implementation file
//
//	Copyright (c) 2002, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//

#include "stdafx.h"
#include "bpaint.h"
#include "FramesStripView.h"


// CFramesStripView

IMPLEMENT_DYNCREATE(CFramesStripView, CView)

CFramesStripView::CFramesStripView()
{
}

CFramesStripView::~CFramesStripView()
{
}

BEGIN_MESSAGE_MAP(CFramesStripView, CView)
END_MESSAGE_MAP()


// CFramesStripView drawing

void CFramesStripView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CFramesStripView diagnostics

#ifdef _DEBUG
void CFramesStripView::AssertValid() const
{
	CView::AssertValid();
}

void CFramesStripView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CFramesStripView message handlers

void CFramesStripView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
}
