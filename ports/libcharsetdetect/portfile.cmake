vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO batterseapower/libcharsetdetect
        REF 7b77a7a9d6359480c2a4c0b6f67610bc79398fea
        SHA512 4d3b1e44307274074ad045cce3bcae02add9f654c51baaa06efab25fa1deb89c025572762ac764096edf6132bc7ed40318b4a0f8b76b916943997e260236c575
        HEAD_REF master
)

#file(COPY
#	${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
#	DESTINATION ${SOURCE_PATH}
#)

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/copyright" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
