// lua_tinker.cpp
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright (c) 2005-2007 Kwon-il Lee (zupet@hitel.net)
//
// please check Licence.txt file for licence and legal issues.

#include "lua_tinker.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#if defined(_MSC_VER)
#define I64_FMT "I64"
#elif defined(__APPLE__)
#define I64_FMT "q"
#else
#define I64_FMT "ll"
#endif

namespace lua_tinker
{
    void set_error_callback(lua_State* L, error_call_back_fn fn)
    {
        lua_pushcclosure(L, fn, 0);
        lua_setglobal(L, ERROR_CALLBACK_NAME);
    }

} // namespace lua_tinker

/*---------------------------------------------------------------------------*/
/* init                                                                      */
/*---------------------------------------------------------------------------*/

struct lua_ext_value
{
    lua_State*                                               m_L;
    typedef std::vector<lua_tinker::Lua_Close_CallBack_Func> CLOSE_CALLBACK_VEC;
    CLOSE_CALLBACK_VEC                                       m_vecCloseCallBack;
#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
    lua_tinker::detail::InheritMap m_inherit_map;
#endif
    lua_ext_value(lua_State* L)
        : m_L(L)
    {
    }
    ~lua_ext_value()
    {
        for(const auto& func: m_vecCloseCallBack)
        {
            func(m_L);
        }
    }
};
constexpr const char* s_lua_ext_value_name = "___lua_ext_value";

void lua_tinker::register_lua_close_callback(lua_State* L, Lua_Close_CallBack_Func&& callback_func)
{
    detail::stack_scope_exit scope_exit(L);
    if(lua_getglobal(L, s_lua_ext_value_name) != LUA_TUSERDATA)
    {
        lua_pop(L, 1); // pop getglobal
        print_error(L, "can't find lua_ext_value");
        return;
    }

    lua_ext_value* p_lua_ext_val = detail::user2type<lua_ext_value*>(L, -1);
    p_lua_ext_val->m_vecCloseCallBack.emplace_back(callback_func);
}

static void init_close_callback(lua_State* L)
{

    new(lua_newuserdata(L, sizeof(lua_ext_value))) lua_ext_value(L);
    // register functor
    {
        lua_createtable(L, 0, 1);
        lua_pushstring(L, "__gc");
        lua_pushcclosure(L, &lua_tinker::detail::destroyer<lua_ext_value>, 0);
        lua_rawset(L, -3);
        lua_setmetatable(L, -2);
    }
    lua_setglobal(L, s_lua_ext_value_name); // pop
}

/*---------------------------------------------------------------------------*/
static void init_shared_ptr(lua_State* L)
{
    using namespace lua_tinker;
    lua_createtable(L, 0, 4);

    lua_pushstring(L, "__name");
    lua_pushstring(L, S_SHARED_PTR_NAME);
    lua_rawset(L, -3);

    lua_pushstring(L, "__index");
    lua_pushcclosure(L, detail::meta_get, 0);
    lua_rawset(L, -3);

    lua_pushstring(L, "__newindex");
    lua_pushcclosure(L, detail::meta_set, 0);
    lua_rawset(L, -3);

    lua_pushstring(L, "__gc");
    lua_pushcclosure(L, &detail::destroyer<detail::UserDataWapper>, 0);
    lua_rawset(L, -3);

    lua_setglobal(L, S_SHARED_PTR_NAME); // pop table
}

int32_t lua_class_call(lua_State* L)
{
    using namespace lua_tinker;
    using namespace lua_tinker::detail;
    const char* szClassName     = read<const char*>(L, lua_upvalueindex(1));
    const char* szBaseClassName = read<const char*>(L, lua_upvalueindex(2));

    int32_t nArgCount = lua_gettop(L);

    push_meta(L, szBaseClassName);
    stack_obj base_meta = stack_obj::get_top(L);
    if(base_meta.is_table())
    {
        stack_obj base_meta_meta = base_meta.get_metatable();
        if(base_meta_meta.is_table())
        {
            stack_obj base_meta_meta_call = base_meta_meta.rawget("__call");
            if(base_meta_meta_call.is_function())
            {
                base_meta_meta_call.pop_up(base_meta._stack_pos);
                base_meta_meta_call.insert_to(1);
                lua_pcall(L, nArgCount, 1, 0);
                // replace ret's metatable
                lua_tinker::detail::push_meta(L, szClassName);
                lua_setmetatable(L, -2);
                return 1;
            }
        }
    }

    lua_pushfstring(L, "can't direct invoke %s", szClassName);
    lua_error(L);

    lua_pop(L, lua_gettop(L) - nArgCount);
    return 0;
}
int32_t create_class(lua_State* L)
{
    size_t      nClassName;
    const char* szClassName = luaL_checklstring(L, -2, &nClassName);
    if(!szClassName)
    {
        return 0;
    }

    size_t      nBaseClassName;
    const char* szBaseClassName = luaL_checklstring(L, -1, &nBaseClassName);
    if(!szBaseClassName)
    {
        return 0;
    }

    lua_createtable(L, 0, 5);

    lua_pushstring(L, "__name");
    lua_pushstring(L, szClassName);
    lua_rawset(L, -3);

    lua_pushstring(L, "__index");
    lua_pushcclosure(L, lua_tinker::detail::meta_get, 0);
    lua_rawset(L, -3);

    lua_pushstring(L, "__newindex");
    lua_pushcclosure(L, lua_tinker::detail::meta_set, 0);
    lua_rawset(L, -3);

    lua_pushstring(L, "__gc");
    lua_pushcclosure(L, lua_tinker::detail::destroyer<lua_tinker::detail::UserDataWapper>, 0);
    lua_rawset(L, -3);

    lua_pushstring(L, "__parent");
    lua_tinker::detail::push_meta(L, szBaseClassName);
    lua_rawset(L, -3);

    {
        lua_createtable(L, 0, 1);
        {
            lua_pushstring(L, "__call");
            {
                lua_pushstring(L, szClassName);
                lua_pushstring(L, szBaseClassName);
                lua_pushcclosure(L, &lua_class_call, 2);
            }
            lua_rawset(L, -3);
        }
        lua_setmetatable(L, -2);
    }

    lua_setglobal(L, szClassName);

    return 1;
}

void lua_tinker::init(lua_State* L)
{
    init_shared_ptr(L);
    detail::add_gc_mate(L);
    init_close_callback(L);

    lua_register(L, "lua_create_class", create_class);
    set_error_callback(L, &on_error);
}

#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO

bool find_inherit(size_t idThisType, size_t idTypeBase, lua_tinker::detail::InheritMap& refMap)
{
    auto itFindPair = refMap.equal_range(idThisType);
    if(itFindPair.first == refMap.end())
        return false;
    for(auto itFind = itFindPair.first; itFind != itFindPair.second; itFind++)
    {
        if(itFind->second == idTypeBase)
            return true;
        if(find_inherit(itFind->second, idTypeBase, refMap) == true)
            return true;
    }
    return false;
}

bool lua_tinker::detail::IsInherit(lua_State* L, size_t idTypeDerived, size_t idTypeBase)
{
    lua_stack_scope_exit scope_exit(L);
    if(lua_getglobal(L, s_lua_ext_value_name) != LUA_TUSERDATA)
    {
        lua_pop(L, 1); // pop getglobal
        print_error(L, "can't find lua_ext_value");
        return false;
    }

    lua_ext_value* p_lua_ext_val = detail::user2type<lua_ext_value*>(L, -1);
    auto&          refMap        = p_lua_ext_val->m_inherit_map;

    return find_inherit(idTypeDerived, idTypeBase, refMap);
}

void lua_tinker::detail::_addInheritMap(lua_State* L, size_t idTypeDerived, size_t idTypeBase)
{
    lua_stack_scope_exit scope_exit(L);
    if(lua_getglobal(L, s_lua_ext_value_name) != LUA_TUSERDATA)
    {
        lua_pop(L, 1); // pop getglobal
        print_error(L, "can't find lua_ext_value");
        return;
    }

    lua_ext_value* p_lua_ext_val = detail::user2type<lua_ext_value*>(L, -1);
    auto&          refMap        = p_lua_ext_val->m_inherit_map;
    refMap.emplace(idTypeDerived, idTypeBase);
}

#endif

/*---------------------------------------------------------------------------*/
/* debug helpers                                                             */
/*---------------------------------------------------------------------------*/
static void call_stack(lua_State* L, int32_t n)
{
    lua_Debug ar;
    if(lua_getstack(L, n, &ar) == 1)
    {
        lua_getinfo(L, "nSlu", &ar);

        const char* indent;
        if(n == 0)
        {
            indent = "->\t";
            lua_tinker::print_error(L, "\t<call stack>");
        }
        else
        {
            indent = "\t";
        }

        if(ar.name)
            lua_tinker::print_error(L, "%s%s():line %d[%s:%d]", indent, ar.name, ar.currentline, ar.source, ar.linedefined);
        else
            lua_tinker::print_error(L, "%sunknown:line %d[%s:%d]", indent, ar.currentline, ar.source, ar.linedefined);

        call_stack(L, n + 1);
    }
}

/*---------------------------------------------------------------------------*/
int32_t lua_tinker::on_error(lua_State* L)
{
    print_error(L, "%s", lua_tostring(L, -1));

    call_stack(L, 0);

    return 0;
}

/*---------------------------------------------------------------------------*/
void lua_tinker::print_error(lua_State* L, const char* fmt, ...)
{
    char text[4096];

    va_list args;
    va_start(args, fmt);
    vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);

    if(lua_getglobal(L, "_ALERT") == LUA_TFUNCTION)
    {
        lua_pushstring(L, text);
        lua_call(L, 1, 0);
    }
    else
    {
        lua_pop(L, 1); // pop getglobal
        printf("%s\n", text);
    }
}

/*---------------------------------------------------------------------------*/
void lua_tinker::enum_stack(lua_State* L)
{
    int32_t top = lua_gettop(L);
    print_error(L, "%s", "----------stack----------");
    print_error(L, "Type:%d", top);
    for(int32_t i = 1; i <= lua_gettop(L); ++i)
    {
        switch(lua_type(L, i))
        {
            case LUA_TNIL:
                print_error(L, "\t%s", lua_typename(L, lua_type(L, i)));
                break;
            case LUA_TBOOLEAN:
                print_error(L, "\t%s    %s", lua_typename(L, lua_type(L, i)), lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TLIGHTUSERDATA:
                print_error(L, "\t%s    0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
                break;
            case LUA_TNUMBER:
                print_error(L, "\t%s    %f", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));
                break;
            case LUA_TSTRING:
                print_error(L, "\t%s    %s", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));
                break;
            case LUA_TTABLE:
            {
                std::string name;
                lua_pushstring(L, "__name");
                if(lua_rawget(L, i) == LUA_TSTRING)
                {
                    name.assign(lua_tostring(L, -1));
                    lua_remove(L, -1);
                    print_error(L, "\t%s    0x%08p [%s]", lua_typename(L, lua_type(L, i)), lua_topointer(L, i), name.c_str());
                }
                else
                {
                    lua_remove(L, -1);
                    print_error(L, "\t%s    0x%08p ", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
                }
            }
            break;
            case LUA_TFUNCTION:
                print_error(L, "\t%s()  0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
                break;
            case LUA_TUSERDATA:
                print_error(L, "\t%s    0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
                break;
            case LUA_TTHREAD:
                print_error(L, "\t%s", lua_typename(L, lua_type(L, i)));
                break;
        }
    }
    print_error(L, "%s", "-------------------------");
}

void lua_tinker::clear_stack(lua_State* L)
{
    lua_settop(L, 0);
}

// getmetatable(scope_global_name)[name] = getmetatable(global_name)
void lua_tinker::scope_inner(lua_State* L, const char* scope_global_name, const char* name, const char* global_name)
{
    using namespace detail;
    stack_scope_exit scope_exit(L);
    if(push_meta(L, scope_global_name) == LUA_TTABLE)
    {
        lua_pushstring(L, name);
        push_meta(L, global_name);
        lua_rawset(L, -3);
    }
}

// namespace
void lua_tinker::namespace_add(lua_State* L, const char* namespace_name)
{
    lua_createtable(L, 0, 3);

    lua_pushstring(L, "__name");
    lua_pushstring(L, namespace_name);
    lua_rawset(L, -3);

    lua_pushstring(L, "__index");
    lua_pushcclosure(L, detail::meta_get, 0);
    lua_rawset(L, -3);

    lua_pushstring(L, "__newindex");
    lua_pushcclosure(L, detail::meta_set, 0);
    lua_rawset(L, -3);

    lua_setglobal(L, namespace_name);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* lua_tinker::detail::_stack_help<char*>::_read(lua_State* L, int32_t index)
{
    return (char*)lua_tostring(L, index);
}

void lua_tinker::detail::_stack_help<char*>::_push(lua_State* L, char* ret)
{
    lua_pushstring(L, ret);
}

const char* lua_tinker::detail::_stack_help<const char*>::_read(lua_State* L, int32_t index)
{
    return (const char*)lua_tostring(L, index);
}

void lua_tinker::detail::_stack_help<const char*>::_push(lua_State* L, const char* ret)
{
    lua_pushstring(L, ret);
}

bool lua_tinker::detail::_stack_help<bool>::_read(lua_State* L, int32_t index)
{
    if(lua_isboolean(L, index))
        return lua_toboolean(L, index) != 0;
    else
        return lua_tointeger(L, index) != 0;
}
void lua_tinker::detail::_stack_help<bool>::_push(lua_State* L, bool ret)
{
    lua_pushboolean(L, ret);
}

void lua_tinker::detail::_stack_help<lua_tinker::lua_value*>::_push(lua_State* L, lua_value* ret)
{
    if(ret)
        ret->to_lua(L);
    else
        lua_pushnil(L);
}

lua_tinker::table_onstack lua_tinker::detail::_stack_help<lua_tinker::table_onstack>::_read(lua_State* L, int32_t index)
{
    return lua_tinker::table_onstack(L, index);
}

void lua_tinker::detail::_stack_help<lua_tinker::table_onstack>::_push(lua_State* L, const lua_tinker::table_onstack& ret)
{
    lua_pushvalue(L, ret.m_obj->m_index);
}

std::string lua_tinker::detail::_stack_help<std::string>::_read(lua_State* L, int32_t index)
{
    const char* strLua = lua_tostring(L, index);
    if(strLua)
        return std::string(strLua);
    else
        return std::string();
}

void lua_tinker::detail::_stack_help<std::string>::_push(lua_State* L, const std::string& ret)
{
    lua_pushlstring(L, ret.data(), ret.size());
}

std::string_view lua_tinker::detail::_stack_help<std::string_view>::_read(lua_State* L, int32_t index)
{
    const char* strLua = lua_tostring(L, index);
    if(strLua)
        return std::string_view{strLua, strlen(strLua)};
    else
        return std::string_view();
}

// void lua_tinker::detail::_stack_help<std::string_view>::_push(lua_State* L, const std::string_view& ret)
// {
//     lua_pushlstring(L, ret.data(), ret.size());
// }

// void lua_tinker::detail::_stack_help<std::string_view>::_push(lua_State* L, std::string_view&& ret)
// {
//     lua_pushlstring(L, ret.data(), ret.size());
// }

void lua_tinker::detail::_stack_help<std::string_view>::_push(lua_State* L, std::string_view ret)
{
    lua_pushlstring(L, ret.data(), ret.size());
}

/*---------------------------------------------------------------------------*/
/* pop                                                                       */
/*---------------------------------------------------------------------------*/

void lua_tinker::detail::pop<void>::apply(lua_State* L)
{
    // lua_pop(L, 1);
}

lua_tinker::table_ref lua_tinker::detail::pop<lua_tinker::table_ref>::apply(lua_State* L)
{
    stack_delay_pop _dealy(L, nresult);
    return lua_tinker::table_ref::make_table_ref(L, lua_gettop(L));
}

lua_tinker::table_onstack lua_tinker::detail::pop<lua_tinker::table_onstack>::apply(lua_State* L)
{
    // didn't need pop it
    // stack_delay_pop  _dealy(L, nresult);
    return table_onstack(L, lua_gettop(L));
}

/*---------------------------------------------------------------------------*/
/* Tinker Class Helper                                                       */
/*---------------------------------------------------------------------------*/

static void invoke_parent(lua_State* L)
{
    using namespace lua_tinker;
    using namespace lua_tinker::detail;

    stack_obj index_key(L, 2);
    stack_obj class_meta   = stack_obj::get_top(L);
    stack_obj parent_table = class_meta.rawget("__parent");
    if(parent_table.is_nil())
    {
        return;
    }
    else if(parent_table.is_table())
    {
        stack_obj val_obj = parent_table.rawget(index_key); // parent_table[key]
        if(val_obj.is_nil() == false)
        {
            val_obj.pop_up(parent_table._stack_pos); // pop all after this
            return;
        }
        else
        {
            val_obj.remove();
            invoke_parent(L);
            stack_obj result = stack_obj::get_top(L);
            if(result.is_nil() == false)
            {
                val_obj.pop_up(parent_table._stack_pos); // pop all after this
                return;
            }
            else
            {
                result.remove();
            }
        }
    }
#ifdef LUATINKER_MULTI_INHERITANCE

    // try multi_parent
    {
        parent_table.remove();
        stack_obj parent_table = class_meta.rawget("__multi_parent");
        if(parent_table.is_table())
        {
            table_iterator it(parent_table);
            while(it.hasNext())
            {
                stack_obj __base_table = it.value();
                if(__base_table.is_table())
                {
                    stack_obj val_obj = __base_table.rawget(index_key); //__multi_parent[n][key]
                    if(val_obj.is_nil() == false)
                    {
                        val_obj.pop_up(parent_table._stack_pos); // pop all after this
                        return;
                    }
                    else
                    {
                        val_obj.remove();
                        invoke_parent(L);
                        stack_obj result = stack_obj::get_top(L);
                        if(result.is_nil() == false)
                        {
                            val_obj.pop_up(parent_table._stack_pos); // pop all after this
                            return;
                        }
                        else
                        {
                            result.remove();
                        }
                    }
                }
                it.moveNext();
            }
            lua_pushnil(L); // not find return nil
        }
    }

#endif
}

/*---------------------------------------------------------------------------*/
int32_t lua_tinker::detail::meta_get(lua_State* L)
{
    stack_obj class_obj(L, 1);
    stack_obj key_obj(L, 2);
    stack_obj class_meta = class_obj.get_metatable();
    stack_obj val_obj    = class_meta.rawget(key_obj);
    if(val_obj.is_userdata())
    {
        detail::user2type<detail::var_base*>(L, val_obj._stack_pos)->get(L); // push a val
        val_obj.remove();
    }
    else if(val_obj.is_nil())
    {
        val_obj.remove();
        invoke_parent(L);
        val_obj = stack_obj::get_top(L);
        if(val_obj.is_userdata())
        {
            detail::user2type<detail::var_base*>(L, val_obj._stack_pos)->get(L); // push a val
            val_obj.remove();
        }
        else if(val_obj.is_nil())
        {
            lua_pushfstring(L,
                            "can't find '%s' class variable. (forgot registering class variable ?)",
                            read_nocheck<const char*>(L, key_obj._stack_pos));
            lua_error(L);
        }
    }
    class_meta.remove();
    return 1;
}

/*---------------------------------------------------------------------------*/
int32_t lua_tinker::detail::meta_set(lua_State* L)
{
    stack_scope_exit scope_exit(L);
    stack_obj        class_obj(L, 1);
    stack_obj        key_obj(L, 2);
    stack_obj        class_meta = class_obj.get_metatable();
    stack_obj        val_obj    = class_meta.rawget(key_obj); // class_meta[key]

    if(val_obj.is_userdata())
    {
        detail::user2type<detail::var_base*>(L, val_obj._stack_pos)->set(L);
    }
    else if(val_obj.is_nil())
    {
        val_obj.remove();
        key_obj.push_top();
        class_meta.push_top();
        invoke_parent(L);
        val_obj = stack_obj::get_top(L);
        if(val_obj.is_userdata())
        {
            detail::user2type<detail::var_base*>(L, val_obj._stack_pos)->set(L);
        }
        else if(val_obj.is_nil())
        {
            lua_pushfstring(L,
                            "can't find '%s' class variable. (forgot registering class variable ?)",
                            read_nocheck<const char*>(L, key_obj._stack_pos));
            lua_error(L);
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*/
int32_t lua_tinker::detail::push_meta(lua_State* L, const char* name)
{
    return lua_getglobal(L, name);
}

void lua_tinker::detail::push_args(lua_State* L) {}

void lua_tinker::detail::add_gc_mate(lua_State* L)
{
    lua_createtable(L, 0, 1);
    lua_pushstring(L, "__gc");
    lua_pushcclosure(L, detail::destroyer<detail::UserDataWapper>, 0);
    lua_rawset(L, -3);
    lua_setglobal(L, "__onlygc_meta");
}

bool lua_tinker::detail::CheckSameMetaTable(lua_State* L, int32_t nIndex, const char* tname)
{
    bool  bResult = true;
    void* p       = lua_touserdata(L, nIndex);
    if(p != NULL)
    { /* value is a userdata? */
        if(lua_getmetatable(L, nIndex))
        {                                /* does it have a metatable? */
            push_meta(L, tname);         /* get correct metatable */
            if(!lua_rawequal(L, -1, -2)) /* not the same? */
                bResult = false;         /* value is a userdata with wrong metatable */
            lua_pop(L, 2);               /* remove both metatables */
            return bResult;
        }
    }
    return false;
}

bool lua_tinker::detail::push_upval_to_stack(lua_State* L, int32_t nArgsCount, int32_t nArgsNeed, int32_t default_upval_start)
{
    if(nArgsCount < nArgsNeed)
    {
        // need use upval
        int32_t nNeedUpval  = nArgsNeed - nArgsCount;
        int32_t nUpvalCount = read<int32_t>(L, lua_upvalueindex(default_upval_start));
        if(nUpvalCount < 0)
        {
            return false;
        }
        for(int32_t i = nUpvalCount - nNeedUpval; i < nUpvalCount; i++)
        {
            lua_pushvalue(L, lua_upvalueindex(default_upval_start + 1 + i));
        }
    }

    return true;
}

bool lua_tinker::detail::push_upval_to_stack(lua_State* L, int32_t nArgsCount, int32_t nArgsNeed, int32_t nUpvalCount, int32_t UpvalStart)
{
    if(nArgsCount < nArgsNeed)
    {
        // need use upval
        int32_t nNeedUpval = nArgsNeed - nArgsCount;
        if(nUpvalCount < 0)
        {
            return false;
        }
        constexpr int32_t default_upval_start = 2;
        for(int32_t i = nUpvalCount - nNeedUpval; i < nUpvalCount; i++)
        {
            lua_pushvalue(L, lua_upvalueindex(default_upval_start + UpvalStart + i));
        }
    }

    return true;
}

void lua_tinker::detail::_set_signature_bit(uint64_t& sig, size_t idx, uint8_t c)
{
    if(idx > sizeof(sig) * 2)
        return;
    sig = (sig & ~(0xF << (idx * 4))) | ((c & 0xF) << (idx * 4));
}

uint8_t lua_tinker::detail::_get_signature_bit(const uint64_t& sig, size_t idx)
{
    if(idx > sizeof(sig) * 2)
        return 0;
    return (sig >> (idx * 4)) & 0xF;
}

/*---------------------------------------------------------------------------*/
/* table object on stack                                                     */
/*---------------------------------------------------------------------------*/
lua_tinker::detail::table_obj::table_obj(lua_State* L, int32_t index)
    : m_L(L)
    , m_index(index)
    , m_ref(0)
{
    if(lua_isnil(m_L, m_index))
    {
        m_pointer = NULL;
        lua_remove(m_L, m_index);
    }
    else
    {
        m_pointer = lua_topointer(m_L, m_index);
    }
}

lua_tinker::detail::table_obj::~table_obj()
{
    if(validate())
    {
        lua_remove(m_L, m_index);
    }
}

void lua_tinker::detail::table_obj::inc_ref()
{
    ++m_ref;
}

void lua_tinker::detail::table_obj::dec_ref()
{
    if(--m_ref == 0)
        delete this;
}

bool lua_tinker::detail::table_obj::validate()
{
    if(m_pointer != NULL)
    {
        if(m_pointer == lua_topointer(m_L, m_index))
        {
            return true;
        }
        else
        {
            int32_t top = lua_gettop(m_L);

            for(int32_t i = 1; i <= top; ++i)
            {
                if(m_pointer == lua_topointer(m_L, i))
                {
                    m_index = i;
                    return true;
                }
            }

            m_pointer = NULL;
            return false;
        }
    }
    else
    {
        return false;
    }
}

/*---------------------------------------------------------------------------*/
/* Table Object Holder                                                       */
/*---------------------------------------------------------------------------*/
lua_tinker::table_onstack::table_onstack(lua_State* L)
{
    lua_newtable(L);

    m_obj = new detail::table_obj(L, lua_gettop(L));

    m_obj->inc_ref();
}

lua_tinker::table_onstack::table_onstack(lua_State* L, const char* name)
{
    if(lua_getglobal(L, name) != LUA_TTABLE)
    {
        lua_pop(L, 1);

        lua_newtable(L);
        lua_setglobal(L, name);
        lua_getglobal(L, name);
    }

    m_obj = new detail::table_obj(L, lua_gettop(L));

    m_obj->inc_ref();
}

lua_tinker::table_onstack::table_onstack(lua_State* L, int32_t index)
{
    if(index < 0)
    {
        index = lua_gettop(L) + index + 1;
    }

    m_obj = new detail::table_obj(L, index);

    m_obj->inc_ref();
}

lua_tinker::table_onstack::table_onstack(const table_onstack& input)
{
    m_obj = input.m_obj;

    m_obj->inc_ref();
}

lua_tinker::table_onstack::~table_onstack()
{
    m_obj->dec_ref();
}

/*---------------------------------------------------------------------------*/

namespace lua_tinker
{
    namespace detail
    {

        lua_ref_base::lua_ref_base(lua_State* L, int32_t regidx)
            : m_L(L)
            , m_regidx(regidx)
            , m_pRef(new int32_t(0))
        {
            inc_ref();
        }

        lua_ref_base::lua_ref_base(const lua_ref_base& rht)
            : m_L(rht.m_L)
            , m_regidx(rht.m_regidx)
            , m_pRef(rht.m_pRef)
        {
            inc_ref();
        }

        lua_ref_base::lua_ref_base(lua_ref_base&& rht) noexcept
            : m_L(rht.m_L)
            , m_regidx(rht.m_regidx)
            , m_pRef(rht.m_pRef)
        {
            rht.m_pRef = nullptr;
        }

        lua_ref_base& lua_ref_base::operator=(const lua_ref_base& rht)
        {
            if(this != &rht)
            {
                dec_ref();
                m_L      = rht.m_L;
                m_regidx = rht.m_regidx;
                m_pRef   = rht.m_pRef;
                inc_ref();
            }
            return *this;
        }

        lua_ref_base::~lua_ref_base()
        {
            // if find it, than unref, else maybe lua is closed
            dec_ref();
        }

        void lua_ref_base::destory()
        {
            luaL_unref(m_L, LUA_REGISTRYINDEX, m_regidx);
            delete m_pRef;
        }

        void lua_ref_base::reset()
        {
            dec_ref();
            m_L      = nullptr;
            m_regidx = -1;
            m_pRef   = nullptr;
        }

        void lua_ref_base::inc_ref()
        {
            if(m_pRef)
                ++(*m_pRef);
        }

        void lua_ref_base::dec_ref()
        {
            if(m_pRef)
            {
                if(--(*m_pRef) == 0)
                    destory();
            }
        }
    } // namespace detail
} // namespace lua_tinker
