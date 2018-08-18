cmake_minimum_required(VERSION 3.0.2)
project(CuttleFileUI VERSION 0.2 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CMakeToolsHelpers OPTIONAL)
include(add_dependency)

set(CMAKE_BUILD_TYPE Debug)

set(BOOST_ROOT ../boost)
find_package(Boost 1.65.1 REQUIRED COMPONENTS filesystem unit_test_framework)

set(CMAKE_CXX_STANDARD 14)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -Wextra")
  set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -Wextra -O3")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
endif()

add_dependency(cuttle-parser)
add_dependency(cuttle-translator)
add_dependency(cuttle-generator)
add_dependency(cuttle-vm)
add_dependency(cuttle-lang)

include_directories(
        "../cuttle-parser/src"
        "../cuttle-translator/src"
        "../cuttle-generator/src"
        "${Boost_INCLUDE_DIR}/libs/filesystem/include"
        "${Boost_INCLUDE_DIR}")

add_subdirectory(src)
add_subdirectory(tests)
