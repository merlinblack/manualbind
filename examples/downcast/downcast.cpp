#include "common.h"
#include <iostream>
#include "LuaBinding.h"

// Down casting example.
// Not often needed, but here is how to do it, while maintaining 
// correct refcounts in the shared pointers.

using std::cout;
using std::endl;

class Vehicle
{
    public:
    virtual bool Start() {
        cout << "Engine running!" << endl;
        return true;
    }
 
    virtual ~Vehicle() {
    }
};

class Car: public Vehicle
{
    public:
    bool LoadGroceries() {
        cout << "Grocieries have been loaded." << endl;
        return true;
    }
};

/****************************** Bindings ***************************/

typedef std::shared_ptr<Vehicle> VehiclePtr;
typedef std::shared_ptr<Car> CarPtr;

struct VehicleBinding: public Binding<VehicleBinding, Vehicle> {
    
    static constexpr const char* class_name = "Vehicle";

    static luaL_Reg* members()
    {
        static luaL_Reg members[] = {
            { "Start", Start },
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

    static int create( lua_State *L )
    {
        VehiclePtr vp = std::make_shared<Vehicle>();
        push( L, vp );
        return 1;
    }

    static int Start( lua_State *L )
    {
        VehiclePtr vp = fromStack( L, 1 );
        lua_pushboolean( L, vp->Start() );
        return 1;
    }
};

struct CarBinding: public Binding<CarBinding, Car> {
    
    static constexpr const char* class_name = "Car";

    static luaL_Reg* members()
    {
        static luaL_Reg members[] = {
            { "Start", Start },
            { "LoadGroceries", LoadGroceries },
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

    static int create( lua_State *L )
    {
        CarPtr cp = std::make_shared<Car>();
        push( L, cp );
        return 1;
    }

    static int Start( lua_State *L )
    {
        CarPtr cp = fromStack( L, 1 );
        lua_pushboolean( L, cp->Start() );
        return 1;
    }

    static int LoadGroceries( lua_State *L )
    {
        CarPtr cp = fromStack( L, 1 );
        lua_pushboolean( L, cp->LoadGroceries() );
        return 1;
    }

    // Defined here for easy access to CarBinding::push.
    static int downcast( lua_State *L )
    {
        // Next two lines are similar to the fromStack function
        void* ud = luaL_checkudata( L, 1, "Vehicle" );

        VehiclePtr vp = *((std::shared_ptr<Vehicle>*)ud);

        // This may return nullptr, if the class pointed to by vp
        // is not a Car, and the refcount will not be updated.
        CarPtr cp = std::dynamic_pointer_cast<Car>( vp );

        push( L, cp );

        return 1;
    }
};

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs( L );

    VehicleBinding::register_class( L );
    CarBinding::register_class( L );
    // Separately registered so it can be called without an instance
    // to Car.
    lua_register( L, "downcast", CarBinding::downcast );

    VehiclePtr newcar(new Car());
    
    VehicleBinding::push( L, newcar );
    lua_setglobal( L, "newcar" );

    run( L, "newcar:Start()" );
    cout << "Newcar use count: " << newcar.use_count() << endl;

    run( L, "newcar:LoadGroceries()" );
    cout << "Newcar use count: " << newcar.use_count() << endl;

    run( L, "p = downcast( newcar )" );
    // Use count should be higher.
    cout << "Newcar use count: " << newcar.use_count() << endl;

    run( L, "p:LoadGroceries()" );
    cout << "Newcar use count: " << newcar.use_count() << endl;

    VehiclePtr motorbike(new Vehicle());
    VehicleBinding::push( L, motorbike );
    lua_setglobal( L, "motorbike" );

    run( L, "motorbike:Start()" );
    cout << "Motorbike use count: " << motorbike.use_count() << endl;

    run( L, "p = downcast( motorbike )" );
    cout << "Motorbike use count: " << motorbike.use_count() << endl;

    run( L, "if p == nil then print( 'Motorbike is not a car. Can not load groceries. Go have fun instead.' ) end" );    
    run( L, "print(p)" );

    lua_close(L);
    // Both of these should be back down to 1.
    cout << "Newcar use count: " << newcar.use_count() << endl;
    cout << "Motorbike use count: " << motorbike.use_count() << endl;

    return 0;
}
