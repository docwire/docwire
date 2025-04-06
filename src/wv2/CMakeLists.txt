PROJECT( wvWare )

cmake_minimum_required(VERSION 2.6)

# wv2 versioning
SET( WV2_MAJOR_VERSION 0 )
SET( WV2_MINOR_VERSION 2 )
SET( WV2_MICRO_VERSION 3 )
SET( WV2_VERSION ${WV2_MAJOR_VERSION}.${WV2_MINOR_VERSION}.${WV2_MICRO_VERSION} )

# libtool versioning
SET( LT_VERSION_CURRENT 4 )
SET( LT_VERSION_REVISION 1 )
SET( LT_VERSION_AGE 0 )

# For automake. Is this required in CMake? (I don't think so)
SET( VERSION ${WV2_VERSION} )
SET( PACKAGE wv2 )

SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${wvWare_SOURCE_DIR}/cmake )

FIND_PACKAGE( Iconv REQUIRED )
IF( ICONV_FOUND )
  SET( HAVE_ICONV_H 1 )
  SET( ICONV_REQUIRES_CONST ${ICONV_SECOND_ARGUMENT_IS_CONST} )
ENDIF( ICONV_FOUND )

OPTION( WITH_ZLIB "Build wv2 with zlib (with compression features)" ON )
IF( WITH_ZLIB )
  FIND_PACKAGE( ZLIB REQUIRED )
ENDIF( WITH_ZLIB )

INCLUDE_DIRECTORIES( ${ICONV_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} )

#
# Iconv checks
# 

INCLUDE( CheckIncludeFile )

CHECK_INCLUDE_FILE( sys/iconv.h HAVE_SYS_ICONV_H )

# Add "COMPILE_DEFINITIONS definitions" to TRY_RUN only if we have compile definitions

# Make sure ICONV_COMPILE_DEFINITIONS will never be empty (in case we define neither HAVE_ICONV_H nor HAVE_SYS_ICONV_H),
# otherwise TRY_RUN will fail due to COMPILE_DEFINITIONS being followed by nothing

SET( ICONV_COMPILE_DEFINITIONS "-DBLAH" )

IF( HAVE_ICONV_H )
  SET( ICONV_COMPILE_DEFINITIONS ${ICONV_COMPILE_DEFINITIONS} "-DHAVE_ICONV_H" )
ENDIF( HAVE_ICONV_H )

IF( HAVE_SYS_ICONV_H )
  SET( ICONV_COMPILE_DEFINITIONS ${ICONV_COMPILE_DEFINITIONS} "-DHAVE_SYS_ICONV_H" )
ENDIF( HAVE_SYS_ICONV_H )

#TRY_RUN( MODERN_ICONV_RUN MODERN_ICONV_COMPILE ${wvWare_BINARY_DIR}/CMakeTmp ${wvWare_SOURCE_DIR}/cmake/TestModernIconv.c COMPILE_DEFINITIONS ${ICONV_COMPILE_DEFINITIONS} )

#IF( MODERN_ICONV_RUN GREATER 0 OR NOT MODERN_ICONV_COMPILE )
#  MESSAGE( STATUS "wv2 depends on a modern iconv installation, supporting UNICODELITTLE and" )
#  MESSAGE( STATUS "UNICODEBIG. The detected iconv version doesn't support these conversions." )
#  MESSAGE( STATUS "" )
#  MESSAGE( STATUS "Please get a new libiconv from http://www.gnu.org/software/libiconv/" )
#  MESSAGE( STATUS "You might want to install the library to some alternative directory, in" )
#  MESSAGE( STATUS "order not to overwrite your current installation. Please use the options" )
#  MESSAGE( STATUS "-DICONV_INCLUDE_DIR=DIR and -DICONV_LIBRARIES=DIR to specify the location." )
#  MESSAGE( STATUS "" )
#  MESSAGE( FATAL_ERROR "* * * No iconv support - unable to continue. * * *" )
#ENDIF( MODERN_ICONV_RUN GREATER 0 OR NOT MODERN_ICONV_COMPILE )

#
# Various checks
#

INCLUDE( TestBigEndian )
TEST_BIG_ENDIAN( WORDS_BIGENDIAN )

CHECK_INCLUDE_FILE( dlfcn.h HAVE_DLFCN_H )
CHECK_INCLUDE_FILE( strings.h HAVE_STRINGS_H )
CHECK_INCLUDE_FILE( string.h HAVE_STRING_H )
CHECK_INCLUDE_FILE( math.h HAVE_MATH_H )
CHECK_INCLUDE_FILE( float.h HAVE_FLOAT_H )
CHECK_INCLUDE_FILE( ieeefp.h HAVE_IEEEFP_H )
CHECK_INCLUDE_FILE( errno.h HAVE_ERRNO_H )
CHECK_INCLUDE_FILE( inttypes.h HAVE_INTTYPES_H )
CHECK_INCLUDE_FILE( memory.h HAVE_MEMORY_H )
CHECK_INCLUDE_FILE( stdlib.h HAVE_STDLIB_H )
CHECK_INCLUDE_FILE( unistd.h HAVE_UNISTD_H )
CHECK_INCLUDE_FILE( stdint.h HAVE_STDINT_H ) # Not really needed because CHECK_TYPE_SIZE already performs this test
CHECK_INCLUDE_FILE( stdint.h HAVE_STDINT_H ) # Not really needed because CHECK_TYPE_SIZE already performs this test
CHECK_INCLUDE_FILE( sys/types.h HAVE_SYS_TYPES_H ) # Not really needed because CHECK_TYPE_SIZE already performs this test
CHECK_INCLUDE_FILE( sys/stat.h HAVE_SYS_STAT_H )

INCLUDE( CheckTypeSize )
CHECK_TYPE_SIZE( char SIZEOF_CHAR )
CHECK_TYPE_SIZE( short SIZEOF_SHORT )
CHECK_TYPE_SIZE( long SIZEOF_LONG )
CHECK_TYPE_SIZE( int SIZEOF_INT )
CHECK_TYPE_SIZE( "void *" SIZEOF_VOID_P )

INCLUDE( CheckFunctionExists )
IF( NOT MSVC )
  # libm does not exist on MSVC
  SET( CMAKE_REQUIRED_LIBRARIES m )
  SET( CMAKE_REQUIRED_INCLUDES math.h )
ENDIF( NOT MSVC )

CHECK_FUNCTION_EXISTS( isinf HAVE_FUNC_ISINF )
CHECK_FUNCTION_EXISTS( isnan HAVE_FUNC_ISNAN )
CHECK_FUNCTION_EXISTS( finite HAVE_FUNC_FINITE )
CHECK_FUNCTION_EXISTS( _finite HAVE_FUNC__FINITE )

#
# Check zlib is modern enough
#

SET( NEON_ZLIB 0 ) # By default, we are not modern enough
SET( CMAKE_REQUIRED_LIBRARIES ${ZLIB_LIBRARIES} )
SET( CMAKE_REQUIRED_INCLUDES zlib.h )

CHECK_FUNCTION_EXISTS( inflate ZLIB_HAS_INFLATE )

IF( ZLIB_HAS_INFLATE )
  TRY_RUN( MODERN_ZLIB_RUN MODERN_ZLIB_COMPILE ${wvWare_BINARY_DIR}/CMakeTmp ${wvWare_SOURCE_DIR}/cmake/TestModernZlib.c )
ENDIF( ZLIB_HAS_INFLATE )

IF( MODERN_ZLIB_RUN GREATER 0 AND WITH_ZLIB )
  MESSAGE( FATAL_ERROR "Your version of zlib is too old for wv2" )
ENDIF( MODERN_ZLIB_RUN GREATER 0 AND WITH_ZLIB )

#
# Set cflags and ldflags
#

IF( ZLIB_FOUND )
  SET( _WV2_LDFLAGS ${_WV2_LDFLAGS} ${ZLIB_LIBRARIES} )
  SET( _WV2_CFLAGS ${_WV2_CFLAGS} ${ZLIB_INCLUDE_DIR} )
ENDIF( ZLIB_FOUND )

IF( ICONV_FOUND )
  SET( _WV2_LDFLAGS ${_WV2_LDFLAGS} ${ICONV_LIBRARIES} )
  SET( _WV2_CFLAGS ${_WV2_CFLAGS} ${ICONV_INCLUDE_DIR} )
ENDIF( ICONV_FOUND )


#
# Clean and prepare
#
LIST( REMOVE_DUPLICATES _WV2_LDFLAGS )
LIST( REMOVE_DUPLICATES _WV2_CFLAGS )

FOREACH( _lib ${_WV2_LDFLAGS} )
  # Remove path to the library and suffixes. Transformation example: libglib-2.0.so => glib-2.0
  STRING( REGEX REPLACE "[\\\\ _\\/\\.a-zA-Z0-9\\-]*\\/lib([_\\.a-zA-Z0-9\\-]*)\\.[_a-zA-Z0-9\\-\\.]*" \\1 _lib_no_path ${_lib} )
  SET( WV2_LDFLAGS "${WV2_LDFLAGS} ${CMAKE_LINK_LIBRARY_FLAG}${_lib_no_path}" )
ENDFOREACH( _lib )

FOREACH( _inc ${_WV2_CFLAGS} )
  SET( WV2_CFLAGS "${WV2_CFLAGS} -I${_inc}" )
ENDFOREACH( _inc )

# Generate configuration files
CONFIGURE_FILE( config.h.cmake ${wvWare_BINARY_DIR}/config.h )
CONFIGURE_FILE( wv2-config.cmake ${wvWare_BINARY_DIR}/wv2-config @ONLY )

# Source directories
ADD_SUBDIRECTORY( src )

# Installation (more in src/CMakeLists.txt)

INSTALL( FILES ${wvWare_BINARY_DIR}/wv2-config  
	DESTINATION bin 
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
      )

# "make dist"

SET(ARCHIVE_NAME ${PACKAGE}-${WV2_VERSION})
ADD_CUSTOM_TARGET(dist
    COMMAND svn export ${wvWare_SOURCE_DIR} ${wvWare_BINARY_DIR}/${ARCHIVE_NAME}
    COMMAND tar -C ${wvWare_BINARY_DIR} -c -v -z -f ${ARCHIVE_NAME}.tar.gz ${ARCHIVE_NAME}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${wvWare_BINARY_DIR}/${ARCHIVE_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
