// (c) Eric Korff de Gidts
// SPDX-License-Identifier: MIT
// HidHideClient.cpp
#include "stdafx.h"
#include "HidHideClient.h"
#include "HidHideClientDlg.h"
#include "HidHideApi.h"
#include "Logging.h"

CHidHideClientApp theApp;

BEGIN_MESSAGE_MAP(CHidHideClientApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

namespace
{
    HHOOK s_hHook;

    // Alter message box labels and detach from the window activiation notification
    LRESULT CALLBACK LocalizedMessageBoxCBTProc(_In_ INT code, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        // Act during window activitation
        if (HCBT_ACTIVATE == code)
        {
            TRACE_ALWAYS(L"");
            auto dlg{ reinterpret_cast<HWND>(wParam) };

            // Alter labels
            if (nullptr != ::GetDlgItem(dlg, IDOK))     ::SetDlgItemTextW(dlg, IDOK,     HidHide::StringTable(IDS_STATIC_MESSAGEBOX_OK).c_str());
            if (nullptr != ::GetDlgItem(dlg, IDCANCEL)) ::SetDlgItemTextW(dlg, IDCANCEL, HidHide::StringTable(IDS_STATIC_MESSAGEBOX_CANCEL).c_str());
            if (nullptr != ::GetDlgItem(dlg, IDRETRY))  ::SetDlgItemTextW(dlg, IDRETRY,  HidHide::StringTable(IDS_STATIC_MESSAGEBOX_RETRY).c_str());
            if (nullptr != ::GetDlgItem(dlg, IDIGNORE)) ::SetDlgItemTextW(dlg, IDIGNORE, HidHide::StringTable(IDS_STATIC_MESSAGEBOX_IGNORE).c_str());
            if (nullptr != ::GetDlgItem(dlg, IDABORT))  ::SetDlgItemTextW(dlg, IDABORT,  HidHide::StringTable(IDS_STATIC_MESSAGEBOX_ABORT).c_str());
            if (nullptr != ::GetDlgItem(dlg, IDYES))    ::SetDlgItemTextW(dlg, IDYES,    HidHide::StringTable(IDS_STATIC_MESSAGEBOX_YES).c_str());
            if (nullptr != ::GetDlgItem(dlg, IDNO))     ::SetDlgItemTextW(dlg, IDNO,     HidHide::StringTable(IDS_STATIC_MESSAGEBOX_NO).c_str());

            // Fire-once so detach again
            ::UnhookWindowsHookEx(s_hHook);
        }

        // Allow other hooks to act too
        ::CallNextHookEx(s_hHook, code, wParam, lParam);
        return (0);
    }

    // Show message box with localized buttons
    INT WINAPI LocalizedMessageBox(_In_ UINT resourceId, _In_ UINT type)
    {
        TRACE_ALWAYS(L"");

        // Attach hook (fire-once)
        s_hHook = ::SetWindowsHookExW(WH_CBT, &LocalizedMessageBoxCBTProc, 0, ::GetCurrentThreadId());
        return (::MessageBoxExW(::AfxGetApp()->GetMainWnd()->m_hWnd, HidHide::StringTable(resourceId).c_str(), HidHide::StringTable(IDS_DIALOG_APPLICATION).c_str(), type, LANG_USER_DEFAULT));
    }
}

CHidHideClientApp::CHidHideClientApp() noexcept
{
    LogRegisterProviders();
}

CHidHideClientApp::~CHidHideClientApp()
{
    LogUnregisterProviders();
}

BOOL CHidHideClientApp::InitInstance()
{
    TRACE_ALWAYS(L"");

    // Initialize the common controls .dll first
    INITCOMMONCONTROLSEX initCommonControlsEx;
    initCommonControlsEx.dwSize = sizeof(initCommonControlsEx);
    initCommonControlsEx.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&initCommonControlsEx);

    // Initialize the application instance
    CWinApp::InitInstance();

    // Initialize COM services
    AfxEnableControlContainer();

    // Create the shell manager, in case the dialog contains any shell tree view or shell list view controls
    std::unique_ptr<CShellManager> const shellManager{ std::make_unique<CShellManager>() };

    // Activate "Windows Native" visual manager for enabling themes in MFC controls
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

    // We can't do anything when the control device isn't present so allow for a retry on failure
    CHidHideClientDlg dlg(nullptr);
    m_pMainWnd = &dlg;
    while (!HidHide::Present())
    {
        TRACE_ALWAYS(L"");
        if (IDRETRY != LocalizedMessageBox(IDS_STATIC_MESSAGEBOX_PRESENT, (MB_RETRYCANCEL | MB_ICONEXCLAMATION)))
        {
            return (FALSE);
        }
    }

    // We use exception handling so catch it at top-level and bail out
    try
    {
        if (-1 == dlg.DoModal()) THROW_WIN32_LAST_ERROR;
    }
    catch (...)
    {
        LOGEXC_AND_CONTINUE;
        LocalizedMessageBox(IDS_STATIC_MESSAGEBOX_EXCEPTION, (MB_OK | MB_ICONERROR));
    }

    // Don't start the application's message pump as we are done already
    return (FALSE);
}
