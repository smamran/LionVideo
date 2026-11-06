#!/usr/bin/env bash
set -e

# -----------------------------------------------------------------------------
# Lion Media Player - Debian/Ubuntu (.deb) Packaging Script
# Defaults to building the self-contained standalone .deb with all dependencies
# -----------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

# Run standalone builder (bundles all Qt6 and multimedia dependencies into /opt/lionvideo)
exec python3 "${SCRIPT_DIR}/build-standalone-deb.py" "$@"

