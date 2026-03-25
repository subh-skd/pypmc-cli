# pypmc-cli — An npm-like Package Manager for Python (C Edition)

**pypmc** is a fast, native C reimplementation of [pynpm-cli](https://github.com/subh-skd/pynpm-cli). It provides npm-style dependency management for Python projects using `pip` under the hood — with zero runtime dependencies beyond a C compiler to build.

## Features

- **`pypmc init`** — Initialize a project with `package.yml` and `.venv` (like `npm init`)
- **`pypmc install <pkg>`** — Install a package and save it to `package.yml` (like `npm install`)
- **`pypmc install`** — Install all dependencies from `package.yml` / lockfile (like `npm install`)
- **`pypmc uninstall <pkg>`** — Remove a package and clean up orphaned transitive deps
- **`pypmc list`** — Show project dependencies and their installed versions
- **`pypmc run <script>`** — Run scripts defined in `package.yml` (like `npm run`)
- **Automatic `.venv`** — Creates and uses a virtual environment automatically
- **Lockfile** — `package-lock.yml` locks exact versions for reproducible installs
- **Orphan cleanup** — Uninstalling a package also removes its unused transitive dependencies

## Installation

### One-line install (recommended)

**Linux / macOS:**

```bash
curl -fsSL https://raw.githubusercontent.com/subh-skd/pypmc-cli/main/install.sh | bash
```

**Windows (PowerShell):**

```powershell
irm https://raw.githubusercontent.com/subh-skd/pypmc-cli/main/install.ps1 | iex
```

This downloads the latest release binary for your platform and installs it to your PATH automatically.

### Manual download

Download the latest binary from the [Releases](../../releases) page:

| Platform | Binary | Install location |
|---|---|---|
| Linux (x64) | `pypmc-linux-amd64` | `/usr/local/bin/pypmc` |
| macOS (x64) | `pypmc-macos-amd64` | `/usr/local/bin/pypmc` |
| Windows (x64) | `pypmc-windows-amd64.exe` | `%LOCALAPPDATA%\pypmc\pypmc.exe` |

```bash
# Linux / macOS
chmod +x pypmc-linux-amd64
sudo mv pypmc-linux-amd64 /usr/local/bin/pypmc

# Windows — rename to pypmc.exe and add its folder to PATH
```

### Build from source

Requires a C compiler (gcc, clang, or MSVC) and CMake 3.10+.

```bash
# Using CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Or using Make (Linux/macOS)
make
```

The binary is at `build/pypmc` (or `build/Release/pypmc.exe` on Windows).

## Quick Start

```bash
# Create a new project
mkdir my-project && cd my-project
pypmc init

# Install packages
pypmc install requests
pypmc install flask

# Install dev dependencies
pypmc install -D pytest

# Install all deps from package.yml (like npm install)
pypmc install

# Run a script
pypmc run test

# Remove a package
pypmc uninstall flask

# List dependencies
pypmc list
```

## package.yml

Your project configuration lives in `package.yml`:

```yaml
name: my-project
version: 1.0.0
description: My awesome project
author: Your Name
license: MIT
python: '>=3.8'

scripts:
  start: python main.py
  test: pytest

dependencies:
  requests: '==2.31.0'
  flask: '==3.0.0'

dev_dependencies:
  pytest: '==8.0.0'
```

## package-lock.yml

The lockfile captures exact versions of all installed packages for reproducible installs:

```yaml
lockfile_version: 1
packages:
  requests:
    version: '2.31.0'
  urllib3:
    version: '2.1.0'
  charset-normalizer:
    version: '3.3.2'
```

## Command Aliases

| Full Command | Alias |
|---|---|
| `pypmc install` | `pypmc i` |
| `pypmc uninstall` | `pypmc rm` |

## How It Works

1. **`pypmc init`** creates `package.yml`, `.venv`, `main.py`, and `.gitignore`
2. **`pypmc install <pkg>`** installs into `.venv` via pip, records the direct dependency in `package.yml`, and snapshots all installed versions into `package-lock.yml`
3. **`pypmc uninstall <pkg>`** removes the package, cleans up orphaned transitive deps, and updates both config files
4. **`pypmc install`** (no args) reads `package-lock.yml` for exact versions, or falls back to `package.yml` version specs

## Publishing

This project uses GitHub Actions to automatically build cross-platform binaries. To publish:

1. Push your code to GitHub
2. Create a new Release — the workflow builds Linux, macOS, and Windows binaries and attaches them to the release
3. Users download the binary for their platform, no compilation needed

## License

MIT
