file(GLOB SRCS "*.cpp")

add_subdirectory(lib)

include_directories("lib/")
link_libraries(cuttle-fileui)

add_executable(cuttle-fileui-bin ${SRCS})
set_target_properties(cuttle-fileui-bin PROPERTIES OUTPUT_NAME cuttle-fileui)