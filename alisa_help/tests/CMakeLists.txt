# Build the test runner executable.
add_executable(test_runner test_runner.cpp)
target_link_libraries(test_runner PRIVATE alisa_lib)

# Register one CTest per *.in file that has a matching *.out file.
file(GLOB TEST_INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/data/*.in")
foreach(inp ${TEST_INPUTS})
    get_filename_component(base ${inp} NAME_WE)
    set(expected "${CMAKE_CURRENT_SOURCE_DIR}/data/${base}.out")
    if(EXISTS ${expected})
        add_test(
            NAME    test_${base}
            COMMAND test_runner ${inp} ${expected}
        )
    endif()
endforeach()
