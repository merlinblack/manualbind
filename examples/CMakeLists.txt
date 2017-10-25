cmake_minimum_required(VERSION 2.8)

find_package( Lua )

include_directories( 
    ../src
    common
    ${LUA_INCLUDE_DIR}
)

add_library( common common/common.cpp common/common.h ../src/LuaBinding.cpp )

set_property( TARGET common PROPERTY CXX_STANDARD 11 )
set_property( TARGET common PROPERTY CXX_STANDARD_REQUIRED ON )

function( example NAME DIR )

    file( GLOB SRCS "${DIR}/*.cpp" "${DIR}/*.h" "${DIR}/*.lua" )

    add_executable( ${NAME} ${SRCS} )

    target_link_libraries( ${NAME}
        common
        ${LUA_LIBRARY}
        ${LUA_MATH_LIBRARY}
    )

    set_property( TARGET ${NAME} PROPERTY CXX_STANDARD 11 )
    set_property( TARGET ${NAME} PROPERTY CXX_STANDARD_REQUIRED ON )

endfunction()

example( exdemo demo )
example( exluaref luaref )
example( exdowncast downcast )
example( exupcast upcast )
example( extables tables )
example( exgui guiexample )

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/gui.lua"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/guiexample/gui.lua" "${CMAKE_CURRENT_BINARY_DIR}/gui.lua"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/guiexample/gui.lua"
)
add_custom_target( scriptcopy ALL DEPENDS "gui.lua" )
