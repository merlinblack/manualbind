#include <cstdlib>
#include <iostream>
#include <string.h>
#include <lua.hpp>

#include "LuaBinding.h"

using std::cout;
using std::endl;

// Some GUI class that represents a 2D rectangular region.
// This could be a 'window' or a 'button', etc.
//
// ( Like Betajean's Gorilla's rectangle. )
//

class Rectangle
{
    int _x, _y, _width, _height;

    public:
    Rectangle( int x, int y, int w, int h ) : _x(x), _y(y), _width(w), _height(h)
    {
        cout << "Created Rectangle" << endl;
    }

    bool isInside( int tx, int ty )
    {
        cout << "Checking x: " << tx << ", y: " << ty;
        cout << " Against ( " << _x << ", " << _y << " ) - ( " << _x + _width << ", " << _y + _height << " )" << endl;
        if( tx < _x )
            return false;
        if( ty < _y )
            return false;
        if( tx > _x + _width )
            return false;
        if( ty > _y + _height )
            return false;

        return true;
    }
    virtual ~Rectangle() 
    {
        cout << "Destroyed Rectangle" << endl;
    }
};

typedef std::shared_ptr<Rectangle> RectanglePtr;

using namespace ManualBind;

struct RectangleBinding: public Binding<RectangleBinding, Rectangle>
{

    static constexpr const char* class_name = "GUIRectangle";

    static luaL_Reg* members()
    {
        static luaL_Reg members[] = {
            { "isInside", isInside },
            { NULL, NULL }
        };
        return members;
    }

    static bind_properties* properties() {
        static bind_properties properties[] = {
            { NULL, NULL, NULL }
        };
        return properties;
    }

    // Lua constructor
    static int create( lua_State *L )
    {
        std::cout << "Create called\n";

        LuaBindingCheckArgCount( L, 4 );

        int x = luaL_checkinteger( L, 1 );
        int y = luaL_checkinteger( L, 2 );
        int w = luaL_checkinteger( L, 3 );
        int h = luaL_checkinteger( L, 4 );

        RectanglePtr sp = std::make_shared<Rectangle>( x, y, w, h );

        push( L, sp );

        return 1;
    }

    // Method glue functions
    //

    static int isInside( lua_State *L )
    {
        LuaBindingCheckArgCount( L, 3 );

        RectanglePtr rect = fromStack( L, 1 );
        int x = luaL_checkinteger( L, 2 );
        int y = luaL_checkinteger( L, 3 );

        lua_pushboolean( L, rect->isInside( x, y ) ); 
        return 1;
    }

    // Property getters and setters

    // None.

};

void test( lua_State* L )
{
    RectangleBinding::register_class( L );

    if( luaL_dofile( L, "test.lua" ) )
    {
        cout << lua_tostring( L, -1 ) << endl;
        lua_pop( L, 1 );
        return;
    }

    if( luaL_dostring( L, "test()" ) )
    {
        cout << lua_tostring( L, -1 ) << endl;
        lua_pop( L, 1 );
        return;
    }

    return;
}

int main()
{
    lua_State* L = luaL_newstate();

    luaL_openlibs( L );

    test( L );

    lua_close( L );

    return EXIT_SUCCESS;
}
