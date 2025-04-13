PROJECT( wvWare )

cmake_minimum_required(VERSION 2.6)

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

# Generate configuration files
CONFIGURE_FILE( config.h.cmake ${wvWare_BINARY_DIR}/config.h )

# Source directories
ADD_SUBDIRECTORY( src )
