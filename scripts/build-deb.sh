#!/usr/bin/env bash
set -e

# -----------------------------------------------------------------------------
# Lion Media Player - Debian/Ubuntu (.deb) Packaging Script
# -----------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
DIST_DIR="${ROOT_DIR}/dist"

echo "🦁 Building Lion Media Player Debian (.deb) package..."
echo "Project root: ${ROOT_DIR}"

# 1. Ensure build directory is configured
if [ ! -f "${BUILD_DIR}/CMakeCache.txt" ]; then
    echo "⚙️  Configuring CMake (Release mode)..."
    cmake -B "${BUILD_DIR}" -S "${ROOT_DIR}" -G Ninja -DCMAKE_BUILD_TYPE=Release
fi

# 2. Build target LionVideo
echo "🔨 Compiling LionVideo..."
cmake --build "${BUILD_DIR}" -j"$(nproc)"

# 3. Create dist directory
mkdir -p "${DIST_DIR}"

# 4. Generate .deb package with CPack
echo "📦 Packaging with CPack (DEB generator)..."
cd "${BUILD_DIR}"
cpack -G DEB

# 5. Copy generated .deb file(s) to dist/
cp -f "${BUILD_DIR}"/lionvideo_*.deb "${DIST_DIR}/"

DEB_FILE=$(ls -t "${DIST_DIR}"/lionvideo_*.deb | head -n 1)
DEB_FILENAME=$(basename "${DEB_FILE}")
DEB_SIZE=$(du -h "${DEB_FILE}" | cut -f1)

echo ""
echo "✅ Debian package successfully created!"
echo "   File: ${DIST_DIR}/${DEB_FILENAME}"
echo "   Size: ${DEB_SIZE}"
echo ""
echo "To inspect package contents:"
echo "   dpkg-deb -I dist/${DEB_FILENAME}   # View control info & dependencies"
echo "   dpkg-deb -c dist/${DEB_FILENAME}   # List installed files"
echo ""
echo "To install on Ubuntu/Debian:"
echo "   sudo apt install ./dist/${DEB_FILENAME}"
echo "or:"
echo "   sudo dpkg -i dist/${DEB_FILENAME}"
echo "   sudo apt-get install -f            # If any missing dependencies"
echo ""
