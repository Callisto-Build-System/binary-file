cmake_minimum_required(VERSION 3.14)
project(binary-file)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ROM_WRAP_SOURCE_FILES "")

list(
        APPEND ROM_WRAP_SOURCE_FILES
        src/binary_file.cpp
        src/rom.cpp
        src/libstr.cpp
        src/address.cpp
)

option(ROM_WRAP_BUILD_DLL "Build Binary File as a dynamic library" ON)
option(ROM_WRAP_BUILD_LIB "Build Binary File as a static library" ON)

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.2.1
)
FetchContent_MakeAvailable(fmt)

if (ROM_WRAP_BUILD_LIB)
    add_library(${PROJECT_NAME}_static STATIC ${ROM_WRAP_SOURCE_FILES})
    target_link_libraries(${PROJECT_NAME}_static PRIVATE fmt::fmt)
endif()

if (ROM_WRAP_BUILD_DLL)
    add_library(${PROJECT_NAME} SHARED ${ROM_WRAP_SOURCE_FILES})
    target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
endif()

target_include_directories(${PROJECT_NAME}_static
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
