// Nigel's new and improved LuaRef
//
// 
// E X P E R I M E N T A L !!!!

#ifndef __LUAREF_H
#define __LUAREF_H

#include <string>
#include "LuaStack.h"
#include "LuaException.h"

#ifdef _DEBUGOUTPUT
#include <iostream>
#define COUT( str, x ) std::cout << str << x << std::endl
#define DUMP( L ) dump( L )
#else
#define COUT( str, x )
#define DUMP( L  )
#endif

struct LuaNil
{
};
template<>
struct LuaStack<LuaNil>
{
    static inline void push( lua_State* L, LuaNil const& nil )
    {
        lua_pushnil(L);
    }
};

class LuaRef;

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

    // These constructors as destructor are protected as this
    // class should not be used directly.

    LuaRefBase( lua_State* L, FromStack ) : m_L( L )
    {
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
        COUT( "New ref: ", m_ref );
    }

    LuaRefBase( lua_State* L, int ref ) : m_L( L ), m_ref( ref )
    {
    }

    ~LuaRefBase()
    {
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_ref );
        COUT( "Zap ref: ", m_ref );
    }

    public:
    virtual void push() const
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        COUT( "Get ref: ", m_ref );
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

    int type () const
    {
        int result;
        push();
        result = lua_type (m_L, -1);
        lua_pop (m_L, 1);
        return result;
    }

    inline bool isNil () const { return type () == LUA_TNIL; }
    inline bool isNumber () const { return type () == LUA_TNUMBER; }
    inline bool isString () const { return type () == LUA_TSTRING; }
    inline bool isTable () const { return type () == LUA_TTABLE; }
    inline bool isFunction () const { return type () == LUA_TFUNCTION; }
    inline bool isUserdata () const { return type () == LUA_TUSERDATA; }
    inline bool isThread () const { return type () == LUA_TTHREAD; }
    inline bool isLightUserdata () const { return type () == LUA_TLIGHTUSERDATA; }

    template<typename... Args>
    LuaRef const operator()( Args... args ) const;

    template<typename T>
    void append( T v ) const
    {
        push();
        LuaStack<T>::push( m_L, v );
        luaL_ref( m_L, -2 );
        lua_pop( m_L, 1 );
    }
};

template<typename K>
class LuaTableElement : public LuaRefBase
{
    friend class LuaRef;
    private:
    K m_key;

    // This constructor has to be public, so that the operator[]
    // with a differing template type can call it.
    // I could not find a way to 'friend' it.
    public:
    // Expects on the Lua stack
    // 1 - The table
    LuaTableElement( lua_State* L, K key ) 
        : LuaRefBase( L, FromStack() )
        , m_key( key )
    {
        COUT( "Created Table Element with key: ", key );
        COUT( "with table ref:", m_ref );
    }

    ~LuaTableElement()
    {
        COUT( "Destroying Table Element", "" );
    }

    void push() const override
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        COUT( "Get ref: ", m_ref );
        LuaStack<K>::push( m_L, m_key );
        COUT( "Get key: ", m_key );
        DUMP( m_L );
        lua_gettable( m_L, -2 );
        lua_remove( m_L, -2 );
        DUMP( m_L );
    }

    // Assign a new value to this table/key.
    template<typename T>
    LuaTableElement& operator= ( T v )
    {
        StackPopper p( m_L );
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        LuaStack<K>::push( m_L, m_key );
        LuaStack<T>::push( m_L, v );
        DUMP( m_L );
        lua_settable( m_L, -3 );
        COUT( "settable", "" );
        return *this;
    }

    template<typename NK>
    LuaTableElement<NK> operator[]( NK key ) const
    {
        push();
        return LuaTableElement<NK>( m_L, key );
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
    friend LuaRefBase;
    private:
    LuaRef( lua_State* L, FromStack fs ) : LuaRefBase( L, fs )
    {}

    public:
    LuaRef( lua_State* L ) : LuaRefBase( L, LUA_REFNIL )
    {
    }

    LuaRef( LuaRef const& other ) : LuaRefBase( other.m_L, LUA_REFNIL )
    {
        other.push();
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
        COUT( "New ref: ", m_ref );
        COUT( " as copy of ", other.m_ref );
    }

    LuaRef( LuaRef&& other ) noexcept : LuaRefBase( other.m_L, other.m_ref )
    {
        other.m_ref = LUA_REFNIL;
        COUT( "Moved new ref: ", m_ref );
    }

    LuaRef& operator=( LuaRef&& other ) noexcept
    {
        if( this == &other ) return *this;

        COUT( "Move assignement, swap ref: ", m_ref );

        std::swap( m_L, other.m_L);
        std::swap( m_ref, other.m_ref);

        COUT( "With ref: ", m_ref );

        return *this;
    }

    LuaRef& operator=( LuaRef const& other )
    {
        if( this == &other ) return *this;
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_ref );
        COUT( "Before reassigning, zap ref: ", m_ref );
        other.push();
        m_L = other.m_L;
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
        COUT( "New ref: ", m_ref );
        COUT( "as assigned copy of ", other.m_ref );
        return *this;
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

template<>
LuaRef const LuaRefBase::operator() () const
{
    push();
    LuaException::pcall (m_L, 0, 1);
    return LuaRef (m_L, FromStack ());
}

template<typename... Args>
LuaRef const LuaRefBase::operator()( Args... args ) const
{
    const int n = sizeof...(Args);
    push();
    // Initializer expansion trick to call push for each arg.
    // https://stackoverflow.com/questions/25680461/variadic-template-pack-expansion
    int dummy[] = { 0, ( (void) LuaStack<Args>::push( m_L, std::forward<Args>(args) ), 0 ) ... };
    LuaException::pcall( m_L, n, 1 );
    return LuaRef (m_L, FromStack ());
}

#endif // __LUAREF_H
