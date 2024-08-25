$ErrorActionPreference = "Stop"

<#
.SYNOPSIS
Invokes an external command and fails the script if it exits with a non-zero exit code.

.DESCRIPTION
The invoke-externalcommand function is needed because PowerShell does not fail a script if a command it invokes exits with a non-zero exit code.
This function works around that behavior by invoking the command and then checking the LASTEXITCODE variable.
If the exit code is not zero, an error is thrown.

.EXAMPLE
Invoke-ExternalCommand { git commit -m "My commit message" }

This command will invoke 'git commit' with the given arguments and fail the script if it exits with a non-zero exit code.
#>
function Invoke-ExternalCommand {
    param(
        [ScriptBlock]$ScriptBlock
    )
    & $ScriptBlock
    if ($LASTEXITCODE -ne 0) {
        $errorMessage = "Command { $ScriptBlock } failed with exit code $LASTEXITCODE."
        throw $errorMessage
    }
}

if ($env:DOWNLOAD_VCPKG -ne "0")
{
    if (Test-Path vcpkg)
    {
        Write-Host "Error: vcpkg directory already exists. Remove it or set DOWNLOAD_VCPKG=0 if you are sure directory content is correct."
        exit 1
    }
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    .\bootstrap-vcpkg.bat
    cd ..
}

if ($env:BINARY_CACHE_GITHUB_TOKEN)
{
    Write-Host "Configuring GitHub packages binary cache."
    $NUGET = & "vcpkg\vcpkg" fetch nuget | select -last 1
    Write-Host "Using NuGet: $NUGET"
    $BINARY_CACHE_GITHUB_OWNERS = $env:BINARY_CACHE_GITHUB_OWNERS ?? "docwire"
    foreach ($OWNER in $BINARY_CACHE_GITHUB_OWNERS.Split(" "))
    {
        $SOURCE_URL = "https://nuget.pkg.github.com/$OWNER/index.json"
        Write-Host "Using cache source: $SOURCE_URL"
        $SOURCE_NAME = "${OWNER}_github"
        & "$NUGET" sources add -source "$SOURCE_URL" -storepasswordincleartext -name "$SOURCE_NAME" -username $env:BINARY_CACHE_GITHUB_USER -password $env:BINARY_CACHE_GITHUB_TOKEN
        & "$NUGET" setapikey $env:BINARY_CACHE_GITHUB_TOKEN -source "$SOURCE_URL"
        $VCPKG_BINARY_SOURCES += ";nuget,$SOURCE_NAME,readwrite"
    }
    $env:VCPKG_BINARY_SOURCES = "clear" + $VCPKG_BINARY_SOURCES
    Write-Host "Using binary sources: $VCPKG_BINARY_SOURCES"
    Write-Host "GitHub packages binary cache enabled."
}
else
{
    Write-Host "GitHub packages binary cache disabled."
}

$VCPKG_TRIPLET="x64-windows"

if ($env:DEBUG -eq "1")
{
    $env:DOCWIRE_LOG_VERBOSITY = "debug"
    $VCPKG_DEBUG_OPTION = "--debug"
}

Get-Date | Out-File -FilePath ports\docwire\disable_binary_cache.tmp
$Env:SOURCE_PATH = "$PWD"
$Env:VCPKG_KEEP_ENV_VARS = "SOURCE_PATH;DOCWIRE_LOG_VERBOSITY;OPENAI_API_KEY;ASAN_OPTIONS;TSAN_OPTIONS"
if ($Env:OPENAI_API_KEY -ne $null -and $env:OPENAI_API_KEY -ne "") {
    Write-Host "DEBUG: OPENAI_API_KEY exists and is not empty."
} elseif ($env:OPENAI_API_KEY -eq "") {
    Write-Host "DEBUG: OPENAI_API_KEY exists but is empty."
} else {
    Write-Host "DEBUG: OPENAI_API_KEY does not exist."
}
Invoke-ExternalCommand {
    vcpkg\vcpkg --overlay-ports=ports install ${VCPKG_DEBUG_OPTION} docwire${env:FEATURES}:${VCPKG_TRIPLET}
}

if ($env:TEST_CLI -eq "1")
{
    Write-Host "Testing DocWire CLI."
    Invoke-ExternalCommand {
        & "vcpkg\installed\${$VCPKG_TRIPLET}\tools\docwire" tests\1.doc
    }
    Invoke-ExternalCommand {
        & "vcpkg\installed\${$VCPKG_TRIPLET}\tools\docwire" tests\1.pdf
    }
    Invoke-ExternalCommand {
        & "vcpkg\installed\${$VCPKG_TRIPLET}\tools\docwire" tests\1.png
    }
    Write-Host "Tests ended."
}

Write-Host "Done."
