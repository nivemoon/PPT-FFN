# PPT-FFN
Global push-to-talk for default microphone on Windows (C, WinAPI).
## Implementation details

- Language: **C** (plain C, without C++).
- Platform: Windows 10/11.
- APIs:
  - WinAPI (`windows.h`)
  - MMDevice API (`mmdeviceapi.h`)
  - `IAudioEndpointVolume` (`endpointvolume.h`) for mic mute/unmute
- Toolchain: **MinGW‑w64 gcc** (tested with x86_64, ucrt, posix, seh).

### Components

- `micctl.exe`  
  Small CLI tool that mutes or unmutes the default communications
  capture device using `IAudioEndpointVolume_SetMute`.

- `pttffn.exe`  
  Tray application with a low-level mouse hook (`WH_MOUSE_LL`) that:
  - listens for mouse XButton1/XButton2 press/release,
  - calls `micctl.exe 0` on press (unmute),
  - calls `micctl.exe 1` on release (mute),
  - runs as a background tray app (no taskbar / console window).

No Python, AutoHotkey or other scripting engines are required.
Everything is native C code compiled to standalone executables.
