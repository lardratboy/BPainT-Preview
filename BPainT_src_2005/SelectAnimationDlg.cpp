// SelectAnimationDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "BPaintDoc.h"
#include "SelectAnimationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectAnimationDlg dialog


CSelectAnimationDlg::CSelectAnimationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectAnimationDlg::IDD, pParent)
{
	m_pDoc = 0;
	m_pAnimation = 0;
	m_pShowcase = 0;

	//{{AFX_DATA_INIT(CSelectAnimationDlg)
	m_bRemoveUnused = TRUE;
	//}}AFX_DATA_INIT
}


void CSelectAnimationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectAnimationDlg)
	DDX_Control(pDX, IDC_ANIMATION_LIST, m_AnimationListBox);
	DDX_Check(pDX, IDC_REMOVE_UNUSED, m_bRemoveUnused);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectAnimationDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectAnimationDlg)
	ON_BN_CLICKED(IDC_PREV_FRAME, OnPrevFrame)
	ON_BN_CLICKED(IDC_NEXT_FRAME, OnNextFrame)
	ON_LBN_SELCHANGE(IDC_ANIMATION_LIST, OnSelchangeAnimationList)
	ON_BN_CLICKED(IDC_PREV_LAYER, OnPrevLayer)
	ON_BN_CLICKED(IDC_NEXT_LAYER, OnNextLayer)
	ON_LBN_DBLCLK(IDC_ANIMATION_LIST, OnDblclkAnimationList)
	ON_BN_CLICKED(IDC_COPY_ANIM, OnCopyAnim)
	ON_BN_CLICKED(IDC_DELETE_ANIM, OnDeleteAnim)
	ON_BN_CLICKED(IDC_PROPERTIES, OnProperties)
	ON_BN_CLICKED(IDC_MAKE_ELEMENTS, OnMakeElements)
	ON_BN_CLICKED(IDC_REMOVE_UNUSED, OnRemoveUnused)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectAnimationDlg message handlers

void CSelectAnimationDlg::OnPrevFrame() 
{
	if ( m_pDoc ) {

		m_pDoc->Request_PrevFrame();

	}
}

void CSelectAnimationDlg::OnNextFrame() 
{
	if ( m_pDoc ) {

		m_pDoc->Request_NextFrame();

	}
}

void CSelectAnimationDlg::OnSelchangeAnimationList() 
{
	int item = m_AnimationListBox.GetCurSel();

	if ( (LB_ERR != item) && m_pDoc ) {

		BPT::CAnimation * pAnim = (BPT::CAnimation *)m_AnimationListBox.GetItemDataPtr( item );

		if ( ((BPT::CAnimation *)-1) != pAnim ) {

			m_pDoc->Request_AnimationChange( pAnim );

			m_pAnimation = pAnim;	// RONG: set so RefreshAnimation will select correct animation

		}

	}

}

// RONG: Added so properties button can refresh

BOOL CSelectAnimationDlg::RefreshAnimList()
{
	m_AnimationListBox.ResetContent();
	
	if ( m_pShowcase ) {

		int animCount = m_pShowcase->AnimationCount();

		BPT::CAnimation * pFirst = m_pShowcase->FirstAnimation();

		BPT::CAnimation * pAnim = pFirst;

		for ( int i = 0; i < animCount; i++ ) {

			int item = m_AnimationListBox.AddString( pAnim->GetName() );

			if ( LB_ERR == item ) {

				return FALSE;

			}

			m_AnimationListBox.SetItemData( item, (DWORD)pAnim );

			if ( pAnim == m_pAnimation ) {

				m_AnimationListBox.SetCurSel( item );

			}

			pAnim = m_pShowcase->NextAnimation( pAnim );

			if ( pAnim == pFirst ) {

				break;

			}

		}
	}

	return TRUE;
}

BOOL CSelectAnimationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	if (RefreshAnimList() == FALSE) {

		return FALSE;

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectAnimationDlg::OnPrevLayer() 
{
	if ( m_pDoc ) {

		m_pDoc->Request_PrevLayer();

	}
}

void CSelectAnimationDlg::OnNextLayer() 
{
	if ( m_pDoc ) {

		m_pDoc->Request_NextLayer();

	}
}

void CSelectAnimationDlg::OnDblclkAnimationList() 
{
	CSelectAnimationDlg::OnOK();
}

// RONG: Added
void CSelectAnimationDlg::OnCopyAnim() 
{
	if (m_pAnimation) {

		m_pShowcase->CopyAnimation( m_pAnimation );
		
		RefreshAnimList();

	}
}

// RONG: Added
void CSelectAnimationDlg::OnDeleteAnim() 
{
	CString text;

	if (m_pShowcase->AnimationCount() == 1) {

		MessageBox("You can not delete the last animation.", "Hey!", MB_OK);

		return;

	}
	
	if (m_pAnimation) {

		text.Format("Are you sure you want to delete the animation %s?", m_pAnimation->GetName());

		if (MessageBox(text, "Huh?", MB_YESNO) == IDYES) {

			m_pShowcase->UnattachAnimation( m_pAnimation );

			delete m_pAnimation;

			m_pAnimation = m_pShowcase->FirstAnimation();

			m_pDoc->Request_AnimationChange( m_pAnimation );

			RefreshAnimList();

		}

	}
}

// RONG: Added
void CSelectAnimationDlg::OnProperties() 
{
	
	m_pDoc->Request_AnimationProperties( this ); // BPT 6/5/02

	RefreshAnimList();

}

void CSelectAnimationDlg::OnMakeElements() 
{

#if 1 // BPT 5/23/01

#if 1 // BPT 6/1/01

	if ( m_bRemoveUnused ) {

		m_pShowcase->ReleaseUnusedVisualElements();

	}

#endif

	if ( m_pShowcase->VisualElementCount() ) {

		if ( MessageBox( "Create an animation that contains every visual element?"
			" (BTW: This is useful for finding/fixing similar images...) ",
			"Are you sure?", MB_YESNO ) ) {

	
			// Use the time/date info for the name of the animation
			// --------------------------------------------------------------------
	
			CTime theTime = CTime::GetCurrentTime();
	
			CString rightNow;
	
			rightNow.Format(
				"- Visual Elements on %d/%d @ (%d:%d) -",
				theTime.GetMonth(),
				theTime.GetDay(),
				theTime.GetHour(),
				theTime.GetMinute()
			);
	
			// Ask the showcase to build the animation
			// --------------------------------------------------------------------
	
			if ( !m_pShowcase->CreateVisualElementsAnim( rightNow ) ) {
	
				MessageBox("Failed to build animation.", "Argh!", MB_ICONERROR | MB_OK );
	
			}

			m_pDoc->SetModifiedFlag();
	
			RefreshAnimList();
	
		}

	}

#endif

}


void CSelectAnimationDlg::OnRemoveUnused() 
{
	m_bRemoveUnused = (0 != IsDlgButtonChecked(IDC_REMOVE_UNUSED)) ? TRUE : FALSE;
	// TODO: Add your control notification handler code here
	
}
