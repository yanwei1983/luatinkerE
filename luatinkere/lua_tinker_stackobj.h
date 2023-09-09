#ifndef __LUA_STACKOBJ_H
#define __LUA_STACKOBJ_H

#include "lua.hpp"
namespace lua_tinker
{
    namespace detail
    {

        struct stack_scope_exit
        {
            lua_State* L;
            int32_t    nTop;
            explicit stack_scope_exit(lua_State* _L)
                : L(_L)
            {
                nTop = lua_gettop(L);
            }
            stack_scope_exit(lua_State* _L, int32_t t)
                : L(_L), nTop(t)
            {
            }
            ~stack_scope_exit() 
            { 
                lua_settop(L, nTop); 
            }

        };
        struct stack_delay_pop
        {
            lua_State* L;
            int32_t    nPop;
            stack_delay_pop(lua_State* _L, int32_t _nPop)
                : L(_L)
                , nPop(_nPop)
            {
            }
            ~stack_delay_pop() { lua_pop(L, nPop); }
        };
        struct stack_obj
        {
            lua_State* L;
            int32_t    _stack_pos = 0;
            explicit stack_obj(lua_State* _L)
                : L(_L)
            {
            }
            stack_obj(lua_State* _L, int32_t nIdx)
                : L(_L)
                , _stack_pos(lua_absindex(L, nIdx))
            {
            }
            ~stack_obj() {}

            static stack_obj get_top(lua_State* L) { return stack_obj(L, lua_gettop(L)); }
            static stack_obj new_table(lua_State* L, int32_t narr = 0, int32_t nrec = 0)
            {
                lua_createtable(L, narr, nrec);
                return stack_obj(L, lua_gettop(L));
            }

            void reset(int32_t nIdx = 0) { _stack_pos = nIdx; }

            bool    is_vaild() const { return _stack_pos != 0; }
            int32_t get_type() const { return is_vaild() && lua_type(L, _stack_pos); }
            bool    is_number() const { return get_type() == LUA_TNUMBER; }
            bool    is_string() const { return is_vaild() && lua_isstring(L, _stack_pos) == 1; }
            bool    is_integer() const { return is_vaild() && lua_isinteger(L, _stack_pos) == 1; }
            bool    is_boolean() const { return is_vaild() && lua_isboolean(L, _stack_pos); }
            bool    is_none() const { return is_vaild() && lua_isnone(L, _stack_pos); }
            bool    is_userdata() const { return is_vaild() && lua_isuserdata(L, _stack_pos) == 1; }
            bool    is_function() const { return is_vaild() && lua_isfunction(L, _stack_pos); }
            bool    is_cfunction() const { return is_vaild() && lua_iscfunction(L, _stack_pos) == 1; }
            bool    is_table() const { return is_vaild() && lua_istable(L, _stack_pos); }
            bool    is_nil() const { return is_vaild() && lua_isnil(L, _stack_pos); }

            const char*   to_string() const { return lua_tostring(L, _stack_pos); }
            lua_Integer   to_integer() const { return lua_tointeger(L, _stack_pos); }
            lua_Number    to_number() const { return lua_tonumber(L, _stack_pos); }
            bool          to_boolean() const { return lua_toboolean(L, _stack_pos); }
            void*         to_userdata() const { return lua_touserdata(L, _stack_pos); }
            lua_CFunction to_cfunction() const { return lua_tocfunction(L, _stack_pos); }
            
            

            void remove(bool force = false)
            {
                if(is_vaild())
                {
                    if(is_top() || force)
                    {
                        lua_remove(L, _stack_pos);
                    }
                    else
                    {
                        lua_pushfstring(L, "stack_obj::remove() error: %d not top stack", _stack_pos);
                        lua_error(L);
                        
                        return;
                    }
                }
                reset(0);
            }

            void remove_up()
            {
                if(!is_vaild())
                    return;
                lua_pop(L, lua_gettop(L) - (_stack_pos -1));
                reset(0);
            }

            void insert_to(int32_t nIdx)
            {
                if(is_top())
                {
                    lua_rotate(L, lua_absindex(L, nIdx), 1);
                    _stack_pos = lua_absindex(L, nIdx);
                }
            }

            void pop_up(int32_t nIdx)
            {
                if(is_top())
                {
                    lua_rotate(L, nIdx, 1);
                    lua_pop(L, lua_gettop(L) - nIdx);
                    _stack_pos = lua_absindex(L, -1);
                }
            }

            void pop_up()
            {
                if(!is_vaild())
                    return;
                if(is_top())
                    return;
                lua_pop(L, lua_gettop(L) - _stack_pos);
            }

            void push_top() const
            {
                if(is_top())
                    return;
                if(is_vaild())
                    lua_pushvalue(L, _stack_pos);
            }

            bool is_top() const { return lua_gettop(L) == _stack_pos; }

            stack_obj get_lenobj() const
            {
                if(is_vaild())
                {
                    lua_len(L, _stack_pos);
                    return get_top(L);
                }
                else
                    return stack_obj(L);
            }

            lua_Integer get_len() const
            {
                if(is_vaild())
                {
                    lua_len(L, _stack_pos);
                    stack_delay_pop _delay(L, 1);
                    return lua_tointeger(L, -1);
                }
                return -1;
            }

            size_t get_rawlen() const
            {
                if(is_vaild())
                {
                    return lua_rawlen(L, _stack_pos);
                }
                return -1;
            }

            stack_obj rawget(const stack_obj& key) const
            {
                if(is_vaild())
                {
                    key.push_top();
                    lua_rawget(L, _stack_pos);
                    return get_top(L);
                }
                return stack_obj(L);
            }

            stack_obj rawget(const char* key) const
            {
                if(is_vaild())
                {
                    lua_pushstring(L, key);
                    lua_rawget(L, _stack_pos);
                    return get_top(L);
                }
                return stack_obj(L);
            }

            stack_obj rawgeti(int32_t n) const
            {
                if(is_vaild())
                {
                    lua_rawgeti(L, _stack_pos, n);
                    return get_top(L);
                }
                return stack_obj(L);
            }

            void rawset()
            {
                if(is_vaild())
                {
                    lua_rawset(L, _stack_pos);
                }
            }

            void rawseti(int32_t n)
            {
                if(is_vaild())
                {
                    lua_rawseti(L, _stack_pos, n);
                }
            }

            stack_obj get_metatable()
            {
                if(is_vaild())
                {
                    if(lua_getmetatable(L, _stack_pos))
                        return get_top(L);
                }
                return stack_obj(L);
            }

            void set_to_global(const char* name)
            {
                lua_setglobal(L, name);
                reset(0);
            }
        };

        class table_iterator
        {
        public:
            table_iterator(const stack_obj& table)
                : m_table(table)
                , m_hasNext(false)
            {
                lua_pushnil(m_table.L);
                m_key = lua_gettop(m_table.L);
                do_next();
            }

            ~table_iterator()
            {
            }

            bool hasNext() const { return m_hasNext; }

            void moveNext()
            {
                int32_t nTop = lua_gettop(m_table.L);
                if(nTop == m_key + 1)
                {
                    lua_pop(m_table.L, 1); // pop value
                    do_next();
                }
                else
                {
                    key().remove_up();
                    m_hasNext = false;
                }
            }

            stack_obj key() const { return stack_obj(m_table.L, m_key); }
            int32_t   key_idx() const { return m_key; }

            stack_obj value() const { return stack_obj(m_table.L, m_key + 1); }
            int32_t   value_idx() const { return m_key + 1; }

            void destory()
            {
                key().remove();
                m_table._stack_pos = 0;
                m_key              = 0;
                m_hasNext          = false;
            }

        private:
            void      do_next()
            { 
                m_hasNext = lua_next(m_table.L, m_table._stack_pos) == 1;
            }
            stack_obj m_table;
            int32_t   m_key;
            bool      m_hasNext;
        };

    } // namespace detail

}; // namespace lua_tinker

#endif