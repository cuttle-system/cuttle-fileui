file(GLOB SRCS "lib/*.cpp")
file(GLOB SRCS_MAIN "*.cpp")

include_directories("lib/")
link_libraries(
    cuttle-parser
    cuttle-generator
    cuttle-translator
    cuttle-vm
    cuttle-lang
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY})

add_library(cuttle-fileui SHARED ${SRCS})
add_executable(cuttle-fileui-bin ${SRCS_MAIN})
target_link_libraries(cuttle-fileui-bin cuttle-fileui)

set_target_properties(cuttle-fileui-bin PROPERTIES OUTPUT_NAME cuttle-fileui)