// VisualElementInfoView.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "VisualElementInfoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVisualElementInfoView

IMPLEMENT_DYNCREATE(CVisualElementInfoView, CEditView)

CVisualElementInfoView::CVisualElementInfoView()
{
}

CVisualElementInfoView::~CVisualElementInfoView()
{
}


BEGIN_MESSAGE_MAP(CVisualElementInfoView, CEditView)
	//{{AFX_MSG_MAP(CVisualElementInfoView)
	ON_COMMAND(ID_VEMODE_REPLACE_ASK, OnVemodeReplaceAsk)
	ON_UPDATE_COMMAND_UI(ID_VEMODE_REPLACE_ASK, OnUpdateVemodeReplaceAsk)
	ON_COMMAND(ID_VEMODE_REPLACE_DONT_ASK, OnVemodeReplaceDontAsk)
	ON_UPDATE_COMMAND_UI(ID_VEMODE_REPLACE_DONT_ASK, OnUpdateVemodeReplaceDontAsk)
	ON_COMMAND(ID_VEMODE_SIMPLE, OnVemodeSimple)
	ON_UPDATE_COMMAND_UI(ID_VEMODE_SIMPLE, OnUpdateVemodeSimple)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisualElementInfoView drawing

void CVisualElementInfoView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CVisualElementInfoView diagnostics

#ifdef _DEBUG

void CVisualElementInfoView::AssertValid() const
{
	CEditView::AssertValid();
}

void CVisualElementInfoView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CBpaintDoc* CVisualElementInfoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBpaintDoc)));
	return (CBpaintDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVisualElementInfoView message handlers

void CVisualElementInfoView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if ( pHint ) {

		if ( pHint->IsKindOf( RUNTIME_CLASS( CActiveAnimationChange ) ) ) {

			CActiveAnimationChange * pChange = (CActiveAnimationChange *)pHint;

			// future optimized information update...

			SyncTextToCurrentVisualElement();

		} else {

			// future optimized information update...

		}

	}

}

BOOL CVisualElementInfoView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class

	// Need to change the edit window style here!
	
	return CEditView::PreCreateWindow(cs);
}

void CVisualElementInfoView::OnInitialUpdate() 
{
	CEditView::OnInitialUpdate();
	
	// ------------------------------------------------------------------------

	CString title = "Visual Element Info ( " + GetDocument()->GetTitle() + " )";

	SetWindowText( title );

	GetParentFrame()->SetWindowText( title );

#if 1
	
	// Okay toolbar experiment
	// ------------------------------------------------------------------------

	DWORD dwTBFlags = TBSTYLE_FLAT;

	DWORD dwTBStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY
		/* | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER */ ;

	CRect tbRect( 0, 0, 0, 0 );

	// mdi child docking zoom toolbar
	// ------------------------------------------------------------------------

#if 1 // BPT 10/3/02

	CToolBar * pToolbar = new CToolBar; /* This used to cause a resource leak! */

	if ( pToolbar ) AtExitDeleteObject( pToolbar ); // this fixes the leak :)

#else

	CToolBar * pToolbar = new CToolBar; /* This causes a resource leak! */

#endif

	CFrameWnd * pFrame = GetParentFrame();

	if ( pToolbar && pFrame ) {

		if (pToolbar->CreateEx(pFrame, dwTBFlags, dwTBStyle, tbRect, IDR_VEINFOTB ) &&
			pToolbar->LoadToolBar(IDR_VEINFOTB))
		{
	
			// ------------------------------------------------------------------------
		
			pToolbar->EnableDocking( CBRS_ALIGN_ANY );
		
			pFrame->DockControlBar( pToolbar, AFX_IDW_DOCKBAR_TOP );
		
			pFrame->ShowControlBar( pToolbar, TRUE, FALSE );

		}

	}

#endif

	SyncTextToCurrentVisualElement();
}

// ============================================================================

//
//	CGatherVisualElementUse()
//

class CGatherVisualElementUse {

private:

	BPT::CLayer * m_pLayer;
	BPT::CFrame * m_pFrame;
	BPT::CAnimation * m_pAnimation;
	BPT::CAnimationShowcase * m_pShowcase;
	BPT::CVisualElement * m_pVisualElement;
	CString * m_pString;

	int m_nCounter;

public:

	CGatherVisualElementUse(
		BPT::CVisualElement * pVisualElement, CString * pString
	) {

		m_pString = pString;
		m_pVisualElement = pVisualElement;
		m_pLayer = 0;
		m_pFrame = 0;
		m_pAnimation = 0;
		m_pShowcase = 0;
		m_nCounter = 0;

	}

	void operator()( BPT::CLayer * pLayer ) {

		m_pLayer = pLayer;

	}

	void operator()( BPT::CFrame * pFrame ) {

		m_pFrame = pFrame;

	}

	void operator()( BPT::CAnimation * pAnimation ) {

		m_pAnimation = pAnimation;

	}

	void operator()( BPT::CAnimationShowcase * pShowcase ) {

		m_pShowcase = pShowcase;

	}

	bool operator()( BPT::CVisualElement * pVisualElement ) {

		if ( pVisualElement == m_pVisualElement ) {

			++m_nCounter;

			CString text;

			text.Format( "%4d) anim \"", m_nCounter );

			if ( m_pAnimation ) {

				text += m_pAnimation->GetName();

				text += "\"";

				if ( m_pFrame ) {

					CString frameText;

					frameText.Format( " Frame %d", m_pAnimation->FrameIndexFromPointer( m_pFrame ) + 1 );

					text += frameText;

				} else {

					text += " Frame ?";

				}

				if ( m_pLayer && m_pFrame ) {

					int layerIndex = m_pFrame->LayerIndexFromPointer( m_pLayer );

					if ( -1 != layerIndex ) {

						BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( layerIndex );

						if ( pLayerInfo ) {

							text += " Layer \"";

							text += pLayerInfo->GetName();

							text += "\"";

						} else {

							text += " Layer ?";

						}

					} else {

						text += " Layer ?";

					}

				} else {

					text += " Layer ?";

				}

			} else {

				text += " Error processing ";

			}

			text += "\r\n";

			*m_pString += text;

			return true;

		}

		return false;

	}

};

//
//	CVisualElementInfoView::SyncTextToCurrentVisualElement()
//

void 
CVisualElementInfoView::SyncTextToCurrentVisualElement()
{

	// -----------------------------------------------

	CBpaintDoc * pDoc = GetDocument();

	if ( !pDoc ) {

		GetEditCtrl().SetWindowText( "No document?" );

		return;

	}

	BPT::CAnimationShowcase * pShowcase = pDoc->GetShowcaseObject();

	if ( !pShowcase ) {

		GetEditCtrl().SetWindowText( "No showcase?" );

		return;

	}

	// -----------------------------------------------

	BPT::CAnimation * pAnimation = pDoc->CurrentAnimation();

	if ( !pAnimation ) {

		GetEditCtrl().SetWindowText( "No animation?" );

		return;

	}

	// -----------------------------------------------

	BPT::CLayer * pLayer = pAnimation->ActiveLayer();

	if ( !pLayer ) {

		GetEditCtrl().SetWindowText( "No frame layer?" );

		return;

	}

	// -----------------------------------------------

	CString text;

	POINT pos = pLayer->GetPos();

	RECT boundingRect;

	pLayer->BoundingRect( boundingRect );

	// -----------------------------------------------

	BPT::CLayerInfo * pLayerInfo = pAnimation->GetLayerInfo( pAnimation->CurrentLayer() );

	char *pLayerName = 0;

	if ( pLayerInfo ) {

		pLayerName = pLayerInfo->GetName();

	}

	if ( !pLayerName ) {

		pLayerName = "Unamed layer?";

	}

	// -----------------------------------------------

	BPT::CVisualElement * pVisualElement = pLayer->GetVisualElement();

	int visualElementRefCount = 0;

	CSize visualElementSize( 0, 0 );

	int visualElementID = 0;

	RECT visualElementRect = { 0, 0, 0, 0 };

	if ( pVisualElement ) {

		visualElementID = pVisualElement->ID();

		visualElementRefCount = pVisualElement->RefCount() - 2;

		visualElementSize = pVisualElement->Size();

		visualElementRect =  pVisualElement->BoundingRect();

	}

	// -----------------------------------------------

	if ( pLayer->HasVisualElement() ) {
	
		text.Format(
	
			"Frame %d layer \"%s\"\r\n"
			"VisualElement ( %d ) used %d times, size %d x %d\r\n"
			"%d nested visual elements\r\n"
			"Position Delta %d, %d\r\n"
			"Bounding Rectangle (%d, %d, %d, %d)\r\n"
			"VisualElementCaptureRect  (%d, %d, %d, %d)\r\n"
	
			,pAnimation->CurrentFrame() + 1
			,pLayerName
			,visualElementID
			,visualElementRefCount
			,visualElementSize.cx
			,visualElementSize.cy
			,((pVisualElement) ? pVisualElement->NestedVisualsCount() : 0)
			,pos.x
			,pos.y
			,boundingRect.left
			,boundingRect.top
			,boundingRect.right
			,boundingRect.bottom
			,visualElementRect.left
			,visualElementRect.top
			,visualElementRect.right
			,visualElementRect.bottom
	
		);

		// It would also be nice to have information like what other
		// animations/frames/use this cel...

		CGatherVisualElementUse infoGatherer( pVisualElement, &text );

		pShowcase->TVisualElementFunctorDispatch( infoGatherer );

	} else {

		text.Format(
	
			"Frame %d layer \"%s\" has no image\r\n"

			,pAnimation->CurrentFrame() + 1
			,pLayerName

		);

	}

	// -----------------------------------------------

	if ( pVisualElement ) {

		pVisualElement->Release();

	}

	// -----------------------------------------------

	GetEditCtrl().SetWindowText( text );

}

//
//	CVisualElementInfoView::HasActiveLayer()
//

bool CVisualElementInfoView::HasActiveLayer() {

	// -----------------------------------------------

	CBpaintDoc * pDoc = GetDocument();

	if ( !pDoc ) {

		return false;

	}

	// -----------------------------------------------

	BPT::CAnimation * pAnimation = pDoc->CurrentAnimation();

	if ( !pAnimation ) {

		return false;

	}

	// -----------------------------------------------

	BPT::CLayer * pLayer = pAnimation->ActiveLayer();

	if ( !pLayer ) {

		return false;

	}
	
	return true;

}

// ----------------------------------------------------------------------------

void CVisualElementInfoView::OnVemodeReplaceAsk() 
{
	CBpaintDoc * pDoc = GetDocument();

	if ( pDoc ) {

		if ( CBpaintDoc::ON_REPLACE_ASK == pDoc->m_nReplaceVisualElementMode ) {

			pDoc->m_nReplaceVisualElementMode = CBpaintDoc::ON_REPLACE_DO_NOTHING;

		} else {

			pDoc->m_nReplaceVisualElementMode = CBpaintDoc::ON_REPLACE_ASK;

		}

	}

}

void CVisualElementInfoView::OnUpdateVemodeReplaceAsk(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( HasActiveLayer() ? TRUE : FALSE );

	CBpaintDoc * pDoc = GetDocument();

	if ( pDoc ) {

		pCmdUI->SetCheck( (CBpaintDoc::ON_REPLACE_ASK == pDoc->m_nReplaceVisualElementMode) ? 1 : 0 );

	}

}

void CVisualElementInfoView::OnVemodeReplaceDontAsk() 
{
	CBpaintDoc * pDoc = GetDocument();

	if ( pDoc ) {

		if ( CBpaintDoc::ON_REPLACE_DONT_ASK == pDoc->m_nReplaceVisualElementMode ) {

			pDoc->m_nReplaceVisualElementMode = CBpaintDoc::ON_REPLACE_DO_NOTHING;

		} else {

			pDoc->m_nReplaceVisualElementMode = CBpaintDoc::ON_REPLACE_DONT_ASK;

		}

	}

}

void CVisualElementInfoView::OnUpdateVemodeReplaceDontAsk(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( HasActiveLayer() ? TRUE : FALSE );

	CBpaintDoc * pDoc = GetDocument();

	if ( pDoc ) {

		pCmdUI->SetCheck( (CBpaintDoc::ON_REPLACE_DONT_ASK == pDoc->m_nReplaceVisualElementMode) ? 1 : 0 );

	}

}

void CVisualElementInfoView::OnVemodeSimple() 
{
	CBpaintDoc * pDoc = GetDocument();

	if ( pDoc ) {

		pDoc->m_bSimpleEditMode = !pDoc->m_bSimpleEditMode;

	}
}

void CVisualElementInfoView::OnUpdateVemodeSimple(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( HasActiveLayer() ? TRUE : FALSE );

	CBpaintDoc * pDoc = GetDocument();

	if ( pDoc ) {

		pCmdUI->SetCheck( (pDoc->m_bSimpleEditMode) ? 1 : 0 );

	}

}
