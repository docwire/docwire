vcpkg_download_distfile(ARCHIVE
	URLS "https://github.com/tesseract-ocr/tessdata_fast/archive/refs/tags/4.1.0.tar.gz"
	FILENAME "tessdata_fast-4.1.0.tar.gz"
	SHA512 d5e3461d262e300ebf21abc39d19471a6aabed442eab8fdee772368a02c4fe0d2e5738701e31b2d0777989778c9db0169a57ea89537266bc66d2228fb96ea63e
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH SOURCE_PATH
	ARCHIVE ${ARCHIVE}
)

file(INSTALL ${SOURCE_PATH}/ DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} FILES_MATCHING PATTERN "*.traineddata" PATTERN "script" EXCLUDE PATTERN "tessconfigs" EXCLUDE)
file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/include/${PORT})
file(TOUCH ${CURRENT_PACKAGES_DIR}/include/${PORT}/placeholder)

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
