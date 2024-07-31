vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO OpenNMT/Tokenizer
	REF v1.37.1
	SHA512 ddda4853449aa2394c3da5abacbdb2121b5a728eaadf8dee2bd88618b3583cedb97b63a79b5d8e0e4904e1fe2ed0db6dc128d72c000f09885bb563b899880265
)

vcpkg_from_github(
	OUT_SOURCE_PATH CXXOPTS_SOURCE_PATH
	REPO jarro2783/cxxopts
	REF c74846a891b3cc3bfa992d588b1295f528d43039
	SHA512 3e92a67f8d6cb1ba0f80e35b47c9beb9ea14d995bb3e296765475ff31a0b499f3080e359fa87a63273e1e8c5396d4af39d9a47dbe6fa06074bb63a642cf6bfda
)
file(REMOVE_RECURSE ${SOURCE_PATH}/third_party/cxxopts)
file(RENAME ${CXXOPTS_SOURCE_PATH} ${SOURCE_PATH}/third_party/cxxopts)

vcpkg_from_github(
	OUT_SOURCE_PATH GOOGLETEST_SOURCE_PATH
	REPO google/googletest
	REF 58d77fa8070e8cec2dc1ed015d66b454c8d78850
	SHA512 d8153c426e4f9c89a74721cc4a24dfcaf319810f4f10aa25fc972f99da2d96d66bc840cf2f51b756fef6b1ca47e8d2c8633f5862cc24e34d57309ad48802124a
)
file(REMOVE_RECURSE ${SOURCE_PATH}/third_party/googletest)
file(RENAME ${GOOGLETEST_SOURCE_PATH} ${SOURCE_PATH}/third_party/googletest)

vcpkg_from_github(
	OUT_SOURCE_PATH SENTENCEPIECE_SOURCE_PATH
	REPO google/sentencepiece
	REF d8711f55d9b2cb9c77a00adcc18108482b29b675
	SHA512 d92238b15c391451b1bbe72c72105a3d21344ec53a39eaa521b8784f2dce0de77fb27e5aa8072fee2ab8ca775c04425833ca7ffc8e7802ba3540c571bf2b917f
)
file(REMOVE_RECURSE ${SOURCE_PATH}/third_party/sentencepiece)
file(RENAME ${SENTENCEPIECE_SOURCE_PATH} ${SOURCE_PATH}/third_party/sentencepiece)

# https://github.com/google/sentencepiece/issues/1028
if (VCPKG_TARGET_IS_WINDOWS)
	set(NO_ENUM_CONSTEXPR_CONVERSION "")
else()
	set(NO_ENUM_CONSTEXPR_CONVERSION "-DCMAKE_CXX_FLAGS=-Wno-enum-constexpr-conversion")
endif()

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${NO_ENUM_CONSTEXPR_CONVERSION}
)

vcpkg_cmake_install()

vcpkg_install_copyright(FILE_LIST ${SOURCE_PATH}/LICENSE.md)
