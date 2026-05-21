#include <iostream>
#include <memory>
#include <string>
#include "LuaRef.h"
#include "LuaStack.h"

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

void make_capture(lua_State* L)
{
  LuaRef G = LuaRef::globalTable(L);

  auto ptr = std::make_shared<Thingo>("Bob");

  // Capture will increase the refcount on the shared pointer.
  G["capture"] = (CPP_Function)[ptr](lua_State* L)->int
  {
    cout << "Thingo: " << ptr->name << "\n";
    lua_pushstring(L, ptr->name.c_str());
    return 1;
  };

  cout << "Make capture Scope ending\n";

  // `ptr` will go out of scope, and reduce the ref count by one on the
  // shared pointer for the Thingo instance, leaving it at 1.
}

int main()
{
  lua_State* L = luaL_newstate();

  make_capture(L);

  cout << "Calling capture\n";
  luaL_dostring(L, "capture()");

  // After the GC call, the Thingo's shared pointer refcount will become zero,
  // as the lambda is cleaned up and it will destruct the Thingo instance.

  cout << "Remove Lua held reference to the lambda and garbage collect\n";
  luaL_dostring(L, "capture = nil");
  lua_gc(L, LUA_GCCOLLECT);

  make_capture(L);
  cout << "Calling recreated capture\n";
  luaL_dostring(L, "capture()");

  // Lua close will cause the shared pointer's refcount to drop to zero, and
  // hence, destruct the Thingo.
  // If Thingo had anything that needed a live lua statue to shutdown, we would
  // have to shut them down prior to lua_close.
  // For example a LuaRef member could be set to LuaRef:nil().

  cout << "Closing Lua\n";
  lua_close(L);

  return 0;
}
