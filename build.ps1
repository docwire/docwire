choco install archiver -y
choco install doxygen.install -y
choco install graphviz -y

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2023.08.09
.\bootstrap-vcpkg.bat
cd ..

$VCPKG_TRIPLET="x64-windows"

Get-Date | Out-File -FilePath ports\doctotext\disable_binary_cache.tmp
$Env:SOURCE_PATH = "$PWD"
$Env:VCPKG_KEEP_ENV_VARS = "SOURCE_PATH"
vcpkg\vcpkg --overlay-ports=ports install doctotext:$VCPKG_TRIPLET

$version = Get-Content vcpkg\installed\$VCPKG_TRIPLET\share\doctotext\VERSION
vcpkg\vcpkg --overlay-ports=ports export doctotext:$VCPKG_TRIPLET --raw --output=doctotext-$version --output-dir=.

New-Item doctotext-$version\text_extractor.bat -ItemType File -Value "@`"%~dp0\\installed\\x64-windows\\tools\\text_extractor.bat`" %*"

Compress-Archive -LiteralPath doctotext-$version -DestinationPath doctotext-$version-$VCPKG_TRIPLET.zip
Get-FileHash -Algorithm SHA1 doctotext-$version-$VCPKG_TRIPLET.zip > doctotext-$version-$VCPKG_TRIPLET.zip.sha1
