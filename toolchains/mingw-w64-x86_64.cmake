set (CMAKE_SYSTEM_NAME Windows)
set (TOOLCHAIN_PREFIX x86_64-w64-mingw32)

# cross compilers to use for C, C++
set (CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc-posix)
set (CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++-posix)
set (CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

set (CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)
set (CMAKE_STRIP ${TOOLCHAIN_PREFIX}-strip)

# target environment on the build host system
set (CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})

# modify default behavior of FIND_XXX() commands
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (EXTRA_FLAGS -fno-inline -gdwarf-2)
endif()