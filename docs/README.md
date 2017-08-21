(LICENSE)[Licence] (TODO)[Todo List]

ManualBind
==========

Semi-manual binding between C++ and Lua.
See header files and LICENSE for author and license info.

Examples
--------
There are several examples, which show various parts or techniques.
- Demo. Runs LuaBinding.h through it's paces.
- Downcast. How to cast a Lua held class instance to another class that is lower in the hierarchy.
- Upcast. Opposite of downcast. Useful if you are storing instances in a C++ container.
- GUI Example. How a simple GUI might be implemented in Lua, given a simple rectangle class binding.
- LuaRef. Exercises the new LuaRef code.
- Tables. How to push / pull a C++ container to a Lua table.

LuaBinding.h
------------
Basically this makes the boring and difficult stuff easier, but leaving the more fun
not so hard stuff to do. :-)

The memory management, life-cycle, and method despatch is the same for each
bound class, only the glue functions that marshal the parameters are
different.

Hand writing the glue functions also allows them to be as complicated or as
simple as required.  For example you could support optional arguments, or even
overloading.

Shared pointers are used to simplify the question of who owns what, and who
should delete it. Class instances can be created in Lua or C++ and passed to
the other freely, as the last one holding a reference will call the
destructor when required.

There is a POD (plain old data) version of the binding for things like pointers or
tiny classes and structs. These get copied around unlike the shared pointer version which 
keeps one and only one copy.

Extra elements can be assigned to a class instance as long as they do not conflict with an existing
function or property. A table will be created for each class instance on the first value assigned, so
if this feature is not used - no extra memory is required for a class instance.

Methods can be overridden on a class in Lua code, however unlike extra elements, this will affect 
**all** instances, both existing and new.

**Speed**
Method lookup is the fastest, followed by properties, extra elements and finally the case of not found.
Each case requires progressively more table lookups in addition to the order they are done.

Example binding
---------------
This is real life code taken from my game. In this instance Allegro colors are treated
as POD, i.e. they do not get shared, but copied around.  Also there are no member functions
however there are 4 properties. Each property uses the same get and set functions.
```c++
#ifndef LB_COLOR_H
#define LB_COLOR_H

#include <allegro5/allegro.h>
#include "LuaBinding.h"

struct ColorBinding : public PODBinding<ColorBinding,ALLEGRO_COLOR>
{
    static constexpr const char* class_name = "AllegroColor";

    static luaL_Reg* members()
    {
        return nullptr; // No members for this class.
    }

    static bind_properties* properties()
    {
        static bind_properties properties[] = {
            { "r", get_rgba, set_rgba },
            { "g", get_rgba, set_rgba },
            { "b", get_rgba, set_rgba },
            { "a", get_rgba, set_rgba },
            { nullptr, nullptr, nullptr }
        };
        return properties;
    }

    static int create( lua_State* L )
    {
        unsigned char r, g, b, a;

        r = luaL_checkinteger( L, 1 );
        g = luaL_checkinteger( L, 2 );
        b = luaL_checkinteger( L, 3 );
        a = luaL_checkinteger( L, 4 );

        ALLEGRO_COLOR c = al_map_rgba( r, g, b, a );

        push( L, c );

        return 1;
    }

    static const char* prop_keys[];

    static int get_rgba( lua_State* L )
    {
        unsigned char r, g, b, a;

        ALLEGRO_COLOR& c = fromStack( L, 1 );

        al_unmap_rgba( c, &r, &g, &b, &a );

        int which = luaL_checkoption( L, 2, nullptr, ColorBinding::prop_keys );

        switch( which )
        {
            case 0:
                lua_pushinteger( L, r );
                break;

            case 1:
                lua_pushinteger( L, g );
                break;

            case 2:
                lua_pushinteger( L, b );
                break;

            case 3:
                lua_pushinteger( L, a );
                break;

        }

        return 1;
    }

    static int set_rgba( lua_State* L )
    {
        unsigned char r, g, b, a;

        ALLEGRO_COLOR& c = fromStack( L, 1 );

        int which = luaL_checkoption( L, 2, nullptr, ColorBinding::prop_keys );

        al_unmap_rgba( c, &r, &g, &b, &a );

        switch( which )
        {
            case 0:
                r = luaL_checkinteger( L, 3 );
                break;

            case 1:
                g = luaL_checkinteger( L, 3 );
                break;

            case 2:
                b = luaL_checkinteger( L, 3 );
                break;

            case 3:
                a = luaL_checkinteger( L, 3 );
                break;
        }

        c = al_map_rgba( r, g, b, a );

        return 0;
    }

};

const char* ColorBinding::prop_keys[] = { "r", "g", "b", "a", nullptr };

#endif //LB_COLOR_H
```

LuaRef.h
--------
C++ management of Lua data. A re-write of code originally written by me and then 
donated to LuaBridge. 
This version I have changed to use c++11 variable template arguments for
calling Lua, The table element proxy is now public, and the code structure is
re-arranged to put shared functionality between LuaRef, and TableElement in a 
base class.

Allows you to write things like this in C++
```c++
// Grab a ref to settings table.
LuaRef settings = LuaRef( L, "settings" );
// Get the value of 'settings.display.width'
int width = settings["display"]["width"];
// Set a setting
settings["flux"]["capacitor"] = true;

// Grab a ref to the print function
LuaRef print = LuaRef( L, "print" );
// Use it like a C/C++ varadic function.
print( 1, 2, "Hello", width, tbl );
```

LuaException.h
--------------
Support for throwing on Lua errors. Used by LuaRef.

LuaStack.h
----------
From LuaBridge. The original LuaRef.h was changed to use this. I didn't want to reverse
that. Originally called Stack.h. If I wrote this myself, it would look almost the same
anyhow. :-)
