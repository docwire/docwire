# CHECK FOR BREW INCLUDE AND LIBRARY PATHS

find_program(BREW_EXECUTABLE brew)
if(NOT BREW_EXECUTABLE)
    message(WARNING "Brew executable not found!")
else()
    message(STATUS "Found brew: ${BREW_EXECUTABLE}")

    execute_process(COMMAND brew --prefix OUTPUT_VARIABLE brew_prefix OUTPUT_STRIP_TRAILING_WHITESPACE)
    include_directories(${brew_prefix}/include)
    link_directories(${brew_prefix}/lib)
endif()

# FOR SOME REASONS /usr/local PATHS ARE NOT SEARCHED BY DEFAULT
include_directories(/usr/local/include)
link_directories(/usr/local/lib)
