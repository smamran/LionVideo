#!/usr/bin/env python3
"""
Lion Media Player - Standalone Debian Package (.deb) Builder
Bundles all Qt6, multimedia, and third-party runtime dependencies into /opt/lionvideo/
so the resulting .deb installs and runs cleanly on any Debian/Ubuntu system with no external Qt dependencies.
"""

import os
import sys
import re
import shutil
import subprocess

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT_DIR = os.path.dirname(SCRIPT_DIR)
BUILD_DIR = os.path.join(ROOT_DIR, "build")
DIST_DIR = os.path.join(ROOT_DIR, "dist")
STAGING_DIR = os.path.join(BUILD_DIR, "staging-standalone-deb")

PACKAGE_NAME = "lionvideo"
VERSION = "2.0.0"
ARCH = "amd64"
DEB_FILENAME = f"{PACKAGE_NAME}_{VERSION}_{ARCH}.deb"
OUTPUT_DEB = os.path.join(DIST_DIR, DEB_FILENAME)

# System/Kernel/Hardware driver libraries that MUST be provided by the host OS
EXCLUDE_LIBS = {
    "linux-vdso.so.1",
    "ld-linux-x86-64.so.2",
    "libc.so.6",
    "libm.so.6",
    "libpthread.so.0",
    "libdl.so.2",
    "librt.so.1",
    "libresolv.so.2",
    "libgcc_s.so.1",
    "libstdc++.so.6",
    # Host X11 socket client libraries
    "libX11.so.6",
    "libX11-xcb.so.1",
    "libxcb.so.1",
    "libXau.so.6",
    "libXdmcp.so.6",
    "libdrm.so.2",
    # Host init & session bus
    "libsystemd.so.0",
    "libudev.so.1",
    "libasound.so.2",
    "libpulse.so.0",
}

def run_cmd(cmd, cwd=ROOT_DIR):
    print(f"  -> Running: {' '.join(cmd) if isinstance(cmd, list) else cmd}")
    subprocess.check_call(cmd, cwd=cwd, shell=isinstance(cmd, str))

def scan_dependencies(file_path):
    needed = set()
    try:
        out = subprocess.check_output(["ldd", file_path], text=True, stderr=subprocess.DEVNULL)
        for line in out.splitlines():
            m = re.search(r'\s*([^\s]+)\s+=>\s+([^\s]+)', line)
            if m:
                name, path = m.group(1), m.group(2)
                if name in EXCLUDE_LIBS:
                    continue
                if any(name.startswith(prefix) for prefix in [
                    "libc.so", "libm.so", "libpthread.so", "libdl.so",
                    "libdrm", "libX11", "libxcb", "ld-linux"
                ]):
                    continue
                if os.path.exists(path):
                    needed.add((name, os.path.realpath(path)))
    except Exception as e:
        print(f"Warning scanning {file_path}: {e}")
    return needed

def main():
    print("🦁 Building Self-Contained Standalone Debian (.deb) Package...")
    print(f"Project root: {ROOT_DIR}")

    # 1. Compile project
    if not os.path.exists(os.path.join(BUILD_DIR, "CMakeCache.txt")):
        print("⚙️  Configuring CMake (Release mode)...")
        run_cmd(["cmake", "-B", BUILD_DIR, "-S", ROOT_DIR, "-G", "Ninja", "-DCMAKE_BUILD_TYPE=Release"])

    print("🔨 Compiling LionVideo binary...")
    run_cmd(["cmake", "--build", BUILD_DIR, "-j", str(os.cpu_count() or 4)])

    binary_src = os.path.join(BUILD_DIR, "LionVideo")
    if not os.path.exists(binary_src):
        print(f"Error: Compiled binary not found at {binary_src}")
        sys.exit(1)

    # 2. Setup Staging Directory
    if os.path.exists(STAGING_DIR):
        shutil.rmtree(STAGING_DIR)

    opt_dir = os.path.join(STAGING_DIR, "opt", "lionvideo")
    bin_dir = os.path.join(opt_dir, "bin")
    lib_dir = os.path.join(opt_dir, "lib")
    plugins_dir = os.path.join(opt_dir, "plugins")
    debian_dir = os.path.join(STAGING_DIR, "DEBIAN")

    usr_bin = os.path.join(STAGING_DIR, "usr", "bin")
    usr_apps = os.path.join(STAGING_DIR, "usr", "share", "applications")
    usr_icons = os.path.join(STAGING_DIR, "usr", "share", "icons", "hicolor", "256x256", "apps")
    usr_pixmaps = os.path.join(STAGING_DIR, "usr", "share", "pixmaps")

    for d in [bin_dir, lib_dir, plugins_dir, debian_dir, usr_bin, usr_apps, usr_icons, usr_pixmaps]:
        os.makedirs(d, exist_ok=True)

    # 3. Copy LionVideo Binary & Launcher Script
    print("📦 Installing application files...")
    shutil.copy2(binary_src, os.path.join(bin_dir, "LionVideo"))
    os.chmod(os.path.join(bin_dir, "LionVideo"), 0o755)

    launcher_script_path = os.path.join(bin_dir, "lionvideo")
    with open(launcher_script_path, "w", encoding="utf-8") as f:
        f.write("""#!/bin/sh
# Lion Media Player - Standalone Launcher Wrapper
# Automatically routes Qt to bundled libraries and plugins in /opt/lionvideo

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
APP_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

export LD_LIBRARY_PATH="${APP_DIR}/lib:${LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="${APP_DIR}/plugins"
export QT_QPA_PLATFORM_PLUGIN_PATH="${APP_DIR}/plugins/platforms"

exec "${APP_DIR}/bin/LionVideo" "$@"
""")
    os.chmod(launcher_script_path, 0o755)

    # 4. Copy Qt Plugins
    print("🧩 Bundling Qt 6 plugins...")
    qt_plugin_dirs = [
        "platforms",
        "multimedia",
        "imageformats",
        "iconengines",
        "tls",
        "xcbglintegrations",
        "wayland-shell-integration",
        "wayland-decoration-client",
        "wayland-graphics-integration-client",
    ]
    qt_base_plugin = "/usr/lib/x86_64-linux-gnu/qt6/plugins"
    for pdir in qt_plugin_dirs:
        src_p = os.path.join(qt_base_plugin, pdir)
        if os.path.exists(src_p):
            dst_p = os.path.join(plugins_dir, pdir)
            shutil.copytree(src_p, dst_p)

    # 5. Collect and bundle all required shared libraries
    print("📚 Bundling shared libraries into /opt/lionvideo/lib/...")
    queue = [os.path.join(bin_dir, "LionVideo")]
    for root, _, files in os.walk(plugins_dir):
        for f in files:
            if f.endswith(".so"):
                queue.append(os.path.join(root, f))

    visited = set()
    bundled_libraries = set()

    while queue:
        target = queue.pop(0)
        if target in visited:
            continue
        visited.add(target)
        deps = scan_dependencies(target)
        for name, realpath in deps:
            if realpath not in visited:
                bundled_libraries.add((name, realpath))
                queue.append(realpath)

    for name, realpath in sorted(bundled_libraries):
        dst = os.path.join(lib_dir, name)
        if not os.path.exists(dst):
            shutil.copy2(realpath, dst)

    print(f"   Copied {len(bundled_libraries)} shared libraries into bundle.")

    # 6. Desktop Integration & Symlinks
    print("🖥️  Configuring desktop shortcuts, icons, and symlinks...")
    # Symlinks in /usr/bin/
    os.symlink("/opt/lionvideo/bin/lionvideo", os.path.join(usr_bin, "lionvideo"))
    os.symlink("/opt/lionvideo/bin/lionvideo", os.path.join(usr_bin, "LionVideo"))

    # Desktop file
    desktop_src = os.path.join(ROOT_DIR, "resources", "lionvideo.desktop")
    shutil.copy2(desktop_src, os.path.join(usr_apps, "lionvideo.desktop"))

    # Icons
    icon_src = os.path.join(ROOT_DIR, "resources", "player.png")
    shutil.copy2(icon_src, os.path.join(usr_icons, "lionvideo.png"))
    shutil.copy2(icon_src, os.path.join(usr_pixmaps, "lionvideo.png"))

    # 7. Maintainer Scripts (postinst / postrm)
    postinst_src = os.path.join(ROOT_DIR, "packaging", "debian", "postinst")
    postrm_src = os.path.join(ROOT_DIR, "packaging", "debian", "postrm")
    shutil.copy2(postinst_src, os.path.join(debian_dir, "postinst"))
    shutil.copy2(postrm_src, os.path.join(debian_dir, "postrm"))
    os.chmod(os.path.join(debian_dir, "postinst"), 0o755)
    os.chmod(os.path.join(debian_dir, "postrm"), 0o755)

    # 8. Calculate Installed-Size
    installed_size_kb = sum(
        os.lstat(os.path.join(dirpath, filename)).st_size
        for dirpath, _, filenames in os.walk(STAGING_DIR)
        for filename in filenames
    ) // 1024

    # 9. Write DEBIAN/control
    control_content = f"""Package: {PACKAGE_NAME}
Version: {VERSION}
Architecture: {ARCH}
Maintainer: LionVideo Project <info@lionvideo.org>
Installed-Size: {installed_size_kb}
Depends: libc6 (>= 2.34)
Recommends: libgl1, libegl1, libpulse0
Section: video
Priority: optional
Homepage: https://github.com/smamran/LionVideo
Description: Standalone high-performance media player built with Qt 6 and C++20
 Lion Media Player is a modern, fast desktop media player.
 This package is fully self-contained with bundled Qt 6, OpenGL dispatchers,
 and multimedia libraries in /opt/lionvideo. It installs cleanly with sudo dpkg -i
 with zero external Qt or OpenGL package dependencies required from APT.
"""
    with open(os.path.join(debian_dir, "control"), "w", encoding="utf-8") as f:
        f.write(control_content)

    # 10. Build .deb package with dpkg-deb
    print("📦 Compressing into standalone .deb package...")
    os.makedirs(DIST_DIR, exist_ok=True)
    run_cmd(["dpkg-deb", "-Zxz", "--root-owner-group", "-b", STAGING_DIR, OUTPUT_DEB])

    deb_size = os.path.getsize(OUTPUT_DEB)
    deb_size_mb = deb_size / (1024 * 1024)

    # Clean up staging directory
    shutil.rmtree(STAGING_DIR)

    # Calculate sha256
    sha256_out = subprocess.check_output(["sha256sum", OUTPUT_DEB], text=True).split()[0]

    print("\n" + "=" * 60)
    print("✅ Standalone Debian package successfully created!")
    print(f"   Output File: {OUTPUT_DEB}")
    print(f"   File Size:   {deb_size_mb:.2f} MB ({deb_size:,} bytes)")
    print(f"   SHA-256:     {sha256_out.upper()}")
    print("=" * 60)
    print("\nThis package has all Qt 6 and multimedia libraries bundled.")
    print("It can be installed directly on any Debian/Ubuntu system with:")
    print(f"   sudo dpkg -i dist/{DEB_FILENAME}")
    print("or:")
    print(f"   sudo apt install ./dist/{DEB_FILENAME}")
    print()

if __name__ == "__main__":
    main()
