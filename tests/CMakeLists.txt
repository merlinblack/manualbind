# Bulid tests
cmake_minimum_required(VERSION 2.8)

find_package( Lua )

file( GLOB TEST_SRCS *.cpp )

include_directories( 
    ../src
    ${LUA_INCLUDE_DIR}
 )

add_executable( tests ${TEST_SRCS} ../src/LuaBinding.cpp )

target_link_libraries( tests
    ${LUA_LIBRARY}
    ${LUA_MATH_LIBRARY}
)

set_property( TARGET tests PROPERTY CXX_STANDARD 11 )
set_property( TARGET tests PROPERTY CXX_STANDARD_REQUIRED ON )
