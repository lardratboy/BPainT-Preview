// RememberSizeToolbar.cpp : implementation file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "RememberSizeToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRememberSizeToolbar

CRememberSizeToolbar::CRememberSizeToolbar() : m_bRememberSizeWhenDocked( true )
{
}

CRememberSizeToolbar::~CRememberSizeToolbar()
{
}


BEGIN_MESSAGE_MAP(CRememberSizeToolbar, CToolBar)
	//{{AFX_MSG_MAP(CRememberSizeToolbar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRememberSizeToolbar message handlers

CSize CRememberSizeToolbar::CalcDynamicLayout (int nLength, DWORD nMode) {

	if ( m_bRememberSizeWhenDocked && (nMode & LM_VERTDOCK)) {

		return CToolBar::CalcDynamicLayout(
			nLength, (nMode & ~LM_VERTDOCK) | (LM_MRUWIDTH)
		);

	}

	return CToolBar::CalcDynamicLayout(nLength, nMode);

}

