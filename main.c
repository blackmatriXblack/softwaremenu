/*
 * System Tray Launcher (Rich Right-Click Menu) - Fixed Version
 * Compile (MinGW-w64):
 *   gcc -o tray_launcher.exe tray_launcher.c -lole32 -lshell32 -mwindows -O2
 */

#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <stdio.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")

// ---------- Website data ----------
typedef struct {
    const WCHAR *name;
    const WCHAR *url;
} WebSite;

typedef struct {
    const WCHAR *category;
    const WebSite *sites;
    int count;
} WebCategory;

WebSite searchSites[] = {
    { L"Google",        L"https://www.google.com" },
    { L"Bing",          L"https://www.bing.com" },
    { L"DuckDuckGo",    L"https://duckduckgo.com" },
    { L"Yahoo",         L"https://www.yahoo.com" },
    { L"Baidu",         L"https://www.baidu.com" }
};

WebSite socialSites[] = {
    { L"Facebook",      L"https://www.facebook.com" },
    { L"Twitter",       L"https://twitter.com" },
    { L"Reddit",        L"https://www.reddit.com" },
    { L"Instagram",     L"https://www.instagram.com" },
    { L"LinkedIn",      L"https://www.linkedin.com" },
    { L"Pinterest",     L"https://www.pinterest.com" },
    { L"Tumblr",        L"https://www.tumblr.com" }
};

WebSite videoSites[] = {
    { L"YouTube",       L"https://www.youtube.com" },
    { L"Twitch",        L"https://www.twitch.tv" },
    { L"Vimeo",         L"https://vimeo.com" },
    { L"Dailymotion",   L"https://www.dailymotion.com" }
};

WebSite devSites[] = {
    { L"GitHub",        L"https://github.com" },
    { L"GitLab",        L"https://gitlab.com" },
    { L"Stack Overflow",L"https://stackoverflow.com" },
    { L"MDN Web Docs",  L"https://developer.mozilla.org" },
    { L"W3Schools",     L"https://www.w3schools.com" },
    { L"CodePen",       L"https://codepen.io" },
    { L"Replit",        L"https://replit.com" },
    { L"DevDocs",       L"https://devdocs.io" }
};

WebSite newsSites[] = {
    { L"CNN",           L"https://www.cnn.com" },
    { L"BBC News",      L"https://www.bbc.com/news" },
    { L"Reuters",       L"https://www.reuters.com" },
    { L"The Guardian",  L"https://www.theguardian.com" },
    { L"Al Jazeera",    L"https://www.aljazeera.com" },
    { L"NPR",           L"https://www.npr.org" }
};

WebSite shoppingSites[] = {
    { L"Amazon",        L"https://www.amazon.com" },
    { L"eBay",          L"https://www.ebay.com" },
    { L"Etsy",          L"https://www.etsy.com" },
    { L"Best Buy",      L"https://www.bestbuy.com" }
};

WebSite entertainmentSites[] = {
    { L"Netflix",       L"https://www.netflix.com" },
    { L"Spotify",       L"https://www.spotify.com" },
    { L"IMDb",          L"https://www.imdb.com" },
    { L"Hulu",          L"https://www.hulu.com" },
    { L"Disney+",       L"https://www.disneyplus.com" }
};

WebSite technologySites[] = {
    { L"TechCrunch",    L"https://techcrunch.com" },
    { L"The Verge",     L"https://www.theverge.com" },
    { L"Ars Technica",  L"https://arstechnica.com" },
    { L"Hacker News",   L"https://news.ycombinator.com" },
    { L"Slashdot",      L"https://slashdot.org" }
};

WebSite educationSites[] = {
    { L"Wikipedia",     L"https://en.wikipedia.org" },
    { L"Coursera",      L"https://www.coursera.org" },
    { L"Khan Academy",  L"https://www.khanacademy.org" },
    { L"edX",           L"https://www.edx.org" },
    { L"Udemy",         L"https://www.udemy.com" }
};

WebCategory g_webCategories[] = {
    { L"Search Engines",   searchSites,    5 },
    { L"Social Media",     socialSites,    7 },
    { L"Video / Streaming",videoSites,     4 },
    { L"Development",      devSites,       8 },
    { L"News",             newsSites,      6 },
    { L"Shopping",         shoppingSites,  4 },
    { L"Entertainment",    entertainmentSites,5 },
    { L"Technology",       technologySites,5 },
    { L"Education",        educationSites, 5 }
};
#define WEBCAT_COUNT (sizeof(g_webCategories)/sizeof(g_webCategories[0]))

// ---------- System tools ----------
typedef struct {
    const WCHAR *name;
    const WCHAR *cmd;
    const WCHAR *args;
} ToolItem;

ToolItem g_tools[] = {
    { L"Notepad",              L"notepad.exe",        L"" },
    { L"Calculator",           L"calc.exe",           L"" },
    { L"Command Prompt",       L"cmd.exe",            L"" },
    { L"PowerShell",           L"powershell.exe",     L"" },
    { L"Task Manager",         L"taskmgr.exe",        L"" },
    { L"File Explorer",        L"explorer.exe",       L"" },
    { L"Control Panel",        L"control.exe",        L"" },
    { L"Registry Editor",      L"regedit.exe",        L"" },
    { L"MSConfig",             L"msconfig.exe",       L"" },
    { L"Services",             L"services.msc",       L"" },
    { L"Disk Management",      L"diskmgmt.msc",       L"" },
    { L"Device Manager",       L"devmgmt.msc",        L"" },
    { L"Event Viewer",         L"eventvwr.msc",       L"" },
    { L"Performance Monitor",  L"perfmon.msc",        L"" },
    { L"System Information",   L"msinfo32.exe",       L"" }
};
#define TOOL_COUNT (sizeof(g_tools)/sizeof(g_tools[0]))

// ---------- Special folders ----------
ToolItem g_folders[] = {
    { L"Downloads",     L"explorer.exe", L"shell:Downloads" },
    { L"Desktop",       L"explorer.exe", L"shell:Desktop" },
    { L"Documents",     L"explorer.exe", L"shell:Personal" },
    { L"Music",         L"explorer.exe", L"shell:My Music" },
    { L"Pictures",      L"explorer.exe", L"shell:My Pictures" },
    { L"Videos",        L"explorer.exe", L"shell:My Video" }
};
#define FOLDER_COUNT (sizeof(g_folders)/sizeof(g_folders[0]))

// ---------- Power options ----------
ToolItem g_power[] = {
    { L"Shutdown",   L"shutdown.exe", L"/s /t 5 /c \"Shutting down in 5 seconds\"" },
    { L"Restart",    L"shutdown.exe", L"/r /t 5 /c \"Restarting in 5 seconds\"" },
    { L"Sleep",      L"rundll32.exe", L"powrprof.dll,SetSuspendState 0,1,0" },
    { L"Hibernate",  L"shutdown.exe", L"/h" },
    { L"Lock",       L"rundll32.exe", L"user32.dll,LockWorkStation" },
    { L"Log Off",    L"shutdown.exe", L"/l" }
};
#define POWER_COUNT (sizeof(g_power)/sizeof(g_power[0]))

// ---------- Installed software scan ----------
#define MAX_PROGS 2000
typedef struct {
    WCHAR name[MAX_PATH];
    WCHAR target[MAX_PATH];
} ProgInfo;

ProgInfo g_progs[MAX_PROGS];
int g_progCount = 0;

// Tray icon parameters
#define WM_TRAYICON (WM_APP + 1)
#define ID_TRAYICON 1
NOTIFYICONDATAW g_nid = {0};

// Forward declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void AddTrayIcon(HWND hwnd);
void RemoveTrayIcon(HWND hwnd);
void ScanAllPrograms();
BOOL ResolveShortcut(const WCHAR *lnkPath, WCHAR *target, WCHAR *desc);
HMENU BuildTrayMenu();
void ExecuteCommand(const WCHAR *cmd, const WCHAR *args);
void HandleMenuCommand(HWND hwnd, WORD id);

// ---------- Shortcut resolver ----------
BOOL ResolveShortcut(const WCHAR *lnkPath, WCHAR *outTarget, WCHAR *outDesc) {
    IShellLinkW *psl = NULL;
    IPersistFile *ppf = NULL;
    BOOL ok = FALSE;
    HRESULT hr;

    hr = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                          &IID_IShellLinkW, (void**)&psl);
    if (FAILED(hr)) return FALSE;

    hr = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (void**)&ppf);
    if (FAILED(hr)) { psl->lpVtbl->Release(psl); return FALSE; }

    hr = ppf->lpVtbl->Load(ppf, lnkPath, STGM_READ);
    if (SUCCEEDED(hr)) {
        hr = psl->lpVtbl->GetPath(psl, outTarget, MAX_PATH, NULL, SLGP_RAWPATH);
        if (FAILED(hr)) outTarget[0] = L'\0';

        hr = psl->lpVtbl->GetDescription(psl, outDesc, MAX_PATH);
        if (FAILED(hr) || wcslen(outDesc) == 0) {
            // Fallback to filename without .lnk extension
            WCHAR fname[MAX_PATH];
            wcscpy_s(fname, MAX_PATH, lnkPath);
            WCHAR *dot = wcsrchr(fname, L'.');
            if (dot) *dot = L'\0';
            WCHAR *name = wcsrchr(fname, L'\\');
            if (name)
                wcscpy_s(outDesc, MAX_PATH, name + 1);
            else
                wcscpy_s(outDesc, MAX_PATH, fname);
        }
        ok = TRUE;
    }

    if (ppf) ppf->lpVtbl->Release(ppf);
    if (psl) psl->lpVtbl->Release(psl);
    return ok;
}

// ---------- Scan Start Menu ----------
void ScanDirectory(const WCHAR *dir) {
    WCHAR search[MAX_PATH];
    swprintf_s(search, MAX_PATH, L"%s\\*", dir);
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(search, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0)
            continue;
        WCHAR full[MAX_PATH];
        swprintf_s(full, MAX_PATH, L"%s\\%s", dir, fd.cFileName);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            ScanDirectory(full);
        } else if (wcsstr(fd.cFileName, L".lnk")) {
            if (g_progCount >= MAX_PROGS) break;
            WCHAR target[MAX_PATH] = L"", desc[MAX_PATH] = L"";
            if (ResolveShortcut(full, target, desc) && wcslen(target) > 0) {
                wcscpy_s(g_progs[g_progCount].name, MAX_PATH, desc);
                wcscpy_s(g_progs[g_progCount].target, MAX_PATH, target);
                g_progCount++;
            }
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
}

void ScanAllPrograms() {
    WCHAR common[MAX_PATH], user[MAX_PATH];
    if (SHGetSpecialFolderPathW(NULL, common, CSIDL_COMMON_PROGRAMS, FALSE))
        ScanDirectory(common);
    if (SHGetSpecialFolderPathW(NULL, user, CSIDL_PROGRAMS, FALSE))
        ScanDirectory(user);
}

// ---------- Build right-click menu ----------
HMENU BuildTrayMenu() {
    HMENU hMain = CreatePopupMenu();
    HMENU hWeb = CreatePopupMenu();
    HMENU hTools = CreatePopupMenu();
    HMENU hFolders = CreatePopupMenu();
    HMENU hPower = CreatePopupMenu();
    HMENU hPrograms = CreatePopupMenu();

    // Websites submenu
    for (int c = 0; c < WEBCAT_COUNT; c++) {
        HMENU hCat = CreatePopupMenu();
        for (int i = 0; i < g_webCategories[c].count; i++) {
            AppendMenuW(hCat, MF_STRING,
                        (UINT_PTR)(100 + c*100 + i),
                        g_webCategories[c].sites[i].name);
        }
        AppendMenuW(hWeb, MF_POPUP, (UINT_PTR)hCat, g_webCategories[c].category);
    }

    // System tools
    for (int i = 0; i < TOOL_COUNT; i++) {
        AppendMenuW(hTools, MF_STRING, (UINT_PTR)(1000 + i), g_tools[i].name);
    }

    // Special folders
    for (int i = 0; i < FOLDER_COUNT; i++) {
        AppendMenuW(hFolders, MF_STRING, (UINT_PTR)(2000 + i), g_folders[i].name);
    }

    // Power options
    for (int i = 0; i < POWER_COUNT; i++) {
        AppendMenuW(hPower, MF_STRING, (UINT_PTR)(3000 + i), g_power[i].name);
    }

    // Installed programs
    if (g_progCount > 0) {
        for (int i = 0; i < g_progCount; i++) {
            AppendMenuW(hPrograms, MF_STRING, (UINT_PTR)(4000 + i), g_progs[i].name);
        }
    } else {
        AppendMenuW(hPrograms, MF_STRING | MF_GRAYED, 0, L"No software found");
    }

    // Assemble main menu
    AppendMenuW(hMain, MF_POPUP, (UINT_PTR)hWeb,       L"Open Website");
    AppendMenuW(hMain, MF_POPUP, (UINT_PTR)hTools,     L"System Tools");
    AppendMenuW(hMain, MF_POPUP, (UINT_PTR)hFolders,   L"Special Folders");
    AppendMenuW(hMain, MF_POPUP, (UINT_PTR)hPrograms,  L"Installed Programs");
    AppendMenuW(hMain, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMain, MF_POPUP, (UINT_PTR)hPower,     L"Power Control");
    AppendMenuW(hMain, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMain, MF_STRING, 9000, L"About");
    AppendMenuW(hMain, MF_STRING, 9001, L"Exit");

    return hMain;
}

// ---------- Execute command ----------
void ExecuteCommand(const WCHAR *cmd, const WCHAR *args) {
    ShellExecuteW(NULL, L"open", cmd, args, NULL, SW_SHOWNORMAL);
}

// ---------- Menu click handler ----------
void HandleMenuCommand(HWND hwnd, WORD id) {
    // Website
    if (id >= 100 && id < 1000) {
        int cat = (id - 100) / 100;
        int idx = (id - 100) % 100;
        if (cat < WEBCAT_COUNT && idx < g_webCategories[cat].count) {
            ShellExecuteW(NULL, L"open", g_webCategories[cat].sites[idx].url,
                          NULL, NULL, SW_SHOWNORMAL);
        }
    }
    // System tools
    else if (id >= 1000 && id < 1000 + TOOL_COUNT) {
        int idx = id - 1000;
        ExecuteCommand(g_tools[idx].cmd, g_tools[idx].args);
    }
    // Special folders
    else if (id >= 2000 && id < 2000 + FOLDER_COUNT) {
        int idx = id - 2000;
        ExecuteCommand(g_folders[idx].cmd, g_folders[idx].args);
    }
    // Power options (with confirmation)
    else if (id >= 3000 && id < 3000 + POWER_COUNT) {
        int idx = id - 3000;
        WCHAR msg[128];
        swprintf_s(msg, 128, L"Are you sure you want to %s?", g_power[idx].name);
        if (MessageBoxW(NULL, msg, L"Confirm", MB_YESNO | MB_ICONQUESTION) == IDYES) {
            ExecuteCommand(g_power[idx].cmd, g_power[idx].args);
        }
    }
    // Installed programs
    else if (id >= 4000 && id < 4000 + MAX_PROGS) {
        int idx = id - 4000;
        if (idx < g_progCount) {
            ShellExecuteW(NULL, L"open", g_progs[idx].target, NULL, NULL, SW_SHOWNORMAL);
        }
    }
    // About
    else if (id == 9000) {
        MessageBoxW(NULL, L"System Tray Launcher\n\n"
                          L"Right-click the tray icon to access:\n"
                          L"- 50+ websites\n"
                          L"- System tools & folders\n"
                          L"- All installed programs\n"
                          L"- Power control\n\n"
                          L"Hover over icon and right-click.",
                    L"About", MB_OK | MB_ICONINFORMATION);
    }
    // Exit
    else if (id == 9001) {
        RemoveTrayIcon(hwnd);
        DestroyWindow(hwnd);
    }
}

// ---------- Tray icon management ----------
void AddTrayIcon(HWND hwnd) {
    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
    g_nid.hWnd = hwnd;
    g_nid.uID = ID_TRAYICON;
    g_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 16, 16, LR_SHARED);
    wcscpy_s(g_nid.szTip, sizeof(g_nid.szTip)/sizeof(WCHAR), L"Tool Launcher - Right-click for menu");
    Shell_NotifyIconW(NIM_ADD, &g_nid);
}

void RemoveTrayIcon(HWND hwnd) {
    Shell_NotifyIconW(NIM_DELETE, &g_nid);
}

// ---------- Window procedure (hidden window) ----------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            ScanAllPrograms();
            AddTrayIcon(hwnd);
            break;

        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);

                HMENU hMenu = BuildTrayMenu();
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN,
                               pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(hMenu);
                PostMessage(hwnd, WM_NULL, 0, 0);
            }
            break;

        case WM_COMMAND:
            HandleMenuCommand(hwnd, LOWORD(wParam));
            break;

        case WM_DESTROY:
            RemoveTrayIcon(hwnd);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// ---------- WinMain ----------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    WNDCLASSEXW wc = {0};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"TrayLauncherClass";
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Window class registration failed!", L"Error", MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowExW(0, wc.lpszClassName, L"TrayLauncher",
                                WS_OVERLAPPEDWINDOW,
                                0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    if (!hwnd) {
        MessageBoxW(NULL, L"Window creation failed!", L"Error", MB_ICONERROR);
        CoUninitialize();
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    return (int)msg.wParam;
}