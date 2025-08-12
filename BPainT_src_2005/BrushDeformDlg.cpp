// BrushDeformDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "BrushDeformDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrushDeformDlg dialog


CBrushDeformDlg::CBrushDeformDlg(CWnd* pParent /*=NULL*/, POINT * pQuad )
	: CDialog(CBrushDeformDlg::IDD, pParent), m_pQuad( pQuad )
{
	//{{AFX_DATA_INIT(CBrushDeformDlg)
	m_x1 = 0;
	m_x2 = 0;
	m_x3 = 0;
	m_x4 = 0;
	m_y3 = 0;
	m_y1 = 0;
	m_y2 = 0;
	m_y4 = 0;
	m_originalSizeMsg = _T("");
	m_outputSizeMsg = _T("");
	//}}AFX_DATA_INIT
}


void CBrushDeformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrushDeformDlg)
	DDX_Text(pDX, IDC_X1_EDIT, m_x1);
	DDX_Text(pDX, IDC_X2_EDIT, m_x2);
	DDX_Text(pDX, IDC_X3_EDIT, m_x3);
	DDX_Text(pDX, IDC_X4_EDIT, m_x4);
	DDX_Text(pDX, IDC_Y3_EDIT, m_y3);
	DDX_Text(pDX, IDC_Y1_EDIT, m_y1);
	DDX_Text(pDX, IDC_Y2_EDIT, m_y2);
	DDX_Text(pDX, IDC_Y4_EDIT, m_y4);
	DDX_Text(pDX, IDC_ORIGINAL_SIZE_STATIC, m_originalSizeMsg);
	DDX_Text(pDX, IDC_OUTPUT_SIZE_STATIC, m_outputSizeMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrushDeformDlg, CDialog)
	//{{AFX_MSG_MAP(CBrushDeformDlg)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_X1_EDIT, OnChangeX)
	ON_EN_CHANGE(IDC_Y1_EDIT, OnChangeX)
	ON_EN_CHANGE(IDC_X2_EDIT, OnChangeX)
	ON_EN_CHANGE(IDC_Y2_EDIT, OnChangeX)
	ON_EN_CHANGE(IDC_X3_EDIT, OnChangeX)
	ON_EN_CHANGE(IDC_Y3_EDIT, OnChangeX)
	ON_EN_CHANGE(IDC_X4_EDIT, OnChangeX)
	ON_EN_CHANGE(IDC_Y4_EDIT, OnChangeX)
END_MESSAGE_MAP()

// ----------------------------------------------------------------------------

//
//	DeformationDialog()
//

bool DeformationDialog(
	CWnd * pWnd, const char * title, POINT * pQuad,
	const char * pSettingsStorage /* = 0 */
)
{
	CBrushDeformDlg deformDlg( (pWnd) ? pWnd : AfxGetMainWnd(), pQuad );

	// retrieve settings...

	if ( IDOK == deformDlg.DoModal() ) {

		pQuad[ 0 ].x = deformDlg.m_x1;
		pQuad[ 0 ].y = deformDlg.m_y1;

		pQuad[ 1 ].x = deformDlg.m_x2;
		pQuad[ 1 ].y = deformDlg.m_y2;

		pQuad[ 2 ].x = deformDlg.m_x3;
		pQuad[ 2 ].y = deformDlg.m_y3;

		pQuad[ 3 ].x = deformDlg.m_x4;
		pQuad[ 3 ].y = deformDlg.m_y4;

		// store settings...

		return true;

	}

	return false;

}

//
//	CBrushDeformDlg::RestoreSettingsFromQuad()
//

void
CBrushDeformDlg::RestoreSettingsFromQuad()
{
	if ( m_pQuad ) {

		m_CurrentQuad[ 0 ] = m_pQuad[ 0 ];
		m_x1 = m_pQuad[ 0 ].x;
		m_y1 = m_pQuad[ 0 ].y;

		m_CurrentQuad[ 1 ] = m_pQuad[ 1 ];
		m_x2 = m_pQuad[ 1 ].x;
		m_y2 = m_pQuad[ 1 ].y;

		m_CurrentQuad[ 2 ] = m_pQuad[ 2 ];
		m_x3 = m_pQuad[ 2 ].x;
		m_y3 = m_pQuad[ 2 ].y;

		m_CurrentQuad[ 3 ] = m_pQuad[ 3 ];
		m_x4 = m_pQuad[ 3 ].x;
		m_y4 = m_pQuad[ 3 ].y;

	}

	SyncStatusMessages();

	// lazy

	UpdateData( false );
}

void 
CBrushDeformDlg::BuildMsgFromQuad(
	CString & msg, const char * prefix, const POINT * pQuad
)
{
	// find the bounding rect.

	CRect boundingRect( INT_MAX, INT_MAX, INT_MIN, INT_MIN );

	for ( int i = 0; i < 4; i++ ) {

		if ( pQuad[ i ].x < boundingRect.left ) boundingRect.left = pQuad[ i ].x;
		if ( pQuad[ i ].y < boundingRect.top ) boundingRect.top = pQuad[ i ].y;
		if ( pQuad[ i ].x > boundingRect.right ) boundingRect.right = pQuad[ i ].x;
		if ( pQuad[ i ].y > boundingRect.bottom ) boundingRect.bottom = pQuad[ i ].y;

	}

	msg.Format(
		"%s width %d height %d", prefix, 
		boundingRect.Width(), boundingRect.Height()
	);

}

//
//	CBrushDeformDlg::SyncStatusMessages()
//

void
CBrushDeformDlg::SyncStatusMessages()
{
	BuildMsgFromQuad( m_originalSizeMsg, "Original", m_pQuad );
	BuildMsgFromQuad( m_outputSizeMsg, "Output", m_CurrentQuad );
}

//
//	CBrushDeformDlg::RefreshQuadFromValues()
//

void 
CBrushDeformDlg::RefreshQuadFromValues()
{
	UpdateData( true );

	m_CurrentQuad[ 0 ].x = m_x1;
	m_CurrentQuad[ 0 ].y = m_y1;

	m_CurrentQuad[ 1 ].x = m_x2;
	m_CurrentQuad[ 1 ].y = m_y2;

	m_CurrentQuad[ 2 ].x = m_x3;
	m_CurrentQuad[ 2 ].y = m_y3;

	m_CurrentQuad[ 3 ].x = m_x4;
	m_CurrentQuad[ 3 ].y = m_y4;

	SyncStatusMessages();
	UpdateData( false );

}

/////////////////////////////////////////////////////////////////////////////
// CBrushDeformDlg message handlers


BOOL CBrushDeformDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RestoreSettingsFromQuad();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBrushDeformDlg::OnChangeX() 
{
	RefreshQuadFromValues();
}
