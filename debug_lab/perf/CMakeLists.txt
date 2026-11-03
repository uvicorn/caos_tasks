cmake_minimum_required(VERSION 3.11)

# select compilers
set(CMAKE_C_STANDARD 17)

project(Lab7_RMQ_RSQ)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_C_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

set(SRC_DIR "src")
set(TEST_DIR "tests")

message("DEFAULT_FLAGS: ${CMAKE_C_FLAGS}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Mode: DEBUG")
    add_compile_options(-Og -ggdb -fsanitize=address -DDEBUG)
    add_link_options(-fsanitize=address)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Mode: Release")
    add_compile_options(-Ofast -march=native -DNDEBUG -ggdb)
endif()


# clear target
add_custom_target(clear
    COMMAND rm -rf "${PROJECT_BINARY_DIR}"
    COMMAND rm -rf "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)


# main lib
file(
    GLOB MAIN_LIB_SRC CONFIGURE_DEPENDS 
    "${SRC_DIR}/*.h" "${SRC_DIR}/*.c"
)
message("MAIN_SRC: ${MAIN_LIB_SRC}")

add_library(main_library STATIC ${MAIN_LIB_SRC})
target_include_directories(main_library PRIVATE include)

# test runner
file(GLOB TEST_RUNNER_SRC CONFIGURE_DEPENDS "${TEST_DIR}/*.h" "${TEST_DIR}/*.c")
message("TEST_RUNNER_SRC: ${TEST_RUNNER_SRC}")

add_executable(
    test_runner
    ${TEST_RUNNER_SRC}
)

target_include_directories(test_runner PRIVATE include)
target_link_libraries(test_runner PRIVATE main_library)