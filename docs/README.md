ManualBind
==========

[Licence](../LICENSE) | [Tutorial](quick%20tutorial) | [Todo List](TODO)

[![Software License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat-square)](LICENSE)

Semi-manual binding between C++ and Lua.
See header files and LICENSE for author and license info.

*Update December 2025.*
I've checked this still works with Lua 5.5 rc4. All tests pass, and the examples are working.
I'm still using this library a lot, mainly with my 'disorganiser' project.

*Update December 2020.*
I've checked this still works with Lua 5.4. All tests pass, and the examples are working.

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

This is a binding from my 'disorganiser' project. It binds a class representing a rounded rectangle,
to be drawn by SDL. This can be 'upcast' to it's base class 'Renderable' so that it can be added to
a render list which may contain other graphic elements which all share Renderable as a base.

No properties used on this one as I had no need to retreive values, however note that the last
parameter (fillColor) of the constructor is optional.

The tostring member function is used for debugging.

```c++
#ifndef __LB_ROUNDED_RECTANGLE_H
#define __LB_ROUNDED_RECTANGLE_H

#include "LuaBinding.h"
#include "LuaRef.h"
#include "bindings.h"
#include "rounded_rectangle.h"
#include "lb_renderlist.h"
#include "lb_color.h"
#include <sstream>

struct RoundedRectangleBinding : public ManualBind::Binding<RoundedRectangleBinding,RoundedRectangle>
{
 static constexpr const char* class_name = "RoundedRectangle";

 static luaL_Reg* members()
 {
  static luaL_Reg members[] = {
   { "__upcast", upcast },
   { "__tostring", tostring },
   { "setDest", setDest },
   { "setColor", setColor },
   { "setFill", setFill },
   { "setRadius", setRadius },
   { nullptr, nullptr }
  };
  return members;
 }

 static int create(lua_State *L)
 {
  using ManualBind::LuaRef;

  SDL_Color &color = ColorBinding::fromStack(L, 1);
  SDL_Rect dest = getRectFromTable(L, 2);
  int radius = lua_tointeger(L, 3);

  if (lua_isnoneornil( L, 4))
  {
   RoundedRectanglePtr rectangle = std::make_shared<RoundedRectangle>(color, dest, radius);

   push(L, rectangle);
   return 1;
  }

  SDL_Color &fillColor = ColorBinding::fromStack(L, 4);

  RoundedRectanglePtr rectangle = std::make_shared<RoundedRectangle>(color, dest, radius, fillColor);

  push(L, rectangle);

  return 1;
 }

 static int upcast( lua_State *L )
 {
  RoundedRectanglePtr p = fromStack( L, 1 );

  RenderablePtr rp = std::dynamic_pointer_cast<Renderable>( p );

  RenderableBinding::push( L, rp );

  return 1;
 }

 static int tostring( lua_State* L )
 {
  RoundedRectanglePtr r = fromStack( L, 1 );
  std::stringstream ss;

  ss << "Rounded Rectangle: " << std::hex << r.get() << std::dec;
  ss << " dest: { ";
  SDL_Rect rect = r->getDest();
  ss << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h;
  ss << "}";

  lua_pushstring( L, ss.str().c_str() );

  return 1;
 }

 static int setDest( lua_State* L )
 {
  RoundedRectanglePtr p = fromStack( L, 1 );
  SDL_Rect r = getRectFromTable( L, 2 );

  p->setDest(r);
  return 0;
 }

 static int setColor( lua_State* L )
 {
  RoundedRectanglePtr p = fromStack( L, 1 );
  SDL_Color& c = ColorBinding::fromStack( L, 2 );

  p->setColor(c);
  return 0;
 }

 static int setFill( lua_State* L )
 {
  RoundedRectanglePtr p = fromStack( L, 1 );
  bool fill = lua_toboolean( L, 2 );

  p->setFill(fill);
  return 0;
 }

 static int setRadius( lua_State* L )
 {
  RoundedRectanglePtr p = fromStack( L, 1 );
  int radius = lua_tointeger( L, 2 );

  p->setRadius(radius);
  return 0;
 }
};

#endif //__LB_ROUNDED_RECTANGLE_H
```

LuaRef.h
--------

C++ management of Lua data. A re-write of code originally written by me and then
donated to LuaBridge.
This version I have changed to use varidict template arguments and template pack
folding for calling Lua, This means at least C++17 is required.
The table element proxy is now public, and the code structure is
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
