#include "common.h"
#include <iostream>
#include <lua.hpp>
#include "MyActor.h"
#include "MyActorBinding.h"
#include "MyVector3d.h"
#include "MyVector3dBinding.h"
#include "LuaRef.h"

using std::cout;
using std::endl;

int main(int argc, char **argv )
{
    lua_State* L = luaL_newstate();
    luaL_openlibs( L );
    run( L, "print( 'Selam Dünya' ) print( _VERSION )" );

    MyActorBinding::register_class( L );
    MyVector3dBinding::register_class( L );

    run( L, "a = MyActor( 'Alara', 27 )" );
    run( L, "a:walk()" );
    run( L, "b = a" );
    run( L, "a:setName('Awesome Chick')" );
    run( L, "a.extra = 'Coffee!'" );

    {
        lua_getglobal( L, "b" );
        MyActorPtr b = MyActorBinding::fromStackThrow( L, -1 );
        lua_pop( L, 1 );

        cout << "Use count is now: " << b.use_count() << endl;
        b->walk();
    }

    {
        MyActorPtr actor = std::make_shared<MyActor>("Nigel",39);
        cout << "Actor use count is: " << actor.use_count() << endl;
        MyActorBinding::push( L, actor );
        lua_setglobal( L, "actor" );
        cout << "Pushed to Lua" << endl;
        cout << "Actor use count is: " << actor.use_count() << endl;
        run( L, "actor:walk()" );
        run( L, "actor.age = actor.age + 1 print( 'Happy Birthday')" );
        run( L, "print( actor.age )" );
        cout << actor->_age << endl;
        // Should print Coffee, nil as 'added' members/properties are per instance.
        run( L, "print( a.extra, actor.extra )" ); 
    }

    {
        lua_getglobal( L, "a" );
        LuaBindGetExtraValuesTable( L, -1 );
        LuaRef extras = LuaRef::fromStack( L, -1 );
        lua_pop( L, 2 );    // "a", and the table.
        cout << extras["extra"].tostring() << endl;
    }

    run( L, "b = MyActor( 'Short lived', 0 )" );
    run( L, "b = nil" );
    run( L, "collectgarbage()" );

    {
        MyActorPtr shorty = std::make_shared<MyActor>("Shorty", 5);
        cout << "Shorty use count is: " << shorty.use_count() << endl;
        MyActorBinding::push( L, shorty );
        lua_setglobal( L, "shorty" );
        cout << "Shorty use count is: " << shorty.use_count() << endl;
        lua_pushnil( L );
        lua_setglobal( L, "shorty" );
        run( L, "collectgarbage()" );
        cout << "Shorty use count is: " << shorty.use_count() << endl;
    }

    // Override (for all instances) a method, while calling the old implementation
    // This updates the metatable because 'walk' is already defined there.
    run( L, "local old = actor.walk actor.walk = function(self) old(self) print( 'RUN!' ) end" );
    run( L, "actor:walk()" );

    run( L, "a:splat()" );

    run( L, "v = MyVector3d( 1, 2, 3 )" );
    run( L, "v.x = 4" );
    run( L, "print( v.x, v.y, v.z )" );
    run( L, "print( MyVector3d.ZERO )" );

    cout << "Closing Lua\n";
    lua_close( L );

    return 0;
}
