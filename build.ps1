choco install archiver -y
choco install doxygen.install -y
choco install graphviz -y

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2023.08.09
.\bootstrap-vcpkg.bat
cd ..

$VCPKG_TRIPLET="x64-windows"

Get-Date | Out-File -FilePath ports\docwire\disable_binary_cache.tmp
$Env:SOURCE_PATH = "$PWD"
$Env:VCPKG_KEEP_ENV_VARS = "SOURCE_PATH"
vcpkg\vcpkg --overlay-ports=ports install docwire:$VCPKG_TRIPLET

$version = Get-Content vcpkg\installed\$VCPKG_TRIPLET\share\docwire\VERSION
vcpkg\vcpkg --overlay-ports=ports export docwire:$VCPKG_TRIPLET --raw --output=docwire-$version --output-dir=.

New-Item docwire-$version\docwire.bat -ItemType File -Value "@`"%~dp0\\installed\\x64-windows\\tools\\docwire.bat`" %*"

Compress-Archive -LiteralPath docwire-$version -DestinationPath docwire-$version-$VCPKG_TRIPLET.zip
Get-FileHash -Algorithm SHA1 docwire-$version-$VCPKG_TRIPLET.zip > docwire-$version-$VCPKG_TRIPLET.zip.sha1
