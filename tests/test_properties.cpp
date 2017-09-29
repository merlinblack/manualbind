#include "catch.hpp"
#include "LuaBinding.h"

void run( lua_State* L, const char* code );

using namespace ManualBind;

class Person
{
    public:
    std::string name;
};

using PersonPtr = std::shared_ptr<Person>;

struct PersonBinding : public Binding<PersonBinding,Person>
{
    static constexpr const char* class_name = "Person";

    static bind_properties* properties()
    {
        static bind_properties properties[] =
        {
            { "name", getName, setName },
            { nullptr, nullptr, nullptr }
        };
        return properties;
    }

    static int getName( lua_State* L )
    {
        PersonPtr p = fromStack( L, 1 );

        lua_pushstring( L, p->name.c_str() );
        
        return 1;
    }

    static int setName( lua_State* L )
    {
        PersonPtr p = fromStack( L, 1 );

        // 2 is the property name

        const char* name = luaL_checkstring( L, 3 );

        p->name = name;

        return 0;
    }

};

TEST_CASE( "Lua can get and set properties." ) {

    lua_State* L = luaL_newstate();

    PersonBinding::register_class( L );

    PersonPtr pp = std::make_shared<Person>();

    PersonBinding::push( L, pp );
    lua_setglobal( L, "person" );

    run( L, "person.name ='Bob'" );

    REQUIRE( pp->name == "Bob" );

    pp->name = "Jane";

    run( L, "name = person.name" );

    lua_getglobal( L, "name" );
    const char* name = luaL_checkstring( L, -1 );

    REQUIRE( std::string(name) == "Jane" );

    lua_close( L );
}
