#ifndef __MYVECTOR3DBINDING_H
#define __MYVECTOR3DBINDING_H
#include "LuaBinding.h"
#include "MyVector3d.h"

typedef std::shared_ptr<MyVector3d> MyVector3dPtr;

struct MyVector3dBinding: public Binding<MyVector3dBinding, MyVector3d> {

    static constexpr const char* class_name = "MyVector3d";

    static luaL_Reg* members()
    {
        static luaL_Reg members[] = {
            { NULL, NULL }
        };
        return members;
    }

    static bind_properties* properties() {
        static bind_properties properties[] = {
            { "x", get, set },
            { "y", get, set },
            { "z", get, set },
            { NULL, NULL, NULL }
        };
        return properties;
    }

    // Lua constructor
    static int create( lua_State *L )
    {
        std::cout << "Create called\n";

        checkArgCount( L, 3 );

        float x = luaL_checknumber( L, 1 );
        float y = luaL_checknumber( L, 2 );
        float z = luaL_checknumber( L, 3 );

        MyVector3dPtr sp = std::make_shared<MyVector3d>( x, y, z );

        push( L, sp );

        return 1;
    }

    // Method glue functions
    //

    // [none]

    // Property getters and setters
    static const char *prop_keys[];

    // 1 - class metatable
    // 2 - key
    static int get( lua_State *L )
    {
        checkArgCount( L, 2 );

        MyVector3dPtr v = fromStack( L, 1 );

        int which = luaL_checkoption( L, 2, NULL, MyVector3dBinding::prop_keys );

        switch( which )
        {
            case 0:
                lua_pushnumber( L, v->x );
                break;
            case 1:
                lua_pushnumber( L, v->y );
                break;
            case 2:
                lua_pushnumber( L, v->z );
                break;
            default:
                luaL_argerror( L, 2, "What?" );
                break;
        }

        return 1;
    }

    // 1 - class metatable
    // 2 - key
    // 3 - value
    static int set( lua_State *L )
    {
        checkArgCount( L, 3 );

        MyVector3dPtr v = fromStack( L, 1 );

        int which = luaL_checkoption( L, 2, NULL, MyVector3dBinding::prop_keys );

        float value = lua_tonumber( L, 3 );

        switch( which )
        {
            case 0:
                v->x = value;
                break;
            case 1:
                v->y = value;
                break;
            case 2:
                v->z = value;
                break;
            default:
                luaL_argerror( L, 2, "What?" );
                break;
        }
        return 0;
    }

};

const char *MyVector3dBinding::prop_keys[] = { "x", "y", "z", NULL };

#endif // __MYVECTOR3DBINDING_H
