#include "common.h"
#include <iostream>
#include <vector>
#include <memory>
#include "LuaBinding.h"

// Up casting example.
// Needed when you need to call methods or functions expecting
// a base class when you have a derived class. You might do this when
// you need to process different derived classes in the same way,
// for example rendering different GUI elements in a list.
// Maintains correct refcounts in the shared pointers.

using namespace ManualBind;
using std::cout;
using std::endl;
using std::hex;
using std::dec;

// base / interface (doesn't have to be abstract)
class Renderable
{
public:
    virtual bool Render() = 0;
};

class Square: public Renderable
{
public:
    bool Render() {
        cout << hex << this << dec << " - I'm a Square!" << endl;
        return true;
    }
    virtual ~Square() {
    }
};

class Circle: public Renderable
{
public:
    bool Render() {
        cout << hex << this << dec << " - I'm a Circle!" << endl;
        return true;
    }
    virtual ~Circle() {
    }
};

using RenderablePtr = std::shared_ptr<Renderable>;
using SquarePtr = std::shared_ptr<Square>;
using CirclePtr = std::shared_ptr<Circle>;
using RenderList = std::vector<RenderablePtr>;

void RenderAll( const RenderList& rl )
{
    cout << "Rendering list:" << endl;

    for( const auto& obj : rl )
    {
        obj->Render();
    }
}

void AddObject( RenderList& rl, RenderablePtr& rp )
{
    rl.push_back( rp );
}

/****************************** Bindings ***************************/


struct RenderableBinding: public Binding<RenderableBinding, Renderable> {

    static constexpr const char* class_name = "Renderable";

};

struct SquareBinding: public Binding<SquareBinding, Square> {

    static constexpr const char* class_name = "Square";

    static luaL_Reg* members()
    {
        static luaL_Reg members[] = {
            { "__upcast", upcast },
            { nullptr, nullptr }
        };
        return members;
    }

    static int create( lua_State *L )
    {
        SquarePtr p = std::make_shared<Square>();
        push( L, p );
        return 1;
    }

    static int upcast( lua_State *L )
    {
        SquarePtr p = fromStack( L, 1 );

        RenderablePtr rp = std::dynamic_pointer_cast<Renderable>( p );

        RenderableBinding::push( L, rp );

        return 1;
    }
};

struct CircleBinding: public Binding<CircleBinding, Circle> {

    static constexpr const char* class_name = "Circle";

    static luaL_Reg* members()
    {
        static luaL_Reg members[] = {
            { "__upcast", upcast },
            { nullptr, nullptr }
        };
        return members;
    }

    static int create( lua_State *L )
    {
        CirclePtr p = std::make_shared<Circle>();
        push( L, p );
        return 1;
    }

    static int upcast( lua_State *L )
    {
        CirclePtr p = fromStack( L, 1 );

        RenderablePtr rp = std::dynamic_pointer_cast<Renderable>( p );

        RenderableBinding::push( L, rp );

        return 1;
    }
};

// Lua render list operations on a global list...
// Normally the list would not be global but in the interest
// of making this example smaller...

RenderList gRenderList;

int lua_render_all( lua_State* L )
{
    RenderAll( gRenderList );
    return 0;
}

int lua_add_object( lua_State* L )
{
    // Before upcast'ing, you may want to check if the 
    // class is the type you want already.
    // In this example the base is abstract so it will
    // not happen.
    
    // We are expecting an object with an __upcast function
    // returning a Renderable.
    LuaBindingUpCast( L, 1 );

    // Here you could check for nil, meaning the upcast failed.

    RenderablePtr rp = RenderableBinding::fromStack( L, 1 );

    AddObject( gRenderList, rp );

    return 0;
}

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs( L );

    RenderableBinding::register_class( L );
    SquareBinding::register_class( L );
    CircleBinding::register_class( L );

    lua_register( L, "add", lua_add_object );
    lua_register( L, "render", lua_render_all );

    run( L, "print 'Upcast Example'" );
    run( L, "add( Square() )" );
    run( L, "add( Circle() )" );
    run( L, "add( Circle() )" );
    run( L, "add( Square() )" );
    run( L, "render()" );

    lua_close(L);

    return 0;
}
