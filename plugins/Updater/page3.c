/*
 * Copyright (c) 2022 Winsider Seminars & Solutions, Inc.  All rights reserved.
 *
 * This file is part of System Informer.
 *
 * Authors:
 *
 *     dmex    2016-2019
 *
 */

#include "updater.h"

static TASKDIALOG_BUTTON TaskDialogButtonArray[] =
{
    { IDOK, L"下载" }
};

HRESULT CALLBACK ShowAvailableCallbackProc(
    _In_ HWND hwndDlg,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    _In_ LONG_PTR dwRefData
    )
{
    PPH_UPDATER_CONTEXT context = (PPH_UPDATER_CONTEXT)dwRefData;

    switch (uMsg)
    {
    case TDN_NAVIGATED:
        PhSetEvent(&InitializedEvent);
        break;
    case TDN_BUTTON_CLICKED:
        {
            if ((INT)wParam == IDOK)
            {
                ShowProgressDialog(context);
                return S_FALSE;
            }
        }
        break;
    case TDN_HYPERLINK_CLICKED:
        {
            TaskDialogLinkClicked(context);
            return S_FALSE;
        }
        break;
    }

    return S_OK;
}

VOID ShowAvailableDialog(
    _In_ PPH_UPDATER_CONTEXT Context
    )
{
    TASKDIALOGCONFIG config;

    memset(&config, 0, sizeof(TASKDIALOGCONFIG));
    config.cbSize = sizeof(TASKDIALOGCONFIG);
    config.dwFlags = TDF_USE_HICON_MAIN | TDF_ALLOW_DIALOG_CANCELLATION | TDF_CAN_BE_MINIMIZED | TDF_ENABLE_HYPERLINKS;
    config.dwCommonButtons = TDCBF_CANCEL_BUTTON;
    config.hMainIcon = PhGetApplicationIcon(FALSE);
    config.cxWidth = 200;
    config.pButtons = TaskDialogButtonArray;
    config.cButtons = RTL_NUMBER_OF(TaskDialogButtonArray);
    config.lpCallbackData = (LONG_PTR)Context;
    config.pfCallback = ShowAvailableCallbackProc;

    config.pszWindowTitle = L"System Informer - 更新工具";
    config.pszMainInstruction = L"检查到一个可用的更新。";
    config.pszContent = PhaFormatString(L"版本: %s\r\n下载大小: %s\r\n\r\n<A HREF=\"changelog.txt\">查看更新日志</A>",
        PhGetStringOrEmpty(Context->Version),
        PhGetStringOrEmpty(Context->SetupFileLength)
        )->Buffer;

    TaskDialogNavigatePage(Context->DialogHandle, &config);
}
