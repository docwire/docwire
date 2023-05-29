set(CMAKE_CXX_STANDARD 17)
foreach(n RANGE 1 9)
    add_executable(example_${n} example_${n}.cpp)
    target_include_directories(example_${n} PUBLIC ../src)
    target_link_libraries(example_${n} PRIVATE doctotext_core)
    set_property(TARGET example_${n} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_link_libraries(example_${n} PRIVATE doctotext_core)

    install(TARGETS example_${n}
            DESTINATION ${CMAKE_BINARY_DIR})

    file(WRITE ${CMAKE_BINARY_DIR}/example_${n}.sh "BASEDIR=$(dirname \"$0\")
       LD_LIBRARY_PATH=$BASEDIR:$$LD_LIBRARY_PATH ./$BASEDIR/example_${n} \"$@\"")
    execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/example_${n}.sh)
endforeach(n)

foreach(n RANGE 1 8)
    add_executable(c_example_${n} example_${n}.c)
    target_include_directories(c_example_${n} PUBLIC ../src)
    target_link_libraries(c_example_${n} PRIVATE doctotext_core)
    set_property(TARGET c_example_${n} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    install(TARGETS c_example_${n}
            DESTINATION ${CMAKE_BINARY_DIR})

    file(WRITE ${CMAKE_BINARY_DIR}/c_example_${n}.sh "BASEDIR=$(dirname \"$0\")
       LD_LIBRARY_PATH=$BASEDIR:$$LD_LIBRARY_PATH ./$BASEDIR/c_example_${n} \"$@\"")
    execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/c_example_${n}.sh)
endforeach(n)