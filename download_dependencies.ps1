& {
Set-Location src/dependencies
. download_dependencies.ps1
}

& {
Set-Location test/dependencies
. download_dependencies.ps1
}