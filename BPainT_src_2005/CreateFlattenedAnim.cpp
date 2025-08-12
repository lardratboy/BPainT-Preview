// CreateFlattenedAnim.cpp : implementation file
//

#include "stdafx.h"
#include "bpaint.h"
#include "CreateFlattenedAnim.h"


// CCreateFlattenedAnim dialog

IMPLEMENT_DYNAMIC(CCreateFlattenedAnim, CDialog)
CCreateFlattenedAnim::CCreateFlattenedAnim(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateFlattenedAnim::IDD, pParent)
	, m_Name(_T(""))
{
}

CCreateFlattenedAnim::~CCreateFlattenedAnim()
{
}

void CCreateFlattenedAnim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME_EDIT, m_Name);
	DDX_Control(pDX, IDC_SELECTED_LAYERS, m_SelectedLayersListBox);
}


BEGIN_MESSAGE_MAP(CCreateFlattenedAnim, CDialog)
END_MESSAGE_MAP()


// CCreateFlattenedAnim message handlers
