#include "common.h"
#include <iostream>
#include "LuaBinding.h"

// Up casting example.
// Needed when you need to call methods or functions expecting 
// a base class with a derived class. You might do this when
// you need to process different derived classes in the same way,
// for example rendering different GUI elements in a list.
// Maintains correct refcounts in the shared pointers.

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

using VehiclePtr = std::shared_ptr<Vehicle>;
using CarPtr = std::shared_ptr<Car>;

// Takes vehicles or decendants.
void StartVehicle( VehiclePtr veh )
{
    veh->Start();
}

/****************************** Bindings ***************************/

using namespace ManualBind;

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
            { "__upcast", upcast },
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

    static int upcast( lua_State *L )
    {
        CarPtr cp = fromStack( L, 1 );

        VehiclePtr vp = std::dynamic_pointer_cast<Vehicle>( cp );

        VehicleBinding::push( L, vp );

        return 1;
    }
};

int StartVehicleBinding( lua_State* L )
{
    // Get vehicle ptr from index 1.
    // It could be a 'VehiclePtr' or 'CarPtr'
    if( !luaL_testudata( L, 1, VehicleBinding::class_name ) )
    {
        // Will replace whatever is at index 1 with nil,
        // or the result of the __upcast call.
        LuaBindingUpCast( L, 1 );
    }

    VehiclePtr veh = VehicleBinding::fromStack( L, 1 );
    StartVehicle( veh );
    return 0;

}

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs( L );

    VehicleBinding::register_class( L );
    CarBinding::register_class( L );
    lua_register( L, "StartEngine", StartVehicleBinding );

    CarPtr newcar(new Car());

    CarBinding::push( L, newcar );
    lua_setglobal( L, "newcar" );

    run( L, "StartEngine(newcar)" );
    cout << "Newcar use count (should be 3): " << newcar.use_count() << endl;
    cout << "Garbage collect temporary up casted object" << endl;
    lua_gc( L, LUA_GCCOLLECT, 0 );
    cout << "Newcar use count (should be 2): " << newcar.use_count() << endl;

    VehiclePtr motorbike(new Vehicle());
    VehicleBinding::push( L, motorbike );
    lua_setglobal( L, "motorbike" );

    run( L, "StartEngine(motorbike)" );
    cout << "Motorbike use count (should be 2): " << motorbike.use_count() << endl;

    cout << "Shut down Lua" << endl;
    lua_close(L);
    // Both of these should be back down to 1.
    cout << "Newcar use count: " << newcar.use_count() << endl;
    cout << "Motorbike use count: " << motorbike.use_count() << endl;

    return 0;
}
