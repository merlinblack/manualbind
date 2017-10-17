cmake_minimum_required(VERSION 2.8)

find_package( Lua )

include_directories( 
    ../src
    common
    ${LUA_INCLUDE_PREFIX}
)

add_library( common common/common.cpp ../src/LuaBinding.cpp )

function( example NAME SRC )

    add_executable( ${NAME} ${SRC} )

    target_link_libraries( ${NAME}
        common
        ${LUA_LIBRARY}
        ${LUA_MATH_LIBRARY}
    )

    set_property( TARGET ${NAME} PROPERTY CXX_STANDARD 11 )
    set_property( TARGET ${NAME} PROPERTY CXX_STANDARD_REQUIRED ON )

endfunction()

example( exdemo demo/demo.cpp )
example( exluaref luaref/luaref.cpp )
example( exdowncast downcast/downcast.cpp )
example( exupcast upcast/upcast.cpp )
example( extables tables/tables.cpp )
example( exgui guiexample/guiexample.cpp )

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/gui.lua"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/guiexample/gui.lua" "${CMAKE_CURRENT_BINARY_DIR}/gui.lua"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/guiexample/gui.lua"
)
add_custom_target( scriptcopy ALL DEPENDS "gui.lua" )
