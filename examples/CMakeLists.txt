cmake_minimum_required(VERSION 3.0...3.31)

find_package( Lua )

include_directories(
    ../src
    common
    ${LUA_INCLUDE_DIR}
)

add_library( common common/common.cpp common/common.h ../src/LuaBinding.cpp )

set_property( TARGET common PROPERTY CXX_STANDARD 20 )
set_property( TARGET common PROPERTY CXX_STANDARD_REQUIRED ON )

function( example name dir )

  file( GLOB SRCS "${dir}/*.cpp" "${dir}/*.h" "${dir}/*.lua" )

  add_executable( ${name} ${SRCS} )

  target_link_libraries(${name}
        common
        ${LUA_LIBRARY}
        ${LUA_MATH_LIBRARY}
  )

  set_property( TARGET ${name} PROPERTY CXX_STANDARD 20 )
  set_property( TARGET ${name} PROPERTY CXX_STANDARD_REQUIRED ON )

endfunction()

example( exdemo demo )
example( exluaref luaref )
example( exdowncast downcast )
example( exupcast upcast )
example( extables1 tables1 )
example( extables2 tables2 )
example( extables3 tables3 )
example( exgui guiexample )

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/gui.lua"
    COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_CURRENT_SOURCE_DIR}/guiexample/gui.lua"
      "${CMAKE_CURRENT_BINARY_DIR}/gui.lua"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/guiexample/gui.lua"
)
add_custom_target( scriptcopy ALL DEPENDS "gui.lua" )
