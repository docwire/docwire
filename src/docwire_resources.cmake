#[=======================================================================[.rst:
docwire_find_resource
---------------------
Finds a resource file or directory on the system or in the build environment.

  docwire_find_resource(<result_var> REL_PATH <path> [REQUIRED])

This function searches standard locations and CMAKE_PREFIX_PATH/share.
#]=======================================================================]
function(docwire_find_resource result_var)
    cmake_parse_arguments(PARSE_ARGS "REQUIRED" "REL_PATH" "" ${ARGN})

    if(NOT PARSE_ARGS_REL_PATH)
        message(FATAL_ERROR "docwire_find_resource requires REL_PATH argument.")
    endif()

    get_filename_component(RESOURCE_NAME "${PARSE_ARGS_REL_PATH}" NAME)
    get_filename_component(RESOURCE_DIR_REL "${PARSE_ARGS_REL_PATH}" DIRECTORY)

    # Create a unique and predictable variable name for the cache.
    string(TOUPPER "${PARSE_ARGS_REL_PATH}" CACHE_VAR_SUFFIX)
    string(REGEX REPLACE "[/.-]" "_" CACHE_VAR_SUFFIX "${CACHE_VAR_SUFFIX}")
    set(CACHE_VAR_NAME "DOCWIRE_RESOURCE_${CACHE_VAR_SUFFIX}")

    set(FIND_ARGS "")
    if(PARSE_ARGS_REQUIRED)
        list(APPEND FIND_ARGS "REQUIRED")
    endif()

    set(FOUND_PATH "")

    set(docwire_search_paths "")
    if(DEFINED DOCWIRE_DATADIR)
        list(APPEND docwire_search_paths "${DOCWIRE_DATADIR}")
    endif()
    foreach(prefix IN LISTS CMAKE_PREFIX_PATH)
        list(APPEND docwire_search_paths "${prefix}/share")
    endforeach()
    foreach(prefix IN LISTS CMAKE_SYSTEM_PREFIX_PATH)
        list(APPEND docwire_search_paths "${prefix}/share")
    endforeach()

    find_path(${CACHE_VAR_NAME}
        NAMES ${RESOURCE_NAME}
        PATHS ${docwire_search_paths}
        PATH_SUFFIXES ${RESOURCE_DIR_REL}
        NO_DEFAULT_PATH
        DOC "Path to resource: ${PARSE_ARGS_REL_PATH}"
        ${FIND_ARGS}
    )
    if(${CACHE_VAR_NAME})
         set(FOUND_PATH "${${CACHE_VAR_NAME}}/${RESOURCE_NAME}")
    endif()

    set(${result_var} "${FOUND_PATH}" PARENT_SCOPE)
endfunction()

#[=======================================================================[.rst:
docwire_target_resources
------------------------
Associates a resource with a target for deployment.

  docwire_target_resources(<target> <rel_dest_path> SOURCE <abs_source_path>)

This sets properties on the target to track where the resource exists in the
source tree and where it should be located in the installation (relative to share/).
#]=======================================================================]
function(docwire_target_resources target rel_dest_path)
    cmake_parse_arguments(PARSE_ARGS "" "SOURCE" "" ${ARGN})
    if(NOT PARSE_ARGS_SOURCE)
        message(FATAL_ERROR "docwire_target_resources for target '${target}' requires a SOURCE argument.")
    endif()

    get_filename_component(abs_source_path "${PARSE_ARGS_SOURCE}" ABSOLUTE)
    if(NOT EXISTS "${abs_source_path}")
        message(WARNING "docwire_target_resources: SOURCE path '${PARSE_ARGS_SOURCE}' does not exist for target '${target}'.")
    endif()

    # Property for use within the current build tree (e.g., for tests).
    # This is a non-interface property and will not be exported with the target.
    set_property(TARGET "${target}" APPEND PROPERTY
        _DOCWIRE_RESOURCE_SOURCES
        "${abs_source_path}"
    )

    # Property for use by downstream consumers after installation.
    # This is a public property and will be part of the installed target information via EXPORT_PROPERTIES.
    set_property(TARGET "${target}" APPEND PROPERTY
        DOCWIRE_RESOURCES
        "${rel_dest_path}"
    )

    # Explicitly mark the custom property for export so it appears in docwire-targets.cmake
    get_target_property(_current_export_props "${target}" EXPORT_PROPERTIES)
    if(NOT _current_export_props OR NOT "DOCWIRE_RESOURCES" IN_LIST _current_export_props)
        set_property(TARGET "${target}" APPEND PROPERTY
            EXPORT_PROPERTIES
            DOCWIRE_RESOURCES
        )
    endif()
endfunction()

#[=======================================================================[.rst:
docwire_collect_transitive_dependencies
---------------------------------------
Collects all transitive dependencies of a target.

  docwire_collect_transitive_dependencies(<target> <out_var>)

Returns a list of all targets reachable from the given target via
INTERFACE_LINK_LIBRARIES and LINK_LIBRARIES properties.
#]=======================================================================]
function(docwire_collect_transitive_dependencies root_target out_var)
    set(visited_targets "")
    set(targets_to_visit ${root_target})

    while(targets_to_visit)
        list(POP_FRONT targets_to_visit current_target)
        
        if(current_target IN_LIST visited_targets)
            continue()
        endif()
        
        if(NOT TARGET ${current_target})
            continue()
        endif()

        list(APPEND visited_targets ${current_target})

        set(deps "")
        get_target_property(iface_libs ${current_target} INTERFACE_LINK_LIBRARIES)
        if(iface_libs)
            list(APPEND deps ${iface_libs})
        endif()

        get_target_property(type ${current_target} TYPE)
        if(NOT type STREQUAL "INTERFACE_LIBRARY")
            get_target_property(libs ${current_target} LINK_LIBRARIES)
            if(libs)
                list(APPEND deps ${libs})
            endif()
        endif()

        foreach(dep IN LISTS deps)
            # Unwrap generator expressions (e.g. $<LINK_ONLY:tgt>, $<BUILD_INTERFACE:tgt>).
            # We strip wrappers instead of evaluating them because we are at configuration time
            # and exact build configuration might be unknown. This heuristic ensures we find
            # all potential dependencies. It might collect resources from dependencies that
            # are eventually not linked (e.g. debug-only libs in release build), but over-collecting
            # is safer than missing required resources.
            set(potential_targets "${dep}")
            while(potential_targets MATCHES "^\\$<[^:]+:(.+)>$")
                set(potential_targets "${CMAKE_MATCH_1}")
            endwhile()

            foreach(t IN LISTS potential_targets)
                if(TARGET "${t}")
                    list(APPEND targets_to_visit "${t}")
                endif()
            endforeach()
        endforeach()
    endwhile()

    set(${out_var} "${visited_targets}" PARENT_SCOPE)
endfunction()

function(_docwire_encode_resource out_var source dest)
    set(${out_var} "${source}|${dest}" PARENT_SCOPE)
endfunction()

function(_docwire_decode_resource entry out_source_var out_dest_var)
    string(FIND "${entry}" "|" separator_pos REVERSE)
    if(separator_pos EQUAL -1)
        message(FATAL_ERROR "DocWire: Invalid resource entry format: ${entry}")
    endif()
    string(SUBSTRING "${entry}" 0 ${separator_pos} _abs_path)
    math(EXPR separator_pos_plus_1 "${separator_pos} + 1")
    string(SUBSTRING "${entry}" ${separator_pos_plus_1} -1 _rel_dest_path)
    
    set(${out_source_var} "${_abs_path}" PARENT_SCOPE)
    set(${out_dest_var} "${_rel_dest_path}" PARENT_SCOPE)
endfunction()

function(_docwire_collect_resources_for_target app_target out_var)
    docwire_collect_transitive_dependencies(${app_target} dependency_targets)

    set(collected_resources "")

    foreach(current_target IN LISTS dependency_targets)
        get_target_property(interface_res ${current_target} DOCWIRE_RESOURCES)
        if(interface_res)
            get_target_property(build_sources ${current_target} _DOCWIRE_RESOURCE_SOURCES)
            if(build_sources)
                # Build tree context: Zip the lists
                list(LENGTH interface_res count_dst)
                list(LENGTH build_sources count_src)
                if(NOT count_dst EQUAL count_src)
                    message(FATAL_ERROR "DocWire: Resource property mismatch for target ${current_target}. DOCWIRE_RESOURCES and _DOCWIRE_RESOURCE_SOURCES have different lengths.")
                endif()
                math(EXPR loop_max "${count_dst} - 1")
                foreach(i RANGE ${loop_max})
                    list(GET build_sources ${i} src)
                    list(GET interface_res ${i} dst)
                    _docwire_encode_resource(encoded_res "${src}" "${dst}")
                    list(APPEND collected_resources "${encoded_res}")
                endforeach()
            else()
                # Imported target context (package consumers)
                foreach(rel_dest_path IN LISTS interface_res)
                    docwire_find_resource(found_path REL_PATH "${rel_dest_path}" REQUIRED)
                    _docwire_encode_resource(encoded_res "${found_path}" "${rel_dest_path}")
                    list(APPEND collected_resources "${encoded_res}")
                endforeach()
            endif()
        endif()
    endforeach()

    if(collected_resources)
        list(REMOVE_DUPLICATES collected_resources)
    endif()

    set(${out_var} "${collected_resources}" PARENT_SCOPE)
endfunction()

#[=======================================================================[.rst:
docwire_deploy_resources
------------------------
Prepares resources for an executable or library to be run from the build tree.

  docwire_deploy_resources(TARGETS <target> ...)

This function is intended for development and testing. It allows an application
to find its resources when run directly from the build output directory, without
needing to copy large resource files. It works by creating small `.path` files
that point to the actual location of the resources.

For creating a distributable installation, use `docwire_install_resources` instead.
#]=======================================================================]
function(docwire_deploy_resources)
    cmake_parse_arguments(PARSE_ARGS "" "" "TARGETS" ${ARGN})

    if(NOT PARSE_ARGS_TARGETS)
        message(FATAL_ERROR "docwire_deploy_resources requires TARGETS argument.")
    endif()

    foreach(app_target IN LISTS PARSE_ARGS_TARGETS)
        if(NOT TARGET ${app_target})
            message(FATAL_ERROR "docwire_deploy_resources: ${app_target} is not a target")
        endif()

        _docwire_collect_resources_for_target(${app_target} resources_to_deploy)

        if(resources_to_deploy)
            # Always deploy to ./share relative to the target executable.
            # This ensures resources remain inside the build directory structure on all platforms.
            set(deploy_base "$<TARGET_FILE_DIR:${app_target}>/share")

            foreach(res_pair IN LISTS resources_to_deploy)
                _docwire_decode_resource("${res_pair}" abs_path rel_dest_path)

                # Generate the .path file in a stable intermediate location.
                # We use $<CONFIG> to handle multi-configuration generators where abs_path might vary.
                set(intermediate_path "${CMAKE_CURRENT_BINARY_DIR}/docwire_deploy_resources_temp/${app_target}/$<CONFIG>/${rel_dest_path}.path")
                message(VERBOSE "Generating .path file for ${app_target}: ${rel_dest_path} -> ${abs_path}")
                file(GENERATE OUTPUT "${intermediate_path}" CONTENT "${abs_path}")

                set(final_path "${deploy_base}/${rel_dest_path}.path")
                get_filename_component(rel_dest_dir "${rel_dest_path}" DIRECTORY)
                set(final_dest_dir "${deploy_base}/${rel_dest_dir}")

                add_custom_command(TARGET ${app_target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${final_dest_dir}"
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${intermediate_path}" "${final_path}"
                    COMMENT "Linking resource pointer for ${rel_dest_path}"
                )
            endforeach()
        endif()
    endforeach()
endfunction()

include(GNUInstallDirs)

#[=======================================================================[.rst:
docwire_install_resources
-------------------------
Installs DocWire resources (data files) for the given targets.

  docwire_install_resources(TARGETS <target> ...)

This function should be used alongside standard CMake install commands to ensure
that data files required by DocWire libraries (like models or dictionaries)
are included in the final installation.
#]=======================================================================]
function(docwire_install_resources)
    cmake_parse_arguments(PARSE_ARGS "" "" "TARGETS" ${ARGN})

    if(NOT PARSE_ARGS_TARGETS)
        message(FATAL_ERROR "docwire_install_resources requires TARGETS argument.")
    endif()

    set(resources_to_install "")
    foreach(target IN LISTS PARSE_ARGS_TARGETS)
        if(NOT TARGET ${target})
            message(FATAL_ERROR "docwire_install_resources: ${target} is not a target")
        endif()
        _docwire_collect_resources_for_target(${target} target_resources)
        list(APPEND resources_to_install ${target_resources})
    endforeach()
    list(REMOVE_DUPLICATES resources_to_install)

    foreach(res_pair IN LISTS resources_to_install)
        _docwire_decode_resource("${res_pair}" abs_path rel_dest_path)

        if(IS_DIRECTORY "${abs_path}")
            install(DIRECTORY "${abs_path}/" DESTINATION "${CMAKE_INSTALL_DATADIR}/${rel_dest_path}" COMPONENT "Resources")
        else() # It's a file
            get_filename_component(dest_dir "${rel_dest_path}" DIRECTORY)
            install(FILES "${abs_path}" DESTINATION "${CMAKE_INSTALL_DATADIR}/${dest_dir}" COMPONENT "Resources")
        endif()
    endforeach()
endfunction()
