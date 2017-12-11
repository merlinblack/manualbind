cmake_minimum_required( VERSION 2.8 )
project( ManualBind )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

enable_testing()

add_subdirectory( examples )
add_subdirectory( tests )

message( STATUS "Lua Library: ${LUA_LIBRARY}" )
