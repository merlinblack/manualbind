# Bulid tests
cmake_minimum_required(VERSION 3.0)

find_package( Lua )

add_subdirectory(Catch2)

file( GLOB TEST_SRCS *.cpp )

include_directories( 
    ../src
    ${LUA_INCLUDE_DIR}
)

add_executable( runtests ${TEST_SRCS} ../src/LuaBinding.cpp )

target_link_libraries( runtests
    PRIVATE
    ${LUA_LIBRARY}
    ${LUA_MATH_LIBRARY}
    Catch2::Catch2WithMain
)

add_test( tests runtests )

set_property( TARGET runtests PROPERTY CXX_STANDARD 20 )
set_property( TARGET runtests PROPERTY CXX_STANDARD_REQUIRED ON )
