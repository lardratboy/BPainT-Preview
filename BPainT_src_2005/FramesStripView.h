#pragma once

// CFramesStripView view
//
//	Copyright (c) 2002, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//

class CFramesStripView : public CView
{
	DECLARE_DYNCREATE(CFramesStripView)

protected:
	CFramesStripView();           // protected constructor used by dynamic creation
	virtual ~CFramesStripView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};


