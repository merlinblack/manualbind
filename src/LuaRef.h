// Nigel's new and improved LuaRef
//
// 
// E X P E R I M E N T A L !!!!

#ifndef __LUAREF_H
#define __LUAREF_H

#include <string>
#include "LuaStack.h"
#include "LuaException.h"

#include <iostream>
using std::cout;
using std::endl;

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

    LuaRefBase( lua_State* L, FromStack ) : m_L( L )
    {
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
        cout << "New ref: " << m_ref << endl;
    }

    LuaRefBase( lua_State* L, int ref ) : m_L( L ), m_ref( ref )
    {
    }

    virtual ~LuaRefBase()
    {
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_ref );
        cout << "Zap ref: " << m_ref << endl;
    }

    public:
    virtual void push() const
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        cout << "Get ref: " << m_ref << endl;
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

    protected:
    template<typename T> 
    inline void varpush( T first ) const
    {
        LuaStack<T>::push( m_L, first );
    }

    template<typename T, typename... Args>
    inline void varpush( T first, Args... args ) const
    {
        LuaStack<T>::push( m_L, first );
        varpush( args... );
    }

    public:

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
        cout << "Created Table Element with key '" << key << "'" << endl;
    }

    ~LuaTableElement()
    {
        cout << "Destroying Table Element" << endl;
    }

    void push() const
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        cout << "Get ref: " << m_ref << endl;
        LuaStack<K>::push( m_L, m_key );
        cout << "Get key: " << m_key << endl;
        dump( m_L );
        lua_gettable( m_L, -2 );
        lua_remove( m_L, -2 );
        dump( m_L );
    }

    // Assign a new value to this table/key.
    template<typename T>
    LuaTableElement& operator= ( T v )
    {
        StackPopper p( m_L );
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        LuaStack<K>::push( m_L, m_key );
        LuaStack<T>::push( m_L, v );
        dump( m_L );
        lua_settable( m_L, -3 );
        cout << "settable" << endl;
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
        cout << "New ref: " << m_ref << " as copy of " << other.m_ref << endl;
    }

    LuaRef( LuaRef&& other ) : LuaRefBase( other.m_L, other.m_ref )
    {
        other.m_ref = LUA_REFNIL;
        cout << "New ref: " << m_ref << " moved." << endl;
    }

    LuaRef& operator=( LuaRef&& other )
    {
        if( this == &other ) return *this;
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_ref );
        cout << "Zap ref: " << m_ref << " before reassigning." << endl;

        m_L = other.m_L;
        m_ref = other.m_ref;

        other.m_ref = LUA_REFNIL;
        cout << "New ref: " << m_ref << " moved by assignment." << endl;

        return *this;
    }

    LuaRef& operator=( LuaRef const& other )
    {
        if( this == &other ) return *this;
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_ref );
        cout << "Zap ref: " << m_ref << " before reassigning." << endl;
        other.push();
        m_L = other.m_L;
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
        cout << "New ref: " << m_ref << " as assigned copy of " << other.m_ref << endl;
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
    varpush( args... );
    LuaException::pcall( m_L, n, 1 );
    return LuaRef (m_L, FromStack ());
}

#endif // __LUAREF_H
