# pypmc-cli — An npm-like Package Manager for Python (C Edition)

**pypmc** is a fast, native C reimplementation of [pynpm-cli](https://github.com/subh-skd/pynpm-cli). It provides npm-style dependency management for Python projects using `pip` under the hood — with zero runtime dependencies beyond a C compiler to build.

## Features

- **`pypmc init`** — Initialize a project with `package.yml` and `.venv` (like `npm init`)
- **`pypmc install <pkg>`** — Install a package and save it to `package.yml` (like `npm install`)
- **`pypmc install`** — Install all dependencies from `package.yml` / lockfile (like `npm install`)
- **`pypmc uninstall <pkg>`** — Remove a package and clean up orphaned transitive deps
- **`pypmc update [pkg]`** — Update a package (or all) to the latest version
- **`pypmc outdated`** — Check which packages have newer versions available
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

**Windows (PowerShell) — run as Administrator:**

```powershell
irm https://raw.githubusercontent.com/subh-skd/pypmc-cli/main/install.ps1 | iex
```

**Windows (CMD) — run as Administrator:**

```cmd
curl -fsSL https://raw.githubusercontent.com/subh-skd/pypmc-cli/main/install.bat -o %TEMP%\install-pypmc.bat && %TEMP%\install-pypmc.bat
```

> **Note:** On Windows, the installer requires **Administrator privileges** to install to `C:\Program Files\pypmc` and add it to the system PATH. Right-click your terminal and select **"Run as administrator"** before running the command.

The installer downloads the latest release binary, places it in the correct location, and adds it to your system PATH automatically.

### Post-install: Adding to PATH

The install scripts add pypmc to your PATH automatically. If `pypmc` is not recognized after installation, **restart your terminal** and try again. If it still doesn't work, add the install location manually:

| Platform | Install location | What to add to PATH |
|---|---|---|
| Linux | `/usr/local/bin/pypmc` | Usually already in PATH |
| macOS | `/usr/local/bin/pypmc` | Usually already in PATH |
| Windows | `%ProgramFiles%\pypmc\pypmc.exe` | `%ProgramFiles%\pypmc` |

<details>
<summary><b>Linux / macOS — manual PATH setup</b></summary>

```bash
# bash
echo 'export PATH="/usr/local/bin:$PATH"' >> ~/.bashrc && source ~/.bashrc

# zsh
echo 'export PATH="/usr/local/bin:$PATH"' >> ~/.zshrc && source ~/.zshrc

# fish
fish_add_path /usr/local/bin
```

</details>

<details>
<summary><b>Windows — manual PATH setup</b></summary>

**Option 1: Via GUI**
1. Press `Win + R`, type `sysdm.cpl`, press Enter
2. Go to **Advanced** tab > **Environment Variables**
3. Under **System variables**, select `Path` and click **Edit**
4. Click **New** and add: `%ProgramFiles%\pypmc`
5. Click OK, then restart your terminal

**Option 2: Via CMD (run as Administrator)**
```cmd
reg add "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_EXPAND_SZ /d "%Path%;%ProgramFiles%\pypmc" /f
```

**Option 3: Via PowerShell (run as Administrator)**
```powershell
[Environment]::SetEnvironmentVariable("Path", "$([Environment]::GetEnvironmentVariable('Path', 'Machine'));$env:ProgramFiles\pypmc", "Machine")
```

</details>

### Manual download

Download the latest binary from the [Releases](../../releases) page:

| Platform | Binary |
|---|---|
| Linux (x64) | `pypmc-linux-amd64` |
| Linux (ARM64) | `pypmc-linux-arm64` |
| macOS (x64) | `pypmc-macos-amd64` |
| macOS (Apple Silicon) | `pypmc-macos-arm64` |
| Windows (x64) | `pypmc-windows-amd64.exe` |

```bash
# Linux / macOS
chmod +x pypmc-linux-amd64
sudo mv pypmc-linux-amd64 /usr/local/bin/pypmc

# Windows (CMD, run as Administrator) — download and install manually
mkdir "%ProgramFiles%\pypmc"
move pypmc-windows-amd64.exe "%ProgramFiles%\pypmc\pypmc.exe"
reg add "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_EXPAND_SZ /d "%Path%;%ProgramFiles%\pypmc" /f
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

# Update a single package
pypmc update requests

# Update all packages
pypmc update

# Check for outdated packages
pypmc outdated

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
5. **`pypmc update [pkg]`** upgrades to latest via `pip install --upgrade` and updates `package.yml` + lockfile
6. **`pypmc outdated`** queries PyPI via pip for newer versions and displays a table

## Shell Completions

pypmc supports tab-completion for bash, zsh, and fish. Completions cover commands, flags, package names from `package.yml`, and script names.

**bash:**
```bash
eval "$(pypmc completions bash)"
# Or add to ~/.bashrc for persistence
echo 'eval "$(pypmc completions bash)"' >> ~/.bashrc
```

**zsh:**
```zsh
eval "$(pypmc completions zsh)"
# Or add to ~/.zshrc for persistence
echo 'eval "$(pypmc completions zsh)"' >> ~/.zshrc
```

**fish:**
```fish
pypmc completions fish | source
# Or save permanently
pypmc completions fish > ~/.config/fish/completions/pypmc.fish
```

## Publishing

This project uses GitHub Actions to automatically build cross-platform binaries. To publish:

1. Push your code to GitHub
2. Create a new Release — the workflow builds Linux (amd64/arm64), macOS (amd64/arm64), and Windows binaries and attaches them to the release
3. Users download the binary for their platform, no compilation needed

## License

MIT
