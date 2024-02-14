$ErrorActionPreference = "Stop"

if (-not $args) {
    $vcpkg_triplet = "" # this line can be replaced during build stage
    if (-not $vcpkg_triplet) {
        Write-Host "Usage: . ./setup_env.sh [<directory>]" -ForegroundColor Red
        Exit 1
    }
    $installed_dir = Split-Path -Parent $MyInvocation.MyCommand.Path
}
else {
    $installed_dir = (Resolve-Path $args[0]).Path
}

if (-not (Test-Path "$installed_dir\tools\docwire.exe")) {
    Write-Host "Error: directory `"$installed_dir`" does not look correctly." -ForegroundColor Red
    Exit 1
}

$env:PATH = "$installed_dir\tools;$installed_dir\bin;$env:PATH"
$env:CPLUS_INCLUDE_PATH = "$installed_dir\include;$env:CPLUS_INCLUDE_PATH"
$env:LIBRARY_PATH = "$installed_dir\lib;$env:LIBRARY_PATH"
$env:OSSL_MODULES = "$installed_dir\bin"

Write-Host "The environment has been configured for DocWire SDK installed in $installed_dir."
