$ErrorActionPreference = "Stop"

choco install archiver -y
choco install doxygen.install -y
choco install graphviz -y

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2024.01.12
.\bootstrap-vcpkg.bat
cd ..

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
vcpkg\vcpkg --overlay-ports=ports install docwire${FEATURES}:${VCPKG_TRIPLET}

$version = Get-Content vcpkg\installed\$VCPKG_TRIPLET\share\docwire\VERSION
vcpkg\vcpkg --overlay-ports=ports export docwire:$VCPKG_TRIPLET --raw --output=docwire-$version --output-dir=.

(Get-Content tools\setup_env.ps1) -replace 'vcpkg_triplet = .*', "vcpkg_triplet = `"$VCPKG_TRIPLET`"" | Set-Content docwire-$version\setup_env.ps1

Write-Host "Testing setup_env.ps1 and DocWire CLI."
& {
    Write-Host "Testing setup_env.ps1 and DocWire CLI with relative path."
    Write-Host "Executing setup_env.ps1."
    . docwire-$version\setup_env.ps1
    Write-Host "Executing docwire tests\1.pdf"
    try
    {
        docwire tests\1.pdf
    }
    catch
    {
        Write-Host "docwire tests\1.pdf failed: $($_.Exception.Message)"
    }
    Write-Host "Test ended."
} 2>&1

& {
    Write-Host "Testing setup_env.ps1 and Docwire CLI with absolute path."
    Write-Host "Executing setup_env.ps1."
    . $PWD\docwire-$version\setup_env.ps1
    Write-Host "Executing docwire tests\1.doc"
    try
    {
        docwire tests\1.doc
    }
    catch
    {
        Write-Host "docwire tests\1.doc failed: $($_.Exception.Message)"
    }
    Write-Host "Test ended."
} 2>&1
Write-Host "Tests ended."

Write-Host "Calculating archive suffix."
$abi_suffix = Get-Content vcpkg\installed\$VCPKG_TRIPLET\share\docwire\abi-id.txt
$full_suffix = "$version-$VCPKG_TRIPLET-$abi_suffix"
Write-Host "Archive suffix is $full_suffix."

Write-Host "Compressing archive."
Compress-Archive -LiteralPath docwire-$version -DestinationPath docwire-$full_suffix.zip
Write-Host "Calculating SHA1."
Get-FileHash -Algorithm SHA1 docwire-$full_suffix.zip > docwire-$full_suffix.zip.sha1
Write-Host "Done."
