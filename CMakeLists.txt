cmake_minimum_required(VERSION 3.23)
project(Salix)

set(CMAKE_CXX_STANDARD 20)
set(X86E_VERSION "b0.1.3-a")

file(GLOB_RECURSE project_files
        "src/*.h"
        "src/*.cpp"
        "include/*.h"
        "include/*.cpp"
        )

file(GLOB_RECURSE tests_files
        "tests/*.h"
        "tests/*.cpp"
        )

add_executable(${PROJECT_NAME} ${project_files})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_compile_definitions(${PROJECT_NAME} PRIVATE VERSION=\"${X86E_VERSION}\")

add_executable(${PROJECT_NAME}_TESTS ${tests_files})
