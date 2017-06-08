#include "common.h"
#include "LuaBinding.h"
#include <iostream>
#include <vector>
#include "MyActor.h"
#include "MyActorBinding.h"

using std::cout;
using std::endl;

typedef std::vector<MyActorPtr> MyActorPtrList;

MyActorPtrList createList()
{
    const char* names[] = {
        "James",
        "Who? Random extra",
        "Harry",
        "Mike",
        nullptr
    };

    MyActorPtrList actors;

    for( int i = 0; names[i] != nullptr; i++ )
    {
        actors.push_back( std::make_shared<MyActor>( names[i] ) );
    }

    return actors;
}

void pushToLua( lua_State* L, MyActorPtrList list )
{
    lua_newtable( L );
    
    for( const auto& actor : list ) {
        MyActorBinding::push( L, actor );
        luaL_ref( L, -2 );
    }
}

MyActorPtrList pullFromLua( lua_State* L )
{
    // Note this only stores the values, not the keys/indexes,
    // which are usually just numbers.
    MyActorPtrList list;

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
        MyActorPtrList actors = createList();
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
        MyActorPtrList actors = pullFromLua( L );
        lua_pop( L, 1 );

        for( const auto& actor : actors )
        {
            cout << actor->_name << endl;
        }
    }

    lua_close(L);
}
