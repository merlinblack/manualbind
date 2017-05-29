// Nigel's new and improved LuaRef
//
// 
// E X P E R I M E N T A L !!!!

#ifndef __LUAREF_H
#define __LUAREF_H

#include <string>

struct Nil;

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

    LuaTableElement( lua_State* L, int tableRef ) 
        : LuaRefBase( L, LUA_REFNIL )
        , m_tableRef( tableRef )
    {
        m_ref = luaL_ref( L, LUA_REGISTRYINDEX );
    }

    public:
    ~LuaTableElement()
    {
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_ref );
    }

    inline void push() const
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_tableRef );
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        lua_gettable( m_L, -2 );
    }

    // Assign a new value to this table/key.
    LuaTableElement& operator= ( const char* str )
    {
        StackPopper p( m_L );
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_tableRef );
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        lua_pushstring( m_L, str );
        lua_settable( m_L, -3 );
        return *this;
    }

    LuaTableElement operator[]( const char* key ) const
    {
        push();
        int newref = luaL_ref( m_L, LUA_REGISTRYINDEX );
        lua_pushstring( m_L, key );
        return LuaTableElement( m_L, newref );
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

    LuaTableElement operator[]( const char* key ) const
    {
        lua_pushstring( m_L, key );
        return LuaTableElement( m_L, m_ref );
    }

    LuaTableElement operator[]( const int key ) const
    {
        lua_pushinteger( m_L, key );
        return LuaTableElement( m_L, m_ref );
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

#endif // __LUAREF_H
