$ErrorActionPreference = 'Stop'

$packageName = 'elizaos-cpp'
$toolsDir = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url64 = 'https://github.com/o9nn/elizaos-cpp/releases/download/v1.0.0/elizaos-cpp-1.0.0-win64.zip'

$packageArgs = @{
  packageName   = $packageName
  unzipLocation = $toolsDir
  url64bit      = $url64
  checksum64    = ''
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs

# Add to PATH
$installPath = Join-Path $toolsDir 'bin'
Install-ChocolateyPath -PathToInstall $installPath -PathType 'Machine'

Write-Host "ElizaOS C++ has been installed successfully!"
Write-Host "Installation directory: $toolsDir"
Write-Host "Executables are available in: $installPath"
Write-Host ""
Write-Host "To get started, see: https://github.com/o9nn/elizaos-cpp"
