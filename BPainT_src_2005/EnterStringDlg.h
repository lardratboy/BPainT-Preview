// EnterStringDlg.h : header file
//
//	Copyright (c) 2002, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#pragma once


// CEnterStringDlg dialog

class CEnterStringDlg : public CDialog
{
	DECLARE_DYNAMIC(CEnterStringDlg)

public:
	CEnterStringDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEnterStringDlg();

// Dialog Data
	enum { IDD = IDD_ENTER_STRING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_DescriptionString;
	CString m_EditString;
	CString m_Title;
	virtual BOOL OnInitDialog();
};
