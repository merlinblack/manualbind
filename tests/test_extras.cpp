#include <catch2/catch_test_macros.hpp>
#include "LuaBinding.h"

void run( lua_State* L, const char* code );

using namespace ManualBind;

class Extra
{
};

using ExtraPtr = std::shared_ptr<Extra>;

struct ExtraBinding : public Binding<ExtraBinding,Extra>
{
    static constexpr const char* class_name = "Extra";

    static void setExtraMeta( lua_State* L )
    {
        lua_pushliteral( L, "Alara" );
        lua_setfield( L, -2, "DEF_NAME" );
    }
};

TEST_CASE( "Can set extra elements in class meta table." ) {

    lua_State* L = luaL_newstate();

    ExtraBinding::register_class( L );

    ExtraPtr pp = std::make_shared<Extra>();

    ExtraBinding::push( L, pp );
    lua_setglobal( L, "person" );

    run( L, "name = person.DEF_NAME" );

    lua_getglobal( L, "name" );
    std::string name( lua_tostring( L, -1 ) );

    REQUIRE( name == "Alara" );

    lua_close( L );
}

TEST_CASE( "Lua can assign extra elements per instance." ) {

    lua_State* L = luaL_newstate();

    ExtraBinding::register_class( L );

    ExtraPtr p1 = std::make_shared<Extra>();
    ExtraBinding::push( L, p1 );
    lua_setglobal( L, "p1" );

    ExtraPtr p2 = std::make_shared<Extra>();
    ExtraBinding::push( L, p2 );
    lua_setglobal( L, "p2" );

    run( L, "p1.perinstance = 'Nigel'" );

    run( L, "name = p1.perinstance" );

    lua_getglobal( L, "name" );
    std::string name( lua_tostring( L, -1 ) );

    REQUIRE( name == "Nigel" );

    run( L, "name = p2.perinstance" );

    lua_getglobal( L, "name" );

    REQUIRE( lua_type( L, -1 ) == LUA_TNIL );

    lua_close( L );
};
