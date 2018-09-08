file(GLOB SRCS "*.cpp")

link_libraries(
        cuttle-parser
        cuttle-generator
        cuttle-translator
        cuttle-vm
        cuttle-lang
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY})

add_library(cuttle-fileui SHARED ${SRCS})