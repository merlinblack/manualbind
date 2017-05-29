//------------------------------------------------------------------------------
/*
  https://github.com/vinniefalco/LuaBridge

  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
  Copyright 2007, Nathan Reed

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

#ifndef LUASTACK_H
#define LUASTACK_H

template<typename T>
struct LuaStack;

//------------------------------------------------------------------------------
/**
    Push an object onto the Lua stack.
*/
template <class T>
inline void lua_push (lua_State* L, T t)
{
  LuaStack <T>::push (L, t);
}

//------------------------------------------------------------------------------
/**
    Receive the lua_State* as an argument.
*/
template <>
struct LuaStack <lua_State*>
{
  static lua_State* get (lua_State* L, int)
  {
    return L;
  }
};

//------------------------------------------------------------------------------
/**
    Push a lua_CFunction.
*/
template <>
struct LuaStack <lua_CFunction>
{
  static void push (lua_State* L, lua_CFunction f)
  {
    lua_pushcfunction (L, f);
  }

  static lua_CFunction get (lua_State* L, int index)
  {
    return lua_tocfunction (L, index);
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `int`.
*/
template <>
struct LuaStack <int>
{
  static inline void push (lua_State* L, int value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static inline int get (lua_State* L, int index)
  {
    return static_cast <int> (luaL_checkinteger (L, index));
  }
};

template <>
struct LuaStack <int const&>
{
  static inline void push (lua_State* L, int value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static inline int get (lua_State* L, int index)
  {
    return static_cast <int > (luaL_checknumber (L, index));
  }
};
//------------------------------------------------------------------------------
/**
    LuaStack specialization for `unsigned int`.
*/
template <>
struct LuaStack <unsigned int>
{
  static inline void push (lua_State* L, unsigned int value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static inline unsigned int get (lua_State* L, int index)
  {
    return static_cast <unsigned int> (luaL_checkinteger (L, index));
  }
};

template <>
struct LuaStack <unsigned int const&>
{
  static inline void push (lua_State* L, unsigned int value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static inline unsigned int get (lua_State* L, int index)
  {
    return static_cast <unsigned int > (luaL_checknumber (L, index));
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `unsigned char`.
*/
template <>
struct LuaStack <unsigned char>
{
  static inline void push (lua_State* L, unsigned char value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static inline unsigned char get (lua_State* L, int index)
  {
    return static_cast <unsigned char> (luaL_checkinteger (L, index));
  }
};

template <>
struct LuaStack <unsigned char const&>
{
  static inline void push (lua_State* L, unsigned char value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static inline unsigned char get (lua_State* L, int index)
  {
    return static_cast <unsigned char> (luaL_checknumber (L, index));
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `short`.
*/
template <>
struct LuaStack <short>
{
  static inline void push (lua_State* L, short value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static inline short get (lua_State* L, int index)
  {
    return static_cast <short> (luaL_checkinteger (L, index));
  }
};

template <>
struct LuaStack <short const&>
{
  static inline void push (lua_State* L, short value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static inline short get (lua_State* L, int index)
  {
    return static_cast <short> (luaL_checknumber (L, index));
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `unsigned short`.
*/
template <>
struct LuaStack <unsigned short>
{
  static inline void push (lua_State* L, unsigned short value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static inline unsigned short get (lua_State* L, int index)
  {
    return static_cast <unsigned short> (luaL_checkinteger (L, index));
  }
};

template <>
struct LuaStack <unsigned short const&>
{
  static inline void push (lua_State* L, unsigned short value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static inline unsigned short get (lua_State* L, int index)
  {
    return static_cast <unsigned short> (luaL_checknumber (L, index));
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `long`.
*/
template <>
struct LuaStack <long>
{
  static inline void push (lua_State* L, long value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static inline long get (lua_State* L, int index)
  {
    return static_cast <long> (luaL_checkinteger (L, index));
  }
};

template <>
struct LuaStack <long const&>
{
  static inline void push (lua_State* L, long value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static inline long get (lua_State* L, int index)
  {
    return static_cast <long> (luaL_checknumber (L, index));
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `unsigned long`.
*/
template <>
struct LuaStack <unsigned long>
{
  static inline void push (lua_State* L, unsigned long value)
  {
    lua_pushinteger (L, static_cast <lua_Integer> (value));
  }
  
  static inline unsigned long get (lua_State* L, int index)
  {
    return static_cast <unsigned long> (luaL_checkinteger (L, index));
  }
};

template <>
struct LuaStack <unsigned long const&>
{
  static inline void push (lua_State* L, unsigned long value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static inline unsigned long get (lua_State* L, int index)
  {
    return static_cast <unsigned long> (luaL_checknumber (L, index));
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `float`.
*/
template <>
struct LuaStack <float>
{
  static inline void push (lua_State* L, float value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static inline float get (lua_State* L, int index)
  {
    return static_cast <float> (luaL_checknumber (L, index));
  }
};

template <>
struct LuaStack <float const&>
{
  static inline void push (lua_State* L, float value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static inline float get (lua_State* L, int index)
  {
    return static_cast <float> (luaL_checknumber (L, index));
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `double`.
*/
template <> struct LuaStack <double>
{
  static inline void push (lua_State* L, double value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static inline double get (lua_State* L, int index)
  {
    return static_cast <double> (luaL_checknumber (L, index));
  }
};

template <> struct LuaStack <double const&>
{
  static inline void push (lua_State* L, double value)
  {
    lua_pushnumber (L, static_cast <lua_Number> (value));
  }
  
  static inline double get (lua_State* L, int index)
  {
    return static_cast <double> (luaL_checknumber (L, index));
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `bool`.
*/
template <>
struct LuaStack <bool> {
  static inline void push (lua_State* L, bool value)
  {
    lua_pushboolean (L, value ? 1 : 0);
  }
  
  static inline bool get (lua_State* L, int index)
  {
    return lua_toboolean (L, index) ? true : false;
  }
};

template <>
struct LuaStack <bool const&> {
  static inline void push (lua_State* L, bool value)
  {
    lua_pushboolean (L, value ? 1 : 0);
  }
  
  static inline bool get (lua_State* L, int index)
  {
    return lua_toboolean (L, index) ? true : false;
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `char`.
*/
template <>
struct LuaStack <char>
{
  static inline void push (lua_State* L, char value)
  {
    char str [2] = { value, 0 };
    lua_pushstring (L, str);
  }
  
  static inline char get (lua_State* L, int index)
  {
    return luaL_checkstring (L, index) [0];
  }
};

template <>
struct LuaStack <char const&>
{
  static inline void push (lua_State* L, char value)
  {
    char str [2] = { value, 0 };
    lua_pushstring (L, str);
  }
  
  static inline char get (lua_State* L, int index)
  {
    return luaL_checkstring (L, index) [0];
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `float`.
*/
template <>
struct LuaStack <char const*>
{
  static inline void push (lua_State* L, char const* str)
  {
    if (str != 0)
      lua_pushstring (L, str);
    else
      lua_pushnil (L);
  }

  static inline char const* get (lua_State* L, int index)
  {
    return lua_isnil (L, index) ? 0 : luaL_checkstring (L, index);
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `std::string`.
*/
template <>
struct LuaStack <std::string>
{
  static inline void push (lua_State* L, std::string const& str)
  {
    lua_pushlstring (L, str.c_str (), str.size());
  }

  static inline std::string get (lua_State* L, int index)
  {
    size_t len;
    const char *str = luaL_checklstring(L, index, &len);
    return std::string (str, len);
  }
};

//------------------------------------------------------------------------------
/**
    LuaStack specialization for `std::string const&`.
*/
template <>
struct LuaStack <std::string const&>
{
  static inline void push (lua_State* L, std::string const& str)
  {
    lua_pushlstring (L, str.c_str(), str.size());
  }

  static inline std::string get (lua_State* L, int index)
  {
    size_t len;
    const char *str = luaL_checklstring(L, index, &len);
    return std::string (str, len);
  }
};

#endif // LUASTACK_H
