# Get doctotext version from git and store it in DOCTOTEXT_VERSION
function(doctotext_extract_version)
    find_package(Git)
    if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
        execute_process(COMMAND ${GIT_EXECUTABLE} describe --always --dirty
                        OUTPUT_VARIABLE doc_ver
                        RESULT_VARIABLE GIT_SUBMOD_RESULT # Return code
        )
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git describe --always --dirty failed with ${GIT_SUBMOD_RESULT}")
        endif()
    endif()

    # Match version in format 'maj.min.patch'-[anything else]
    set(simple_doc_ver, doc_ver)
    string(REGEX MATCH "^([^-\n]+)" simple_doc_ver "${simple_doc_ver}")
    string(REGEX MATCH "^([^\n]+)" doc_ver "${doc_ver}")

    set(empty_str "")
    if(doc_ver STREQUAL empty_str)
        message(FATAL_ERROR "Could not extract version number from git")
    endif()

    # Function output
    set(DOCTOTEXT_VERSION ${doc_ver} PARENT_SCOPE)
    set(SIMPLE_DOCTOTEXT_VERSION ${simple_doc_ver} PARENT_SCOPE)
endfunction()