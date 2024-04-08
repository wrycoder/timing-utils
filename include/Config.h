#pragma once

#include <windows.h>

#define DEFAULT_TARGET_DURATION   (60 * 56)
#define APP_CONFIG_SUBKEY         TEXT("Software\\SmoothTransition\\TR")
#define APP_CONFIG_BASEDIR        TEXT("BASE DIRECTORY")
#define REGISTRY_UPDATE_ERROR     0x20
#define REGISTRY_CLOSURE_ERROR    0x25
#define REGISTRY_QUERY_ERROR      0x30

/* Directory-selection procedure for our "configuration" dialog */
INT_PTR CALLBACK ConfigurationDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* Show our "configuration" dialog */
void ShowConfigurationDialog(HWND owner);

LRESULT SetBaseDirectory(HKEY hKey, HWND hWnd); 
