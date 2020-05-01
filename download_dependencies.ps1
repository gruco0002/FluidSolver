$current = Get-Location

Set-Location src/dependencies
./download_dependencies.ps1

Set-Location $current

Set-Location src/engine/dependencies
./download_dependencies.ps1


Set-Location $current

Set-Location tests/dependencies
./download_dependencies.ps1
