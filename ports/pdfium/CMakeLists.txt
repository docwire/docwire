cmake_minimum_required(VERSION 3.16)
project(PDFium)

function(parse_gn_token token_regex value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    string(REGEX MATCH "^([ \n]|#[^\n]*)*${token_regex}" token_match "${input_string}")
    if(token_match)
        set(${value_out_var} "${CMAKE_MATCH_2}" PARENT_SCOPE)
        string(LENGTH "${token_match}" full_match_length)
        string(SUBSTRING "${input_string}" ${full_match_length} -1 remaining_string)
        set(${input_string_var} "${remaining_string}" PARENT_SCOPE)
    else()
        set(${value_out_var} "NOT_FOUND" PARENT_SCOPE)
    endif()
endfunction()

function(parse_gn_keyword value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    parse_gn_token("([a-zA-Z_][a-zA-Z0-9_]*)" value_out input_string)
    set(${value_out_var} "${value_out}" PARENT_SCOPE)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_literal value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    parse_gn_token("\"([^\"]*)\"" value_out input_string)
    set(${value_out_var} "${value_out}" PARENT_SCOPE)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(assert_gn_token token_regex input_string_var)
    set(input_string "${${input_string_var}}")
    parse_gn_token(${token_regex} value_out input_string)
    if(value_out STREQUAL "NOT_FOUND")
        message(FATAL_ERROR "Failed to parse token ${token} at ${input_string}")
    endif()
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_argument value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    assert_gn_token("\\(" input_string)
    parse_gn_literal(arg_value input_string)
    assert_gn_token("\\)" input_string)
    set(${value_out_var} "${arg_val}" PARENT_SCOPE)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_array value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    set(array_elements "")
    assert_gn_token("\\[" input_string)
    while(TRUE)
        parse_gn_literal(current_element input_string)
        if(current_element STREQUAL "NOT_FOUND")
            break()
        endif()
        list(APPEND array_elements "${current_element}")
        parse_gn_token("," comma input_string)
        if(comma STREQUAL "NOT_FOUND")
            break()
        endif()
    endwhile()
    assert_gn_token("\\]" input_string)
    set(${value_out_var} "${array_elements}" PARENT_SCOPE)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_import value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    parse_gn_argument(arg_value input_string)
    set(${value_out_var} "${arg_value}" PARENT_SCOPE)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_operator value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    parse_gn_token("([-+]?=)" operator input_string)
    set(${value_out_var} "${operator}" PARENT_SCOPE)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_sources value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    parse_gn_operator(operator input_string)
    parse_gn_array(array_elements input_string)
    set(${value_out_var} "${array_elements}" PARENT_SCOPE)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_configs input_string_var)
    set(input_string "${${input_string_var}}")
    parse_gn_operator(operator input_string)
    parse_gn_array(array_elements input_string)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_deps input_string_var)
    set(input_string "${${input_string_var}}")
    parse_gn_operator(operator input_string)
    parse_gn_array(array_elements input_string)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_group value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    assert_gn_token("{" input_string)
    while(TRUE)
        parse_gn_keyword(keyword input_string)
        if(keyword STREQUAL "sources")
            parse_gn_sources(sources_list input_string)
        elseif(keyword STREQUAL "configs")
            parse_gn_configs(input_string)
        elseif(keyword STREQUAL "deps")
            parse_gn_deps(input_string)
        elseif(keyword STREQUAL "public_deps")
            parse_gn_deps(input_string)
        elseif(keyword STREQUAL "defines")
            parse_gn_deps(input_string)
        elseif(keyword STREQUAL "frameworks")
            parse_gn_deps(input_string)
        elseif(keyword STREQUAL "visibility")
            parse_gn_deps(input_string)
        elseif(keyword STREQUAL "allow_circular_includes_from")
            parse_gn_deps(input_string)
        elseif(keyword STREQUAL "if")
            parse_gn_if(optional_sources input_string)
            list(APPEND sources_list ${optional_sources})
        elseif(keyword STREQUAL "NOT_FOUND")
            break()
        else()
            message(FATAL_ERROR "Unexpected keyword ${keyword}")
        endif()
    endwhile()
    assert_gn_token("}" input_string)
    set(${value_out_var} "${sources_list}" PARENT_SCOPE)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

set(GN_VAR_is_linux FALSE)
set(GN_VAR_is_win FALSE)
set(GN_VAR_is_mac FALSE)
set(GN_VAR_is_android FALSE)
set(GN_VAR_is_chromeos FALSE)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(GN_VAR_is_linux TRUE)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(GN_VAR_is_win TRUE)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(GN_VAR_is_mac TRUE)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(GN_VAR_is_android TRUE)
endif()

if(UNIX)
    set(GN_VAR_is_posix TRUE)
else()
    set(GN_VAR_is_posix FALSE)
endif()

set(GN_VAR_is_component_build FALSE)
set(GN_VAR_pdf_use_partition_alloc FALSE)
set(GN_VAR_pdf_enable_xfa FALSE)
set(GN_VAR_pdf_enable_xfa_bmp FALSE)
set(GN_VAR_pdf_enable_xfa_gif FALSE)
set(GN_VAR_pdf_enable_xfa_png FALSE)
set(GN_VAR_pdf_enable_xfa_tiff FALSE)
set(GN_VAR_pdf_enable_v8 FALSE)
set(GN_VAR_pdf_use_skia FALSE)
set(GN_VAR_use_system_freetype TRUE)

function(parse_gn_condition value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    set(result FALSE)
    while(TRUE)
        parse_gn_keyword(var_name input_string)
        if(var_name STREQUAL "NOT_FOUND")
            message(FATAL_ERROR "GN variable name expected")
        endif()
        if (DEFINED GN_VAR_${var_name})
            if(${GN_VAR_${var_name}})
                set(result TRUE)
            endif()
        else()
            message(FATAL_ERROR "Unknown GN variable \"${var_name}\"")
        endif()
        set(first FALSE)
        parse_gn_token("\\|\\|" or_op input_string)
        if(or_op STREQUAL "NOT_FOUND")
            break()
        endif()
    endwhile()
    set(${value_out_var} "${result}" PARENT_SCOPE)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_if value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    assert_gn_token("\\(" input_string)
    parse_gn_condition(if_result input_string)
    assert_gn_token("\\)" input_string)
    parse_gn_group(sources_list input_string)
    parse_gn_token("else" else input_string)
    if (else STREQUAL "NOT_FOUND")
        set(alternative_sources_list "")
    else()
        parse_gn_group(alternative_sources_list input_string)
    endif()
    if (NOT if_result)
        set(sources_list "${alternative_sources_list}")
    endif()
    set(${value_out_var} "${sources_list}" PARENT_SCOPE)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_source_set value_out_var input_string_var)
    set(input_string "${${input_string_var}}")
    parse_gn_argument(set_name input_string)
    parse_gn_group(sources_list input_string)
    set(${value_out_var} "${sources_list}" PARENT_SCOPE)
    set(${input_string_var} "${input_string}" PARENT_SCOPE)
endfunction()

function(parse_gn_build_file dir source_set out)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/BUILD.gn" gn_build_content)
    string(REGEX MATCH "source_set(\\(\"${source_set}\"\\)[ \n]*{.*)" _ "${gn_build_content}")
    set(gn_source_set_content "${CMAKE_MATCH_1}")
    parse_gn_source_set(extracted_filenames gn_source_set_content)
    set(sources_with_prefix "")
    foreach(source ${extracted_filenames})
        list(APPEND sources_with_prefix "${dir}/${source}")
    endforeach()
    set(${out} "${sources_with_prefix}" PARENT_SCOPE)
endfunction()

parse_gn_build_file("fpdfsdk" "fpdfsdk" fpdfsdk_sources)
parse_gn_build_file("core/fpdftext" "fpdftext" fpdftext_sources)
parse_gn_build_file("core/fpdfapi/parser" "parser" parser_sources)
parse_gn_build_file("core/fpdfapi/render" "render" render_sources)
parse_gn_build_file("core/fpdfapi/page" "page" page_sources)
parse_gn_build_file("core/fpdfapi/font" "font" font_sources)
parse_gn_build_file("core/fpdfapi/cmaps" "cmaps" cmaps_sources)
parse_gn_build_file("core/fxcrt" "fxcrt" fxcrt_sources)
parse_gn_build_file("core/fxge" "fxge" fxge_sources)
parse_gn_build_file("core/fxcodec" "fxcodec" fxcodec_sources)
parse_gn_build_file("core/fdrm" "fdrm" fdrm_sources)
parse_gn_build_file("third_party" "fx_agg" fx_agg_sources)
parse_gn_build_file("constants" "constants" constants_sources)
parse_gn_build_file("fxjs" "fxjs" fxjs_sources)
parse_gn_build_file("core/fpdfdoc" "fpdfdoc" fpdfdoc_sources)
parse_gn_build_file("fpdfsdk/formfiller" "formfiller" formfiller_sources)
parse_gn_build_file("fpdfsdk/pwl" "pwl" pwl_sources)
parse_gn_build_file("core/fpdfapi/edit" "edit" edit_sources)
parse_gn_build_file("core/fpdfapi/edit" "contentstream_write_utils" contentstream_write_utils_sources)
parse_gn_build_file("third_party" "fx_lcms2" fx_lcms2_sources)

add_library(pdfium SHARED
    ${fpdfsdk_sources}
    ${fpdftext_sources}
    ${parser_sources}
    ${render_sources}
    ${page_sources}
    ${font_sources}
    ${cmaps_sources}
    ${fxcrt_sources}
    ${fxge_sources}
    ${fxcodec_sources}
    ${fdrm_sources}
    ${fx_agg_sources}
    ${constants_sources}
    ${fxjs_sources}
    ${fpdfdoc_sources}
    ${formfiller_sources}
    ${pwl_sources}
    ${edit_sources}
    ${contentstream_write_utils_sources}
    ${fx_lcms2_sources}
)
target_include_directories(pdfium PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_CURRENT_SOURCE_DIR}/third_party/abseil-cpp)

set(platform_defines "")
if (WIN32)
    list(APPEND platform_defines WCHAR_T_IS_16_BIT)
else()
    list(APPEND platform_defines WCHAR_T_IS_32_BIT)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
    list(APPEND platform_defines "COMPILER_GCC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND platform_defines
        "COMPILER_MSVC"
        "NOMINMAX"
        "WIN32_LEAN_AND_MEAN"
        "_HAS_ITERATOR_DEBUGGING=0"
        "_UNICODE"
        "UNICODE"
    )
endif()
target_compile_definitions(pdfium PRIVATE
    USE_SYSTEM_ICUUC USE_SYSTEM_ZLIB USE_SYSTEM_LIBJPEG USE_SYSTEM_LIBOPENJPEG2 DEFINE_PS_TABLES_DATA
    ${platform_defines}
)

target_compile_features(pdfium PRIVATE cxx_std_20)

set(build_config_content
    "#include \"buildflag.h\"
    #define BUILDFLAG_INTERNAL_IS_POSIX() ${GN_VAR_is_posix}
    #define BUILDFLAG_INTERNAL_IS_LINUX() ${GN_VAR_is_linux}
    #define BUILDFLAG_INTERNAL_IS_WIN() ${GN_VAR_is_win}
    #define BUILDFLAG_INTERNAL_IS_APPLE() ${GN_VAR_is_mac}
    #define BUILDFLAG_INTERNAL_IS_ANDROID() ${GN_VAR_is_android}
    #define BUILDFLAG_INTERNAL_IS_CHROMEOS() ${GN_VAR_is_chromeos}
    #define BUILDFLAG_INTERNAL_IS_NACL() FALSE
    #define BUILDFLAG_INTERNAL_IS_ASMJS() FALSE
")
string(REPLACE "TRUE" "1" build_config_content "${build_config_content}")
string(REPLACE "FALSE" "0" build_config_content "${build_config_content}")
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86|X86|i[3-6]86|amd64|AMD64|x86_64)$")
    string(APPEND build_config_content "#define ARCH_CPU_X86_FAMILY\n")
endif()
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|ARM64|aarch64|AARCH64)$")
    string(APPEND build_config_content "#define ARCH_CPU_ARM64\n")
endif()
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/build/build_config.h" "${build_config_content}")

find_package(Freetype REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc)
find_package(OpenJPEG CONFIG REQUIRED)
find_package(JPEG REQUIRED)
find_package(ZLIB REQUIRED)
target_link_libraries(pdfium PRIVATE Freetype::Freetype ICU::uc openjp2 JPEG::JPEG ZLIB::ZLIB)
if(APPLE)
    find_library(CoreFoundation CoreFoundation REQUIRED)
    find_library(CoreGraphics CoreGraphics REQUIRED)
    target_link_libraries(pdfium PRIVATE ${CoreFoundation} ${CoreGraphics})
endif()

file(GLOB HEADERS "public/*.h")
install(FILES ${HEADERS} DESTINATION include/pdfium)
file(GLOB CPP_HEADERS "public/cpp/*.h")
install(FILES ${CPP_HEADERS} DESTINATION include/pdfium/cpp)

install(TARGETS pdfium
    EXPORT PDFium_targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${CMAKE_BINARY_DIR}/pdfium.lib DESTINATION lib)
