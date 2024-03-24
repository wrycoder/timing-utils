#include <windows.h>
#include <commctrl.h>
#include "Globals.h"
#include "MainWindow.h"
#include "Resource.h"

/* Global instance handle */
HINSTANCE g_hInstance = NULL;
HWND  hwndButton;
extern BOOL buttonClicked;
extern LPWSTR selectedDay;
extern LPWSTR selectedHour;
HWND hSubdirectoryWnd, hHourWnd;

const DayInfo days[] = {
  {TEXT("Monday"), TEXT("mon")},
  {TEXT("Tuesday"), TEXT("tue")},
  {TEXT("Wednesday"), TEXT("wed")},
  {TEXT("Thursday"), TEXT("thu")},
  {TEXT("Friday"), TEXT("fri")}
};

const HourInfo hours[] = {
  {TEXT("N1"), TEXT("12am")}, {TEXT("N2"), TEXT("1am")},
  {TEXT("L1"), TEXT("2am")}, {TEXT("N3"), TEXT("3am")},
  {TEXT("N4"), TEXT("4am")}, {TEXT("L2"), TEXT("5am")},
  {TEXT("A1"), TEXT("6am")}, {TEXT("A2"), TEXT("7am")},
  {TEXT("A3"), TEXT("8am")}, {TEXT("L3"), TEXT("9am")},
  {TEXT("L4"), TEXT("10am")}, {TEXT("P3"), TEXT("1pm")},
  {TEXT("P1"), TEXT("4pm")}, {TEXT("P2"), TEXT("5pm")}
};

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  INITCOMMONCONTROLSEX icc;
  WNDCLASSEX wc = { };
  LPCTSTR MainWndClass = TEXT("TR");
  HWND hWnd;
  HMENU hSysMenu;
  HACCEL hAccelerators;
  MSG msg;

  /* Assign global HINSTANCE */
  g_hInstance = hInstance;

  // Initialize common controls
  icc.dwSize = sizeof(icc);
  icc.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&icc);

  // Initialize COM library
  HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  if (FAILED(hr))
  {
      // Handle COM initialization error
      return -1;
  }

  // Class for our main window
  wc.cbSize         = sizeof(wc);
  wc.style          = 0;
  wc.lpfnWndProc    = &MainWndProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = 0;
  wc.hInstance      = hInstance;
  wc.lpszClassName  = MainWndClass;
  wc.hIcon          = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 0, 0,
                              LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
  wc.hCursor        = (HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
  wc.hbrBackground  = (HBRUSH) (COLOR_BTNFACE + 1);
  wc.lpszMenuName   = MAKEINTRESOURCE(IDR_MAINMENU);
  wc.lpszClassName  = MainWndClass;
  wc.hIconSm        = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON,
                                        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
                                        LR_DEFAULTCOLOR | LR_SHARED);

  // Register our window classes, or error
  if (! RegisterClassEx(&wc))
  {
    MessageBox(NULL, TEXT("Error registering window class."), TEXT("Error"), MB_ICONERROR | MB_OK);
    return 0;
  }

  // Create instance of main window
  hWnd = CreateWindowEx(0, MainWndClass, MainWndClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                        136, 400, NULL, NULL, hInstance, NULL);
  // Error if window creation failed
  if (! hWnd)
  {
    MessageBox(NULL, TEXT("Error creating main window."), TEXT("Error"), MB_ICONERROR | MB_OK);
    return 0;
  }

  // Create a list box for the day subdirectory
  hSubdirectoryWnd = CreateWindow(
      TEXT("LISTBOX"),
      NULL,
      WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD,
      10, 10, 100, 200,
      hWnd,
      NULL,
      hInstance,
      NULL
  );

  if (hSubdirectoryWnd == NULL)
  {
    ReportError(hWnd, GetLastError());
    return 0;
  }
  // Add some items to the list box
  for (int i = 0; i < 5; i++)
  {
    SendMessage(hSubdirectoryWnd, LB_ADDSTRING, 0, (LPARAM)days[i].displayString);
  }

  hHourWnd = CreateWindow(
      TEXT("LISTBOX"),
      NULL,
      WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY,
      110, 10, 55, 200,
      hWnd,
      NULL,
      hInstance,
      NULL
  );
  if (hHourWnd == NULL)
  {
    ReportError(hWnd, GetLastError());
    return 0;
  }
  for (int i = 0; i < 14; i++)
  {
    SendMessage(hHourWnd, LB_ADDSTRING, 0, (LPARAM)hours[i].displayString);
  }

  hwndButton = CreateWindow(
    TEXT("BUTTON"),                   // Window class name
    TEXT("Compute Timings"),         // Button text
    WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON,  // Styles
    10,                                // X position
    220,                               // Y position (below list box)
    150,                               // Width
    30,                                // Height
    hWnd,                              // Parent window handle
    NULL,                              // Menu handle
    hInstance,                         // Instance handle
    NULL                               // Additional data
);
  // Load accelerators
  hAccelerators = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

  // Add "about" to the system menu.
  hSysMenu = GetSystemMenu(hWnd, FALSE);
  InsertMenu(hSysMenu, 5, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
  InsertMenu(hSysMenu, 6, MF_BYPOSITION, ID_HELP_ABOUT, TEXT("About"));

  // Show window and force a paint
  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  // Run the message loop.
  while(GetMessage(&msg, NULL, 0, 0) > 0)
  {
    if (! TranslateAccelerator(msg.hwnd, hAccelerators, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  // Uninitialize COM library
  CoUninitialize();
  return msg.wParam;
}
