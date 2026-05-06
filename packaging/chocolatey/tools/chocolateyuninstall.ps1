$ErrorActionPreference = 'Stop'

$packageName = 'elizaos-cpp'
$toolsDir = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"

# Remove from PATH
$installPath = Join-Path $toolsDir 'bin'
Uninstall-ChocolateyPath -PathToUninstall $installPath -PathType 'Machine'

Write-Host "ElizaOS C++ has been uninstalled successfully!"
