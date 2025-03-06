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
  BYTE    data[1024];
  DWORD   dataSize = 1024;
  DWORD   dwType = REG_SZ;

  const PWSTR base_dir = TEXT(APP_BASE_DIRECTORY);

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
      return 0;
    }
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

