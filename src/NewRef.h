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

template<typename K>
class LuaTableElement : public LuaRefBase
{
    friend class LuaRef;
    private:
    K m_key;

    // Expects on the Lua stack
    // 1 - The table
    LuaTableElement( lua_State* L, K key ) 
        : LuaRefBase( L, FromStack() )
        , m_key( key )
    {
    }

    public:
    ~LuaTableElement()
    {
    }

    inline void push() const
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        LuaStack<K>::push( m_L, m_key );
        lua_gettable( m_L, -2 );
        lua_remove( m_L, -2 );
    }

    // Assign a new value to this table/key.
    template<typename T>
    LuaTableElement& operator= ( T v )
    {
        StackPopper p( m_L );
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        LuaStack<K>::push( m_L, m_key );
        LuaStack<T>::push( m_L, v );
        lua_settable( m_L, -3 );
        return *this;
    }

    LuaTableElement<K> operator[]( const char* key ) const
    {
        push();
        return LuaTableElement<K>( m_L, key );
    }
};

template<typename K>
struct LuaStack<LuaTableElement<K> >
{
    static inline void push( lua_State* L, LuaTableElement<K> const& e )
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

    template<typename K>
    LuaTableElement<K> operator[]( K key ) const
    {
        push();
        return LuaTableElement<K>( m_L, key );
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
