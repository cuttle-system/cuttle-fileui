cmake_minimum_required(VERSION 3.0.2)
project(CuttleParser VERSION 0.2 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CMakeToolsHelpers OPTIONAL)
include(add_dependency)

set(CMAKE_BUILD_TYPE Debug)

set(BOOST_ROOT ../boost)
find_package(Boost 1.65.1 REQUIRED COMPONENTS filesystem)

set(CMAKE_CXX_FLAGS_DEBUG
  "${CMAKE_CXX_FLAGS_DEBUG} -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE
  "${CMAKE_CXX_FLAGS_RELEASE} -std=c++14")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -Wextra")
  set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -Wextra -O3")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
endif()

set(SRC_INCLUDE_DIRECTORIES
    "src/lib/"
    "../cuttle-parser/src"
    "../cuttle-translator/src"
    "../cuttle-generator/src"
    "${Boost_INCLUDE_DIR}/libs/filesystem/include"
    "${Boost_INCLUDE_DIR}")

file(GLOB SRCS "src/lib/*.cpp")
file(GLOB SRCS_MAIN "src/*.cpp")
file(GLOB_RECURSE TEST_SRCS "tests/*.cpp")
file(GLOB CUTTLE_TEST_SRCS "../cuttle-test/src/*.cpp")

file(GLOB HEADERS "src/lib/*.hpp")
file(GLOB_RECURSE TEST_HEADERS "tests/lib/*.hpp")

add_dependency(cuttle-parser)
add_dependency(cuttle-translator)
add_dependency(cuttle-generator)
add_dependency(cuttle-vm)
add_dependency(cuttle-lang)

add_library(cuttle-fileui-lib SHARED ${SRCS} ${BOOST_SRCS} ${HEADERS})
add_executable(cuttle-fileui-tests ${CUTTLE_TEST_SRCS} ${TEST_SRCS} ${HEADERS} ${TEST_HEADERS})
add_executable(cuttle-fileui ${SRCS_MAIN} ${HEADERS})

target_include_directories(cuttle-fileui PRIVATE ${SRC_INCLUDE_DIRECTORIES})
target_include_directories(cuttle-fileui-lib PRIVATE ${SRC_INCLUDE_DIRECTORIES})
target_include_directories(cuttle-fileui-tests
    PRIVATE
    "tests/"
    "../cuttle-test/src/"
    ${SRC_INCLUDE_DIRECTORIES})

target_link_libraries(cuttle-fileui-lib
    cuttle-parser
    cuttle-generator
    cuttle-translator
    cuttle-vm
    cuttle-lang
    ${Boost_LIBRARIES})

target_link_libraries(cuttle-fileui-tests cuttle-fileui-lib)
target_link_libraries(cuttle-fileui cuttle-fileui-lib)
