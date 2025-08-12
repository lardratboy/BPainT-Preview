// BrushView.cpp : implementation file
//
//
//	Copyright (c) 2002, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//

#include "stdafx.h"
#include "bpaint.h"
#include "BrushView.h"


// CBrushView

IMPLEMENT_DYNCREATE(CBrushView, CView)

CBrushView::CBrushView()
{
}

CBrushView::~CBrushView()
{
}

BEGIN_MESSAGE_MAP(CBrushView, CView)
END_MESSAGE_MAP()


// CBrushView drawing

void CBrushView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CBrushView diagnostics

#ifdef _DEBUG
void CBrushView::AssertValid() const
{
	CView::AssertValid();
}

void CBrushView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CBrushView message handlers

void CBrushView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
}
