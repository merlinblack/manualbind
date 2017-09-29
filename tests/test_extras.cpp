#include "catch.hpp"
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
    luaL_openlibs( L );

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
