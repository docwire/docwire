choco install archiver -y
choco install doxygen.install -y
choco install graphviz -y

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2023.11.20
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
$Env:VCPKG_KEEP_ENV_VARS = "SOURCE_PATH;OPENAI_API_KEY"
if ($Env:OPENAI_API_KEY -ne $null -and $env:OPENAI_API_KEY -ne "") {
    Write-Host "DEBUG: OPENAI_API_KEY exists and is not empty."
} elseif ($env:OPENAI_API_KEY -eq "") {
    Write-Host "DEBUG: OPENAI_API_KEY exists but is empty."
} else {
    Write-Host "DEBUG: OPENAI_API_KEY does not exist."
}
vcpkg\vcpkg --overlay-ports=ports install docwire$FEATURES:$VCPKG_TRIPLET

$version = Get-Content vcpkg\installed\$VCPKG_TRIPLET\share\docwire\VERSION
vcpkg\vcpkg --overlay-ports=ports export docwire:$VCPKG_TRIPLET --raw --output=docwire-$version --output-dir=.

New-Item docwire-$version\docwire.bat -ItemType File -Value "@`"%~dp0\\installed\\x64-windows\\tools\\docwire.bat`" %*"

Compress-Archive -LiteralPath docwire-$version -DestinationPath docwire-$version-$VCPKG_TRIPLET.zip
Get-FileHash -Algorithm SHA1 docwire-$version-$VCPKG_TRIPLET.zip > docwire-$version-$VCPKG_TRIPLET.zip.sha1
