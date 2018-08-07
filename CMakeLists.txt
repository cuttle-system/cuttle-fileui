cmake_minimum_required(VERSION 3.0.2)

project(CuttleFileUI VERSION 0.1 LANGUAGES CXX)

include(CMakeToolsHelpers OPTIONAL)

set(BOOST_ROOT deps/boost)
find_package(Boost 1.65.1 REQUIRED COMPONENTS filesystem)

file(GLOB SRCS "src/lib/*.cpp")
file(GLOB SRCS_MAIN "src/*.cpp")
file(GLOB_RECURSE TEST_SRCS "tests/*.cpp")

file(GLOB HEADERS "src/lib/*.hpp")
file(GLOB_RECURSE TEST_HEADERS "tests/lib/*.hpp")

set(CMAKE_BUILD_TYPE Debug)

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

add_subdirectory(deps/cuttle-parser)
add_subdirectory(deps/cuttle-translator)
add_subdirectory(deps/cuttle-generator)
add_subdirectory(deps/cuttle-vm)

add_library(cuttle_fileui_lib SHARED ${SRCS} ${BOOST_SRCS} ${HEADERS})
add_executable(cuttle_fileui_tests ${TEST_SRCS} ${HEADERS} ${TEST_HEADERS})
add_executable(cuttle_fileui ${SRCS_MAIN} ${HEADERS})

target_include_directories(cuttle_fileui PRIVATE "src/lib/")
target_include_directories(cuttle_fileui_lib PRIVATE "src/lib/" "src/" "${Boost_INCLUDE_DIR}/libs/filesystem/include" "${Boost_INCLUDE_DIR}")
target_include_directories(cuttle_fileui_tests PRIVATE "src/lib/" "tests/" "tests/cuttle-test/src/" "${Boost_INCLUDE_DIR}/libs/filesystem/include" "${Boost_INCLUDE_DIR}")

target_link_libraries(cuttle_fileui_lib cuttle_parser)
target_link_libraries(cuttle_fileui_lib cuttle_generator)
target_link_libraries(cuttle_fileui_lib cuttle_translator)
target_link_libraries(cuttle_fileui_lib ${Boost_LIBRARIES})

target_link_libraries(cuttle_fileui_tests cuttle_fileui_lib)
target_link_libraries(cuttle_fileui cuttle_fileui_lib)
