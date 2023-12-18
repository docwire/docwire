vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO docwire/htmlcxx
	REF cfff8a76ecabcfeab103007e473cf7953cb0f6b9
	SHA512 ce8863864ee6986327bff66a1db9e9d3eac9e3576edadc68c24f07a2125a2f1d0d8262a79fd50dc32079fff6d02b16863482454e1a50f37ca672226710af46f8
	HEAD_REF master
)

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
vcpkg_install_copyright(FILE_LIST ${SOURCE_PATH}/COPYING ${SOURCE_PATH}/LGPL_V2)
