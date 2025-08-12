#pragma once
#include "afxwin.h"


// CCreateFlattenedAnim dialog

class CCreateFlattenedAnim : public CDialog
{
	DECLARE_DYNAMIC(CCreateFlattenedAnim)

public:
	CCreateFlattenedAnim(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCreateFlattenedAnim();

// Dialog Data
	enum { IDD = IDD_CREATE_FLATTENED_ANIM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_Name;
	CListBox m_SelectedLayersListBox;
};
