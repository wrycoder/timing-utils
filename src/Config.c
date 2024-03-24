#define NTDDI_VERSION 0x0A000000 //NTDDI_WIN10
#define _WIN32_WINNT 0x0A000000 // _WIN32_WINNT_WIN10, the _WIN32_WINNT macro must also be defined when defining NTDDI_VERSION
#include "Config.h"
#include "Globals.h"
#include <winreg.h>
#include <shobjidl.h>

IFileDialog* pDirSelDialog;
static wchar_t starting_directory[sizeof(wchar_t) * MAX_PATH + 1];

/*
INT_PTR CALLBACK ConfigurationDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

}

void ShowConfigurationDialog(HWND owner)
{
  (&ConfigurationDialogProc);
}

*/

LRESULT SetBaseDirectory(HKEY hKey, HWND hWnd)
{
  LSTATUS regResult;
  pDirSelDialog = NULL;
  HRESULT hr;

  hr = CoCreateInstance(&CLSID_FileOpenDialog, NULL, CLSCTX_ALL, &IID_IFileDialog, (void**)&pDirSelDialog);
  if (SUCCEEDED(hr))
  {
    DWORD dwOptions;

    pDirSelDialog->lpVtbl->GetOptions(pDirSelDialog, &dwOptions);
    pDirSelDialog->lpVtbl->SetOptions(pDirSelDialog, dwOptions | FOS_PICKFOLDERS | FOS_FILEMUSTEXIST);
    pDirSelDialog->lpVtbl->SetOkButtonLabel(pDirSelDialog, TEXT("Select Base Folder"));
    GetCurrentDirectory((sizeof(wchar_t) * MAX_PATH), starting_directory);
    hr = pDirSelDialog->lpVtbl->Show(pDirSelDialog, hWnd); 
    if (SUCCEEDED(hr))
    {
      IShellItem* pSelectedItem = NULL;
      wchar_t *pszFolderPath = NULL;
      hr = pDirSelDialog->lpVtbl->GetResult(pDirSelDialog, &pSelectedItem);
      if (SUCCEEDED(hr))
      {
        hr = pSelectedItem->lpVtbl->GetDisplayName(pSelectedItem, SIGDN_FILESYSPATH, &pszFolderPath);
        if (SUCCEEDED(hr))
        {
          regResult = RegSetValueEx(hKey, APP_CONFIG_BASEDIR, 0, REG_SZ,
                                    (PBYTE)pszFolderPath, ((wcslen(pszFolderPath) + 1) * sizeof(wchar_t)));
          if (regResult != ERROR_SUCCESS)
          {
            ReportError(hWnd, regResult);
            CoTaskMemFree(pszFolderPath);
            return REGISTRY_UPDATE_ERROR;
          }
          CoTaskMemFree(pszFolderPath);
        }
      }
    }
    pDirSelDialog->lpVtbl->Release(pDirSelDialog);
    pDirSelDialog = NULL;
  } else {
    return REGISTRY_UPDATE_ERROR;
  }
  CoUninitialize();
  return ERROR_SUCCESS;
}
