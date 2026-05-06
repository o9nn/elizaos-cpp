$ErrorActionPreference = 'Stop'

$packageName = 'elizaos-cpp'
$toolsDir = "$(Split-Path -Parent $MyInvocation.MyCommand.Definition)"
$installPath = Join-Path $toolsDir 'bin'

$payloadDirectories = @('bin', 'include', 'lib', 'share') |
  Where-Object { Test-Path (Join-Path $toolsDir $_) }

if (-not $payloadDirectories -or $payloadDirectories.Count -eq 0) {
  throw "No bundled ElizaOS C++ payload was found inside the Chocolatey package tools directory: $toolsDir"
}

Write-Host "Installing bundled ElizaOS C++ payload from: $toolsDir"
Write-Host "Detected payload directories: $($payloadDirectories -join ', ')"

if (Test-Path $installPath) {
  Install-ChocolateyPath -PathToInstall $installPath -PathType 'Machine'
  Write-Host "Added executables to PATH from: $installPath"
} else {
  Write-Warning "No bin directory was bundled with the package, so no PATH entry was added."
}

Write-Host "ElizaOS C++ has been installed successfully!"
Write-Host "Installation directory: $toolsDir"
Write-Host "To get started, see: https://github.com/o9nn/elizaos-cpp"
