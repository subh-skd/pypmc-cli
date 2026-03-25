# pypmc installer for Windows
# Usage: irm https://raw.githubusercontent.com/subh-skd/pypmc-cli/main/install.ps1 | iex
$ErrorActionPreference = "Stop"

$Repo = "subh-skd/pypmc-cli"
$BinName = "pypmc.exe"
$InstallDir = "$env:LOCALAPPDATA\pypmc"
$AssetName = "pypmc-windows-amd64.exe"

# ── fetch latest release tag ─────────────────────────────────────────

Write-Host "Detecting latest pypmc release..." -ForegroundColor Cyan

$ReleaseUrl = "https://api.github.com/repos/$Repo/releases/latest"
try {
    $Release = Invoke-RestMethod -Uri $ReleaseUrl -UseBasicParsing
    $Tag = $Release.tag_name
} catch {
    Write-Host "Error: Could not fetch latest release from GitHub." -ForegroundColor Red
    Write-Host "Check https://github.com/$Repo/releases manually."
    exit 1
}

Write-Host "Latest version: $Tag"

# ── download binary ──────────────────────────────────────────────────

$DownloadUrl = "https://github.com/$Repo/releases/download/$Tag/$AssetName"
$TmpFile = Join-Path $env:TEMP "pypmc-download.exe"

Write-Host "Downloading $AssetName..."

try {
    Invoke-WebRequest -Uri $DownloadUrl -OutFile $TmpFile -UseBasicParsing
} catch {
    Write-Host "Error: Failed to download from $DownloadUrl" -ForegroundColor Red
    exit 1
}

# ── install ──────────────────────────────────────────────────────────

Write-Host "Installing to $InstallDir..."

if (-not (Test-Path $InstallDir)) {
    New-Item -ItemType Directory -Path $InstallDir -Force | Out-Null
}

$DestPath = Join-Path $InstallDir $BinName
Move-Item -Path $TmpFile -Destination $DestPath -Force

# ── add to PATH if not already there ────────────────────────────────

$UserPath = [Environment]::GetEnvironmentVariable("Path", "User")
if ($UserPath -notlike "*$InstallDir*") {
    Write-Host "Adding $InstallDir to user PATH..."
    [Environment]::SetEnvironmentVariable("Path", "$UserPath;$InstallDir", "User")
    $env:Path = "$env:Path;$InstallDir"
}

# ── verify ───────────────────────────────────────────────────────────

Write-Host ""
try {
    $Version = & $DestPath --version 2>&1
    Write-Host "pypmc installed successfully!" -ForegroundColor Green
    Write-Host $Version
} catch {
    Write-Host "pypmc was installed to $DestPath" -ForegroundColor Green
}

Write-Host ""
Write-Host "Get started:" -ForegroundColor Cyan
Write-Host "  mkdir my-project; cd my-project"
Write-Host "  pypmc init"
Write-Host ""
Write-Host "Note: Restart your terminal for PATH changes to take effect." -ForegroundColor Yellow
