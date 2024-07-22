vcpkg_from_git(
	OUT_SOURCE_PATH SOURCE_PATH
	URL https://huggingface.co/jncraton/flan-t5-large-ct2-int8
	REF 1f570707a4c855399454ee01dceef20ef6bcb752
	LFS
)

vcpkg_download_distfile(
	LICENSE_PATH
	URLS "https://www.apache.org/licenses/LICENSE-2.0.txt"
	FILENAME LICENSE-APACHE-2.0.txt
	SHA512 98f6b79b778f7b0a15415bd750c3a8a097d650511cb4ec8115188e115c47053fe700f578895c097051c9bc3dfb6197c2b13a15de203273e1a3218884f86e90e8
)

file(INSTALL ${SOURCE_PATH}/ DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} FILES_MATCHING PATTERN "*" PATTERN ",gitattributes" EXCLUDE)
vcpkg_install_copyright(FILE_LIST ${LICENSE_PATH})

file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/include/${PORT})
file(TOUCH ${CURRENT_PACKAGES_DIR}/include/${PORT}/placeholder)
