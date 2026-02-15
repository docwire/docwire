#[=======================================================================[.rst:
docwire_find_resource
---------------------
Finds a resource file or directory on the system or in the build environment.

  docwire_find_resource(<result_var> TYPE <FILE|DIRECTORY> REL_PATH <path> [REQUIRED])

This function searches standard locations and CMAKE_PREFIX_PATH/share.
#]=======================================================================]
function(docwire_find_resource result_var)
    cmake_parse_arguments(PARSE_ARGS "REQUIRED" "TYPE;REL_PATH" "" ${ARGN})

    if(NOT PARSE_ARGS_TYPE OR NOT PARSE_ARGS_REL_PATH)
        message(FATAL_ERROR "docwire_find_resource requires TYPE and REL_PATH arguments.")
    endif()

    get_filename_component(RESOURCE_NAME "${PARSE_ARGS_REL_PATH}" NAME)
    get_filename_component(RESOURCE_DIR_REL "${PARSE_ARGS_REL_PATH}" DIRECTORY)

    # Create a unique and predictable variable name for the cache.
    string(TOUPPER "${PARSE_ARGS_REL_PATH}" CACHE_VAR_SUFFIX)
    string(REPLACE "/" "_" CACHE_VAR_SUFFIX "${CACHE_VAR_SUFFIX}")
    string(REPLACE "." "_" CACHE_VAR_SUFFIX "${CACHE_VAR_SUFFIX}")
    set(CACHE_VAR_NAME "DOCWIRE_RESOURCE_${CACHE_VAR_SUFFIX}")

    set(FIND_ARGS "")
    if(PARSE_ARGS_REQUIRED)
        list(APPEND FIND_ARGS "REQUIRED")
    endif()

    set(FOUND_PATH "")

    set(docwire_search_paths "")
    foreach(prefix IN LISTS CMAKE_PREFIX_PATH)
        list(APPEND docwire_search_paths "${prefix}/share")
    endforeach()
    foreach(prefix IN LISTS CMAKE_SYSTEM_PREFIX_PATH)
        list(APPEND docwire_search_paths "${prefix}/share")
    endforeach()

    if("${PARSE_ARGS_TYPE}" STREQUAL "FILE")
        find_file(${CACHE_VAR_NAME}
            NAMES ${RESOURCE_NAME}
            PATHS ${docwire_search_paths}
            PATH_SUFFIXES ${RESOURCE_DIR_REL}
            NO_DEFAULT_PATH
            DOC "Path to resource file: ${PARSE_ARGS_REL_PATH}"
            ${FIND_ARGS}
        )
        set(FOUND_PATH "${${CACHE_VAR_NAME}}")
    elseif("${PARSE_ARGS_TYPE}" STREQUAL "DIRECTORY")
        # For directories, we find the parent path and append the name
        find_path(${CACHE_VAR_NAME}
            NAMES ${RESOURCE_NAME}
            PATHS ${docwire_search_paths}
            PATH_SUFFIXES ${RESOURCE_DIR_REL}
            NO_DEFAULT_PATH
            DOC "Path to resource directory: ${PARSE_ARGS_REL_PATH}"
            ${FIND_ARGS}
        )
        if(${CACHE_VAR_NAME})
             # find_path returns the directory *containing* the name, so we must append the name.
             set(FOUND_PATH "${${CACHE_VAR_NAME}}/${RESOURCE_NAME}")
        endif()
    else()
        message(FATAL_ERROR "docwire_find_resource: TYPE must be FILE or DIRECTORY.")
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
    # This is an INTERFACE property and will be part of the installed target information.
    set_property(TARGET "${target}" APPEND PROPERTY
        INTERFACE_DOCWIRE_RESOURCES
        "${rel_dest_path}"
    )
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
            if(TARGET "${dep}")
                list(APPEND targets_to_visit "${dep}")
            endif()
        endforeach()
    endwhile()

    set(${out_var} "${visited_targets}" PARENT_SCOPE)
endfunction()

#[=======================================================================[.rst:
docwire_deploy_resources
------------------------
Deploys resources for an executable or library in the build tree.

  docwire_deploy_resources(<app_target>)

Traverses the dependency graph of the given target, collects all associated resources,
and creates .path files in the build output directory to point to them.
#]=======================================================================]
function(docwire_deploy_resources app_target)
    if(NOT TARGET ${app_target})
        message(FATAL_ERROR "docwire_deploy_resources: ${app_target} is not a target")
    endif()

    docwire_collect_transitive_dependencies(${app_target} dependency_targets)

    set(resources_to_deploy "")

    foreach(current_target IN LISTS dependency_targets)
        get_target_property(interface_res ${current_target} INTERFACE_DOCWIRE_RESOURCES)
        if(interface_res)
            get_target_property(build_sources ${current_target} _DOCWIRE_RESOURCE_SOURCES)
            if(build_sources)
                # Build tree context: Zip the lists
                list(LENGTH interface_res count_dst)
                list(LENGTH build_sources count_src)
                if(NOT count_dst EQUAL count_src)
                    message(FATAL_ERROR "DocWire: Resource property mismatch for target ${current_target}. INTERFACE_DOCWIRE_RESOURCES and _DOCWIRE_RESOURCE_SOURCES have different lengths.")
                endif()
                math(EXPR loop_max "${count_dst} - 1")
                foreach(i RANGE ${loop_max})
                    list(GET build_sources ${i} src)
                    list(GET interface_res ${i} dst)
                    list(APPEND resources_to_deploy "${src}|${dst}")
                endforeach()
            else()
                # Imported target context (package consumers)
                foreach(rel_dest_path IN LISTS interface_res)
                    set(abs_resource_path "$<TARGET_FILE_DIR:${current_target}>/../share/${rel_dest_path}")
                    list(APPEND resources_to_deploy "${abs_resource_path}|${rel_dest_path}")
                endforeach()
            endif()
        endif()
    endforeach()

    if(NOT resources_to_deploy)
        return()
    endif()

    list(REMOVE_DUPLICATES resources_to_deploy)

    # Always deploy to ./share relative to the target executable.
    # This ensures resources remain inside the build directory structure on all platforms.
    set(deploy_base "$<TARGET_FILE_DIR:${app_target}>/share")

    foreach(res_pair IN LISTS resources_to_deploy)
        string(FIND "${res_pair}" "|" separator_pos REVERSE)
        string(SUBSTRING "${res_pair}" 0 ${separator_pos} abs_path)
        math(EXPR separator_pos_plus_1 "${separator_pos} + 1")
        string(SUBSTRING "${res_pair}" ${separator_pos_plus_1} -1 rel_dest_path)

        # Generate the .path file in a stable intermediate location.
        # We use $<CONFIG> to handle multi-configuration generators where abs_path might vary.
        set(intermediate_path "${CMAKE_CURRENT_BINARY_DIR}/docwire_deploy_resources_temp/${app_target}/$<CONFIG>/${rel_dest_path}.path")
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
endfunction()
