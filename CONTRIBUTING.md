# Contributing to pypmc-cli

Thank you for your interest in contributing! pypmc is a native C project, so contributions range from bug fixes and new commands to documentation and packaging improvements.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Project Structure](#project-structure)
- [Building from Source](#building-from-source)
- [Making Changes](#making-changes)
- [Submitting a Pull Request](#submitting-a-pull-request)
- [Reporting Bugs](#reporting-bugs)
- [Suggesting Features](#suggesting-features)

---

## Code of Conduct

Be respectful, constructive, and welcoming. Harassment of any kind will not be tolerated.

---

## Getting Started

### Prerequisites

- A C99-compatible compiler: `gcc`, `clang`, or MSVC
- CMake 3.10+
- Python 3.8+ (for integration testing)
- Git

### Fork & Clone

```bash
git clone https://github.com/subh-skd/pypmc-cli.git
cd pypmc-cli
```

---

## Project Structure

```
pypmc-cli/
├── src/
│   ├── main.c        # Entry point — command dispatch
│   ├── core.c / core.h   # Install, uninstall, update, list, outdated, run
│   ├── config.c / config.h  # package.yml & lockfile read/write
│   └── venv.c / venv.h   # Virtual environment creation & activation
├── .github/
│   └── workflows/
│       ├── test.yml      # CI: build & verify on Linux, macOS, Windows
│       └── release.yml   # Release: cross-compile & upload binaries
├── CMakeLists.txt
├── Makefile
└── pypmc-docs/       # Documentation site (React/Vite)
```

The codebase is intentionally small (~2,300 lines of C99). Each source file maps directly to a concern — read `core.c` for command logic and `config.c` for file I/O.

---

## Building from Source

```bash
# CMake (recommended, all platforms)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Make shortcut (Linux/macOS)
make
```

The binary is output to `build/pypmc` (or `build/Release/pypmc.exe` on Windows).

### Running the binary

```bash
./build/pypmc --version
./build/pypmc init
```

### CI checks

Every push and pull request runs the `Build & Test` workflow on Ubuntu, macOS, and Windows. Your branch must pass all three before merging.

---

## Making Changes

### Branching

Create a branch from `main` with a descriptive name:

```bash
git checkout -b fix/lockfile-parse-error
git checkout -b feat/add-freeze-command
```

### Coding style

- C99 standard — no compiler extensions
- 4-space indentation, no tabs
- Keep functions focused and small
- Use descriptive variable names; avoid single-letter names outside loop counters
- Add a comment when the logic is not immediately obvious
- No external libraries — the zero-dependency constraint is intentional

### Commit messages

Use the imperative mood and keep the subject line under 72 characters:

```
fix: handle missing package.yml gracefully
feat: add pypmc freeze command
docs: update installation instructions for Windows
```

---

## Submitting a Pull Request

1. Ensure the build passes on your machine for all relevant platforms
2. Keep your PR focused — one feature or fix per PR
3. Describe **what** changed and **why** in the PR description
4. Reference any related issues with `Fixes #123` or `Closes #123`
5. Be responsive to review feedback — small iterations are welcome

PRs that break the CI build or introduce external dependencies will not be merged.

---

## Reporting Bugs

Open an issue at [github.com/subh-skd/pypmc-cli/issues](https://github.com/subh-skd/pypmc-cli/issues) and include:

- Your OS and architecture (e.g. Ubuntu 22.04 x64, Windows 11 ARM)
- The pypmc version (`pypmc --version`)
- The exact command you ran
- The full output / error message
- Your `package.yml` if relevant (redact sensitive values)

---

## Suggesting Features

Open an issue with the `enhancement` label. Describe:

- The problem you're trying to solve
- Your proposed solution or command syntax
- Any alternatives you considered

Feature requests that align with the project's goal — a fast, zero-dependency, npm-like workflow for Python — are most likely to be accepted.

---

## License

By contributing, you agree that your contributions will be licensed under the [MIT License](LICENSE).
