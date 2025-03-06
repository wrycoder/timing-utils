#pragma once

#include <windows.h>

#define   APP_BASE_DIRECTORY  "\\CMITRAX";

/* Global instance handle */
extern HINSTANCE g_hInstance;

void ReportError(HWND hWnd, DWORD dwErr);

typedef struct {
  LPWSTR displayString;
  LPWSTR directoryName;
} DayInfo;

extern const DayInfo days[];

typedef struct {
  LPWSTR hourName;
  LPWSTR displayString;
} HourInfo;

extern const HourInfo hours[];
