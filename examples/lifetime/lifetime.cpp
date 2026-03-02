#include <iostream>
#include <memory>
#include <string>
#include "LuaRef.h"
#include "LuaStack.h"
#include "lua.h"

// Lifetimes.
// The care and feeding of LuaRefs, and lambdas that capture heap objects.
// -----------------------------------------------------------------------
//
// Care must be taken to ensure all LuaRefs are destructed before calling
// lua_close as they will try to call luaL_unref on destruction.
//
// In addition any C++ lambda that captures a heap object, either needs
// to have a lifetime that does not outlive the heap object, or it needs
// to keep that object alive. A shared_ptr is an easy way to keep the
// object around as long as the lambda exists.

using ManualBind::CPP_Function;
using ManualBind::LuaRef;
using std::cout;

struct Thingo {
  std::string name;

  Thingo(std::string _name) : name(_name) { cout << "Created Thingo\n"; }
  ~Thingo() { cout << "Destructed Thingo\n"; }
};

int main()
{
  lua_State* L = luaL_newstate();

  // This scope ensures "G" is destructed before calling lua_close.
  {
    LuaRef G = LuaRef::globalTable(L);

    {
      auto ptr = std::make_shared<Thingo>("Bob");

      // Capture will increase the Thingo instance's refcount.
      G["capture"] = (CPP_Function)[ptr](lua_State * L)->int
      {
        cout << "Thingo: " << ptr->name << "\n";
        lua_pushstring(L, ptr->name.c_str());
        return 1;
      };

      cout << "Inner Scope ending\n";

      // `ptr` will go out of scope, and reduce the ref count by one on the
      // Thingo instance.
    }

    cout << "Calling capture\n";
    luaL_dostring(L, "capture()");

    cout << "Outer Scope ending\n";
  }

  cout << "Calling capture again\n";
  luaL_dostring(L, "capture()");

  // After the GC call, the Thingo's refcount will become zero, as the lambda
  // is cleaned up and it will destruct.

  cout << "Remove reference to lambda and garbage collect\n";
  luaL_dostring(L, "capture = nil");
  lua_gc(L, LUA_GCCOLLECT);

  // If we hadn't cleaned up the lambda before, it would be cleaned up now as
  // Lua closes and cause the Thingo instance's ref count to drop to zero, and
  // hence, destruct.

  cout << "Closing Lua\n";
  lua_close(L);

  return 0;
}
