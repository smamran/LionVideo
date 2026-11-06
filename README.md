# 🦁 Lion Media Player

A modern, fast, hardware-accelerated desktop media player crafted in **C++20** and **Qt 6**. Built with a sleek dark Fluent Design language, Lion Media Player delivers an immersive viewing experience with custom window framing, translucent floating controls, Picture-in-Picture mode with hold-and-drag screen repositioning, smart playlist queue management, and comprehensive keyboard/mouse shortcuts.

<p align="center">
  <img src="resources/app.png" alt="Lion Media Player UI" width="900" />
</p>

---

## 📦 Downloads & Verification

| Asset | Platform / OS | Type | Size | SHA-256 Checksum |
| :--- | :--- | :--- | :--- | :--- |
| **[`lionvideo_2.0.0_amd64.deb`](https://github.com/smamran/LionVideo/releases/download/v2.0.0/lionvideo_2.0.0_amd64.deb)** | Ubuntu / Debian (x86_64) | Standalone Debian Package | 74.04 MB | `E2501FF644D7EA491CE67984D0D87A84347123812D8B6EDC8B66768906B1EC78` |
| **[`LionVideo-2.0.0-Setup.exe`](https://github.com/smamran/LionVideo/releases/download/v2.0.0/LionVideo-2.0.0-Setup.exe)** | Windows 10 / 11 (x64) | Single-File Installer | 17.97 MB | `55A5356B8D474794B6E5FF326869D82A02AF8917B90172106AA0F49BC31F2381` |
| **[`LionVideo-2.0.0-windows-x64-portable.zip`](https://github.com/smamran/LionVideo/releases/download/v2.0.0/LionVideo-2.0.0-windows-x64-portable.zip)** | Windows 10 / 11 (x64) | Portable Archive | 22.31 MB | `4E3255BE4BDEB16550AD2041D444C46A61EBF80D0CC17E196526E3EF0FF68A6F` |

---

## 🚀 Getting Started

- **Ubuntu / Debian (`.deb` - Fully Self-Contained)**:
  Download `lionvideo_2.0.0_amd64.deb`. Because all Qt 6 and multimedia libraries are bundled directly inside the package, it installs seamlessly on fresh systems with **zero missing Qt dependency errors**:
  ```bash
  sudo apt install ./lionvideo_2.0.0_amd64.deb
  # or:
  sudo dpkg -i lionvideo_2.0.0_amd64.deb
  ```
  Launches with `lionvideo` from the terminal or via the **Lion Media Player** desktop icon.

- **Windows Installer (`Setup.exe`)**:
  Run `LionVideo-2.0.0-Setup.exe` to install to your system with desktop and Start Menu shortcuts (per-user or all-users, no admin privileges required).

- **Windows Portable (`.zip`)**:
  Extract `LionVideo-2.0.0-windows-x64-portable.zip` anywhere and run `LionVideo.exe` directly without installation.

---

## 📖 Table of Contents

- [📦 Downloads & Verification](#-downloads--verification)
- [🚀 Getting Started](#-getting-started)
- [User Guide & Features](#-user-guide--features)
  - [Core Playback & Seeking](#core-playback--seeking)
  - [Audio & Volume Control](#audio--volume-control)
  - [Playback Speed & Loops](#playback-speed--loops)
  - [File Opening & Play Queue](#file-opening--play-queue)
  - [Picture-in-Picture (PiP) Mode](#picture-in-picture-pip-mode)
  - [Fullscreen & Floating Overlays](#fullscreen--floating-overlays)
  - [On-Screen Display (OSD) Notifications](#on-screen-display-osd-notifications)
  - [Supported Media Formats](#supported-media-formats)
  - [Keyboard & Mouse Shortcuts Reference](#-keyboard--mouse-shortcuts-reference)
- [Developer Guide & Build Details](#-developer-guide--build-details)
  - [Architecture & Codebase Overview](#architecture--codebase-overview)
  - [Prerequisites & System Requirements](#prerequisites--system-requirements)
  - [Building from Source (Command Line)](#building-from-source-command-line)
  - [Ubuntu / Debian Packaging & Deployment (.deb)](#ubuntu--debian-packaging--deployment-deb)
  - [Building with Qt Creator](#building-with-qt-creator)
  - [Windows Deployment & Packaging](#windows-deployment--packaging)
  - [Technical Highlights](#technical-highlights)

---

# 🎬 User Guide & Features

### Core Playback & Seeking
- **Instant Play / Pause**: Click anywhere on the video screen, press `Spacebar`, or click the large central play button on the control bar.
- **Precision Interactive Seekbar**: Click or drag along the seekbar to jump instantly to any point in the media.
- **Timestamp Hover Preview**: Move your mouse over the seekbar to view a floating real-time preview bubble showing the exact target timestamp.
- **Keyboard Seeking**:
  - `Left Arrow` / `Right Arrow`: Seek backward / forward by 5 seconds.
  - `Shift + Left Arrow` / `Shift + Right Arrow`: Seek backward / forward by 15 seconds.
- **Track Navigation**:
  - `N`: Next track in queue.
  - `PageUp` or `Shift + P`: Previous track (or restarts track if past 3 seconds).

### Audio & Volume Control
- **Interactive Volume Slider**: Fine-tune volume with the slider on the control bar.
- **Scroll Wheel Volume**: Place the cursor over the video and scroll your mouse wheel up or down to adjust volume in 5% steps.
- **Arrow Keys**: Press `Up Arrow` (+5%) or `Down Arrow` (-5%) at any time to adjust volume.
- **One-Key Mute**: Press `M` or click the speaker icon to instantly mute/unmute audio.

### Playback Speed & Loops
- **Speed Presets**: Click the speed badge (`1.0x`) on the control bar to choose from speed presets (`0.5x`, `0.75x`, `1.0x Normal`, `1.25x`, `1.5x`, `2.0x`).
- **Granular Speed Shortcuts**: Press `[` to slow down by -0.25x or `]` to speed up by +0.25x.
- **Loop Modes**: Press `L` or click the Loop icon to cycle between:
  - **Loop Off**: Plays queue once and pauses upon completion.
  - **Loop All**: Cycles through the entire queue indefinitely.
  - **Loop Current**: Repeats the active media file continuously.
- **Shuffle Mode**: Press `S` or click the Shuffle icon to randomize queue playback order.

### File Opening & Play Queue
- **Startup "Browse Files" Button**:
  - When opening the player with no active media, an intuitive **Browse Files** button is prominently displayed in the center alongside drag-and-drop support.
- **Right-Click Context Menu (`Open (O)`)**:
  - Right-click anywhere on the video screen during playback (or on the empty screen) to open a quick context menu with **Open File... (O)**, **Add to Queue... (Ctrl+O)**, **Play / Pause**, **Picture-in-Picture (P)**, and **Fullscreen (F)**.
- **Normal Open (`O`)**:
  - Press `O`, click the Browse button, or drop files onto the video screen to open media.
  - Clears the previous queue, loads the selected file(s), and starts playback immediately.
  - Remembers the **last opened file directory** across application restarts.
- **Add to Queue (`Ctrl + O` or `+` Button)**:
  - Press `Ctrl + O`, click the `+` button in the Play Queue drawer, or drop files directly into the drawer to add files to the playlist.
  - Appends new items to the queue **without interrupting** current playback.
- **Duplicate Prevention**:
  - Automatically identifies identical files by path/URL and ignores duplicates so your queue remains clean and organized.
- **Play Queue Drawer (`Ctrl + Q`)**:
  - Toggle the right-hand slide-out drawer via `Ctrl + Q` or the queue icon.
  - View total track count, ordered list with filenames and duration tags.
  - Double-click any track to jump straight to it.
  - Right-click any track for context actions: *Play Track*, *Remove from Queue*, or *Show in File Explorer*.
  - Trash icon to clear the queue with a single click.

### Picture-in-Picture (PiP) Mode
- **Instant PiP Toggle (`P`)**: Press `P` or click the PiP button in the title bar or control bar to detach into an always-on-top compact window (`420x236`).
- **Always on Top on Both Ubuntu & Windows**:
  - On Windows, uses `Qt::WindowStaysOnTopHint` (`WS_EX_TOPMOST`).
  - On Ubuntu / Linux, sets `Qt::WindowStaysOnTopHint | Qt::Tool` and uses X11/XWayland (`_NET_WM_STATE_ABOVE`), ensuring GNOME Mutter and other Linux window managers keep the player pinned above all other desktop applications.
- **Hold & Drag Window Repositioning**:
  - Click and hold the left mouse button anywhere on the PiP player (video area or control bar) and drag to move the player freely across any monitor or desktop (supporting native Wayland/X11 system moves).
  - The cursor switches to a closed hand grab icon (`Qt::ClosedHandCursor`) during dragging.
  - Releasing after dragging will not trigger an accidental play/pause toggle.
- **Seamless Translucent Controls**: Sliders and buttons render transparently with no dark bounding boxes below the track.
- **Exit PiP**: Press `P`, `Esc`, or click the PiP button to restore the player to its previous window size and position.

### Fullscreen & Floating Overlays
- **Toggle Fullscreen (`F` or Double-Click)**: Enter a distraction-free, edge-to-edge viewing mode. Press `Esc` or `F` to return.
- **Auto-Hiding Controls**: When playing, the floating control bar smoothly fades out after 2.5 seconds of mouse inactivity, and the mouse cursor hides in fullscreen for a true cinematic experience. Moving the mouse wakes the controls immediately.

### On-Screen Display (OSD) Notifications
- Unobtrusive, animated notifications appear in the upper-right corner providing instant feedback for volume changes, seek deltas, playback rate, loop/shuffle toggles, track titles, and errors.

### Supported Media Formats
- **Video Containers & Codecs**: MP4, MKV, AVI, WEBM, FLV, MOV, M4V.
- **Audio Formats**: MP3, WAV, AAC, OGG, FLAC.
- Powered by Qt 6 Multimedia with the FFmpeg backend for wide codec support.

---

## ⌨ Keyboard & Mouse Shortcuts Reference

| Shortcut | Action | Description |
| :--- | :--- | :--- |
| `Space` | Play / Pause | Toggles playback state |
| `Left Arrow` | Seek -5s | Skip backward by 5 seconds |
| `Right Arrow` | Seek +5s | Skip forward by 5 seconds |
| `Shift + Left` | Seek -15s | Skip backward by 15 seconds |
| `Shift + Right` | Seek +15s | Skip forward by 15 seconds |
| `Up Arrow` | Volume Up | Increase audio volume by 5% |
| `Down Arrow` | Volume Down | Decrease audio volume by 5% |
| `Mouse Scroll` | Volume Adjust | Scroll up/down anywhere on the video area |
| `M` | Mute / Unmute | Toggles audio mute state |
| `Right Click` | Context Menu | Open context menu with Open (O), Add to Queue, PiP, etc. |
| `O` | Open Media | Opens file dialog in last directory & plays immediately |
| `Ctrl + O` | Add to Queue | Opens file dialog to append media to queue without stopping |
| `P` | Toggle PiP | Enter or exit Picture-in-Picture always-on-top mode |
| `Left Drag (PiP)` | Move Window | Click, hold, and drag player around the screen in PiP mode |
| `F` | Fullscreen | Toggle fullscreen mode |
| `Double Click` | Fullscreen | Toggle fullscreen mode |
| `Esc` | Exit / Close | Exit fullscreen, exit PiP, or close playlist drawer |
| `Ctrl + Q` | Play Queue | Open or close the playlist drawer |
| `N` | Next Track | Advance to next item in the queue |
| `PageUp` / `Shift+P`| Previous Track | Play previous item (or restart track if > 3s) |
| `L` | Loop Mode | Cycle loop modes: None ➔ All ➔ Current Track |
| `S` | Shuffle | Toggle randomized queue playback order |
| `[` | Speed -0.25x | Decrement playback rate (minimum 0.5x) |
| `]` | Speed +0.25x | Increment playback rate (maximum 2.0x) |

---

# 🛠 Developer Guide & Build Details

### Architecture & Codebase Overview

Lion Media Player is organized into distinct, modular subsystems separating core playback logic, custom UI controls, and utility services:

```
c:\dev\LionVideo\
├── CMakeLists.txt              # Primary CMake project configuration
├── main.cpp                    # Application bootstrap & FFmpeg backend setup
├── README.md                   # Project documentation & user/dev guide
├── resources/
│   ├── app.png                 # Application UI demonstration screenshot
│   ├── app.rc                  # Windows application resource file (icon & metadata)
│   ├── player.ico              # Multi-resolution Windows app icon
│   ├── player.png              # High-resolution PNG app icon
│   ├── player.webp             # Compressed WebP branding asset
│   ├── resources.qrc           # Qt resource bundle descriptor
│   └── icons/                  # 23 customized Fluent SVG icons
└── src/
    ├── core/
    │   ├── MediaPlayerEngine.h   # Playback engine controller
    │   └── MediaPlayerEngine.cpp # QMediaPlayer / QAudioOutput management & queue logic
    ├── ui/
    │   ├── MainWindow.h          # Main frameless container & overlay manager
    │   ├── MainWindow.cpp        # Global event filtering, shortcut routing, PiP logic
    │   ├── VideoWidget.h         # QGraphicsView hosting QGraphicsVideoItem
    │   ├── VideoWidget.cpp       # Aspect ratio scaling & PiP drag handler
    │   ├── ControlBar.h          # Floating translucent glass HUD
    │   ├── ControlBar.cpp        # Responsive playback controls & PiP dragging
    │   ├── ClickableSlider.h     # Custom antialiased seekbar
    │   ├── ClickableSlider.cpp   # Hover ghosting & time preview bubble
    │   ├── PlaylistDrawer.h      # Right-side slide-out playlist drawer
    │   ├── PlaylistDrawer.cpp    # Queue list widget, context menu & drop target
    │   ├── TitleBar.h            # Frameless title bar with minimize/maximize/close
    │   ├── TitleBar.cpp          # Windows window dragging & window controls
    │   ├── OSDLabel.h            # Animated floating notification banner
    │   └── OSDLabel.cpp          # Toast animations with QGraphicsOpacityEffect
    └── utils/
        ├── IconHelper.h          # SVG colorizing and QIcon cache
        ├── IconHelper.cpp        # High-DPI icon rasterizer
        ├── StyleManager.h        # Dark theme stylesheet generator
        ├── StyleManager.cpp      # Fluent Design QSS definitions
        ├── TimeFormatter.h       # Time string formatting
        └── TimeFormatter.cpp     # Formats milliseconds into mm:ss / hh:mm:ss
```

#### Key Architecture Highlights
- **Engine Subsystem (`core::MediaPlayerEngine`)**: Encapsulates `QMediaPlayer` and `QAudioOutput`. Maintains the playlist vector, handles track index state, manages duplicate elimination using canonical path comparison (`QDir::cleanPath`), and implements shuffle sequencing.
- **Event Filtering & Focus Isolation (`ui::MainWindow`)**: Employs an application-level event filter on `qApp` to intercept key combinations (`Space`, `Arrows`, `O`, `P`, `Esc`) before child widgets can consume them, while respecting active modal dialogs (`QFileDialog`) and popup menus.
- **Adaptive Floating Overlays**: `ControlBar`, `PlaylistDrawer`, and `OSDLabel` are children of `VideoWidget`, raised above the video scene. In PiP mode, secondary controls automatically collapse to ensure the volume seekbar and playback buttons fit cleanly without clipping.
- **Hardware-Accelerated Video Rendering**: Utilizes `QGraphicsVideoItem` rendered within an antialiased `QGraphicsView` for smooth letterboxed video scaling (`Qt::KeepAspectRatio`).

---

### Prerequisites & System Requirements

| Dependency | Minimum Version | Recommended | Notes |
| :--- | :--- | :--- | :--- |
| **OS** | Windows 10 (64-bit) / Ubuntu 22.04+ | Windows 11 (64-bit) / Ubuntu 24.04 LTS | Cross-platform (Windows & Linux) |
| **Compiler** | C++20 capable | LLVM Clang 17+ or GCC 11+ or MSVC 2022 | Must support C++20 standard |
| **CMake** | 3.20 | 3.28+ | Build system generator |
| **Build Tool** | Ninja 1.10+ or Make | Ninja 1.12+ | High-speed parallel builds |
| **Qt Framework** | Qt 6.2.0 | Qt 6.5+ or Qt 6.8+ | Modules: `Widgets`, `Multimedia`, `MultimediaWidgets`, `Svg` |
| **Multimedia Backend** | FFmpeg / GStreamer | Built-in Qt backend | Configured automatically per platform |

---

### Building on Ubuntu / Linux

#### 1. Install System Dependencies
Install compiler, CMake, and the required Qt 6 development libraries:
```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    libgl1-mesa-dev \
    qt6-base-dev \
    qt6-multimedia-dev \
    libqt6svg6-dev
```

> [!NOTE]
> On Ubuntu 22.04 LTS or 24.04 LTS, ensure multimedia codec plugins are available for media playback:
> ```bash
> sudo apt install -y \
>     gstreamer1.0-plugins-base \
>     gstreamer1.0-plugins-good \
>     gstreamer1.0-plugins-bad \
>     gstreamer1.0-plugins-ugly \
>     gstreamer1.0-libav
> ```

#### 2. Configure & Build
```bash
# Generate build configuration
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compile using all CPU cores
cmake --build build -j$(nproc)
```

The compiled binary will be placed at:
```bash
./build/LionVideo
```

#### 3. (Optional) Install to System
To install the application, desktop launcher entry, and icon:
```bash
sudo cmake --install build
```

---

### Ubuntu / Debian Packaging & Deployment (.deb)

Lion Media Player provides first-class packaging for **Ubuntu**, **Debian**, **Linux Mint**, **Pop!_OS**, and all Debian-based distributions. By default, the build script generates a **self-contained standalone package** with all runtime dependencies (Qt 6, multimedia plugins, and libraries) bundled inside `/opt/lionvideo/`.

#### 1. Packaging Architecture & Features
- **All-in-One Standalone Portability**: Bundles Qt 6 Core, Gui, Widgets, Multimedia, Svg, Network, DBus, and all plugins (`platforms`, `multimedia`, `imageformats`, `iconengines`, `tls`, `xcbglintegrations`, `wayland`) directly into `/opt/lionvideo/lib` and `/opt/lionvideo/plugins`.
- **Zero Dependency Headaches**: Does **not** require any Qt packages to be installed on the target machine. Only requires baseline system runtime (`libc6`, `libstdc++6`, `libgl1`, `libglx0`, `libopengl0`).
- **Standard Desktop Integration**:
  - Binary installed to `/opt/lionvideo/bin/LionVideo`
  - Automated launcher wrapper at `/opt/lionvideo/bin/lionvideo`
  - System CLI symlinks: `/usr/bin/lionvideo` and `/usr/bin/LionVideo`
  - FreeDesktop `.desktop` entry in `/usr/share/applications/lionvideo.desktop` with full MIME type associations (MP4, MKV, WebM, AVI, FLAC, MP3, etc.)
  - High-res icons installed to both `/usr/share/icons/hicolor/256x256/apps/lionvideo.png` and `/usr/share/pixmaps/lionvideo.png`
- **Maintainer Scripts (`postinst` / `postrm`)**: Automatically refreshes the desktop launcher database (`update-desktop-database`), icon theme cache (`gtk-update-icon-cache`), and MIME database (`update-mime-database`) upon installation and removal.

---

#### 2. Prerequisites
Ensure Debian packaging tools and development libraries are installed:
```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build dpkg-dev
```

---

#### 3. Building the Debian Package
You can generate the `.deb` package using any of the following approaches:

##### Option A: Dedicated Packaging Script (Recommended)
The automated script handles CMake configuration, compiles the binary, invokes CPack, and copies the package to `dist/`:
```bash
./scripts/build-deb.sh
```

##### Option B: CMake / Ninja Build Target
```bash
# Configure if not already done
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build the 'deb' target
cmake --build build --target deb
```

##### Option C: Direct CPack
```bash
cd build
cpack -G DEB
```

##### Option D: Zed Editor
Open the command palette (`Ctrl + Shift + P`), type **`task: spawn`**, and select **`CMake: Package Debian (.deb)`**.

---

#### 4. Package Artifact
The finalized package is placed in the `dist/` directory:
```bash
dist/lionvideo_2.0.0_amd64.deb
```

##### Inspecting the Package
You can inspect package metadata and file contents without installing:
```bash
# View control headers, package dependencies, and descriptions
dpkg-deb -I dist/lionvideo_2.0.0_amd64.deb

# View complete installed filesystem hierarchy
dpkg-deb -c dist/lionvideo_2.0.0_amd64.deb

# Extract package contents into a temporary directory
dpkg-deb -x dist/lionvideo_2.0.0_amd64.deb /tmp/lionvideo-pkg
```

---

#### 5. Deployment & Installation

##### Recommended: Install via APT (Auto-resolves all dependencies)
```bash
sudo apt update
sudo apt install ./dist/lionvideo_2.0.0_amd64.deb
```

##### Alternative: Install via dpkg
```bash
sudo dpkg -i dist/lionvideo_2.0.0_amd64.deb
# If any missing runtime dependencies:
sudo apt-get install -f
```

##### GUI Installation
Double-click `lionvideo_2.0.0_amd64.deb` in the Files file manager to open and install via **Ubuntu Software**, **GNOME Software**, or **GDebi**.

---

#### 6. Multimedia Codec Support Note
Qt 6 Multimedia leverages the system's multimedia backends (FFmpeg / GStreamer). On Ubuntu 22.04 / 24.04 / 26.04, ensure common multimedia plugins are present for playback of proprietary and modern formats (H.264/AVC, H.265/HEVC, AAC, etc.):
```bash
sudo apt install -y \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav
```

---

#### 7. Launching & Uninstallation

##### Launching the Player
```bash
# From terminal:
lionvideo
# Or:
LionVideo

# Or with a video file:
lionvideo ~/Videos/sample.mp4
```
You can also launch **Lion Media Player** directly from your application launcher / GNOME Dash.

##### Upgrading
To upgrade to a new release when a new `.deb` file is available:
```bash
sudo apt install ./dist/lionvideo_2.0.1_amd64.deb
```

##### Uninstallation
```bash
# Remove application (keeps system configuration)
sudo apt remove lionvideo

# Remove application along with all configurations
sudo apt purge lionvideo
```

---

### Building on Windows (Command Line)

#### 1. Open Terminal
Open **PowerShell** or **Command Prompt** with the compiler environment initialized (e.g. LLVM MinGW or MSVC vcvars64).

#### 2. Configure the Project
Create the build directory and run CMake:
```powershell
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
```

> [!TIP]
> If Qt is installed in a non-standard path, specify `CMAKE_PREFIX_PATH`:
> ```powershell
> cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.12.0/llvm-mingw_64"
> ```

#### 3. Compile the Executable
Execute the build with Ninja:
```powershell
cmake --build build --config Release
```

The compiled binary will be placed at:
```
build/LionVideo.exe
```

---

### Building with Qt Creator

1. Open **Qt Creator**.
2. Select **File ➔ Open File or Project...** and choose `c:\dev\LionVideo\CMakeLists.txt`.
3. Select your installed **Qt 6 Desktop Kit** (e.g. *Desktop Qt 6.12.0 LLVM-MinGW 64-bit*).
4. Click **Configure Project**.
5. Build and run using `Ctrl + R` or click the green **Run** button.

---

### Working with Zed Editor (Ubuntu & Windows)

Lion Media Player is fully configured for [Zed Editor](https://zed.dev/) out of the box with relative cross-platform configuration:

1. **Open the Project**:
   - In Zed, select **File ➔ Open Folder...** and choose the `LionVideo` repository directory.
2. **Language Server Intelligence (`clangd`)**:
   - Autocomplete, definitions, refactoring, and inline diagnostics are powered by `clangd` via `.clangd` and `.zed/settings.json`.
   - `compile_commands.json` is exported into `build/` automatically.
3. **Running Tasks in Zed**:
   - Press `Ctrl + Shift + P` (or `Cmd + Shift + P`), type **`task: spawn`** (or press `Ctrl + T`).
   - Select from preconfigured tasks:
     - **`CMake: Configure (Release)`** / **`CMake: Configure (Debug)`**
     - **`CMake: Build`**
     - **`CMake: Run`** (builds and launches the player on both Ubuntu and Windows)
     - **`CMake: Clean`**
4. **Zero-Configuration Qt Discovery**:
   - **Ubuntu**: Uses default system Qt 6 packages (`/usr/lib/...`) automatically.
   - **Windows**: Automatically detects installed Qt 6 (`C:/Programs/Qt/...` or `C:/Qt/...`) without requiring manual `-DCMAKE_PREFIX_PATH` arguments.

---

### Windows Deployment & Packaging

To run the application standalone on machines without Qt installed:

1. Locate the built `LionVideo.exe` in your Release build directory.
2. Run `windeployqt` to copy all required Qt DLLs and plugins:
```powershell
windeployqt --release --qmldir . build/LionVideo.exe
```
3. Ensure the following multimedia and SVG plugins are present in the folder:
   - `multimedia/`
   - `iconengines/`
   - `imageformats/`
   - `platforms/qwindows.dll`

---

### Technical Highlights

- **Frameless Window with Native Snapping**: On Windows, `MainWindow::nativeEvent` processes `WM_NCHITTEST` messages, allowing true native edge/corner resizing, Aero Snap window docking, and smooth window gestures without native OS title bars.
- **Transparent Control Styling (`WA_TranslucentBackground`)**: Custom sliders (`ClickableSlider` and `QSlider`) and `ControlBar` apply `Qt::WA_TranslucentBackground` and `Qt::WA_NoSystemBackground` alongside custom QSS so rounded borders and slider tracks float gracefully over video frames with zero black bounding boxes.
- **Robust Audio Output Routing**: `MediaPlayerEngine` synchronizes `QAudioOutput` with volume clamp boundaries `[0, 100]`, non-linear scaling, and automatic unmuting upon slider interaction.
- **Path Canonicalization**: Queue deduplication utilizes `QDir::cleanPath` with case-insensitive matching to prevent identical files on Windows from entering the queue under alternate drive letter casings or separator styles.

---

## 📄 License

Lion Media Player is open source software. Built for modern desktop performance and multimedia clarity.
