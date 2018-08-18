include_directories("../src/lib" ${Boost_INCLUDE_DIR})
link_libraries(cuttle-fileui ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

file(GLOB_RECURSE TEST_SRCS "*.cpp")

add_executable(cuttle-fileui-tests ${TEST_SRCS})