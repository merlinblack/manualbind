// Nigel's new and improved LuaRef
//
// 
// E X P E R I M E N T A L !!!!

#ifndef __LUAREF_H
#define __LUAREF_H

#include <string>
#include "LuaStack.h"

struct LuaNil;
template<>
struct LuaStack<LuaNil>
{
    static inline void push( lua_State* L, LuaNil& nil )
    {
        lua_pushnil(L);
    }
};

class LuaRefBase
{
    protected:
    lua_State* m_L;
    int m_ref;

    class StackPopper 
    {
        lua_State* m_L;
        int m_count;
        public:
        StackPopper( lua_State* L, int count = 1 ) : m_L(L), m_count(count)
        {}
        ~StackPopper()
        {
            lua_pop( m_L, m_count );
        }
    };

    struct FromStack {};

    LuaRefBase( lua_State* L, FromStack ) : m_L( L )
    {
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
    }

    LuaRefBase( lua_State* L, int ref ) : m_L( L ), m_ref( ref )
    {
    }

    ~LuaRefBase()
    {
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_ref );
    }

    public:
    inline void push() const
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
    }
        
    std::string tostring() const
    {
        lua_getglobal( m_L, "tostring" );
        push();
        lua_call( m_L, 1, 1 );
        const char* str = lua_tostring( m_L, 1 );
        lua_pop( m_L, 1 );
        return std::string(str);
    }
};

class LuaRef;

class LuaTableElement : public LuaRefBase
{
    friend class LuaRef;
    private:
    int m_tableRef;

    // Expects on the Lua stack
    // 1 - The table
    // 2 - The key
    LuaTableElement( lua_State* L ) 
        : LuaRefBase( L, FromStack() )
    {
        m_tableRef = luaL_ref( m_L, LUA_REGISTRYINDEX );
    }

    public:
    ~LuaTableElement()
    {
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_tableRef );
    }

    inline void push() const
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_tableRef );
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        lua_gettable( m_L, -2 );
        lua_remove( m_L, -2 );
    }

    // Assign a new value to this table/key.
    template<class T>
    LuaTableElement& operator= ( T v )
    {
        StackPopper p( m_L );
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_tableRef );
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        LuaStack<T>::push( m_L, v );
        lua_settable( m_L, -3 );
        return *this;
    }

    LuaTableElement operator[]( const char* key ) const
    {
        push();
        lua_pushstring( m_L, key );
        return LuaTableElement( m_L );
    }
};

template<>
struct LuaStack<LuaTableElement>
{
    static inline void push( lua_State* L, LuaTableElement const& e )
    {
        e.push();
    }
};

class LuaRef : public LuaRefBase
{
    private:
    LuaRef( lua_State* L, FromStack fs ) : LuaRefBase( L, fs )
    {}

    public:
    LuaRef( lua_State* L ) : LuaRefBase( L, LUA_REFNIL )
    {
    }

    inline void push() const
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
    }

    LuaTableElement operator[]( const char* key ) const
    {
        push();
        lua_pushstring( m_L, key );
        return LuaTableElement( m_L );
    }

    static LuaRef fromStack( lua_State* L, int index = -1 )
    {
        lua_pushvalue( L, index );
        return LuaRef( L, FromStack() );
    }

    static LuaRef newTable (lua_State* L)
    {
        lua_newtable (L);
        return LuaRef (L, FromStack ());
    }

    static LuaRef getGlobal (lua_State *L, char const* name)
    {
        lua_getglobal (L, name);
        return LuaRef (L, FromStack ());
    }
};

template<>
struct LuaStack<LuaRef>
{
    static inline void push( lua_State* L, LuaRef const& r )
    {
        r.push();
    }
};


#endif // __LUAREF_H
