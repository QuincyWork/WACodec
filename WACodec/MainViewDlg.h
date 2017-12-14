// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "lua.hpp"
#include <atlconv.h>
#include <atlfile.h>


#define WM_BEGIN_LOAD			(WM_USER+1)
#define WM_PROGRESS_LOAD_FILE	(WM_USER+2)

#define ID_MENU_BEGIN			100
#define ID_MENU_OPENAS			(ID_MENU_BEGIN)
#define ID_MENU_PROPERTY		(ID_MENU_BEGIN + 1)
#define ID_MENU_END				ID_MENU_PROPERTY

#define M_LIST_RETURN			(3)
#define M_LIST_FOLDER			(2)
#define M_LIST_FILE				(1)


class CMainViewDlg : 
	public CDialogImpl<CMainViewDlg>, 
	public CUpdateUI<CMainViewDlg>,
	public CMessageFilter, 
	public CIdleHandler
{
public:
	enum { IDD = IDD_MAINVIEWDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);		
	}

	virtual BOOL OnIdle()
	{
		UIUpdateChildWindows();		
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainViewDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainViewDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)		
		//MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
		//MESSAGE_HANDLER(WM_PROGRESS_LOAD_FILE, OnProgressLoad)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)		
		COMMAND_ID_HANDLER(IDC_BTN_OPEN, OnOpenFile)	
		COMMAND_RANGE_HANDLER(ID_MENU_BEGIN, ID_MENU_END, OnContextMenuHandler)				
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)		
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		//_ASSERT(FALSE);

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);

		// init control
		m_ctrlRichText = GetDlgItem(IDC_RICHEDIT);
		_ASSERT(NULL!=m_ctrlRichText.m_hWnd);
		
		m_ctrlStatus = GetDlgItem(IDC_STATUS_TEXT);
		_ASSERT(NULL!=m_ctrlStatus.m_hWnd);
		
		m_ctrlOpenBtn = GetDlgItem(IDC_BTN_OPEN);
		_ASSERT(m_ctrlOpenBtn.m_hWnd);

		m_ctrlSaveBtn = GetDlgItem(IDC_BTN_SHOW);
		_ASSERT(m_ctrlSaveBtn.m_hWnd);

		m_ctrlTitle =  GetDlgItem(IDC_TITLE_TEXT);
		_ASSERT(m_ctrlTitle.m_hWnd);

		m_hBrush = CreateSolidBrush(RGB(255,255,255));
		
		WCHAR szFileName[MAX_PATH] = {0};
		GetModuleFileName(NULL,szFileName,MAX_PATH);
		m_strAppPath = szFileName;
		m_strAppPath = m_strAppPath.Mid(0,m_strAppPath.ReverseFind(_T('\\')));

		return TRUE;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);
		
		if (m_hBrush)
		{
			DeleteObject(m_hBrush);
		}

		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: Add validation code 
		CloseDialog(wID);
		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CloseDialog(IDOK);
		return 0;
	}

	LRESULT OnProgressLoad(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{		
		return 0; 
	}	

	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{		
		CMenu ContextMenu;		
		if (ContextMenu.CreatePopupMenu())
		{
			POINT point;
			::GetCursorPos(&point);
			ContextMenu.InsertMenu(0,MF_STRING,ID_MENU_OPENAS,_T("打开方式(&H)"));			
			ContextMenu.InsertMenu(0,MF_STRING,ID_MENU_PROPERTY,_T("属性(&R)"));
			ContextMenu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, m_hWnd);
			ContextMenu.DestroyMenu();
		}

		return 0;
	}

	LRESULT OnContextMenuHandler(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{				
		return 0;
	}

	LRESULT OnSaveFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{		
		return 0;
	}

	LRESULT OnOpenFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFileDialog OpenDlg(TRUE,0,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT,_T("WA编码字符串(*.*)\0*.*\0"),m_hWnd);
		INT_PTR nRet = OpenDlg.DoModal(m_hWnd);
		if (nRet == IDOK)
		{		
			lua_State *L = luaL_newstate();
			if (L != NULL)
			{
				luaL_openlibs(L);

				CHAR* pszBuffer = NULL;
				CAtlFile hSource;
				HRESULT hr = hSource.Create(OpenDlg.m_szFileName,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
				if (SUCCEEDED(hr))
				{
					ULONGLONG llSize = 0;
					hSource.GetSize(llSize);
					if (llSize > 0)
					{
						pszBuffer = new CHAR[llSize];	
						ZeroMemory(pszBuffer,llSize);
						hSource.Read(pszBuffer,llSize);
					}
				}

				if (pszBuffer==NULL)
				{
					m_ctrlStatus.SetWindowText(_T("打开WA字符串文件失败."));
					return 0;
				}
				
				SetCurrentDirectory(m_strAppPath);
				int nRet = luaL_dofile(L,"WACodec.lua");
				if(LUA_OK == nRet)
				{					
					nRet = lua_getglobal(L,"WADecode");
					if (LUA_OK == nRet)
					{
						lua_pushstring(L,pszBuffer);
						nRet = lua_pcall(L, 1, 1, 0);
					}
				}

				if (LUA_OK == nRet)
				{
					const char* pszDecode = lua_tostring(L, -1);
					m_ctrlRichText.Clear();
					m_ctrlRichText.AppendText(CA2W(pszDecode));
				}
				else
				{
					const char *pszErrorMsg = lua_tostring(L, -1); 
					CString strMessage;
					strMessage.Format(_T("执行LUA错误：%s"),CA2W(pszErrorMsg));
					m_ctrlStatus.SetWindowText(strMessage);
				}
				
				lua_close(L);
			}
		}
		
		return 0;
	}


	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{		
		if (m_ctrlTitle.m_hWnd == (HWND)lParam)
		{
			SetBkColor((HDC)wParam, RGB(255,255,255));
			bHandled = TRUE;
			return (LRESULT)(HBRUSH)m_hBrush;
		}

		return 0;
	}

	/// 辅助函数
	void OnLoadFile()
	{
		if (m_strFilePath.IsEmpty() ||
			!PathFileExists(m_strFilePath))
		{
			return;
		}
	}

	void CloseDialog(int nVal)
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}
	
private:
	
	CString			m_strFilePath;		
	CRichEditCtrl   m_ctrlRichText;	
	CWindow			m_ctrlStatus;
	CWindow			m_ctrlOpenBtn;
	CWindow			m_ctrlSaveBtn;
	CWindow			m_ctrlTitle;
	HBRUSH			m_hBrush;	
	CString			m_strAppPath;
};
