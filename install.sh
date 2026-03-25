#!/usr/bin/env bash
# pypmc installer for Linux and macOS
# Usage: curl -fsSL https://raw.githubusercontent.com/subh-skd/pypmc-cli/main/install.sh | bash
set -euo pipefail

REPO="subh-skd/pypmc-cli"
INSTALL_DIR="/usr/local/bin"
BIN_NAME="pypmc"

# ── detect platform ──────────────────────────────────────────────────

OS="$(uname -s)"
ARCH="$(uname -m)"

case "$OS" in
    Linux)  PLATFORM="linux" ;;
    Darwin) PLATFORM="macos" ;;
    *)
        echo "Error: Unsupported OS: $OS"
        echo "pypmc supports Linux and macOS. For Windows, use install.ps1."
        exit 1
        ;;
esac

case "$ARCH" in
    x86_64|amd64) ARCH_SUFFIX="amd64" ;;
    *)
        echo "Error: Unsupported architecture: $ARCH"
        echo "pypmc currently provides pre-built binaries for x86_64 only."
        exit 1
        ;;
esac

ASSET_NAME="pypmc-${PLATFORM}-${ARCH_SUFFIX}"

# ── fetch latest release tag ─────────────────────────────────────────

echo "Detecting latest pypmc release..."

LATEST_URL="https://api.github.com/repos/${REPO}/releases/latest"
if command -v curl &>/dev/null; then
    RELEASE_TAG=$(curl -fsSL "$LATEST_URL" | grep '"tag_name"' | head -1 | sed -E 's/.*"tag_name":\s*"([^"]+)".*/\1/')
elif command -v wget &>/dev/null; then
    RELEASE_TAG=$(wget -qO- "$LATEST_URL" | grep '"tag_name"' | head -1 | sed -E 's/.*"tag_name":\s*"([^"]+)".*/\1/')
else
    echo "Error: curl or wget is required."
    exit 1
fi

if [ -z "$RELEASE_TAG" ]; then
    echo "Error: Could not determine latest release."
    echo "Check https://github.com/${REPO}/releases manually."
    exit 1
fi

echo "Latest version: $RELEASE_TAG"

# ── download binary ──────────────────────────────────────────────────

DOWNLOAD_URL="https://github.com/${REPO}/releases/download/${RELEASE_TAG}/${ASSET_NAME}"
TMPFILE="$(mktemp)"

echo "Downloading ${ASSET_NAME}..."

if command -v curl &>/dev/null; then
    curl -fSL --progress-bar -o "$TMPFILE" "$DOWNLOAD_URL"
elif command -v wget &>/dev/null; then
    wget -q --show-progress -O "$TMPFILE" "$DOWNLOAD_URL"
fi

chmod +x "$TMPFILE"

# ── install ──────────────────────────────────────────────────────────

echo "Installing to ${INSTALL_DIR}/${BIN_NAME}..."

if [ -w "$INSTALL_DIR" ]; then
    mv "$TMPFILE" "${INSTALL_DIR}/${BIN_NAME}"
else
    sudo mv "$TMPFILE" "${INSTALL_DIR}/${BIN_NAME}"
fi

# ── verify ───────────────────────────────────────────────────────────

if command -v "$BIN_NAME" &>/dev/null; then
    echo ""
    echo "pypmc installed successfully!"
    "$BIN_NAME" --version
    echo ""
    echo "Get started:"
    echo "  mkdir my-project && cd my-project"
    echo "  pypmc init"
else
    echo ""
    echo "pypmc was installed to ${INSTALL_DIR}/${BIN_NAME}"
    echo "Make sure ${INSTALL_DIR} is in your PATH."
fi
