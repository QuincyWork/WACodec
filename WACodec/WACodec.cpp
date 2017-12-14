// GSODPViewer.cpp : main source file for GSODPViewer.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlstr.h>

#include "resource.h"

#include "AboutDlg.h"
#include "MainViewDlg.h"

CAppModule _Module;

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	//_ASSERT(FALSE);
	// Parse CommandLine. Will return FALSE if
	// app was launched with /RegServer, /Unregserver.
	
	// Show MainDlg
	HMODULE hRichDll = LoadLibrary(_T("riched20.dll"));
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);
		
	CMainViewDlg dlgMainView;	
	if(dlgMainView.Create(NULL) == NULL)
	{			
		return 0;
	}
	dlgMainView.ShowWindow(nCmdShow);
	
	int nRet = theLoop.Run();
	_Module.RemoveMessageLoop();

	if(hRichDll)
	{
		FreeLibrary(hRichDll);
		hRichDll= NULL;
	}
	return nRet;
}


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{	
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));
		
	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}