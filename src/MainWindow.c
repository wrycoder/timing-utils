#include "Globals.h"
#include <winreg.h>
#include <strsafe.h>
#include "MainWindow.h"
#include "AboutDialog.h"
#include "Resource.h"
#include "Config.h"
#include "Audio.h"

extern HWND hwndButton;
BOOL buttonClicked;
LPWSTR selectedDay;
LPWSTR selectedHour;
extern HWND hSubdirectoryWnd, hHourWnd;

void ReportError(HWND hWnd, DWORD dwErr)
{
  LPVOID lpErrorBuf = NULL;
  LPVOID lpDisplayBuf = NULL;
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ALLOCATE_BUFFER,
                NULL,
                (DWORD)dwErr,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpErrorBuf,
                199, NULL);
  lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
    (lstrlen((LPCTSTR)lpErrorBuf) + 40) * sizeof(TCHAR));
  StringCchPrintf((LPTSTR)lpDisplayBuf,
    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
    TEXT("Operation failed with error %d: %s"),
    (DWORD)dwErr, lpErrorBuf);
  MessageBox(hWnd, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);
  LocalFree(lpErrorBuf);
  LocalFree(lpDisplayBuf);
}

/* Window procedure for our main window */
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  HKEY    hKey;
  BYTE    data[1024];
  DWORD   dataSize = 1024;
  TCHAR*  base_dir;
  DWORD   dwType = REG_SZ;
  LSTATUS regResult;

  regResult = RegOpenKeyEx(HKEY_CURRENT_USER, APP_CONFIG_SUBKEY,
                           0, KEY_READ | KEY_WRITE , &hKey);
  if (regResult != ERROR_SUCCESS)
  {
    if (regResult == ERROR_FILE_NOT_FOUND)
    {
      /* The app has not been used yet. Set the base directory. */
      regResult = RegCreateKeyEx(HKEY_CURRENT_USER, APP_CONFIG_SUBKEY,
                                  0, NULL, REG_OPTION_NON_VOLATILE,
                                  KEY_WRITE, NULL, &hKey, NULL);
      if (regResult != ERROR_SUCCESS)
      {
        MessageBox(hWnd, TEXT("Unable to add app key to registry."), TEXT("Error"), MB_ICONERROR | MB_OK);
        return 0;
      }
      regResult = SetBaseDirectory(hKey, hWnd);
      if (regResult != ERROR_SUCCESS)
      {
        MessageBox(hWnd, TEXT("Unable to update base directory."), TEXT("Error"), MB_ICONERROR | MB_OK);
        return 0;
      }
    }
  }

  regResult = RegGetValue(hKey, NULL, APP_CONFIG_BASEDIR, RRF_RT_REG_SZ, &dwType, data, &dataSize);
  if (regResult != ERROR_SUCCESS)
  {
    MessageBox(hWnd, TEXT("Unable to retrieve base directory."), TEXT("Error"), MB_ICONERROR | MB_OK);
    RegCloseKey(hKey);
    return 0;
  }

  regResult = RegCloseKey(hKey);
  if (regResult != ERROR_SUCCESS)
  {
    MessageBox(hWnd, TEXT("Unable to close registry."), TEXT("Error"), MB_ICONERROR | MB_OK);
    return 0;
  }

  int wstrLength = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)data, -1, NULL, 0);
  if (wstrLength == 0)
  {
    MessageBox(hWnd, TEXT("Error converting byte array to wide-character string"), TEXT("Error"), MB_ICONERROR | MB_OK);
    return 0;
  }

  base_dir = (TCHAR*)malloc(wstrLength * sizeof(TCHAR));
  if (base_dir == NULL)
  {
    MessageBox(hWnd, TEXT("Memory allocation failed"), TEXT("Error"), MB_ICONERROR | MB_OK);
    return 0;
  }

  if (MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)data, -1, base_dir, wstrLength) == 0)
  {
    MessageBox(hWnd, TEXT("Error converting byte array to wide-character string"), TEXT("Error"), MB_ICONERROR | MB_OK);
    free(base_dir);
    return 0;
  }

  switch (msg)
  {
    case WM_CREATE:
    {
      buttonClicked = FALSE;
      selectedDay = TEXT("");
      selectedHour = TEXT("");
      return 0;
    }
    case WM_COMMAND:
    {
      if (lParam == (LPARAM)hwndButton && HIWORD(wParam) == BN_CLICKED)
      {
        buttonClicked = TRUE;
        if (InvalidateRect(hWnd, NULL, TRUE) == 0)
        {
          ReportError(hWnd, GetLastError());
        }
        int subdirectoryIndex = (int)SendMessage(hSubdirectoryWnd, LB_GETCURSEL, 0, 0);
        if (subdirectoryIndex == LB_ERR)
        {
          MessageBox(hWnd, TEXT("Please select a day"), TEXT("Unknown Day"), MB_ICONERROR | MB_OK);
          return 0;
        }
        int hourIndex = (int)SendMessage(hHourWnd, LB_GETCURSEL, 0, 0);
        if (hourIndex == LB_ERR)
        {
          MessageBox(hWnd, TEXT("Please select an hour"), TEXT("Unknown Hour"), MB_ICONERROR | MB_OK);
          return 0;
        }
        subdirectoryIndex = (int)SendMessage(hSubdirectoryWnd, LB_GETCURSEL, 0, 0);
        hourIndex = (int)SendMessage(hHourWnd, LB_GETCURSEL, 0, 0);
//        StringCchPrintf(fullPath, (sizeof(fullPath) / sizeof(TCHAR)), "%s\\%s", 
        load_filenames(hWnd, base_dir, days[subdirectoryIndex].directoryName, hours[hourIndex].hourName);
        selectedDay = days[subdirectoryIndex].displayString;
        selectedHour = hours[hourIndex].displayString;
        UpdateWindow(hWnd);
        return 0;
      }
      WORD id = LOWORD(wParam);
      switch (id)
      {
        case ID_HELP_ABOUT:
        {
          ShowAboutDialog(hWnd);
          return 0;
        }
        case ID_FILE_BASEDIR:
        {
          regResult = RegOpenKeyEx(HKEY_CURRENT_USER, APP_CONFIG_SUBKEY,
                                  0, KEY_READ | KEY_WRITE , &hKey);
          if (regResult == ERROR_SUCCESS)
          {
            regResult = SetBaseDirectory(hKey, hWnd);
            if (regResult != ERROR_SUCCESS)
            {
              MessageBox(hWnd, TEXT("Unable to update registry."), TEXT("Error"), MB_ICONERROR | MB_OK);
              RegCloseKey(hKey);
              return 0;
            }
          }
          regResult = RegCloseKey(hKey);
          if (regResult != ERROR_SUCCESS)
          {
            MessageBox(hWnd, TEXT("Unable to close registry."), TEXT("Error"), MB_ICONERROR | MB_OK);
            return 0;
          }
          return 0;
        }
        case ID_FILE_EXIT:
        {
          DestroyWindow(hWnd);
          return 0;
        }
      }
    }
    break;
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      if (hdc == NULL)
      {
        ReportError(hWnd, GetLastError());
        return 0;
      }
      if (buttonClicked == TRUE)
      {
        // Draw text
        LPWSTR text = selectedHour;
        if (TextOut(hdc, 10, 260, text, wcslen(text)) == 0)
        {
          ReportError(hWnd, GetLastError());
        }
      }
      EndPaint(hWnd, &ps);
      return 0;
    }
    case WM_GETMINMAXINFO:
    {
      /* Prevent our window from being sized too small */
      MINMAXINFO *minMax = (MINMAXINFO*)lParam;
      minMax->ptMinTrackSize.x = 220;
      minMax->ptMinTrackSize.y = 110;
      return 0;
    }
    case WM_DESTROY:
    {
      PostQuitMessage(0);
      free(base_dir);
      return 0;
    }
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

