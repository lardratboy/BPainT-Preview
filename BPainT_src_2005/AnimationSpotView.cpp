// AnimationSpotView.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "AnimationSpotView.h"
#include "NamedSpotPropDlg.h"
#include "SpotAdjustmentPropDlg.h"
#include "bpaintDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnimationSpotView

IMPLEMENT_DYNCREATE(CAnimationSpotView, CTreeView)

CAnimationSpotView::CAnimationSpotView() : m_pContexMenuNode( 0 )
{
}

CAnimationSpotView::~CAnimationSpotView()
{
}


BEGIN_MESSAGE_MAP(CAnimationSpotView, CTreeView)
	//{{AFX_MSG_MAP(CAnimationSpotView)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_NSPOT_ADJUSTMENT_PROPERTIES, OnNspotAdjustmentProperties)
	ON_COMMAND(ID_NSPOT_CREATE_NEW, OnNspotCreateNew)
	ON_COMMAND(ID_NSPOT_DELETE_SPOT, OnNspotDeleteSpot)
	ON_COMMAND(ID_NSPOT_NEW_ADJUSTMENT, OnNspotNewSpotAdjustment)
	ON_COMMAND(ID_NSPOT_SPOT_PROPERTIES, OnNspotSpotProperties)
	ON_COMMAND(ID_NSPOT_DELETE_ADJUSTMENT, OnNspotDeleteAdjustment)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_COMMAND(ID_EDIT_SPOT_PROPERTIES, OnEditSpotProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SPOT_PROPERTIES, OnUpdateEditSpotProperties)
	ON_COMMAND(ID_DELETE_SPOT_OR_ADJUSTMENT, OnDeleteSpotOrAdjustment)
	ON_UPDATE_COMMAND_UI(ID_DELETE_SPOT_OR_ADJUSTMENT, OnUpdateDeleteSpotOrAdjustment)
	ON_COMMAND(ID_NEW_NAMED_SPOT, OnNewNamedSpot)
	ON_UPDATE_COMMAND_UI(ID_NEW_NAMED_SPOT, OnUpdateNewNamedSpot)
	ON_COMMAND(ID_CREATE_SPOT_ADJUSTMENT, OnCreateSpotAdjustment)
	ON_UPDATE_COMMAND_UI(ID_CREATE_SPOT_ADJUSTMENT, OnUpdateCreateSpotAdjustment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimationSpotView drawing

void CAnimationSpotView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CAnimationSpotView diagnostics

#ifdef _DEBUG
void CAnimationSpotView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CAnimationSpotView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CBpaintDoc* CAnimationSpotView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBpaintDoc)));
	return (CBpaintDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAnimationSpotView message handlers

void CAnimationSpotView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Ask the document to potentially give focus to us :)
	// ------------------------------------------------------------------------

	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc ) {

		pDoc->HandleAutoFocus( this );

	}

	// TODO: Add your message handler code here and/or call default
	
	CTreeView::OnMouseMove(nFlags, point);
}

void CAnimationSpotView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();
	
	// Change the Tree's styles
	// ------------------------------------------------------------------------

	CTreeCtrl & treeCtrl = GetTreeCtrl();

	treeCtrl.ModifyStyle( 
		0, 
		TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS 
	);

	// ------------------------------------------------------------------------

	CString title = "Animation spots ( " + GetDocument()->GetTitle() + " )";

	SetWindowText( title );

	GetParentFrame()->SetWindowText( title );

	// Try to force the menu to be what we really want.
	// ------------------------------------------------------------------------

	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc ) {

		pDoc->EnsureProperMenu( this );

		FillTreeWithAnimationInfo( pDoc->CurrentAnimation() );

	}

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

		if (pToolbar->CreateEx(pFrame, dwTBFlags, dwTBStyle, tbRect, IDR_SPOT_VIEW_TOOLBAR ) &&
			pToolbar->LoadToolBar(IDR_SPOT_VIEW_TOOLBAR))
		{
	
			// ------------------------------------------------------------------------
		
			pToolbar->EnableDocking( CBRS_ALIGN_ANY );
		
			pFrame->DockControlBar( pToolbar, AFX_IDW_DOCKBAR_TOP );
		
			pFrame->ShowControlBar( pToolbar, TRUE, FALSE );

		}

	}

#endif

}

void CAnimationSpotView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if ( pHint ) {

		if ( pHint->IsKindOf( RUNTIME_CLASS( CActiveAnimationChange ) ) ) {

			CActiveAnimationChange * pChange = (CActiveAnimationChange *)pHint;

			CBpaintDoc* pDoc = GetDocument();

			bool bRefreshMode = false;

			if ( pDoc ) switch ( lHint ) {

			case CActiveAnimationChange::NEW_ANIMATION:
				FillTreeWithAnimationInfo( pDoc->CurrentAnimation() );
				break;

			case CActiveAnimationChange::SPOT_DATA_CHANGE:
				bRefreshMode = true;
				// fall through...

			case CActiveAnimationChange::NEW_SPOT_ADJUSTMENT: {
	
					BPT::CAnimation * pAnimation = pDoc->CurrentAnimation();
	
					if ( pAnimation ) {
	
						BPT::CFrame * pFrame = pAnimation->GetFramePtr(
							pChange->m_NewSelection.x
						);
	
						BPT::CNamedSpot * pNamedSpot = pDoc->GetCurrentNamedSpot();
	
						BPT::CNamedSpotAdjustment * pAdjustment = 0;
	
						if ( pFrame && pNamedSpot ) {
	
							pAdjustment = pFrame->FindSpotAdjusterFor(
								pNamedSpot
							);
	
						}
	
						FillTreeWithAnimationInfo(
							pAnimation, pNamedSpot, pAdjustment, bRefreshMode
						);
	
					} else {
	
						FillTreeWithAnimationInfo( pAnimation );
	
					}

				}
				break;

			}

		}

	}
}

// ============================================================================

CString 
CAnimationSpotView::BuildStringForAnimation( BPT::CAnimation * pAnimation )
{
	if ( pAnimation->GetName() ) {

		return CString( pAnimation->GetName() );

	}

	return CString( "Error?" );
}

CString 
CAnimationSpotView::BuildStringForSpot( BPT::CNamedSpot * pSpot )
{
	CString spotName;

	POINT location = pSpot->GetLocation();

	spotName.Format( "%s (%d,%d)", pSpot->GetName(), location.x, location.y );

	return spotName;

}

CString 
CAnimationSpotView::BuildStringForAdjustment(
	BPT::CNamedSpotAdjustment * pAdjustment, const int nFrame )
{
	POINT pos = pAdjustment->GetPosition();
	
	POINT delta = pAdjustment->GetDelta();
	
	CString text;
	
	char * pName = pAdjustment->GetName();
	
	if ( !pName ) {
	
		pName = "Error?";
	
	}
	
	text.Format( 
		"Frame %d position %d, %d (delta %d, %d) spot '%s'", 
		nFrame, pos.x, pos.y, delta.x, delta.y, pName 
	);

	return text;

}

// ============================================================================

//
//	CAnimationSpotView::FillTreeWithAnimationInfoPrim()
//

void 
CAnimationSpotView::FillTreeWithAnimationInfoPrim( 
	BPT::CAnimation * pAnimation, 
	BPT::CNamedSpot * pSelectSpot,
	BPT::CNamedSpotAdjustment * pSelectAdjustment
)
{
	CTreeCtrl & treeCtrl = GetTreeCtrl();

	// Empty out any existing data
	// ------------------------------------------------------------------------

	treeCtrl.DeleteAllItems();

	m_pContexMenuNode = 0;

	m_NodeInfoCollection.clear();

	// if a valid anim pointer was passed then fill in the list with it's info
	// ------------------------------------------------------------------------

	if ( pAnimation ) {

		int nNextIndex = 0;

		HTREEITEM hRoot = treeCtrl.InsertItem( BuildStringForAnimation( pAnimation ) );

		HTREEITEM hEnsureItemIsVisible = hRoot;

		if ( hRoot ) {

			// Add the node to our node info collection
			// ----------------------------------------------------------------

			m_NodeInfoCollection.push_back(
				CNodeInfo( 
					hRoot, CNodeInfo::NODE_TYPE::ANIMATION, pAnimation, 0, 0, 0
				)
			);

			treeCtrl.SetItemData( hRoot, nNextIndex++ );

			// ----------------------------------------------------------------

			BPT::CAnimation::named_spot_collection_type & spotCollection = pAnimation->NamedSpotCollection();

			BPT::CAnimation::named_spot_collection_type::iterator it = spotCollection.begin();

			for ( ;it != spotCollection.end(); it++ ) {

				CString spotName = BuildStringForSpot( (*it) );

				HTREEITEM hSpotRoot = treeCtrl.InsertItem( spotName, hRoot );

				if ( hSpotRoot ) {
	
					// Handle the auto selection & visibility 
					// --------------------------------------------

					if ( pSelectSpot == (*it) ) {

						hEnsureItemIsVisible = hSpotRoot;

					}

					// Add the node to our node info collection
					// --------------------------------------------------------
		
					m_NodeInfoCollection.push_back(
						CNodeInfo( 
							hSpotRoot, CNodeInfo::NODE_TYPE::NAMED_SPOT, pAnimation, *it, 0, 0
						)
					);
		
					treeCtrl.SetItemData( hSpotRoot, nNextIndex++ );

					// --------------------------------------------------------

					BPT::CAnimation::frame_collection_type & frames = pAnimation->GetFrameCollection();

					BPT::CAnimation::frame_collection_type::iterator frameIt = frames.begin();

					for ( int frame = 1; frameIt != frames.end(); frame++, frameIt++ ) {

						BPT::CNamedSpotAdjustment * pAdjustment = (*frameIt)->FindSpotAdjusterFor( *it );

						if ( pAdjustment ) {

							CString text = BuildStringForAdjustment( pAdjustment, frame );

							HTREEITEM hAdjustment = treeCtrl.InsertItem( text, hSpotRoot );

							if ( hAdjustment ) {

								// Handle the auto selection & visibility 
								// --------------------------------------------

								if ( pSelectAdjustment == pAdjustment ) {

									hEnsureItemIsVisible = hAdjustment;

								}
			
								// Add the node to our node info collection
								// --------------------------------------------
					
								m_NodeInfoCollection.push_back(
									CNodeInfo( 
										hAdjustment,
										CNodeInfo::NODE_TYPE::SPOT_ADJUSTMENT, 
										pAnimation, *it, pAdjustment, (frame - 1)
									)
								);
					
								treeCtrl.SetItemData( hAdjustment, nNextIndex++ );

								// Anything else?
								// --------------------------------------------

							}

						}

					}

				}

			}

		}

		if ( hEnsureItemIsVisible ) {

			if ( treeCtrl.ItemHasChildren( hEnsureItemIsVisible ) ) {

				treeCtrl.Expand( hEnsureItemIsVisible, TVE_EXPAND );

			}

			treeCtrl.EnsureVisible( hEnsureItemIsVisible );

			treeCtrl.SelectItem( hEnsureItemIsVisible );

		}

	}

}

// ============================================================================

//
//	CAnimationSpotView::FillTreeWithAnimationInfo()
//

void 
CAnimationSpotView::FillTreeWithAnimationInfo(
	BPT::CAnimation * pAnimation, 
	BPT::CNamedSpot * pSelectSpot,
	BPT::CNamedSpotAdjustment * pSelectAdjustment,
	const bool bRefreshMode
)
{
	if ( bRefreshMode ) {

		// find the info node for this item
		// --------------------------------------------------------------------

		CNodeInfo * pNodeInfo = GetNodeForInfo(
			pAnimation, pSelectSpot, pSelectAdjustment
		);

		if ( !pNodeInfo || (0 == pNodeInfo->m_hTreeItem)) {

			goto DO_WHOLE_TREE;

		}

		// Fill in the string for this node
		// --------------------------------------------------------------------

		CString newString;

		switch ( pNodeInfo->m_NodeType ) {

		case CAnimationSpotView::CNodeInfo::NODE_TYPE::ANIMATION:
			newString = BuildStringForAnimation( pAnimation );
			break;

		case CAnimationSpotView::CNodeInfo::NODE_TYPE::NAMED_SPOT:
			newString = BuildStringForSpot( pSelectSpot );
			break;

		case CAnimationSpotView::CNodeInfo::NODE_TYPE::SPOT_ADJUSTMENT:
			newString = BuildStringForAdjustment(
				pSelectAdjustment, pNodeInfo->m_nFrame + 1
			);
			break;

		}

		// change the items text info

		GetTreeCtrl().SetItemText( pNodeInfo->m_hTreeItem, newString );

		UpdateWindow();

	} else {

	DO_WHOLE_TREE:

		GetTreeCtrl().SetRedraw( FALSE );

		FillTreeWithAnimationInfoPrim(
			pAnimation, pSelectSpot, pSelectAdjustment
		);

		GetTreeCtrl().SetRedraw( TRUE );
		GetTreeCtrl().Invalidate();

	}

}

// ============================================================================

//	CAnimationSpotView::GetNodeForInfo()
//

CAnimationSpotView::CNodeInfo * 
CAnimationSpotView::GetNodeForInfo(
	BPT::CAnimation * pAnimation, 
	BPT::CNamedSpot * pSelectSpot,
	BPT::CNamedSpotAdjustment * pSelectAdjustment
)
{
	node_collection_type::iterator it = m_NodeInfoCollection.begin();

	for ( ; it != m_NodeInfoCollection.end(); it++ ) {

		if ( (pAnimation == it->m_pAnimation) && 
			(pSelectSpot == it->m_pNamedSpot) && 
			(pSelectAdjustment == it->m_pNamedSpotAdjustment) ) {

			return &(*it);

		}

	}

	return 0;
}

// ============================================================================

//
//	CAnimationSpotView::GetNodeInfoPtrForItem()
//

CAnimationSpotView::CNodeInfo * 
CAnimationSpotView::GetNodeInfoPtrForItem( const HTREEITEM hTreeItem )
{
	node_collection_type::iterator it = m_NodeInfoCollection.begin();

	for ( ; it != m_NodeInfoCollection.end(); it++ ) {

		if ( hTreeItem == it->m_hTreeItem ) {

			return &(*it);

		}

	}

	return 0;
}

// ----------------------------------------------------------------------------

void CAnimationSpotView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hCurrent = GetTreeCtrl().GetSelectedItem();

	if ( hCurrent ) {

		CNodeInfo * pNodeInfo = GetNodeInfoPtrForItem( hCurrent );

		CBpaintDoc* pDoc = GetDocument();

		if ( pNodeInfo && pDoc ) {

			if ( pNodeInfo->m_pNamedSpotAdjustment ) {

				pDoc->Request_FrameChange( pNodeInfo->m_nFrame );

			}

			if ( pNodeInfo->m_pNamedSpot ) {

				pDoc->SetCurrentNamedSpot( pNodeInfo->m_pNamedSpot );
	
				pDoc->SetShowLinkPointsAndSpotsMode( true );

				pDoc->ForceFullRedraw();

			} else {

				pDoc->SetCurrentNamedSpot( 0 );

			}

		}

	}

	*pResult = 0;
}

// ----------------------------------------------------------------------------

//
//	CAnimationSpotView::OnRclick()
//

void CAnimationSpotView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hCurrent = GetTreeCtrl().GetSelectedItem();

	if ( hCurrent ) {

		CNodeInfo * pNodeInfo = GetNodeInfoPtrForItem( hCurrent );

		if ( pNodeInfo ) {

			// ----------------------------------------------------------------

			bool bDoMenu = false;

			CMenu popupMenu;

			switch ( pNodeInfo->m_NodeType ) {

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::ANIMATION:
				bDoMenu = (TRUE == popupMenu.LoadMenu(IDR_NAMED_SPOT_ANIM_POPUP));
				break;

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::NAMED_SPOT:
				bDoMenu = (TRUE == popupMenu.LoadMenu(IDR_NAMED_SPOT_SPOT_POPUP));
				break;

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::SPOT_ADJUSTMENT:
				bDoMenu = (TRUE == popupMenu.LoadMenu(IDR_NAMED_SPOT_ADJUSTMENT_POPUP));
				break;

			}

			// ----------------------------------------------------------------

			if ( bDoMenu ) {

				CPoint point;

				GetCursorPos( &point );

				if ( CMenu * pSubMenu = popupMenu.GetSubMenu(0) ) {

					m_pContexMenuNode = pNodeInfo;

					pSubMenu->TrackPopupMenu(
						TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this
					);

				}

			}

		}

	}

	*pResult = 0;
}

//
//	CAnimationSpotView::OnDblclk()
//

void CAnimationSpotView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hCurrent = GetTreeCtrl().GetSelectedItem();

	if ( hCurrent ) {

		CNodeInfo * pNodeInfo = GetNodeInfoPtrForItem( hCurrent );

		if ( pNodeInfo ) {

			// ----------------------------------------------------------------

			switch ( pNodeInfo->m_NodeType ) {

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::ANIMATION:
				break;

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::NAMED_SPOT:
				break;

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::SPOT_ADJUSTMENT:
				OnNspotAdjustmentProperties();
				break;

			}

		}

	}
	
	*pResult = 0;
}

// ----------------------------------------------------------------------------

//
//	CAnimationSpotView::OnNspotNewSpotAdjustment()
//

void CAnimationSpotView::OnNspotNewSpotAdjustment() 
{
	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc && m_pContexMenuNode ) {

		BPT::CAnimation * pAnimation = m_pContexMenuNode->m_pAnimation;

		BPT::CNamedSpot * pNamedSpot = m_pContexMenuNode->m_pNamedSpot;

		if ( pAnimation && pNamedSpot ) {

			// ----------------------------------------------------------------

			CSpotAdjustmentPropDlg dlg( this );

			dlg.m_Note = "";

			POINT location = pNamedSpot->GetLocation();
	
			dlg.m_xPos = location.x;
	
			dlg.m_yPos = location.y;

			dlg.m_nFrame = pAnimation->CurrentFrame() + 1;

			dlg.m_nMinFrame = 1;

			dlg.m_nMaxFrame = pAnimation->FrameCount();
		
			// ----------------------------------------------------------------

		DO_OVER:

			if ( IDOK == dlg.DoModal() ) {

				// Get the frame pointer
				// --------------------------------------------------------

				BPT::CFrame * pFrame = pAnimation->GetFramePtr( dlg.m_nFrame - 1 );

				if ( !pFrame ) {

					goto DO_OVER;

				}

				// Check to see if the user did something wrong.
				// --------------------------------------------------------

				if ( pFrame->FindSpotAdjusterFor( pNamedSpot ) ) {

					CString msg;

					msg.Format( 
						"Spot \"%s\" already has an adjusment on frame %d. "
						"Hit YES to change the frame number, hit NO or CANCEL to leave.",
						pNamedSpot->GetName(), dlg.m_nFrame 
					);

					int result = MessageBox( msg, "Yo!", MB_ICONQUESTION | MB_YESNOCANCEL );

					if ( IDYES == result ) {

						goto DO_OVER;

					}

					return;

				}

				// Create the adjustment object and get down to business
				// --------------------------------------------------------

				BPT::CNamedSpotAdjustment * pNamedSpotAdjustment = new BPT::CNamedSpotAdjustment;

				if ( pNamedSpotAdjustment ) {

					// Fill in the adjustment info
					// --------------------------------------------------------

					POINT delta, location;

					location = pNamedSpot->GetLocation();

					delta.x = dlg.m_xPos - location.x;
					delta.y = dlg.m_yPos - location.y;

					pNamedSpotAdjustment->SetDelta( delta );

					pNamedSpotAdjustment->SetNote( dlg.m_Note );

					pNamedSpotAdjustment->SetNamedSpot( pNamedSpot );

					// Add the spot adjustment to the frame 
					// --------------------------------------------------------

					pFrame->AttachNamedSpotAdjustment( pNamedSpotAdjustment );

					// Notify the document of the change so the UI can update
					// --------------------------------------------------------

					FillTreeWithAnimationInfo( pAnimation, pNamedSpot, pNamedSpotAdjustment );

					// NEED TO TELL THE DOCUMENT THAT AN ADJUSTMENT WAS ADDED
					// NEED TO TELL THE DOCUMENT THAT AN ADJUSTMENT WAS ADDED
					// NEED TO TELL THE DOCUMENT THAT AN ADJUSTMENT WAS ADDED

					pDoc->SetModifiedFlag();
					pDoc->SetCurrentNamedSpot( pNamedSpot );
					pDoc->SetShowLinkPointsAndSpotsMode( true );
					pDoc->ForceFullRedraw();

				}

			}

		}

	}
	
}

//
//	AnimationSpotView::OnNspotAdjustmentProperties()
//

void CAnimationSpotView::OnNspotAdjustmentProperties() 
{
	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc && m_pContexMenuNode ) {

		BPT::CAnimation * pAnimation = m_pContexMenuNode->m_pAnimation;

		BPT::CNamedSpot * pNamedSpot = m_pContexMenuNode->m_pNamedSpot;

		BPT::CNamedSpotAdjustment * pNamedSpotAdjustment = m_pContexMenuNode->m_pNamedSpotAdjustment;

		if ( pAnimation && pNamedSpot && pNamedSpotAdjustment ) {

			// ----------------------------------------------------------------

			CSpotAdjustmentPropDlg dlg( this );

			dlg.m_Note = pNamedSpotAdjustment->GetNote();

			POINT location = pNamedSpotAdjustment->GetPosition();
				
			dlg.m_xPos = location.x;
	
			dlg.m_yPos = location.y;

			dlg.m_nFrame = m_pContexMenuNode->m_nFrame + 1;

			dlg.m_nMinFrame = dlg.m_nFrame;

			dlg.m_nMaxFrame = dlg.m_nFrame;
		
			// ----------------------------------------------------------------

			if ( IDOK == dlg.DoModal() ) {

				// Fill in the adjustment info
				// ------------------------------------------------------------

				POINT delta, location;

				location = pNamedSpot->GetLocation();

				delta.x = dlg.m_xPos - location.x;
				delta.y = dlg.m_yPos - location.y;

				pNamedSpotAdjustment->SetDelta( delta );

				pNamedSpotAdjustment->SetNote( dlg.m_Note );

				// Notify the document of the change so the UI can update
				// ------------------------------------------------------------

				FillTreeWithAnimationInfo( pAnimation, pNamedSpot, pNamedSpotAdjustment );

				// NEED TO TELL THE DOCUMENT THAT AN ADJUSTMENT WAS ADDED
				// NEED TO TELL THE DOCUMENT THAT AN ADJUSTMENT WAS ADDED
				// NEED TO TELL THE DOCUMENT THAT AN ADJUSTMENT WAS ADDED

				pDoc->SetModifiedFlag();
				pDoc->SetCurrentNamedSpot( pNamedSpot );
				pDoc->SetShowLinkPointsAndSpotsMode( true );
				pDoc->ForceFullRedraw();

			}

		}

	}
	
}

//
//	CAnimationSpotView::OnNspotDeleteAdjustment() 
//

void CAnimationSpotView::OnNspotDeleteAdjustment() 
{
	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc && m_pContexMenuNode ) {

		BPT::CAnimation * pAnimation = m_pContexMenuNode->m_pAnimation;

		BPT::CNamedSpot * pNamedSpot = m_pContexMenuNode->m_pNamedSpot;

		BPT::CNamedSpotAdjustment * pNamedSpotAdjustment = m_pContexMenuNode->m_pNamedSpotAdjustment;

		if ( pAnimation && pNamedSpot && pNamedSpotAdjustment ) {

			BPT::CFrame * pFrame = pAnimation->GetFramePtr( m_pContexMenuNode->m_nFrame );

			if ( pFrame ) {

				BPT::CNamedSpotAdjustment * pFound = pFrame->FindSpotAdjusterFor( pNamedSpot );

				if ( pFound == pNamedSpotAdjustment ) {
		
					CString message;
		
					message.Format( 
						"Delete frame %d spot \"%s\" adjustment (this can't be undone.)", 
						m_pContexMenuNode->m_nFrame + 1, pNamedSpot->GetName() 
					);
		
					if ( IDYES == MessageBox( message, "Delete Adjustment?", MB_ICONQUESTION | MB_YESNO ) ) {

						pFrame->DeleteNamedSpotAdjustment( pNamedSpotAdjustment );

						m_pContexMenuNode = 0;
		
						// Notify the document of the change so the UI can update
						// ----------------------------------------------------
		
						FillTreeWithAnimationInfo( pAnimation, pNamedSpot, 0 );
		
						// NEED TO TELL THE DOCUMENT THAT AN ADJUSTMENT WAS ADDED
						// NEED TO TELL THE DOCUMENT THAT AN ADJUSTMENT WAS ADDED
						// NEED TO TELL THE DOCUMENT THAT AN ADJUSTMENT WAS ADDED
		
						pDoc->SetModifiedFlag();
						pDoc->SetCurrentNamedSpot( pNamedSpot );
						pDoc->SetShowLinkPointsAndSpotsMode( true );
						pDoc->ForceFullRedraw();

					}

				}

			}

		}

	}

}

// ----------------------------------------------------------------------------

//
//	CAnimationSpotView::OnNspotCreateNew()
//

void CAnimationSpotView::OnNspotCreateNew() 
{
	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc ) {

		BPT::CAnimation * pAnimation = pDoc->CurrentAnimation();

		if ( pAnimation ) {

			SIZE canvasSize = pDoc->GetCanvasSize();

			// ----------------------------------------------------------------

			CNamedSpotPropDlg dlg( this );
		
			dlg.m_Name.Format( "Spot %d", pAnimation->NamedSpotCount() + 1 );

			dlg.m_Note = "";

			dlg.m_xPos = canvasSize.cx / 2;

			dlg.m_yPos = canvasSize.cy / 2;
		
			// ----------------------------------------------------------------

			if ( IDOK == dlg.DoModal() ) {
		
				BPT::CNamedSpot * pNamedSpot = new BPT::CNamedSpot;

				if ( pNamedSpot ) {

					// Set the spot information from the dialog results
					// --------------------------------------------------------

					pNamedSpot->SetName( dlg.m_Name );
					
					if ( !dlg.m_Note.IsEmpty() ) {

						pNamedSpot->SetNote( dlg.m_Note );

					}

					CPoint location( dlg.m_xPos, dlg.m_yPos );

					pNamedSpot->SetLocation( location );

					// Attach the spot to the animation
					// --------------------------------------------------------

					pAnimation->AttachNamedSpot( pNamedSpot );
	
					// Notify the document of the change so the UI can update
					// --------------------------------------------------------

					FillTreeWithAnimationInfo( pAnimation, pNamedSpot );

					// NEED TO TELL THE DOCUMENT THAT A SPOT WAS ADDED
					// NEED TO TELL THE DOCUMENT THAT A SPOT WAS ADDED
					// NEED TO TELL THE DOCUMENT THAT A SPOT WAS ADDED

					pDoc->SetModifiedFlag();
					pDoc->SetCurrentNamedSpot( pNamedSpot );
					pDoc->SetShowLinkPointsAndSpotsMode( true );
					pDoc->ForceFullRedraw();

				}

			}

		}

	}

}

// ----------------------------------------------------------------------------

//
//	CAnimationSpotView::OnNspotSpotProperties()
//

void CAnimationSpotView::OnNspotSpotProperties() 
{
	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc && m_pContexMenuNode ) {

		BPT::CAnimation * pAnimation = m_pContexMenuNode->m_pAnimation;
		BPT::CNamedSpot * pNamedSpot = m_pContexMenuNode->m_pNamedSpot;
		
		if ( pAnimation && pNamedSpot ) {

			// ----------------------------------------------------------------

			CNamedSpotPropDlg dlg( this );

			if ( pNamedSpot->GetName() ) {

				dlg.m_Name = pNamedSpot->GetName();

			} else {

				dlg.m_Name = "Error?";

			}
		
			dlg.m_Note = pNamedSpot->GetNote();

			POINT location = pNamedSpot->GetLocation();

			dlg.m_xPos = location.x;

			dlg.m_yPos = location.y;
		
			// ----------------------------------------------------------------

			if ( IDOK == dlg.DoModal() ) {
		
				// Set the spot information from the dialog results
				// ------------------------------------------------------------

				pNamedSpot->SetName( dlg.m_Name );
					
				if ( !dlg.m_Note.IsEmpty() ) {

					pNamedSpot->SetNote( dlg.m_Note );

				}

				pNamedSpot->SetLocation( CPoint( dlg.m_xPos, dlg.m_yPos ) );

				// Notify the document of the change so the UI can update
				// ------------------------------------------------------------

				FillTreeWithAnimationInfo( pAnimation, pNamedSpot );

				// NEED TO TELL THE DOCUMENT THAT A SPOT WAS CHANGED
				// NEED TO TELL THE DOCUMENT THAT A SPOT WAS CHANGED
				// NEED TO TELL THE DOCUMENT THAT A SPOT WAS CHANGED

				pDoc->SetModifiedFlag();
				pDoc->SetCurrentNamedSpot( pNamedSpot );
				pDoc->SetShowLinkPointsAndSpotsMode( true );
				pDoc->ForceFullRedraw();

			}

		}

	}

}

// ----------------------------------------------------------------------------

//
//	CAnimationSpotView::OnNspotDeleteSpot()
//

void CAnimationSpotView::OnNspotDeleteSpot() 
{
	CBpaintDoc* pDoc = GetDocument();

	if ( pDoc && m_pContexMenuNode ) {

		BPT::CAnimation * pAnimation = m_pContexMenuNode->m_pAnimation;
		BPT::CNamedSpot * pNamedSpot = m_pContexMenuNode->m_pNamedSpot;
		
		if ( pAnimation && pNamedSpot ) {

			CString message;

			message.Format( 
				"Delete spot \"%s\" (this can't be undone.)", pNamedSpot->GetName() 
			);

			if ( IDYES == MessageBox( message, "Delete Spot?", MB_ICONQUESTION | MB_YESNO ) ) {

				pAnimation->DeleteNamedSpot( pNamedSpot );

				// Notify the document of the change so the UI can update
				// --------------------------------------------------------

				FillTreeWithAnimationInfo( pAnimation, pNamedSpot );

				// NEED TO TELL THE DOCUMENT THAT A SPOT WAS REMOVED
				// NEED TO TELL THE DOCUMENT THAT A SPOT WAS REMOVED
				// NEED TO TELL THE DOCUMENT THAT A SPOT WAS REMOVED

				pDoc->SetModifiedFlag();
				pDoc->SetCurrentNamedSpot( 0 );
				pDoc->SetShowLinkPointsAndSpotsMode( true );
				pDoc->ForceFullRedraw();

			}

		}

	}
}

// ----------------------------------------------------------------------------

//
//	CAnimationSpotView::OnEditSpotProperties()
//

void CAnimationSpotView::OnEditSpotProperties() 
{
	HTREEITEM hCurrent = GetTreeCtrl().GetSelectedItem();

	if ( hCurrent ) {

		CNodeInfo * pNodeInfo = GetNodeInfoPtrForItem( hCurrent );

		if ( pNodeInfo ) {

			m_pContexMenuNode = pNodeInfo;

			// ----------------------------------------------------------------

			switch ( pNodeInfo->m_NodeType ) {

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::ANIMATION:
				break;

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::NAMED_SPOT:
				OnNspotSpotProperties();
				break;

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::SPOT_ADJUSTMENT:
				OnNspotAdjustmentProperties();
				break;

			}

			m_pContexMenuNode = 0;

		}

	}
}

//
//	CAnimationSpotView::OnUpdateEditSpotProperties()
//

void CAnimationSpotView::OnUpdateEditSpotProperties(CCmdUI* pCmdUI) 
{
	HTREEITEM hCurrent = GetTreeCtrl().GetSelectedItem();

	BOOL bEnable = FALSE;

	if ( hCurrent ) {

		CNodeInfo * pNodeInfo = GetNodeInfoPtrForItem( hCurrent );

		bEnable = (0 != pNodeInfo) ? TRUE : FALSE;

	}

	pCmdUI->Enable( bEnable );
}

//
//	CAnimationSpotView::OnDeleteSpotOrAdjustment()
//

void CAnimationSpotView::OnDeleteSpotOrAdjustment() 
{
	HTREEITEM hCurrent = GetTreeCtrl().GetSelectedItem();

	if ( hCurrent ) {

		CNodeInfo * pNodeInfo = GetNodeInfoPtrForItem( hCurrent );

		if ( pNodeInfo ) {

			m_pContexMenuNode = pNodeInfo;

			// ----------------------------------------------------------------

			switch ( pNodeInfo->m_NodeType ) {

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::NAMED_SPOT:
				OnNspotDeleteSpot();
				break;

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::SPOT_ADJUSTMENT:
				OnNspotDeleteAdjustment();
				break;

			}

			m_pContexMenuNode = 0;

		}

	}
}

//
//	CAnimationSpotView::OnUpdateDeleteSpotOrAdjustment()
//

void CAnimationSpotView::OnUpdateDeleteSpotOrAdjustment(CCmdUI* pCmdUI) 
{
	HTREEITEM hCurrent = GetTreeCtrl().GetSelectedItem();

	BOOL bEnable = FALSE;

	if ( hCurrent ) {

		CNodeInfo * pNodeInfo = GetNodeInfoPtrForItem( hCurrent );

		if ( pNodeInfo ) {

			// ----------------------------------------------------------------

			switch ( pNodeInfo->m_NodeType ) {

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::NAMED_SPOT:
				bEnable = TRUE;
				break;

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::SPOT_ADJUSTMENT:
				bEnable = TRUE;
				break;

			}

		}

	}

	pCmdUI->Enable( bEnable );
}

// ----------------------------------------------------------------------------

void CAnimationSpotView::OnNewNamedSpot() 
{
	OnNspotCreateNew();
}

void CAnimationSpotView::OnUpdateNewNamedSpot(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );
}

void CAnimationSpotView::OnCreateSpotAdjustment() 
{
	HTREEITEM hCurrent = GetTreeCtrl().GetSelectedItem();

	if ( hCurrent ) {

		CNodeInfo * pNodeInfo = GetNodeInfoPtrForItem( hCurrent );

		if ( pNodeInfo ) {

			m_pContexMenuNode = pNodeInfo;

			OnNspotNewSpotAdjustment();

			m_pContexMenuNode = 0;

		}

	}

}

void CAnimationSpotView::OnUpdateCreateSpotAdjustment(CCmdUI* pCmdUI) 
{
	HTREEITEM hCurrent = GetTreeCtrl().GetSelectedItem();

	BOOL bEnable = FALSE;

	if ( hCurrent ) {

		CNodeInfo * pNodeInfo = GetNodeInfoPtrForItem( hCurrent );

		if ( pNodeInfo ) {

			// ----------------------------------------------------------------

			switch ( pNodeInfo->m_NodeType ) {

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::NAMED_SPOT:
				bEnable = TRUE;
				break;

			case CAnimationSpotView::CNodeInfo::NODE_TYPE::SPOT_ADJUSTMENT:
				bEnable = TRUE;
				break;

			}

		}

	}

	pCmdUI->Enable( bEnable );
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::OnModifyNamedSpot()
//

void CBpaintDoc::OnModifyNamedSpot() 
{
	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	m_bRenderLinkPointOverlays = true;
	m_bSetAnimLinkPoint = false;
	m_bSetFrameLinkPoint = false;
	
	if ( (SPOT_EDIT_NONE == m_eSetNamedSpotMode) || 
		(SPOT_EDIT_ADJUSTMENT == m_eSetNamedSpotMode) ) {

		m_eSetNamedSpotMode = SPOT_EDIT_ANIM_SPOT;

	} else {

		m_eSetNamedSpotMode = SPOT_EDIT_NONE;

	}

	ForceFullRedraw();
}

//
//	CBpaintDoc::OnUpdateModifyNamedSpot()
//

void CBpaintDoc::OnUpdateModifyNamedSpot(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pCurrentNamedSpot );
	pCmdUI->SetCheck( SPOT_EDIT_ANIM_SPOT == m_eSetNamedSpotMode ? 1 : 0 );
}

//
//	CBpaintDoc::OnModifySpotAdjustment()
//

void CBpaintDoc::OnModifySpotAdjustment() 
{
	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	m_bRenderLinkPointOverlays = true;
	m_bSetAnimLinkPoint = false;
	m_bSetFrameLinkPoint = false;

	if ( (SPOT_EDIT_NONE == m_eSetNamedSpotMode) ||
		(SPOT_EDIT_ANIM_SPOT == m_eSetNamedSpotMode) ) {

		m_eSetNamedSpotMode = SPOT_EDIT_ADJUSTMENT;

	} else {

		m_eSetNamedSpotMode = SPOT_EDIT_NONE;

	}

	ForceFullRedraw();
}

//
//	CBpaintDoc::OnUpdateModifySpotAdjustment()
//

void CBpaintDoc::OnUpdateModifySpotAdjustment(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pCurrentNamedSpot );
	pCmdUI->SetCheck( SPOT_EDIT_ADJUSTMENT == m_eSetNamedSpotMode ? 1 : 0 );
}

// ----------------------------------------------------------------------------
