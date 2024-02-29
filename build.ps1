$ErrorActionPreference = "Stop"

if ($env:DOWNLOAD_VCPKG -ne "0")
{
    if (Test-Path vcpkg)
    {
        Write-Host "Error: vcpkg directory already exists. Remove it or set DOWNLOAD_VCPKG=0 if you are sure directory content is correct."
        exit 1
    }
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    git checkout tags/2024.01.12
    .\bootstrap-vcpkg.bat
    cd ..
}

if ($env:BINARY_CACHE_GITHUB_TOKEN)
{
    Write-Host "Configuring GitHub packages binary cache."
    $NUGET = & "vcpkg\vcpkg" fetch nuget | select -last 1
    Write-Host "Using NuGet: $NUGET"
    $BINARY_CACHE_GITHUB_OWNERS = $env:BINARY_CACHE_GITHUB_OWNERS ?? "docwire"
    foreach ($OWNER in $BINARY_CACHE_GITHUB_OWNERS)
    {
        $SOURCE_URL = "https://nuget.pkg.github.com/$OWNER/index.json"
        Write-Host "Using cache source: $SOURCE_URL"
        $SOURCE_NAME = "${OWNER}_github"
        & "$NUGET" sources add -source "$SOURCE_URL" -storepasswordincleartext -name "$SOURCE_NAME" -username $env:BINARY_CACHE_GITHUB_USER -password $env:BINARY_CACHE_GITHUB_TOKEN
        & "$NUGET" setapikey $env:BINARY_CACHE_GITHUB_TOKEN -source "$SOURCE_URL"
        $VCPKG_BINARY_SOURCES += ";nuget,$SOURCE_NAME,readwrite"
    }
    $VCPKG_BINARY_SOURCES = "clear" + $VCPKG_BINARY_SOURCES
    Write-Host "Using binary sources: $VCPKG_BINARY_SOURCES"
    Write-Host "GitHub packages binary cache enabled."
}
else
{
    Write-Host "GitHub packages binary cache disabled."
}

$VCPKG_TRIPLET="x64-windows"

if ($Env:SANITIZER -eq "address")
{
	$FEATURES = "[tests,address-sanitizer]"
}
elseif ($Env:SANITIZER -eq "thread")
{
	$FEATURES = "[tests,thread-sanitizer]"
}
else
{
	$FEATURES = "[tests]"
}

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
vcpkg\vcpkg --overlay-ports=ports install ${$VCPKG_DEBUG_OPTION} docwire${FEATURES}:${VCPKG_TRIPLET}

if ($Env:SANITIZER -ne $null -and $env:SANITIZER -ne "")
{
    Write-Host "Sanitizer $Env:SANITIZER is enabled. Exiting without building archive and running CLI tests."
    Exit 0
}

if ($env:EXPORT_VCPKG -ne "0")
{
    $version = Get-Content vcpkg\installed\$VCPKG_TRIPLET\share\docwire\VERSION
    vcpkg\vcpkg --overlay-ports=ports export docwire:$VCPKG_TRIPLET --raw --output=docwire-$version --output-dir=.

    (Get-Content tools\setup_env.ps1) -replace 'vcpkg_triplet = .*', "vcpkg_triplet = `"$VCPKG_TRIPLET`"" | Set-Content docwire-$version\setup_env.ps1

    Write-Host "Testing setup_env.ps1 and DocWire CLI."
    & {
        Write-Host "Testing setup_env.ps1 and DocWire CLI with relative path."
        Write-Host "Executing setup_env.ps1."
        . docwire-$version\setup_env.ps1
        Write-Host "Executing docwire tests\1.pdf"
        docwire tests\1.pdf
        Write-Host "Test ended."
    } 2>&1

    & {
        Write-Host "Testing setup_env.ps1 and Docwire CLI with absolute path."
        Write-Host "Executing setup_env.ps1."
        . $PWD\docwire-$version\setup_env.ps1
        Write-Host "Executing docwire tests\1.doc"
        docwire tests\1.doc
        Write-Host "Test ended."
    } 2>&1
    Write-Host "Tests ended."

    if ($env:CREATE_ARCHIVE -eq "1")
    {
        Write-Host "Calculating archive suffix."
        $abi_suffix = Get-Content vcpkg\installed\$VCPKG_TRIPLET\share\docwire\abi-id.txt
        $full_suffix = "$version-$VCPKG_TRIPLET-$abi_suffix"
        Write-Host "Archive suffix is $full_suffix."

        Write-Host "Compressing archive."
        Compress-Archive -LiteralPath docwire-$version -DestinationPath docwire-$full_suffix.zip
        Write-Host "Calculating SHA1."
        Get-FileHash -Algorithm SHA1 docwire-$full_suffix.zip > docwire-$full_suffix.zip.sha1
    }
}

Write-Host "Done."
