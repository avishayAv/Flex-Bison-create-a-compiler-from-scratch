
cmake_minimum_required(VERSION 3.8) # Optional..
project(hw5_compi)

set(CMAKE_CXX_STANDARD 14) # C++ 14 is needed for the following commands!
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0") # -O0 flag to disable compiler optimizations
# to give more precise breakpoints stops.

FIND_PACKAGE(FLEX)  # (For cygwin guide look at the pdf inside this repo).
FIND_PACKAGE(BISON) # Those two should be installed on your toolchain.

FLEX_TARGET(
        Lexer
        scanner.lex ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

BISON_TARGET(
        Parser # Just a name
        parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp # path to save output
)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})


ADD_EXECUTABLE(hw5 ${FLEX_Lexer_OUTPUTS} ${BISON_Parser_OUTPUTS}
        output.cpp output.hpp scanner.lex parser.ypp parser.hpp bp.cpp bp.hpp reg_utils.h)



