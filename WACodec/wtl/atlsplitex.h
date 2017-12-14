//write visualfc@gmail.com
//time  2007-12-06
//intention is allow splitter pane[0]/pane[1] not change parent to splitter wnd, its main dlg child wnd.
//this file request fix WTL80 atlsplit.h
//uses:
//  CSplitterWindowEx	m_wndSplit;
//	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//  {
//      ...
//      m_wndSplit.CreateFromWindow(GetDlgItem(IDC_PANEL_STATIC));
//      m_wndSplit.SetSplitterPanes( GetDlgItem(IDC_EDIT1), GetDlgItem(IDC_EDIT2) );
//  }
//

#ifndef __ATLSPLITEX_H__
#define __ATLSPLITEX_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLSPLIT_H__
	#error atlsplitex.h requires atlsplit.h to be included first
#endif

#ifndef __ATLAPP_H__
	#error atlsplitex.h requires atlapp.h to be included first
#endif

#ifndef __ATLWIN_H__
	#error atlsplitex.h requires atlwin.h to be included first
#endif

///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// CSplitterWindowExT<t_bVertical>


//////////////////////////////////////////////////////////////////
// hwo to fix WTL80 atlsplit.h
// file name : atlsplit.h
// Implementation - internal helpers
/*
	void UpdateSplitterLayout()
	{
		if(m_nSinglePane == SPLIT_PANE_NONE && m_xySplitterPos == -1)
			return;

		T* pT = static_cast<T*>(this);
		RECT rect = { 0, 0, 0, 0 };
		if(m_nSinglePane == SPLIT_PANE_NONE)
		{
			if(GetSplitterBarRect(&rect))
				pT->InvalidateRect(&rect);

			for(int nPane = 0; nPane < m_nPanesCount; nPane++)
			{
				if(GetSplitterPaneRect(nPane, &rect))
				{
					if(m_hWndPane[nPane] != NULL)
					{
						//fix: 2007-12-06
						HWND hWndParent = ::GetParent(m_hWndPane[nPane]);
						if (hWndParent != pT->m_hWnd)
						{
							pT->ClientToScreen(&rect);
							CWindow(hWndParent).ScreenToClient(&rect);
						}
						::SetWindowPos(m_hWndPane[nPane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
					}
					else
						pT->InvalidateRect(&rect);
				}
			}
		}
		else
		{
			if(GetSplitterPaneRect(m_nSinglePane, &rect))
			{
				if(m_hWndPane[m_nSinglePane] != NULL)
				{
					//fix: 2007-12-06
					HWND hWndParent = ::GetParent(m_hWndPane[m_nSinglePane]);
					if (hWndParent != pT->m_hWnd)
					{
						pT->ClientToScreen(&rect);
						CWindow(hWndParent).ScreenToClient(&rect);
					}
					::SetWindowPos(m_hWndPane[m_nSinglePane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
				}
				else
					pT->InvalidateRect(&rect);
			}
		}
	}
*/

namespace WTL
{
template <bool t_bVertical = true>
class CSplitterWindowExT : public CSplitterWindowImpl<CSplitterWindowT<t_bVertical>, t_bVertical>
{
public:
	DECLARE_WND_CLASS_EX(_T("WTL_SplitterWindowEx"), CS_DBLCLKS, COLOR_WINDOW)

	typedef CSplitterWindowImpl<CSplitterWindowT<t_bVertical>, t_bVertical>   _baseClass;

	HWND CreateFromID(HWND hWndParent, UINT nDlgItemID)
	{
		return CreateFromWindow(::GetDlgItem(hWndParent,nDlgItemID));
	}

	HWND CreateFromWindow(HWND hWnd)
	{
		CWindow wnd = hWnd;
		CWindow parent = wnd.GetParent();
		UINT nID = wnd.GetDlgCtrlID();
		RECT rc;
		wnd.GetWindowRect(&rc);

		DWORD dwStyle = wnd.GetStyle();
		dwStyle |= (WS_CHILD | WS_VISIBLE);
		DWORD dwExStyle = wnd.GetExStyle();

		parent.ScreenToClient(&rc);

		HWND hThisWnd = Create(parent,rc,NULL,dwStyle,dwExStyle,nID,NULL);
		if (hThisWnd)
		{
			wnd.DestroyWindow();
		}
		return hThisWnd;
	}

public:
	BEGIN_MSG_MAP(CSplitterWindowExT)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		CHAIN_MSG_MAP(_baseClass)
	END_MSG_MAP()

	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_nSinglePane == SPLIT_PANE_NONE)
		{
			if ( m_hWndPane[SPLIT_PANE_LEFT] )
				::ShowWindow( m_hWndPane[SPLIT_PANE_LEFT], (BOOL)wParam);
			if ( m_hWndPane[SPLIT_PANE_RIGHT] )
				::ShowWindow( m_hWndPane[SPLIT_PANE_RIGHT], (BOOL)wParam);
		}
		else
		{
			if ( m_hWndPane[m_nSinglePane] )
				::ShowWindow( m_hWndPane[m_nSinglePane], (BOOL)wParam);
		}
		bHandled = FALSE;
		return 0;
	}
};

typedef CSplitterWindowExT<true>    CSplitterWindowEx;
typedef CSplitterWindowExT<false>   CHorSplitterWindowEx;


}; // namespace WTL

#endif // __ATLSPLITEX_H__