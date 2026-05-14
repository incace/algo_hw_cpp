# Usage: cmake -DEXE=<path> -DINPUT=<path> -DEXPECTED=<path> -P run_test.cmake
execute_process(
    COMMAND        ${EXE} ${INPUT}
    OUTPUT_VARIABLE actual
    RESULT_VARIABLE rc
)

file(READ ${EXPECTED} expected)

if(NOT "${actual}" STREQUAL "${expected}")
    message(FATAL_ERROR
        "FAIL: ${INPUT}\n"
        "--- expected ---\n${expected}"
        "--- got ---\n${actual}"
    )
endif()

message(STATUS "PASS: ${INPUT}")
