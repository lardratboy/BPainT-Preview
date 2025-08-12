#include "stdafx.h"
#include "..\resource.h"
#include "WindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindowDlg dialog


CWindowDlg::CWindowDlg(CMDIFrameWnd * pMDIFrame)
	: CDialog(CWindowDlg::IDD, pMDIFrame)
{
	m_pMDIFrame=pMDIFrame;
	//{{AFX_DATA_INIT(CWindowDlg)
	//}}AFX_DATA_INIT
}

CWindowDlg::CWindowDlg(CMDIFrameWnd * pMDIFrame,CWnd * pParentWnd)
	: CDialog(CWindowDlg::IDD, pParentWnd)
{
	m_pMDIFrame=pMDIFrame;
	//{{AFX_DATA_INIT(CWindowDlg)
	//}}AFX_DATA_INIT
}


void CWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWindowDlg)
	DDX_Control(pDX, IDC_LIST, m_wndList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWindowDlg, CDialog)
	//{{AFX_MSG_MAP(CWindowDlg)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelChange)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_ACTIVATE, OnActivate)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_TILEHORZ, OnTileHorz)
	ON_BN_CLICKED(IDC_MINIMIZE, OnMinimize)
	ON_BN_CLICKED(IDC_TILEVERT, OnTileVert)
	ON_BN_CLICKED(IDC_CASCADE, OnCascade)
	ON_LBN_DBLCLK(IDC_LIST, OnActivate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindowDlg message handlers
				 
BOOL CWindowDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillWindowList(); 	
	SelActive();
	UpdateButtons();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWindowDlg::OnClose() 
{
	int		nItems=m_wndList.GetCount();
	if(nItems!=LB_ERR && nItems>0)
	{
		HWND	hMDIClient=m_pMDIFrame->m_hWndMDIClient;
		
		m_wndList.SetRedraw(FALSE);

		for(int i=nItems-1;i>=0;i--)
		{
			if(m_wndList.GetSel(i)>0)
			{					   
				HWND hWnd=(HWND) m_wndList.GetItemData(i);
				::SendMessage(hWnd,WM_CLOSE,(WPARAM) 0, (LPARAM) 0);
				if(::GetParent(hWnd)==hMDIClient) break;
			}				  
		}
		m_wndList.SetRedraw(TRUE);
	}
	FillWindowList();
	SelActive();
	UpdateButtons();
}

void CWindowDlg::OnSelChange() 
{
	UpdateButtons();
}
// Enables/Disables states of buttons
void CWindowDlg::UpdateButtons()
{						   
	int	nSel=m_wndList.GetSelCount();

	GetDlgItem(IDC_CLOSE)->EnableWindow(nSel>0);	
	GetDlgItem(IDC_SAVE)->EnableWindow(nSel>0);
	GetDlgItem(IDC_TILEHORZ)->EnableWindow(nSel>=2);
	GetDlgItem(IDC_TILEVERT)->EnableWindow(nSel>=2);
	GetDlgItem(IDC_CASCADE)->EnableWindow(nSel>=2);
	GetDlgItem(IDC_MINIMIZE)->EnableWindow(nSel>0);

	GetDlgItem(IDC_ACTIVATE)->EnableWindow(nSel==1);
}

// Selects currently active window in listbox
void CWindowDlg::SelActive()
{
	int		nItems=m_wndList.GetCount();
	if(nItems != LB_ERR && nItems>0)
	{
	
		m_wndList.SetRedraw(FALSE);
		m_wndList.SelItemRange(FALSE,0,nItems-1);
		
		HWND	hwndActive=(HWND) ::SendMessage(m_pMDIFrame->m_hWndMDIClient,WM_MDIGETACTIVE,0,0);
	
		for(int i=0;i<nItems;i++) {
			if((HWND) m_wndList.GetItemData(i)==hwndActive)  {
				m_wndList.SetSel(i);
				break;
			}
		}
		m_wndList.SetRedraw(TRUE);
	}
}

// Saves selected documents
void CWindowDlg::OnSave() 
{
	int		nItems=m_wndList.GetCount();
	if(nItems!=LB_ERR && nItems>0)
	{
		HWND	hMDIClient=m_pMDIFrame->m_hWndMDIClient;
		
		for(int i=0;i<nItems;i++)
		{
			if(m_wndList.GetSel(i)>0)
			{
				HWND		hWnd=(HWND) m_wndList.GetItemData(i);
				CWnd		*pWnd=CWnd::FromHandle(hWnd);
				CFrameWnd	*pFrame=DYNAMIC_DOWNCAST(CFrameWnd,pWnd);
				if(pFrame != NULL)
				{
					CDocument *pDoc=pFrame->GetActiveDocument();
					if(pDoc != NULL)	pDoc->SaveModified();
				}
			}
		}
	}

	FillWindowList();
	SelActive();
	UpdateButtons();
}

void CWindowDlg::OnActivate() 
{
	if(m_wndList.GetSelCount()==1)
	{
		int		index;	
		if(m_wndList.GetSelItems(1,&index)==1)
		{						 
			DWORD dw=m_wndList.GetItemData(index);
			if(dw!=LB_ERR)
			{
				
				WINDOWPLACEMENT	wndpl;
				::GetWindowPlacement((HWND) dw,&wndpl);
				if(wndpl.showCmd == SW_SHOWMINIMIZED) ::ShowWindow((HWND) dw,SW_RESTORE);
				::SendMessage(m_pMDIFrame->m_hWndMDIClient,WM_MDIACTIVATE,(WPARAM) dw,0);
				EndDialog(IDOK);
			}
		}
	}
}

// Refresh windows list
void CWindowDlg::FillWindowList(void)
{
	m_wndList.SetRedraw(FALSE);
	m_wndList.ResetContent();
	HWND hwndT;
	hwndT=::GetWindow(m_pMDIFrame->m_hWndMDIClient, GW_CHILD);
	while (hwndT != NULL)
	{
		TCHAR	szWndTitle[256];
		::GetWindowText(hwndT,szWndTitle,sizeof(szWndTitle)/sizeof(szWndTitle[0]));

		int index=m_wndList.AddString(szWndTitle);
		m_wndList.SetItemData(index,(DWORD) hwndT);
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
	}
	m_wndList.SetRedraw(TRUE);
}

// Draws listbox item
void CWindowDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS) 
{
	
	if(nIDCtl == IDC_LIST)
	{
		if (lpDIS->itemID == LB_ERR) return; 
  
		HBRUSH		brBackground;
		RECT		rcTemp = lpDIS->rcItem; 
		HDC			hDC=lpDIS->hDC;

		COLORREF	clText; 

		if (lpDIS->itemState & ODS_SELECTED)  { 
			brBackground = GetSysColorBrush (COLOR_HIGHLIGHT); 
			clText = GetSysColor (COLOR_HIGHLIGHTTEXT); 
		} else { 
			brBackground = GetSysColorBrush (COLOR_WINDOW); 
			clText = GetSysColor (COLOR_WINDOWTEXT); 
		} 

		if (lpDIS->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) 	FillRect(hDC,&rcTemp, brBackground); 
	
		int			OldBkMode=::SetBkMode(hDC,TRANSPARENT); 
		COLORREF	clOldText=::SetTextColor(hDC,clText); 

		TCHAR		szBuf[1024];
		::SendMessage(lpDIS->hwndItem,LB_GETTEXT,(WPARAM) lpDIS->itemID,(LPARAM) szBuf);		
		
		int h=rcTemp.bottom-rcTemp.top;
		rcTemp.left+=h+4;
		DrawText(hDC,szBuf,-1,&rcTemp,DT_LEFT|DT_VCENTER|	
			DT_NOPREFIX| DT_SINGLELINE);

		HICON	hIcon=(HICON) ::GetClassLong((HWND) lpDIS->itemData,GCL_HICONSM);/*AfxGetApp()->LoadStandardIcon(IDI_HAND);*///(HICON) ::SendMessage((HWND) lpDIS->itemData,WM_GETICON,(WPARAM)ICON_BIG,(LPARAM) 0);
		rcTemp.left=lpDIS->rcItem.left;
		::DrawIconEx(hDC,rcTemp.left+2,rcTemp.top,			
			hIcon,h,h,0,0,DI_NORMAL);		
		
		::SetTextColor(hDC,clOldText);
		::SetBkMode(hDC,OldBkMode);
  
		if(lpDIS->itemAction & ODA_FOCUS)   DrawFocusRect(hDC,&lpDIS->rcItem); 
		return;		
	}
	CDialog::OnDrawItem(nIDCtl, lpDIS);
}

void CWindowDlg::MDIMessage(UINT uMsg,WPARAM flag)
{
	int		nItems=m_wndList.GetCount();
	if(nItems!=LB_ERR && nItems>0)
	{
		HWND	hMDIClient=m_pMDIFrame->m_hWndMDIClient;
		::LockWindowUpdate(hMDIClient);
		for(int i=nItems-1;i>=0;i--)
		{
			HWND hWnd=(HWND) m_wndList.GetItemData(i);
			if(m_wndList.GetSel(i)>0)	::ShowWindow(hWnd,SW_RESTORE);
			else						::ShowWindow(hWnd,SW_MINIMIZE);
		}
		::SendMessage(hMDIClient,uMsg, flag,0);	
		::LockWindowUpdate(NULL);
	}
}

void CWindowDlg::OnTileHorz() 
{
	MDIMessage(WM_MDITILE,MDITILE_HORIZONTAL);	
}

void CWindowDlg::OnTileVert() 
{
	MDIMessage(WM_MDITILE,MDITILE_VERTICAL);	
}

void CWindowDlg::OnMinimize() 
{
	int		nItems=m_wndList.GetCount();
	if(nItems!=LB_ERR && nItems>0)
	{
		HWND	hMDIClient=m_pMDIFrame->m_hWndMDIClient;
		
		m_wndList.SetRedraw(FALSE);

		for(int i=nItems-1;i>=0;i--)
		{
			if(m_wndList.GetSel(i)>0)
			{
				HWND hWnd=(HWND) m_wndList.GetItemData(i);
				::ShowWindow(hWnd,SW_MINIMIZE);
			}
		}
		m_wndList.SetRedraw(TRUE);
	}

	FillWindowList();
	SelActive();
	UpdateButtons();
}

void CWindowDlg::OnCascade() 
{
	MDIMessage(WM_MDICASCADE,0);
}

/////////////////////////////////////////////////////////////////////////////
// CMDIClient

CMDIClient::CMDIClient()
{
	m_hMenuWindow = 0;
	m_strWindows.LoadString(IDS_WINDOW_MANAGE);
}

CMDIClient::~CMDIClient()
{
}


BEGIN_MESSAGE_MAP(CMDIClient, CWnd)
	//{{AFX_MSG_MAP(CMDIClient)
	ON_MESSAGE(WM_MDIREFRESHMENU,OnRefreshMenu)
	ON_MESSAGE(WM_MDISETMENU,OnSetMenu)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMDIClient message handlers
LRESULT CMDIClient::OnSetMenu(WPARAM wParam,LPARAM lParam)
{
	LRESULT lResult=Default();
	
	// Remember Window submenu handle
	m_hMenuWindow=(HMENU) lParam;
	
	// Refresh window submenu
	SendMessage(WM_MDIREFRESHMENU);
	return lResult;
}

LRESULT CMDIClient::OnRefreshMenu(WPARAM wParam,LPARAM lParam)
{
	LRESULT lResult=Default();

	HMENU hMenu=m_hMenuWindow;

	if(hMenu != NULL)
	{
		UINT uState=::GetMenuState(hMenu,AFX_IDM_FIRST_MDICHILD+9,MF_BYCOMMAND);
		if(uState ==  0xFFFFFFFF) uState=0;

		// Remove old MDI 'Windows...' command
		while(::RemoveMenu(hMenu,AFX_IDM_FIRST_MDICHILD+9,MF_BYCOMMAND)) ;

		// Remove 'Windows' command
		while(::RemoveMenu(hMenu,ID_WINDOW_MANAGE,MF_BYCOMMAND)) ;
		
		::AppendMenu(hMenu,MF_STRING|uState,ID_WINDOW_MANAGE,m_strWindows);
	}

	return lResult;
}


void CMDIClient::ManageWindows(CMDIFrameWnd *pMDIFrame)
{
	CWindowDlg	dlg(pMDIFrame);
	dlg.DoModal();
}

void CMDIClient::ManageWindows(CMDIFrameWnd *pMDIFrame, CWnd * pParentWnd)
{
	CWindowDlg	dlg(pMDIFrame,pParentWnd);
	dlg.DoModal();
}

#if 1 // 1-13-01 BPT

//
//	CMDIClient::OnSetCursor(
//

BOOL CMDIClient::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default

	if ( !IsWindowEnabled() ) {

		return GLOBAL_StencilHackOnSetCursor( pWnd, nHitTest, message );

	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

#endif