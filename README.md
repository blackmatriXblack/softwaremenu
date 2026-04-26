# softwaremenu
a tool of windowssoftware in windows{intstalled software}
## **Websoftware – System Tray Application Launcher – Technical Documentation**

### **1. Overview**
**Websoftware** is a lightweight Windows utility written in C using the Win32 API. It resides in the system notification area (tray) and provides instant access to over 50 categorized websites, dozens of system tools and folders, all installed applications discovered from the Start Menu, and power management options. The program has a minimal footprint, uses no external libraries beyond standard Windows DLLs, and is designed for rapid, out‑of‑the‑way productivity.

### **2. Features**
- **Extensive Website Directory**  
  Over 50 URLs organized into nine logical categories:
  - Search Engines
  - Social Media
  - Video / Streaming
  - Development
  - News
  - Shopping
  - Entertainment
  - Technology
  - Education
- **Full Installed Software List**  
  Dynamically scans the Start Menu folders (`CSIDL_COMMON_PROGRAMS` and `CSIDL_PROGRAMS`) and resolves all `.lnk` shortcuts. The list is presented as a submenu and each entry launches the application directly.
- **System Tools**  
  Quick access to essential Windows utilities:
  - Notepad, Calculator, Command Prompt, PowerShell, Task Manager, File Explorer, Control Panel, Registry Editor, MSConfig, Services, Disk Management, Device Manager, Event Viewer, Performance Monitor, System Information.
- **Special Folders**  
  One‑click access to:
  - Downloads, Desktop, Documents, Music, Pictures, Videos.
- **Power Control**  
  Shutdown, Restart, Sleep, Hibernate, Lock, and Log Off, each with a confirmation dialog to prevent accidental action.
- **Pure Tray Operation**  
  No main window cluttering the desktop; everything is accessed via a right‑click menu on the tray icon.
- **Automatic Startup Scan**  
  Program discovery runs at launch and caches results for the session.
- **English Interface**  
  All menu items, prompts, and messages are in English.

### **3. System Requirements**
- **Operating System**: Windows Vista / 7 / 8 / 10 / 11 (32‑bit or 64‑bit).  
  Uses `SHGetSpecialFolderPathW` and `IShellLinkW`, which are available from Windows 2000 onward.
- **Compiler**: MinGW‑w64 (GCC) or any compatible C compiler with Windows SDK headers.  
  Required link libraries: `ole32`, `shell32` (and implicitly `user32`, `kernel32`).
- **Permissions**: Standard user account. Administrative rights are **not** required.

### **4. Compilation Guide**
#### **4.1 Source File**
Save the source code as `websoftware.c` (UTF‑8 encoding recommended).

#### **4.2 Build Command**
Using MinGW‑w64:
```bash
gcc -o websoftware.exe websoftware.c -lole32 -lshell32 -mwindows -O2
```
- `-mwindows`: Suppresses the console window.
- `-O2`: Optimizes for size and speed.
- Link libraries are specified explicitly; the compiler automatically includes `kernel32`, `user32`, etc.

#### **4.3 Using CMake (Optional)**
If a CMake project is preferred, a minimal `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.10)
project(Websoftware C)

add_executable(websoftware websoftware.c)
target_link_libraries(websoftware ole32 shell32)
target_compile_options(websoftware PRIVATE -mwindows -O2)
```

Compilation will produce a single self‑contained executable (`websoftware.exe`) of approximately 50–100 KB.

### **5. Implementation Details**
#### **5.1 Architecture**
The program consists of a hidden window (class `WebsoftwareTray`) that acts as a message sink. The `WinMain` function initializes COM, registers the window class, creates the hidden window, and enters the standard message loop. All logic is event‑driven:

- `WM_CREATE` – Scans the Start Menu for installed programs; adds the tray icon.
- `WM_TRAYICON` (custom message `WM_APP+1`) – Responds to right‑click on the tray icon by building and displaying a popup menu.
- `WM_COMMAND` – Dispatches menu selections to the appropriate handler.
- `WM_DESTROY` – Removes the tray icon and posts the quit message.

#### **5.2 Shortcut Resolution**
The function `ResolveShortcut()` uses COM interfaces:
1. `CoCreateInstance(CLSID_ShellLink, ...)` to create an `IShellLinkW` object.
2. `QueryInterface` for `IPersistFile` to load the `.lnk` file.
3. `GetPath` retrieves the target executable; `GetDescription` provides the display name.
4. If no description is available, the filename (minus `.lnk`) is used.

This avoids registry enumeration and works for both per‑user and system‑wide shortcuts.

#### **5.3 Menu Construction**
The popup menu is assembled dynamically with nested submenus:
- Websites: nine category submenus each containing up to eight items (menu IDs 100–999).
- System Tools: flat list (IDs 1000–1099).
- Special Folders: flat list (IDs 2000–2099).
- Power Options: flat list (IDs 3000–3099).
- Installed Programs: dynamic list limited to 2000 entries (IDs 4000–5999).
- Fixed items About (9000) and Exit (9001).

`TrackPopupMenu` is called with the `TPM_RIGHTBUTTON` alignment.

#### **5.4 Action Execution**
All commands (websites, tools, programs) are launched via `ShellExecuteW(NULL, L"open", ...)`. For power operations, a `MessageBoxW` confirmation is shown before execution. No further process control is attempted; the launched application runs independently.

#### **5.5 Memory and Safety**
- Global arrays for websites, tools, and discovered programs are statically allocated due to the low complexity. Maximum program count is 2000; further `.lnk` files are ignored.
- String operations use the safe `*_s` variants (`wcscpy_s`, `swprintf_s`) to prevent buffer overflows.
- COM is properly uninitialized on exit (`CoUninitialize`).

### **6. Usage Instructions**
1. Launch `websoftware.exe`. No window will appear.
2. Locate the new tray icon (a generic application icon) in the notification area. The tooltip reads *“Tool Launcher – Right‑click for menu”*.
3. **Right‑click** the icon to open the hierarchical menu.
4. Navigate through:
   - **Open Website** → category → specific site → opens in default browser.
   - **System Tools** → click a tool to launch it.
   - **Special Folders** → opens the corresponding folder in File Explorer.
   - **Installed Programs** → lists software discovered from the Start Menu; click to run.
   - **Power Control** → select an option; confirm in the dialog box to proceed.
5. To close the application, choose **Exit** from the menu. This removes the tray icon and terminates the process.

### **7. Known Limitations & Future Improvements**
- **Static program list**: Discovered software is only scanned at startup. Newly installed applications will not appear until the launcher is restarted.
- **Tray icon reintroduction**: If `explorer.exe` restarts, the tray icon is lost. A watchdog timer could re‑add the icon.
- **No configuration file**: Websites and tools are hardcoded. External JSON/XML configuration could be added.
- **Large Start Menu**: If more than 2000 shortcuts exist, only the first 2000 are shown. Dynamic allocation or pagination could handle larger sets.
- **Platform dependency**: Purely Windows‑only; not portable to other operating systems.

### **8. File Structure**
```
websoftware.c      – Single source file containing all logic.
websoftware.exe    – Compiled executable.
```

### **9. License and Distribution**
This software is provided as open source under the MIT License. You are free to use, modify, and distribute it, subject to the license terms. Attribution is appreciated but not required.

**End of Document**
