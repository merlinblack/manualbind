#include "catch.hpp"
#include "LuaBinding.h"

void run( lua_State* L, const char* code );

using namespace ManualBind;

class Widget
{
    public:
    std::string stringy;
    int numbery;

    void setThings( std::string str, int num )
    {
        stringy = str;
        numbery = num;
    }
};

using WidgetPtr = std::shared_ptr<Widget>;

struct WidgetBinding : public Binding<WidgetBinding,Widget>
{
    static constexpr const char* class_name = "Widget";

    static luaL_Reg* members()
    {
        static luaL_Reg members[] =
        {
            { "setThings", callSetThings },
            { nullptr, nullptr }
        };
        return members;
    }

    static int create( lua_State* L )
    {
        return luaL_error( L, "Can not create an instance." );
    }

    static int callSetThings( lua_State* L )
    {
        WidgetPtr p = fromStack( L, 1 );

        std::string str( luaL_checkstring( L, 2 ) );
        int num = luaL_checkinteger( L, 3 );

        p->setThings( str, num );
        
        return 1;
    }
};

TEST_CASE( "Lua can call member functions." ) {

    lua_State* L = luaL_newstate();

    WidgetBinding::register_class( L );

    WidgetPtr p = std::make_shared<Widget>();

    WidgetBinding::push( L, p );
    lua_setglobal( L, "widget" );

    run( L, "widget:setThings( 'Hello World', 42 )" );

    REQUIRE( p->stringy == "Hello World" );

    REQUIRE( p->numbery == 42 );

    lua_close( L );
}
