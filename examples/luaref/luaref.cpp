#include <iostream>
#include <lua.hpp>
#include "common.h"
#include "LuaRef.h"

using namespace std;
using namespace ManualBind;

LuaRef getTesting( lua_State* L )
{
    lua_getglobal( L, "testing" );
    return LuaRef::fromStack(L);
}

void printString( const std::string& str )
{
    cout << str << endl;
}

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    run( L, "function testing( ... ) print( '> ', ... ) end" );

    {
        LuaRef testing( L, "testing" );
        LuaRef table = LuaRef::newTable( L );
        table["testing"] = testing;

        table.push();
        lua_setglobal( L, "a" );

        run( L, "print( a.testing )" );
        run( L, "a.b = {}" );
        run( L, "a.b.c = {}" );

        cout << "Is table a table? "
             << (table.isTable() ? "true" : "false" )
             << endl;
        cout << "Is table[\"b\"] a table? "
             << (table["b"].isTable() ? "true" : "false" )
             << endl;

        table["b"]["c"]["hello"] = "World!";
        // A lot happens here behind the scenes.
        // The previous line is effectively the same as:
        // {
        //   temp = table["b"];
        //   temp2 = temp["c"];
        //   temp3 = temp2["hello"];
        //   temp3 = "World!"; // Calls overloaded = operator.
        //   // Here temps go out of scope and are destructed.
        // }
        //
        // Each temp is a pair of luaL_ref and luaL_unref's.
        //
        // If you are in a tight loop you will want to do this...
        // auto hello = table["b"]["c"]["hello"];
        // for( ... ) {
        //   hello = something( ..., ..., ... );
        //   whatever( hello );
        // }

        run( L, "print( a.b.c.hello )" );

        auto b = table["b"]; // returns a LuaTableElement
        b[3] = "Index 3";

        LuaRef faster_b = b; // Convert LuaTableElement to LuaRef for faster pushing 

        for( int i = 1; i < 5; i++ )
        {
            faster_b.append( i );
        }
        b[1] = LuaNil();
        b.append( "Add more." );

        run( L, "for k,v in pairs( a.b ) do print( k,v ) end" );

        table["b"] = LuaNil();

        run( L, "print( a.b )" );

        testing();
        testing( 1, 2, 3 );
        testing( "Hello", "World" );

        testing( "Hello", "World", 1, 2, 3, testing );

        testing( "Nigel", "Alara", "Aldora", "Ayna",
                 "Sarah", "Gavin", "Joe",    "Linda",
                 "Tom",   "Sonja", "Greg",   "Trish"
               );

        // No return value
        testing.call( 0, "No return value." );

        table["testing"](testing,3,2,1,"Calling array element");
        table["testing"]();

        LuaRef newfuncref( L );

        newfuncref = testing;

        newfuncref( "Did it copy correctly?" );

        newfuncref( getTesting( L ) ); // Check move semantics

        newfuncref = getTesting( L ); // Check move semantics

        run( L, "text = 'This has been implicitly cast to std::string'" );

        LuaRef luaStr1( L, "text" );

        std::string str1 = luaStr1;

        printString( str1 );

        run( L, "a.text = text" );

        printString( table["text"] );

    }

    lua_close(L);
    return 0;
}
