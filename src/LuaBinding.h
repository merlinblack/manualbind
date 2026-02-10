//------------------------------------------------------------------------------
/*

   Copyright 2017, Nigel Atkinson

License: The MIT License (http://www.opensource.org/licenses/mit-license.php)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
//==============================================================================

/* Special members.
 * ----------------
 *  In addition to assigning a function to the likes of __tostring and other
 * built in Lua metamethods, three additional members can be used.
 *
 *  __arrayindex    This is called to provide a result when an object is indexed
 *                  with a number. E.g. a = obj[5]
 *
 *  __arraynewindex This is called to store or otherwise use a value when
 * assigned to an object via a numerical index. E.g. obj[7] = a
 *
 *  __upcast        This will be called by LuaBindingUpCast to return a shared
 *                  pointer to a parent class, allowing for polymorphic use.
 *                  See upcast in the examples.
 *
 */

/* I didn't realise at the time I initailly wrote this, that the binding classes
 * implement the "Curiously Recurring Template Pattern", or CRTP.
 * https://en.cppreference.com/w/cpp/language/crtp.html
 */

#ifndef BINDING_H
#define BINDING_H
#include <lua.hpp>
#include <memory>
#include "LuaException.h"

namespace ManualBind {

// helper struct
struct bind_properties {
  const char* name;
  lua_CFunction getter;
  lua_CFunction setter;
};

// concepts for detecting user binding functions
template <typename Binding>
concept HasMembersFunc = requires() {
  { Binding::members() } -> std::same_as<luaL_Reg*>;
};

template <typename Binding>
concept HasPropertiesFunc = requires() {
  { Binding::properties() } -> std::same_as<bind_properties*>;
};

template <typename Binding>
concept HasExtraMetaFunc = requires(lua_State* L) {
  { Binding::setExtraMeta(L) } -> std::same_as<void>;
};

template <typename Binding>
concept HasCreateFunc = requires(lua_State* L) {
  { Binding::create(L) } -> std::same_as<int>;
};

// Called when Lua object is indexed: obj[ndx]
int LuaBindingIndex(lua_State* L);

// Called whe Lua object index is assigned: obj[ndx] = blah
int LuaBindingNewIndex(lua_State* L);

// Gets the table of extra values assigned to an instance.
int LuaBindingGetExtraValuesTable(lua_State* L, int index);

// Creates a lua table of functions to get and/or set properties.
void LuaBindingSetProperties(lua_State* L, bind_properties* properties);

// If the object at 'index' is a userdata with a metatable containing a
// __upcast function, then replaces the userdata at 'index' in the stack with
// the result of calling __upcast. Otherwise the object at index is replaced
// with nil.
int LuaBindingUpCast(lua_State* L, int index);

// Check the number of arguments are as expected.
// Throw an error if not.
void CheckArgCount(lua_State* L, int expected);

// B - the binding class / struct
// T - the class you are binding to Lua.

// Shared pointer version
// ----------------------
// Use this for classes that need to be shared between C++ and Lua,
// or are expensive to copy. Think of it as like "by Reference".
template <class B, class T>
struct Binding {
  // Push the object on to the Lua stack
  static void push(lua_State* L, const std::shared_ptr<T>& sp)
  {
    if (sp == nullptr) {
      lua_pushnil(L);
      return;
    }

    void* ud = lua_newuserdata(L, sizeof(std::shared_ptr<T>));

    new (ud) std::shared_ptr<T>(sp);

    luaL_setmetatable(L, B::class_name);
  }

  static void setMembers(lua_State* L)
  {
    if constexpr (HasMembersFunc<B>) {
      luaL_setfuncs(L, B::members(), 0);
    }
  }

  static void setProperties(lua_State* L)
  {
    if constexpr (HasPropertiesFunc<B>) {
      bind_properties* props = B::properties();
      LuaBindingSetProperties(L, props);
    }
  }

  static void setExtras(lua_State* L)
  {
    if constexpr (HasExtraMetaFunc<B>) {
      B::setExtraMeta(L);
    }
  }

  static int construct(lua_State* L)
  {
    if constexpr (HasCreateFunc<B>) {
      // Remove table from stack.
      lua_remove(L, 1);

      return B::create(L);
    }
    else {
      return luaL_error(L, "Can not create an instance of %s.", B::class_name);
    }
  }

  static int pairs(lua_State* L)
  {
    lua_getglobal(L, "pairs");
    luaL_getmetatable(L, B::class_name);
    lua_pcall(L, 1, LUA_MULTRET, 0);
    return 3;
  }

  // Create metatable and register Lua constructor
  static void register_class(lua_State* L)
  {
    lua_newtable(L);  // Class access
    lua_newtable(L);  // Class access metatable

    luaL_newmetatable(L, B::class_name);
    setMembers(L);
    lua_pushcfunction(L, LuaBindingIndex);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, LuaBindingNewIndex);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, destroy);
    lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, close);
    lua_setfield(L, -2, "__close");
    lua_newtable(L);  // __properties
    setProperties(L);
    lua_setfield(L, -2, "__properties");
    setExtras(L);
    lua_pushcfunction(L, pairs);
    lua_setfield(L, -2, "__pairs");

    lua_setfield(L, -2, "__index");  // Set metatable as index table.

    lua_pushcfunction(L, construct);
    lua_setfield(L, -2, "__call");

    lua_pushcfunction(L, pairs);
    lua_setfield(L, -2, "__pairs");

    lua_setmetatable(L, -2);
    lua_setglobal(L, B::class_name);
  }

  // Called when Lua object is garbage collected.
  static int destroy(lua_State* L)
  {
    void* ud = luaL_checkudata(L, 1, B::class_name);

    auto sp = static_cast<std::shared_ptr<T>*>(ud);

    // Explicitly called, as this was 'placement new'd
    sp->~shared_ptr();

    return 0;
  }

  // Called when Lua object goes out of scope with the <close> annotation
  static int close(lua_State* L)
  {
    void* ud = luaL_checkudata(L, 1, B::class_name);

    auto sp = static_cast<std::shared_ptr<T>*>(ud);

    sp->reset();

    return 0;
  }

  // Grab object shared pointer from the Lua stack
  static const std::shared_ptr<T>& fromStack(lua_State* L, int index)
  {
    void* ud = luaL_checkudata(L, index, B::class_name);

    auto sp = static_cast<std::shared_ptr<T>*>(ud);

    return *sp;
  }

  static const std::shared_ptr<T>& fromStackThrow(lua_State* L, int index)
  {
    void* ud = luaL_testudata(L, index, B::class_name);

    if (ud == nullptr)
      throw LuaException("Unexpected item on Lua stack.");

    auto sp = static_cast<std::shared_ptr<T>*>(ud);

    return *sp;
  }

  static bool isType(lua_State* L, int index)
  {
    return luaL_testudata(L, index, B::class_name) != nullptr;
  }
};

// Plain Old Data POD version.
// ---------------------------
// Use this for simpler classes/structures where coping is fairly cheap, and
// C++ and Lua *do not* need to operate on the *same instance*.
// I.e. push and fromStack create copies.
// Think of this as "by Value"
template <class B, class T>
struct PODBinding {
  // Push the object on to the Lua stack
  static void push(lua_State* L, const T& value)
  {
    void* ud = lua_newuserdata(L, sizeof(T));

    new (ud) T(value);

    luaL_setmetatable(L, B::class_name);
  }

  static void setMembers(lua_State* L)
  {
    if constexpr (HasMembersFunc<B>) {
      luaL_setfuncs(L, B::members(), 0);
    }
  }

  static void setProperties(lua_State* L)
  {
    if constexpr (HasPropertiesFunc<B>) {
      bind_properties* props = B::properties();
      LuaBindingSetProperties(L, props);
    }
  }

  static void setExtras(lua_State* L)
  {
    if constexpr (HasExtraMetaFunc<B>) {
      B::setExtraMeta(L);
    }
  }

  static int construct(lua_State* L)
  {
    if constexpr (HasCreateFunc<B>) {
      // Remove table from stack.
      lua_remove(L, 1);

      return B::create(L);
    }
    else {
      return luaL_error(L, "Can not create an instance of %s.", B::class_name);
    }
  }

  static int pairs(lua_State* L)
  {
    lua_getglobal(L, "pairs");
    luaL_getmetatable(L, B::class_name);
    lua_pcall(L, 1, LUA_MULTRET, 0);
    return 3;
  }

  // Create metatable and register Lua constructor
  static void register_class(lua_State* L)
  {
    lua_newtable(L);  // Class access
    lua_newtable(L);  // Class access metatable

    luaL_newmetatable(L, B::class_name);
    setMembers(L);
    lua_pushcfunction(L, LuaBindingIndex);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, LuaBindingNewIndex);
    lua_setfield(L, -2, "__newindex");
    // lua_pushcfunction( L, destroy );  -- if you need to destruct a POD
    // lua_setfield( L, -2, "__gc" );    -- set __gc to destory in the members.
    lua_newtable(L);  // __properties
    setProperties(L);
    lua_setfield(L, -2, "__properties");
    setExtras(L);

    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, construct);
    lua_setfield(L, -2, "__call");

    lua_pushcfunction(L, pairs);
    lua_setfield(L, -2, "__pairs");

    lua_setmetatable(L, -2);
    lua_setglobal(L, B::class_name);
  }

  // This is still here in case a POD really need
  // destructing. Shouldn't be a common case.
  static int destroy(lua_State* L)
  {
    void* ud = luaL_checkudata(L, 1, B::class_name);

    auto p = static_cast<T*>(ud);

    // Explicitly called, as this was 'placement new'd
    p->~T();

    return 0;
  }

  // Grab object pointer from the Lua stack
  static T& fromStack(lua_State* L, int index)
  {
    void* ud = luaL_checkudata(L, index, B::class_name);

    auto p = static_cast<T*>(ud);

    return *p;
  }

  static T& fromStackThrow(lua_State* L, int index)
  {
    void* ud = luaL_testudata(L, index, B::class_name);

    if (ud == nullptr)
      throw LuaException("Unexpected item on Lua stack.");

    auto p = static_cast<T*>(ud);

    return *p;
  }

  static bool isType(lua_State* L, int index)
  {
    return luaL_testudata(L, index, B::class_name) != nullptr;
  }
};
};  // namespace ManualBind

#endif  // BINDING_H
