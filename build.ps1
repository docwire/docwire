choco install archiver -y
choco install doxygen.install -y
choco install graphviz -y

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2023.01.09
.\bootstrap-vcpkg.bat
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'libtool-2.4.6-9-x86_64.pkg.tar.xz', 'libtool-2.4.6-14-x86_64.pkg.tar.zst' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'b309799e5a9d248ef66eaf11a0bd21bf4e8b9bd5c677c627ec83fa760ce9f0b54ddf1b62cbb436e641fbbde71e3b61cb71ff541d866f8ca7717a3a0dbeb00ebf', 'ba983ed9c2996d06b0d21b8fab9505267115f2106341f130e92d6b66dad87b0f0e82707daf0b676a28966bfaa24f6c41b6eef9e1f9bf985611471024f2b0ac97' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'grep-3.0-2-x86_64.pkg.tar.xz', 'grep-1~3.0-6-x86_64.pkg.tar.zst' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'c784d5f8a929ae251f2ffaccf7ab0b3936ae9f012041e8f074826dd6077ad0a859abba19feade1e71b3289cc640626dfe827afe91c272b38a1808f228f2fdd00', '79b4c652082db04c2ca8a46ed43a86d74c47112932802b7c463469d2b73e731003adb1daf06b08cf75dc1087f0e2cdfa6fec0e8386ada47714b4cff8a2d841e1' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'msys2-runtime-3.2.0-8-x86_64.pkg.tar.zst', 'msys2-runtime-3.2.0-15-x86_64.pkg.tar.zst' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'fdd86f4ffa6e274d6fef1676a4987971b1f2e1ec556eee947adcb4240dc562180afc4914c2bdecba284012967d3d3cf4d1a392f798a3b32a3668d6678a86e8d3', 'e054f5e1fc87add7fc3f5cbafd0ff5f3f77a30385073c17d0a8882feb3319a4cba355ccb1cb67ccb9fa704b514c3d05f54c45199011f604c69bb70e52fa33455' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
mkdir downloads
Invoke-WebRequest -Uri https://github.com/microsoft/vcpkg/files/7075269/nasm-2.15.05-win32.zip -OutFile downloads\nasm-2.15.05-win32.zip
cd ..

$VCPKG_TRIPLET="x64-windows"

$exclude = @("vcpkg", "doctotext*.zip*", ".git", "sources-temp.tar")
Write-Host "before PWD"
Write-Host "PWD: $PWD"
Write-Host "after PWD"
$files = Get-ChildItem -Exclude $exclude
Write-Host "files="
$files | Write-Output
$files_no_exclude = Get-ChildItem
Write-Host "files_no_exclude="
$files_no_exclude | Write-Output
Write-Host "compressing"
Compress-Archive -Path $files -DestinationPath sources-temp.zip -CompressionLevel NoCompression
$Env:SOURCES_ARCHIVE = "$PWD\sources-temp.zip"
vcpkg\vcpkg --overlay-ports=ports install doctotext:$VCPKG_TRIPLET
cat vcpkg\buildtrees\doctotext\install-x64-windows-dbg-out.log

$version = Get-Content build/VERSION
vcpkg\vcpkg --overlay-ports=ports export doctotext:$VCPKG_TRIPLET --raw --output=doctotext-$version --output-dir=.

Compress-Archive -LiteralPath doctotext-$version -DestinationPath doctotext-$version-$VCPKG_TRIPLET.zip
Get-FileHash -Algorithm SHA1 doctotext-$version-$VCPKG_TRIPLET.zip > doctotext-$version-$VCPKG_TRIPLET.zip.sha1
