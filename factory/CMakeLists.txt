cmake_minimum_required(VERSION 3.14)
project(Factory CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(factory main.cpp)

enable_testing()

file(GLOB TEST_INPUTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/*.in")
foreach(inp ${TEST_INPUTS})
    get_filename_component(base ${inp} NAME_WE)
    set(expected "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/${base}.out")
    if(EXISTS ${expected})
        add_test(
            NAME    test_${base}
            COMMAND ${CMAKE_COMMAND}
                    -DEXE=$<TARGET_FILE:factory>
                    -DINPUT=${inp}
                    -DEXPECTED=${expected}
                    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_test.cmake
        )
    endif()
endforeach()
