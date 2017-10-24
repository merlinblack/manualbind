# Bulid tests
cmake_minimum_required(VERSION 2.8)

find_package( Lua )

file( GLOB TEST_SRCS *.cpp )

include_directories( 
    ../src
    ${LUA_INCLUDE_DIR}
 )

add_executable( runtests ${TEST_SRCS} ../src/LuaBinding.cpp )

target_link_libraries( runtests
    ${LUA_LIBRARY}
    ${LUA_MATH_LIBRARY}
)

add_test( tests runtests )

set_property( TARGET runtests PROPERTY CXX_STANDARD 11 )
set_property( TARGET runtests PROPERTY CXX_STANDARD_REQUIRED ON )
