#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#define WM_APP_TRAY     (WM_APP + 1)
#define ID_TRAY_XBUTTON1 1001
#define ID_TRAY_XBUTTON2 1002
#define ID_TRAY_EXIT     1003

HINSTANCE g_hInst;
HHOOK g_hHook;
NOTIFYICONDATAA g_nid;
int g_useXButton2 = 1; // 1 = нижняя (XButton2), 0 = верхняя (XButton1)

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;

        DWORD btn = (p->mouseData >> 16) & 0xFFFF;

        // Выбираем какую кнопку слушать
        if ((g_useXButton2 && (btn == XBUTTON2)) ||
            (!g_useXButton2 && (btn == XBUTTON1))) {

            if (wParam == WM_XBUTTONDOWN) {
                ShellExecuteA(NULL, "open", "micctl.exe", "0", NULL, SW_HIDE);
            } else if (wParam == WM_XBUTTONUP) {
                ShellExecuteA(NULL, "open", "micctl.exe", "1", NULL, SW_HIDE);
            }
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

void UpdateTrayTip() {
    if (!g_nid.hWnd) return;

    char tip[64];
    if (g_useXButton2)
        lstrcpyA(tip, "PTT FFN: XButton5");
    else
        lstrcpyA(tip, "PTT FFN: XButton4");

    lstrcpynA(g_nid.szTip, tip, sizeof(g_nid.szTip));
    g_nid.uFlags = NIF_TIP;
    Shell_NotifyIconA(NIM_MODIFY, &g_nid);
}

void AddTrayIcon(HWND hWnd) {
    ZeroMemory(&g_nid, sizeof(g_nid));
    g_nid.cbSize = sizeof(NOTIFYICONDATAA);
    g_nid.hWnd = hWnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    g_nid.uCallbackMessage = WM_APP_TRAY;

    // Загружаем иконку из файла pttffn.ico в этой же папке
char iconPath[MAX_PATH];
GetModuleFileNameA(NULL, iconPath, MAX_PATH);

// Обрубаем имя exe, оставляем путь к папке
char *slash = strrchr(iconPath, '\\');
if (slash) {
    *(slash + 1) = '\0';
}

// Добавляем имя файла иконки
strcat_s(iconPath, MAX_PATH, "pttffn.ico");

// Загружаем иконку
g_nid.hIcon = (HICON)LoadImageA(
    NULL,
    iconPath,
    IMAGE_ICON,
    0, 0,
    LR_LOADFROMFILE | LR_DEFAULTSIZE
);

// Fallback на стандартную, если файл не нашли
if (!g_nid.hIcon) {
    g_nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
}

lstrcpyA(g_nid.szTip, "PTT FFN: XButton2 (lower)");

    Shell_NotifyIconA(NIM_ADD, &g_nid);
}

void RemoveTrayIcon() {
    if (g_nid.hWnd) {
        Shell_NotifyIconA(NIM_DELETE, &g_nid);
    }
}

void ShowTrayMenu(HWND hWnd) {
    POINT pt;
    GetCursorPos(&pt);

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;

    InsertMenuA(hMenu, -1, MF_BYPOSITION | MF_STRING |
                         (g_useXButton2 ? 0 : MF_CHECKED),
                ID_TRAY_XBUTTON1, "PTT Button 4");

    InsertMenuA(hMenu, -1, MF_BYPOSITION | MF_STRING |
                         (g_useXButton2 ? MF_CHECKED : 0),
                ID_TRAY_XBUTTON2, "PTT Button 5");

    InsertMenuA(hMenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenuA(hMenu, -1, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT, "Exit");

    SetForegroundWindow(hWnd);
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN,
                   pt.x, pt.y, 0, hWnd, NULL);

    DestroyMenu(hMenu);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_APP_TRAY:
        if (lParam == WM_RBUTTONUP || lParam == WM_CONTEXTMENU) {
            ShowTrayMenu(hWnd);
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_TRAY_XBUTTON1:
            g_useXButton2 = 0;
            UpdateTrayTip();
            break;
        case ID_TRAY_XBUTTON2:
            g_useXButton2 = 1;
            UpdateTrayTip();
            break;
        case ID_TRAY_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nShowCmd) {
    g_hInst = hInstance;

    const char *CLASS_NAME = "PTTFFN_CLASS";

    WNDCLASSEXA wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassExA(&wc)) {
        return 1;
    }

    // WS_EX_TOOLWINDOW и SW_HIDE => не видно в панели задач
    HWND hWnd = CreateWindowExA(
        WS_EX_TOOLWINDOW,
        CLASS_NAME,
        "PTTFFN",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );
    if (!hWnd) {
        return 1;
    }

    ShowWindow(hWnd, SW_HIDE);

    AddTrayIcon(hWnd);

    g_hHook = SetWindowsHookExA(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);
    if (!g_hHook) {
        MessageBoxA(NULL, "Не удалось установить хук мыши", "pttffn", MB_ICONERROR);
        RemoveTrayIcon();
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(g_hHook);
    RemoveTrayIcon();
    return 0;
}
