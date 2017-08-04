#include "common.h"
#include "LuaBinding.h"
#include <iostream>
#include <vector>
#include "MyActor.h"
#include "MyActorBinding.h"

using std::cout;
using std::endl;

using MyActorList = std::vector<MyActorPtr>;

MyActorList createList()
{
    MyActorList actors {
        std::make_shared<MyActor>( "James" ),
        std::make_shared<MyActor>( "Who? Random extra" ),
        std::make_shared<MyActor>( "Harry" ),
        std::make_shared<MyActor>( "Mike" )
    };

    return actors;
}

void pushToLua( lua_State* L, MyActorList list )
{
    lua_newtable( L );

    for( const auto& actor : list ) {
        MyActorBinding::push( L, actor );
        luaL_ref( L, -2 );
    }
}

MyActorList pullFromLua( lua_State* L )
{
    // Note this only stores the values, not the keys/indexes,
    // which are usually just numbers.
    // Also it simply skips over any elements that are not 'MyActor's.
    MyActorList list;

    if( lua_istable( L, -1 ) )
    {
        lua_pushnil( L );

        while( lua_next( L, -2 ) )
        {
            if( luaL_testudata( L, -1, "MyActor" ) )
            {
                list.push_back( MyActorBinding::fromStack( L, -1 ) );
            }
            lua_pop( L, 1 );
        }
    }

    return list;
}

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs( L );

    MyActorBinding::register_class( L );

    {
        cout << "Pushing actor list to Lua." << endl;
        MyActorList actors = createList();
        pushToLua( L, actors );
        lua_setglobal( L, "actors" );
    }

    run( L, "for k,v in pairs(actors) do print( k, v.name ) end" );

    cout << "Editing actor list..." << endl;
    run( L, "actors[2] = nil                    -- Who is this? Pfft, delete." );
    run( L, "collectgarbage()" );
    run( L, "actors['test'] = MyActor('Bob')    -- Hey Bob welcome." );

    run( L, "for k,v in pairs(actors) do print( k, v.name ) end" );

    {
        cout << "Pull list back into C++ vector, and list..." << endl;
        lua_getglobal( L, "actors" );
        MyActorList actors = pullFromLua( L );
        lua_pop( L, 1 );

        for( const auto& actor : actors )
        {
            cout << actor->_name << endl;
        }
    }

    lua_close(L);
}
