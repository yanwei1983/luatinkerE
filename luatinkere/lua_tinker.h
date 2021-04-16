// lua_tinker.h
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright (c) 2005-2007 Kwon-il Lee (zupet@hitel.net)
//
// please check Licence.txt file for licence and legal issues.

#if !defined(_LUA_TINKER_H_)
#define _LUA_TINKER_H_

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <new>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <typeinfo>

//#include<set>
#include <map>
//#include<vector>

#include "lua.hpp"
#include "lua_tinker_stackobj.h"
#include "type_traits_ext.h"

extern "C" {
#include "lstate.h"
}

#ifdef _DEBUG
#define LUATINKER_USERDATA_CHECK_TYPEINFO
#define LUATINKER_USERDATA_CHECK_CONST
#endif //  _DEBUG

#define _ALLOW_SHAREDPTR_INVOKE

#ifdef LUA_CALL_CFUNC_NEED_ALL_PARAM
#define LUA_CHECK_HAVE_THIS_PARAM(L, index)                          \
    if(lua_isnone(L, index))                                         \
    {                                                                \
        call_error(L, "need argument %d to call cfunc", index);     \
    }
#define LUA_CHECK_HAVE_THIS_PARAM_AND_NOT_NIL(L, index)              \
    if(lua_isnoneornil(L, index))                                    \
    {                                                                \
        call_error(L, "need argument %d to call cfunc", index);         \
    }
#else
#define LUA_CHECK_HAVE_THIS_PARAM(L, index)
#define LUA_CHECK_HAVE_THIS_PARAM_AND_NOT_NIL(L, index)
#endif

#define CHECK_CLASS_PTR(T)                                                                              \
    {                                                                                                   \
        if(lua_isnoneornil(L, 1))                                                                       \
        {                                                                                               \
            call_error(L, "class_ptr %s is nil or none", lua_tinker::detail::get_class_name<T>()); \
        }                                                                                               \
    }

#define TRY_LUA_TINKER_INVOKE()   try
#define CATCH_LUA_TINKER_INVOKE() catch(...)

#define LUATINKER_MULTI_INHERITANCE

namespace lua_tinker
{
    constexpr const char* S_SHARED_PTR_NAME   = "__shared_ptr";
    constexpr const char* ERROR_CALLBACK_NAME = "__on_error";
    constexpr const char* S_EMPTY             = "";
    // init LuaTinker
    void init(lua_State* L);

    // close callback func
    typedef std::function<void(lua_State*)> Lua_Close_CallBack_Func;

    void register_lua_close_callback(lua_State* L, Lua_Close_CallBack_Func&& callback_func);

    // error callback
    typedef int32_t (*error_call_back_fn)(lua_State* L);
    void set_error_callback(lua_State* L, error_call_back_fn fn);

    // string-buffer excution
    template<typename RVal = void>
    RVal dofile(lua_State* L, const char* filename);
    template<typename RVal = void>
    RVal dostring(lua_State* L, const std::string& buff, const char* name = nullptr);
    template<typename RVal = void>
    RVal dobuffer(lua_State* L, const char* buff, size_t sz, const char* name = nullptr);

    // debug helpers
    void    enum_stack(lua_State* L);
    void    clear_stack(lua_State* L);
    int32_t on_error(lua_State* L);
    void    print_error(lua_State* L, const char* fmt, ...);

    template<typename... Args>
    void    call_error(lua_State* L, const char* fmt, Args&&...args)
    {
        if(L->errfunc)
        {
            lua_pushfstring(L, fmt, std::forward<Args>(args)...);
            lua_error(L);
        }
        else
        {
            print_error(L, fmt, std::forward<Args>(args)...);
        }
    }

    template<typename T>
    T return_empty()
    {
        if constexpr(std::is_reference<T>::value)
        {
            throw std::runtime_error("return a empty reference");
            base_type<T>* empty_ptr = nullptr;
            return *empty_ptr;
        }
        else
        {
            return {};
        }
    }

    // dynamic type extention
    struct lua_value
    {
        virtual ~lua_value() {}
        virtual void to_lua(lua_State* L) = 0;
    };
    struct table_onstack;
    struct table_ref;
    struct args_type_overload_functor_base;

    struct lua_function_ref;

    // global function
    template<typename Func, typename... DefaultArgs>
    void def(lua_State* L, const char* name, Func&& func, DefaultArgs&&... default_args);
    // global variable
    template<typename T>
    void set(lua_State* L, const char* name, T&& object);
    template<typename T>
    T get(lua_State* L, const char* name);
    template<typename T>
    void decl(lua_State* L, const char* name, T&& object);
    // call lua func
    template<typename RVal, typename... Args>
    RVal call(lua_State* L, const char* name, Args&&... arg);
    template<typename RVal, typename... Args>
    RVal call_stackfunc(lua_State* L, Args&&... arg);
    // getmetatable(scope_global_name)[name] = getmetatable(global_name)
    void scope_inner(lua_State* L, const char* scope_global_name, const char* name, const char* global_name);
    // namespace
    void namespace_add(lua_State* L, const char* namespace_name);
    template<typename T>
    void namespace_set(lua_State* L, const char* namespace_name, const char* name, T&& object);
    template<typename T>
    T namespace_get(lua_State* L, const char* namespace_name, const char* name);

    // namespace func
    template<typename Func, typename... DefaultArgs>
    void namespace_def(lua_State*  L,
                       const char* namespace_name,
                       const char* name,
                       Func&&      func,
                       DefaultArgs&&... default_args);

    // class init
    template<typename T>
    void class_add(lua_State* L, const char* name, bool bInitShared = false);
    // reg shared_ptr<T>
    template<typename T>
    void class_add_shared(lua_State* L, const char* name);
    // Tinker Class Constructor
    template<typename T, typename F, typename... DefaultArgs>
    void class_con(lua_State* L, F&& func, DefaultArgs&&... default_args);
    // Tinker Class Inheritance
    template<typename T, typename P>
    void class_inh(lua_State* L);
    // Tinker Class alias
    template<typename T, typename Alias>
    void class_alias(lua_State* L, const char* name);

    // Tinker Class Functions
    template<typename T, typename Func, typename... DefaultArgs>
    void class_def(lua_State* L, const char* name, Func&& func, DefaultArgs&&... default_args);
    template<typename T, typename Func, typename... DefaultArgs>
    void class_def_static(lua_State* L, const char* name, Func&& func, DefaultArgs&&... default_args);

    // Tinker Class Variables
    template<typename T, typename BASE, typename VAR>
    void class_mem(lua_State* L, const char* name, VAR BASE::*val);
    template<typename T, typename BASE, typename VAR>
    void class_mem_readonly(lua_State* L, const char* name, VAR BASE::*val);
    // Tinker Class Static Variables
    template<typename T, typename VAR>
    void class_mem_static(lua_State* L, const char* name, VAR* val);
    template<typename T, typename VAR>
    void class_mem_static_readonly(lua_State* L, const char* name, VAR* val);
    template<typename T, typename VAR>
    void class_var_static(lua_State* L, const char* name, VAR&& val);

    // Tinker Class Property
    template<typename T, typename GET_FUNC, typename SET_FUNC>
    void class_property(lua_State* L, const char* name, GET_FUNC&& get_func, SET_FUNC&& set_func);

    namespace detail
    {
        // class helper
        int32_t meta_get(lua_State* L);
        int32_t meta_set(lua_State* L);
        int32_t push_meta(lua_State* L, const char* name);
        void    add_gc_mate(lua_State* L);

        template<typename T>
        struct class_name
        {
            // global name
            static const char* name(const char* name = NULL) { return name_str(name).c_str(); }

            static const std::string& name_str(const char* name = NULL)
            {
                static std::string s_name = S_EMPTY;
                if(name != NULL)
                    s_name.assign(name);
                return s_name;
            }
        };

        template<typename T>
        using base_type =
            typename std::remove_cv<typename std::remove_reference<typename std::remove_pointer<T>::type>::type>::type;

        template<typename T>
        constexpr const char* get_class_name()
        {
            if constexpr(is_shared_ptr<T>::value)
            {
                const std::string& strSharedName = class_name<base_type<T>>::name_str();
                if(strSharedName.empty())
                {
                    return S_SHARED_PTR_NAME;
                }
                else
                {
                    return strSharedName.c_str();
                }
            }
            else
            {
                return class_name<base_type<T>>::name();
            }
        }

        template<typename T>
        constexpr const size_t get_type_idx()
        {
            // convert all T to T*'s typeid's hash_code
#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
            return typeid(T*).hash_code();
#else
            return 0;
#endif
        }

        struct lua_stack_scope_exit
        {
            int32_t    m_nOldTop;
            lua_State* m_L;
            lua_stack_scope_exit(lua_State* L)
                : m_L(L)
            {
                m_nOldTop = lua_gettop(m_L);
            }
            ~lua_stack_scope_exit() { lua_settop(m_L, m_nOldTop); }
        };

#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
        // inherit map
        typedef std::multimap<size_t, size_t> InheritMap;
        bool                                  IsInherit(lua_State* L, size_t idTypeDerived, size_t idTypeBase);
        void                                  _addInheritMap(lua_State* L, size_t idTypeDerived, size_t idTypeBase);

        template<typename T, typename P>
        void addInheritMap(lua_State* L)
        {
            _addInheritMap(L, get_type_idx<base_type<T>>(), get_type_idx<base_type<P>>());
        }

#endif

        // type trait
        template<typename T>
        struct class_name;

        template<typename T, typename Enable = void>
        struct _stack_help;

        // forward delcare
        template<typename T>
        auto read(lua_State* L, int32_t index) -> decltype(_stack_help<T>::_read(L, index));
        template<typename T>
        auto read_nocheck(lua_State* L, int32_t index) -> decltype(_stack_help<T>::_read(L, index));

        template<typename T>
        void push(lua_State* L, T ret); // here need a T/T*/T& not a T&&

        template<typename R, typename... ARGS, typename... DEFAULT_ARGS>
        void _push_functor(lua_State* L, R(func)(ARGS...), DEFAULT_ARGS&&... default_args);
        template<typename R, typename... ARGS, typename... DEFAULT_ARGS>
        void _push_functor(lua_State* L, const std::function<R(ARGS...)>& func, DEFAULT_ARGS&&... default_args);
        template<typename R, typename... ARGS, typename... DEFAULT_ARGS>
        void _push_functor(lua_State* L, std::function<R(ARGS...)>&& func, DEFAULT_ARGS&&... default_args);

        // from lua
        // param to pointer
        template<typename T>
        T void2type(void* ptr)
        {
            if constexpr(std::is_pointer<T>::value)
                return (base_type<T>*)ptr;
            else if constexpr(std::is_reference<T>::value)
                return *(base_type<T>*)ptr;
            else
                return *(base_type<T>*)ptr;
        }

        // userdata to T，T*，T&
        template<typename T>
        T user2type(lua_State* L, int32_t index)
        {
            return void2type<T>(lua_touserdata(L, index));
        }

        // userdata holder
        struct UserDataWapper
        {
            template<typename T>
            explicit UserDataWapper(T* p)
                : m_p(p)
#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
                , m_type_idx(get_type_idx<T>())
#endif
            {
            }

            // push a const to lua will lost constant qualifier
            template<typename T>
            explicit UserDataWapper(const T* p)
                : m_p(const_cast<T*>(p))
#ifdef LUATINKER_USERDATA_CHECK_CONST
                , m_bConst(true)
#endif
#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
                , m_type_idx(get_type_idx<T>())
#endif
            {
            }

#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
            template<typename T>
            explicit UserDataWapper(T* p, size_t nTypeIdx)
                : m_p(p)
                , m_type_idx(nTypeIdx)
            {
            }
#endif

            virtual ~UserDataWapper() {}
            virtual bool isSharedPtr() const { return false; }
            virtual bool haveOwership() const { return false; }

            void* m_p;
#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
            size_t m_type_idx;
#endif

#ifdef LUATINKER_USERDATA_CHECK_CONST
            bool is_const() const { return m_bConst; }
            bool m_bConst = false;
#endif
        };

        template<class... Args>
        struct class_tag
        {
        };

        template<typename T>
        struct val2user : UserDataWapper
        {
            val2user()
                : UserDataWapper(new T)
            {
            }
            val2user(const T& t)
                : UserDataWapper(new T(t))
            {
            }
            val2user(T&& t)
                : UserDataWapper(new T(std::forward<T>(t)))
            {
            }

            // tuple is hold the params, so unpack it
            // template<typename Tup, size_t ...index>
            // val2user(Tup&& tup, std::index_sequence<index...>) : UserDataWapper(new
            // T(std::get<index>(std::forward<Tup>(tup))...)) {}

            // template<typename Tup,typename = typename std::enable_if<is_tuple<Tup>::value, void>::type >
            // val2user(Tup&& tup) : val2user(std::forward<Tup>(tup), std::make_index_sequence<std::tuple_size<typename
            // std::decay<Tup>::type>::value>{}) {}

            // direct read args, use type_list to help hold Args
            template<typename... Args, size_t... index>
            val2user(lua_State* L, std::index_sequence<index...>, class_tag<Args...> tag)
                : UserDataWapper(new T(read<Args>(L, 2 + index)...))
            {
            }

            template<typename... Args>
            val2user(lua_State* L, class_tag<Args...> tag)
                : val2user(L, std::make_index_sequence<sizeof...(Args)>(), tag)
            {
            }

            val2user(lua_State* L, class_tag<void> tag)
                : val2user()
            {
            }

            virtual bool haveOwership() const { return true; }

            ~val2user() { delete((T*)m_p); }
        };

        template<typename T>
        struct ptr2user : UserDataWapper
        {
            ptr2user(T* t)
                : UserDataWapper(t)
            {
            }
            ptr2user(const T* t)
                : UserDataWapper(t)
            {
            }
        };

        template<typename T>
        struct ref2user : UserDataWapper
        {
            ref2user(T& t)
                : UserDataWapper(&t)
            {
            }
        };

        template<typename T>
        struct weakptr2user : UserDataWapper
        {
            weakptr2user(const std::shared_ptr<T>& rht)
                : UserDataWapper(&m_holder
#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
                                 ,
                                 get_type_idx<std::shared_ptr<T>>()
#endif
                                     )
                , m_holder(rht)
            {
            }

            virtual bool isSharedPtr() const override { return true; }
            // use weak_ptr to hold it
            ~weakptr2user() { m_holder.reset(); }

            std::weak_ptr<T> m_holder;
        };

        template<typename T>
        struct sharedptr2user : UserDataWapper
        {
            sharedptr2user(const std::shared_ptr<T>& rht)
                : UserDataWapper(&m_holder
#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
                                 ,
                                 get_type_idx<std::shared_ptr<T>>()
#endif
                                     )
                , m_holder(rht)
            {
            }
            sharedptr2user(std::shared_ptr<T>&& rht)
                : UserDataWapper(&m_holder
#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
                                 ,
                                 get_type_idx<std::shared_ptr<T>>()
#endif
                                     )
                , m_holder(std::forward<std::shared_ptr<T>>(rht))
            {
            }
            virtual bool isSharedPtr() const override { return true; }
            virtual bool haveOwership() const override { return true; }
            // use weak_ptr to hold it
            ~sharedptr2user() { m_holder.reset(); }

            std::shared_ptr<T> m_holder;
        };

        // pop a value from lua stack
        template<typename T>
        struct pop
        {
            static constexpr const int32_t nresult = 1;

            static T apply(lua_State* L)
            {
                stack_delay_pop _dealy(L, nresult);
                return read_nocheck<T>(L, -1);
            }
        };

        template<typename... TS>
        struct pop<std::tuple<TS...>>
        {
            static constexpr const int32_t nresult = sizeof...(TS);

            static std::tuple<TS...> apply(lua_State* L)
            {
                stack_delay_pop _dealy(L, nresult);
                return apply_help(L, std::make_index_sequence<nresult>{});
            }

            template<std::size_t... index>
            static std::tuple<TS...> apply_help(lua_State* L, std::index_sequence<index...>)
            {
                return std::make_tuple(read_nocheck<TS>(L, (int32_t)(index - nresult))...);
            }
        };

        template<>
        struct pop<void>
        {
            static constexpr const int32_t nresult = 0;

            static void apply(lua_State* L);
        };

        template<>
        struct pop<table_ref>
        {
            static constexpr const int32_t nresult = 1;

            static table_ref apply(lua_State* L);
        };

        template<>
        struct pop<table_onstack>
        {
            static constexpr const int32_t nresult = 1;

            static table_onstack apply(lua_State* L);
        };

        template<typename RVal>
        RVal pop_nil(lua_State* L)
        {
            for(int32_t i = 0; i < detail::pop<RVal>::nresult; i++)
            {
                lua_pushnil(L);
            };
            return detail::pop<RVal>::apply(L);
        }
    }

    


    namespace detail
    {
        struct lua_ref_base
        {
            lua_State* m_L      = nullptr;
            int32_t    m_regidx = 0;
            int32_t*   m_pRef   = nullptr;

            void inc_ref();
            void dec_ref();

            bool empty() const { return m_L == nullptr; }
            void destory();
            void reset();

            operator bool()const
            {
                return empty();
            }

            lua_ref_base() {}
            lua_ref_base(lua_State* L, int32_t regidx);
            virtual ~lua_ref_base();
            lua_ref_base(const lua_ref_base& rht);
            lua_ref_base(lua_ref_base&& rht) noexcept;
            lua_ref_base& operator=(const lua_ref_base& rht);
        };
    }

    struct lua_function_ref : public detail::lua_ref_base
    {
        using lua_ref_base::lua_ref_base;

        template<typename RVal, typename... Args>
        RVal operator()(Args&&... args) const
        {
            return invoke<RVal>(std::forward<Args>(args)...);
        }
        
        template<typename RVal, typename... Args>
        RVal invoke(Args&&... args) const
        {
            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_regidx);
            if(lua_isfunction(m_L, -1) == false)
            {
                lua_pop(m_L, 1); // pop func
                print_error(m_L, "lua_tinker::call() attempt to call register_func[`%d'] (not a function)", m_regidx);

                return detail::pop_nil<RVal>(m_L);
            }
            return call_stackfunc<RVal>(m_L, std::forward<Args>(args)...);
        }
    };

    namespace detail
    {
        // push value_list to lua stack //here need a T/T*/T& not a T&&
        void push_args(lua_State* L);
        template<typename T, typename... Args>
        void push_args(lua_State* L, T&& ret)
        {
            push(L, std::forward<T>(ret));
        }
        template<typename T, typename... Args>
        void push_args(lua_State* L, T&& ret, Args&&... args)
        {
            push(L, std::forward<T>(ret));
            push_args<Args...>(L, std::forward<Args>(args)...);
        }

        // to lua
        // userdata pointer to lua
        template<typename T>
        void object2lua(lua_State* L, T&& input)
        {
            if constexpr(std::is_pointer<T>::value)
            {
                if(input)
                {
                    new(lua_newuserdata(L, sizeof(ptr2user<base_type<T>>))) ptr2user<base_type<T>>(std::forward<T>(input));
                }
                else
                {
                    lua_pushnil(L);
                }
            }
            else if constexpr(std::is_rvalue_reference<T>::value)
            {
                new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(std::forward<T>(input));
            }
            else if constexpr(std::is_reference<T>::value)
            {
                new(lua_newuserdata(L, sizeof(ref2user<T>))) ref2user<T>(std::forward<T>(input));
            }
            else
            {
                new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(std::forward<T>(input));
            }
        }

        // get value from cclosure
        template<typename T>
        T upvalue_(lua_State* L)
        {
            return user2type<T>(L, lua_upvalueindex(1));
        }

        enum OVERLOAD_PARAMTYPE : uint8_t
        {
            CLT_NONE     = LUA_TNIL,
            CLT_BOOLEAN  = LUA_TBOOLEAN,
            CLT_NOUSE1   = LUA_TLIGHTUSERDATA,
            CLT_INT      = LUA_TNUMBER,
            CLT_STRING   = LUA_TSTRING,
            CLT_TABLE    = LUA_TTABLE,
            CLT_FUNCTION = LUA_TFUNCTION,
            CLT_USERDATA = LUA_TUSERDATA,
            CLT_NOUSE2   = LUA_TTHREAD,
            CLT_DOUBLE,
            CLT_MAX = 0xF,
        };
        const char* const OVERLOAD_PARAMTYPE_NAME[] = {
            "CLT_NONE",
            "CLT_BOOLEAN",
            "CLT_NOUSE1",
            "CLT_INT",
            "CLT_STRING",
            "CLT_TABLE",
            "CLT_FUNCTION",
            "CLT_USERDATA",
            "CLT_NOUSE2",
            "CLT_DOUBLE",
        };

        inline uint8_t LType2ParamsType(lua_State* L, int32_t idx)
        {
            int32_t nType = lua_type(L, idx);
            if(nType != LUA_TNUMBER)
                return nType;
            if(!lua_isinteger(L, idx))
                return CLT_DOUBLE;
            else
                return CLT_INT;
        }

        template<typename _T>
        static _T _lua2type(lua_State* L, int32_t index)
        {
            if(!lua_isuserdata(L, index))
            {
                call_error(L, "can't convert argument %d to class %s", index, get_class_name<_T>());
                return return_empty<_T>();
            }

            UserDataWapper* pWapper = user2type<UserDataWapper*>(L, index);
            if(pWapper == nullptr)
            {
                call_error(L, "can't convert argument %d to class %s", index, get_class_name<_T>());
                return return_empty<_T>();
            }

            if constexpr(!std::is_same<base_type<_T>, void*>::value)
            {
                return void2type<_T>(pWapper->m_p);
            }

#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
            if(pWapper->m_type_idx != get_type_idx<base_type<_T>>())
            {
                // maybe derived to base
                if(IsInherit(L, pWapper->m_type_idx, get_type_idx<base_type<_T>>()) == false)
                {
                    call_error(L, "can't convert argument %d to class %s", index, get_class_name<_T>());
                    return return_empty<_T>();
                }
            }
#endif
#ifdef LUATINKER_USERDATA_CHECK_CONST
            if((std::is_reference<_T>::value || std::is_pointer<_T>::value) && pWapper->is_const() == true &&
               std::is_const<typename std::remove_reference<typename std::remove_pointer<_T>::type>::type>::value == false)
            {
                call_error(L, "can't convert argument %d from const class %s", index, get_class_name<_T>());
                return return_empty<_T>();
            }
#endif

            return void2type<_T>(pWapper->m_p);
        }

        template<typename T>
        void add_container_mate(lua_State* L);

        template<typename _T>
        void _noregtype2lua(lua_State* L, _T&& val)
        {
            if constexpr(!std::is_same<base_type<_T>, std::string>::value && is_container<base_type<_T>>::value)
            {
                // container warp
                add_container_mate<base_type<_T>>(L);
                push_meta(L, get_class_name<_T>());
                lua_setmetatable(L, -2);
            }
            else if(!std::is_reference<_T>::value && !std::is_pointer<_T>::value)
            {
                // val2usr must add _gc
                push_meta(L, "__onlygc_meta");
                lua_setmetatable(L, -2);
            }
        }

        template<typename _T>
        void _type2lua(lua_State* L, _T&& val)
        {
            object2lua(L, std::forward<_T>(val));
            if(get_class_name<_T>() != std::string(""))
            {
                push_meta(L, get_class_name<_T>());
                lua_setmetatable(L, -2);
            }
            else
            {
                _noregtype2lua(L, std::forward<_T>(val));
            }
        }

        // lua stack help to read/push
        template<typename T, typename Enable>
        struct _stack_help
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_USERDATA; }

            static T _read(lua_State* L, int32_t index) { return lua2type<T>(L, index); }

            // get userdata ptr from lua, can handle nil an 0
            template<typename _T>
            static _T lua2type(lua_State* L, int32_t index)
            {
                if constexpr(std::is_pointer<_T>::value)
                {
                    if(lua_isnoneornil(L, index))
                    {
                        return nullptr;
                    }
                    else if(lua_isnumber(L, index) && lua_tonumber(L, index) == 0)
                    {
                        return nullptr;
                    }
                    return _lua2type<_T>(L, index);
                }
                else
                {
                    return _lua2type<_T>(L, index);
                }
            }

            // obj to lua
            template<typename _T>
            static void _push(lua_State* L, _T&& val)
            {
                _type2lua(L, std::forward<_T>(val));
            }
        };

        template<>
        struct _stack_help<char*>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_STRING; }
            static char*             _read(lua_State* L, int32_t index);
            static void              _push(lua_State* L, char* ret);
        };

        template<>
        struct _stack_help<const char*>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_STRING; }
            static const char*       _read(lua_State* L, int32_t index);
            static void              _push(lua_State* L, const char* ret);
        };

        template<>
        struct _stack_help<bool>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_BOOLEAN; }

            static bool _read(lua_State* L, int32_t index);
            static void _push(lua_State* L, bool ret);
        };

        // integral
        template<typename T>
        struct _stack_help<T, typename std::enable_if<std::is_integral<T>::value>::type>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_INT; }

            static T    _read(lua_State* L, int32_t index) { return (T)lua_tointeger(L, index); }
            static void _push(lua_State* L, T ret) { lua_pushinteger(L, ret); }
        };

        // float pointer
        template<typename T>
        struct _stack_help<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_DOUBLE; }

            static T    _read(lua_State* L, int32_t index) { return (T)lua_tonumber(L, index); }
            static void _push(lua_State* L, T ret) { lua_pushnumber(L, ret); }
        };

        template<>
        struct _stack_help<std::string>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_STRING; }

            static std::string _read(lua_State* L, int32_t index);
            static void        _push(lua_State* L, const std::string& ret);
        };
        template<>
        struct _stack_help<const std::string&> : public _stack_help<std::string>
        {
        };

        template<>
        struct _stack_help<std::string_view>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_STRING; }

            static std::string_view _read(lua_State* L, int32_t index);
            // static void             _push(lua_State* L, const std::string_view& ret);
            // static void             _push(lua_State* L, std::string_view&& ret);
            static void _push(lua_State* L, std::string_view ret);
        };
        template<>
        struct _stack_help<const std::string_view&> : public _stack_help<std::string_view>
        {
        };

        template<>
        struct _stack_help<table_onstack>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_TABLE; }
            static table_onstack     _read(lua_State* L, int32_t index);
            static void              _push(lua_State* L, const table_onstack& ret);
        };

        template<>
        struct _stack_help<lua_value*>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_USERDATA; }
            static lua_value*        _read(lua_State* L, int32_t index);
            static void              _push(lua_State* L, lua_value* ret);
        };

        // enum
        template<typename T>
        struct _stack_help<T, typename std::enable_if<std::is_enum<T>::value>::type>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_INT; }
            static T                 _read(lua_State* L, int32_t index) { return (T)lua_tointeger(L, index); }
            static void              _push(lua_State* L, T ret) { lua_pushinteger(L, (int32_t)ret); }
        };

        // support container
        template<typename _T>
        _T _readfromtable(lua_State* L, int32_t index)
        {
            stack_obj table_obj(L, index);
            if(table_obj.is_table() == false)
            {
                call_error(L, "convert k-v container from argument %d must be a table", index);
                return return_empty<_T>();
            }

            _T t;

            table_iterator it(table_obj);
            while(it.hasNext())
            {
                if constexpr(is_map_like<_T>::value)
                {
                    t.emplace(std::make_pair(read<typename _T::key_type>(L, it.key_idx()), read<typename _T::mapped_type>(L, it.value_idx())));
                }
                else if constexpr(is_set_like<_T>::value)
                {
                    t.emplace(read<typename _T::value_type>(L, it.value_idx()));
                }
                else
                {
                    t.emplace_back(read<typename _T::value_type>(L, it.value_idx()));
                }
                it.moveNext();
            }

            return t;
        }

        // container to lua
        template<typename _T>
        void _pushtotable(lua_State* L, const _T& ret)
        {

            if constexpr(is_map_like<_T>::value)
            {
                stack_obj table_obj = stack_obj::new_table(L, 0, ret.size());

                for(auto it = ret.begin(); it != ret.end(); it++)
                {
                    push(L, it->first);
                    push(L, it->second);
                    lua_settable(L, table_obj._stack_pos);
                }
            }
            else
            {
                stack_obj table_obj = stack_obj::new_table(L, ret.size(), 0);

                auto it = ret.begin();
                for(int32_t i = 1; it != ret.end(); it++, i++)
                {
                    push(L, i);
                    push(L, *it);
                    lua_settable(L, table_obj._stack_pos);
                }
            }
        }

        // stl container T
        template<typename T>
        struct _stack_help<T,
                           typename std::enable_if<!std::is_reference<T>::value && !std::is_pointer<T>::value &&
                                                   is_container<base_type<T>>::value>::type>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_TABLE; }

            static T _read(lua_State* L, int32_t index)
            {
                if(lua_istable(L, index))
                    return _readfromtable<T>(L, index);
                else
                {
                    return _lua2type<T>(L, index);
                }
            }

            // template<typename _T>
            // static void _push(lua_State *L, _T&& val)
            //{
            //	return _pushtotable(L, std::forward<_T>(val));
            //}

            // static T _read(lua_State *L, int32_t index)
            //{
            //	return _lua2type<T>(L, index);
            //}

            template<typename _T>
            static void _push(lua_State* L, _T&& val)
            {
                return _type2lua(L, std::forward<T>(val));
            }
            template<typename _T>
            static void _push(lua_State* L, const _T& val)
            {
                return _type2lua(L, val);
            }
            template<typename _T>
            static void _push(lua_State* L, _T& val)
            {
                return _type2lua(L, val);
            }
        };

        template<typename... Args>
        struct _stack_help<std::tuple<Args...>>
        {
            using TupleType = std::tuple<Args...>;
            static constexpr int32_t cover_to_lua_type() { return CLT_TABLE; }

            template<typename T, std::size_t I>
            static T _read_tuple_element_fromtable_helper(lua_State* L, int32_t table_stack_pos)
            {
                T t;
                push(L, I);
                lua_gettable(L, table_stack_pos);
                t = read<T>(L, -1);

                return t;
            }

            template<typename Tuple, std::size_t... index>
            static Tuple _read_tuple_fromtable(lua_State* L, int32_t table_stack_pos, std::index_sequence<index...>)
            {
                return std::make_tuple(
                    _read_tuple_element_fromtable_helper<typename std::tuple_element<index, Tuple>::type, index + 1>(
                        L,
                        table_stack_pos)...);
            }

            template<typename Tuple>
            static Tuple _read_tuple_fromtable(lua_State* L, int32_t index)
            {
                stack_obj table_obj(L, index);
                if(table_obj.is_table() == false)
                {
                    call_error(L, "convert set from argument %d must be a table", index);
                    return {};
                }

                return _read_tuple_fromtable<Tuple>(L,
                                                    table_obj._stack_pos,
                                                    std::make_index_sequence<std::tuple_size<Tuple>::value>{});
            }

            static TupleType _read(lua_State* L, int32_t index)
            {
                if(lua_istable(L, index))
                    return _read_tuple_fromtable<TupleType>(L, index);
                else
                {
                    return _lua2type<TupleType>(L, index);
                }
            }

            template<typename Tuple>
            static void _push_tuple_totable_helper(lua_State* L,
                                                   int32_t    table_stack_pos,
                                                   Tuple&&    tuple,
                                                   std::index_sequence<>)
            {
            }

            template<typename Tuple, std::size_t first, std::size_t... is>
            static void _push_tuple_totable_helper(lua_State* L,
                                                   int32_t    table_stack_pos,
                                                   Tuple&&    tuple,
                                                   std::index_sequence<first, is...>)
            {
                push(L, first + 1); // lua table index from 1~x
                push(L, std::get<first>(std::forward<Tuple>(tuple)));
                lua_settable(L, table_stack_pos);
                _push_tuple_totable_helper(L,
                                           table_stack_pos,
                                           std::forward<Tuple>(tuple),
                                           std::index_sequence<is...>{});
            }

            static void _push(lua_State* L, TupleType&& tuple)
            {
                stack_obj        table_obj  = stack_obj::new_table(L, std::tuple_size<TupleType>(), 0);
                constexpr size_t tuple_size = std::tuple_size<TupleType>::value;
                _push_tuple_totable_helper(L,
                                           table_obj._stack_pos,
                                           std::forward<TupleType>(tuple),
                                           std::make_index_sequence<tuple_size>());
            }

            static void _push(lua_State* L, const TupleType& val) { return _type2lua(L, val); }

            static void _push(lua_State* L, TupleType& val) { return _type2lua(L, val); }
        };
        template<typename RVal, typename... Args>
        struct _stack_help<std::function<RVal(Args...)>>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_FUNCTION; }

            // func must be release before lua close.....user_conctrl
            static std::function<RVal(Args...)> _read(lua_State* L, int32_t index)
            {
                if(lua_isfunction(L, index) == false)
                {
                    call_error(L, "can't convert argument %d to function", index);
                    return {};
                }

                // copy idx to top
                lua_pushvalue(L, index);
                // make ref
                int32_t lua_callback = luaL_ref(L, LUA_REGISTRYINDEX);

                lua_function_ref callback_ref(L, lua_callback);
                
                return [callback_ref=std::move(callback_ref)](Args&& ... args)->RVal
                {
                    return callback_ref.invoke<RVal>(std::forward<Args>(args)...);
                };
            }

            static void _push(lua_State* L, std::function<RVal(Args...)>&& func)
            {
                _push_functor(L, std::forward<std::function<RVal(Args...)>>(func));
            }
            static void _push(lua_State* L, const std::function<RVal(Args...)>& func)
            {
                _push_functor(L, std::forward<std::function<RVal(Args...)>>(func));
            }
        };
        template<typename RVal, typename... Args>
        struct _stack_help<const std::function<RVal(Args...)>&> : public _stack_help<std::function<RVal(Args...)>>
        {
        };


        template<>
        struct _stack_help<lua_function_ref>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_FUNCTION; }
            // func must be release before lua close.....user_conctrl
            static lua_function_ref _read(lua_State* L, int32_t index)
            {
                if(lua_isfunction(L, index) == false)
                {
                    call_error(L, "can't convert argument %d to function", index);
                    return {};
                }

                // copy to top
                lua_pushvalue(L, index);
                // move top to ref
                int32_t lua_callback = luaL_ref(L, LUA_REGISTRYINDEX);

                lua_function_ref callback_ref(L, lua_callback);

                return callback_ref;
            }

            static void _push(lua_State* L, lua_function_ref&& func)
            {
                if(func.m_L != L)
                {
                    call_error(L, "lua_function was not create by the same lua_State");
                    return;
                }

                lua_rawgeti(func.m_L, LUA_REGISTRYINDEX, func.m_regidx);
            }
        };

        template<>
        struct _stack_help<const lua_function_ref&> : public _stack_help<lua_function_ref>
        {
        };

        template<typename T>
        struct _stack_help<std::shared_ptr<T>>
        {
            static constexpr int32_t cover_to_lua_type() { return CLT_USERDATA; }

            static std::shared_ptr<T> _read(lua_State* L, int32_t index)
            {
                if(!lua_isuserdata(L, index))
                {
                    call_error(L, "can't convert argument %d to class %s", index, get_class_name<T>());
                    return {};
                }

                UserDataWapper* pWapper = user2type<UserDataWapper*>(L, index);
                if(pWapper->isSharedPtr() == false)
                {
                    call_error(L, "can't convert argument %d to class %s", index, get_class_name<T>());
                    return {};
                }

#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
                if(pWapper->m_type_idx != get_type_idx<std::shared_ptr<T>>())
                {
                    // maybe derived to base
                    if(IsInherit(L, pWapper->m_type_idx, get_type_idx<std::shared_ptr<T>>()) == false)
                    {
                        call_error(L, "can't convert argument %d to class %s", index, get_class_name<T>());
                        return {};
                    }
                }
#endif
                if(pWapper->haveOwership())
                {
                    sharedptr2user<T>* pSharedWapper = static_cast<sharedptr2user<T>*>(pWapper);
                    return pSharedWapper->m_holder;
                }
                else
                {
                    weakptr2user<T>* pSharedWapper = static_cast<weakptr2user<T>*>(pWapper);
                    return pSharedWapper->m_holder.lock();
                }
            }
            // shared_ptr to lua
            static void _push(lua_State* L, std::shared_ptr<T>&& val)
            {
                if(val)
                {
                    if(val.use_count() == 1) // last count,if we didn't hold it, it will lost
                    {
                        new(lua_newuserdata(L, sizeof(sharedptr2user<T>))) sharedptr2user<T>(std::forward<std::shared_ptr<T>>(val));
                    }
                    else
                    {
                        new(lua_newuserdata(L, sizeof(weakptr2user<T>))) weakptr2user<T>(val);
                    }
                }
                else
                    lua_pushnil(L);

                push_meta(L, get_class_name<std::shared_ptr<T>>());
                lua_setmetatable(L, -2);
            }
        };
        template<typename T>
        struct _stack_help<const std::shared_ptr<T>&> : public _stack_help<std::shared_ptr<T>>
        {
            static void _push(lua_State* L, const std::shared_ptr<T>& val)
            {
                if(val)
                {
                    // if (val.use_count() == 1)	//last count,if we didn't hold it, it will lost
                    //{
                    //	new(lua_newuserdata(L, sizeof(sharedptr2user<T>))) sharedptr2user<T>(val);
                    //}
                    // else
                    {
                        new(lua_newuserdata(L, sizeof(weakptr2user<T>))) weakptr2user<T>(val);
                    }
                }
                else
                    lua_pushnil(L);

                push_meta(L, get_class_name<std::shared_ptr<T>>());
                lua_setmetatable(L, -2);
            }
        };

        // read_weap
        template<typename T>
        auto read(lua_State* L, int32_t index) -> decltype(_stack_help<T>::_read(L, index))
        {
#ifdef LUA_CALL_CFUNC_NEED_ALL_PARAM
            if constexpr(std::is_pointer<T>)
            {
                LUA_CHECK_HAVE_THIS_PARAM(L, index);
            }
            else
            {
                LUA_CHECK_HAVE_THIS_PARAM_AND_NOT_NIL(L, index);
            }
#endif
            return _stack_help<T>::_read(L, index);
        }

        // read_weap
        template<typename T>
        auto read_nocheck(lua_State* L, int32_t index) -> decltype(_stack_help<T>::_read(L, index))
        {
            return _stack_help<T>::_read(L, index);
        }

        // push warp
        template<typename T>
        void push(lua_State* L, T ret) // here need a T/T*/T& not a T&&
        {
            _stack_help<T>::_push(L, std::forward<T>(ret));
        }
        template<typename T>
        void push_rv(lua_State* L, T&& ret) // here need a T/T*/T& not a T&&
        {
            _stack_help<T>::_push(L, std::forward<T>(ret));
        }

        // invoke func tuple hold params
        // template<typename Func, typename Tup, std::size_t... index>
        // decltype(auto) invoke_helper(Func&& func, Tup&& tup, std::index_sequence<index...>)
        //{
        //	return std::invoke(func, std::get<index>(std::forward<Tup>(tup))...);
        //}

        // template<typename Func, typename Tup>
        // decltype(auto) invoke_func(Func&& func, Tup&& tup)
        //{
        //	constexpr auto Size = std::tuple_size<typename std::decay<Tup>::type>::value;
        //	return invoke_helper(std::forward<Func>(func),
        //		std::forward<Tup>(tup),
        //		std::make_index_sequence<Size>{});
        //}

        template<typename RVal, typename Func>
        RVal invoke_func(Func func)
        {
            return func();
        }

        // direct invoke func
        template<int32_t nIdxParams, typename RVal, typename Func, typename... Args, std::size_t... index>
        RVal direct_invoke_invoke_helper(Func&& func, lua_State* L, std::index_sequence<index...>)
        {
            return stdext::invoke(std::forward<Func>(func), read<Args>(L, index + nIdxParams)...);
        }

        template<int32_t nIdxParams, typename RVal, typename Func, typename... Args>
        RVal direct_invoke_func(Func&& func, lua_State* L)
        {
            return direct_invoke_invoke_helper<nIdxParams, RVal, Func, Args...>(std::forward<Func>(func),
                                                                                L,
                                                                                std::make_index_sequence<sizeof...(Args)>{});
        }

        template<int32_t nIdxParams, typename RVal, typename Func, typename CT, typename... Args, std::size_t... index>
        RVal direct_invoke_invoke_helper(Func&& func, lua_State* L, CT* pClassPtr, std::index_sequence<index...>)
        {
            return stdext::invoke(std::forward<Func>(func), pClassPtr, read<Args>(L, index + nIdxParams)...);
        }

        template<int32_t nIdxParams, typename RVal, typename Func, typename CT, typename... Args>
        RVal direct_invoke_member_func(Func&& func, lua_State* L, CT* pClassPtr)
        {
            return direct_invoke_invoke_helper<nIdxParams, RVal, Func, CT, Args...>(std::forward<Func>(func),
                                                                                    L,
                                                                                    pClassPtr,
                                                                                    std::make_index_sequence<sizeof...(Args)>{});
        }

        // make params to tuple
        // template<typename...T>
        // struct ParamHolder
        //{
        //	typedef std::tuple<typename std::remove_cv<typename std::remove_reference<T>::type>::type...> type;
        //};
        // template<typename...T>
        // using ParamHolder_T = typename ParamHolder<T...>::type;

        // template <int32_t nIdxParams, typename... T, std::size_t... N>
        // ParamHolder_T<T...> _get_args(lua_State *L, std::index_sequence<N...>)
        //{
        //	return std::forward<ParamHolder_T<T...>>(ParamHolder_T<T...>{ read<T>(L, N + nIdxParams)... });
        //}

        // template <int32_t nIdxParams, typename... T>
        // ParamHolder_T<T...> _get_args(lua_State *L)
        //{
        //	constexpr std::size_t num_args = sizeof...(T);
        //	return _get_args<nIdxParams, T...>(L, std::make_index_sequence<num_args>());
        //}

        bool CheckSameMetaTable(lua_State* L, int32_t nIndex, const char* tname);

        template<typename T, bool bConstMemberFunc>
        T* _read_classptr_from_index1(lua_State* L)
        {
            // index 1 must be userdata
            UserDataWapper* pWapper = user2type<UserDataWapper*>(L, 1);
            if(pWapper == nullptr)
            {
                call_error(L,
                                "call member func must need a class_ptr, plz use %s:func instead %s.func",
                                get_class_name<T>(),
                                get_class_name<T>());
                return nullptr;
            }

#ifdef _ALLOW_SHAREDPTR_INVOKE
            if(pWapper->isSharedPtr())
            {
                // try covert shared_ptr<T> to T*, don't need check type_idx because call invoke,must be obj:func(), use
                // matatable __parent
                if(pWapper->haveOwership())
                {
                    sharedptr2user<T>* pSharedWapper = static_cast<sharedptr2user<T>*>(pWapper);
                    return pSharedWapper->m_holder.get();
                }
                else
                {
                    weakptr2user<T>* pSharedWapper = static_cast<weakptr2user<T>*>(pWapper);
                    return pSharedWapper->m_holder.lock().get();
                }
            }
            else
#endif
            {
#ifdef LUATINKER_USERDATA_CHECK_CONST
                if(pWapper->is_const() == true && bConstMemberFunc == false)
                {
                    call_error(L, "const class_ptr %s can't invoke non-const member func.", get_class_name<T>());
                    return return_empty<_T>();
                }
#endif
                return void2type<T*>(pWapper->m_p);
            }
        }

        // upval to stack helper
        bool push_upval_to_stack(lua_State* L, int32_t nArgsCount, int32_t nArgsNeed, int32_t default_upval_start = 2);
        bool push_upval_to_stack(lua_State* L, int32_t nArgsCount, int32_t nArgsNeed, int32_t nUpvalCount, int32_t UpvalStart);
        // functor
        struct functor_base
        {
            virtual ~functor_base() {}
            virtual int32_t apply(lua_State* L) = 0;
        };

        template<bool bConst, typename CT, typename RVal, typename... Args>
        struct member_functor : public functor_base
        {
            using FuncType = RVal (CT::*)(Args...);
            typedef std::function<RVal(CT*, Args...)> FunctionType;
            FunctionType                              m_func;

            int32_t m_nDefaultParamCount  = 0;
            int32_t m_nDefaultParamsStart = 0;
            member_functor(const FunctionType& func, int32_t nDefaultParamCount = 0, int32_t nDefaultParamStart = 0)
                : m_func(func)
                , m_nDefaultParamCount(nDefaultParamCount)
                , m_nDefaultParamsStart(nDefaultParamStart)
            {
            }
            member_functor(FunctionType&& func, int32_t nDefaultParamCount = 0, int32_t nDefaultParamStart = 0)
                : m_func(std::move(func))
                , m_nDefaultParamCount(nDefaultParamCount)
                , m_nDefaultParamsStart(nDefaultParamStart)
            {
            }

            ~member_functor() {}

            int32_t getDefaultArgsNum() { return m_nDefaultParamCount; }

            virtual int32_t apply(lua_State* L) override
            {
                CHECK_CLASS_PTR(CT);
                TRY_LUA_TINKER_INVOKE()
                {
                    size_t           nArgsCount = lua_gettop(L) - 1;
                    constexpr size_t nArgsNeed  = sizeof...(Args);
                    push_upval_to_stack(L, nArgsCount, nArgsNeed, m_nDefaultParamCount, m_nDefaultParamsStart);
                    _invoke_function<RVal>(L, m_func, _read_classptr_from_index1<CT, bConst>(L));
                    return 1;
                }
                CATCH_LUA_TINKER_INVOKE()
                {
                    call_error(L, "lua fail to invoke functor");
                }
                return 0;
            }

            static int32_t invoke(lua_State* L)
            {
                CHECK_CLASS_PTR(CT);
                TRY_LUA_TINKER_INVOKE()
                {
                    size_t           nArgsCount = lua_gettop(L) - 1;
                    constexpr size_t nArgsNeed  = sizeof...(Args);
                    push_upval_to_stack(L, nArgsCount, nArgsNeed);
                    _invoke<RVal>(L, upvalue_<FuncType>(L), _read_classptr_from_index1<CT, bConst>(L));
                    return 1;
                }
                CATCH_LUA_TINKER_INVOKE()
                {
                    call_error(L, "lua fail to invoke functor");
                }
                return 0;
            }

            template<typename T>
            static void _invoke(lua_State* L, FuncType&& func, CT* pClassPtr)
            {
                if constexpr(!std::is_void<T>::value)
                {

                    push_rv<RVal>(L, direct_invoke_member_func<2, RVal, FuncType, CT, Args...>(std::forward<FuncType>(func), L, pClassPtr));
                }
                else
                    direct_invoke_member_func<2, RVal, FuncType, CT, Args...>(std::forward<FuncType>(func), L, pClassPtr);
            }

            static int32_t invoke_function(lua_State* L)
            {
                CHECK_CLASS_PTR(CT);
                TRY_LUA_TINKER_INVOKE()
                {
                    using FuncWarpType          = member_functor<bConst, CT, RVal, Args...>;
                    size_t           nArgsCount = lua_gettop(L) - 1;
                    constexpr size_t nArgsNeed  = sizeof...(Args);
                    push_upval_to_stack(L, nArgsCount, nArgsNeed);
                    _invoke_function<RVal>(L, upvalue_<FuncWarpType*>(L)->m_pfunc, _read_classptr_from_index1<CT, bConst>(L));
                    return 1;
                }
                CATCH_LUA_TINKER_INVOKE()
                {
                    call_error(L, "lua fail to invoke functor");
                }
                return 0;
            }

            template<typename T, typename F>
            static void _invoke_function(lua_State* L, F&& func, CT* pClassPtr)
            {
                if constexpr(!std::is_void<T>::value)
                {
                    push_rv<RVal>(L, direct_invoke_member_func<2, RVal, FunctionType, CT, Args...>(std::forward<FunctionType>(func), L, pClassPtr));
                }
                else
                    direct_invoke_member_func<2, RVal, FunctionType, CT, Args...>(std::forward<FunctionType>(func), L, pClassPtr);
            }
        };

        template<typename RVal, typename... Args>
        struct functor : public functor_base
        {
            using FuncType = RVal (*)(Args...);
            typedef std::function<RVal(Args...)> FunctionType;
            using FuncWarpType = functor<RVal, Args...>;

            FunctionType m_func;

            int32_t m_nDefaultParamCount  = 0;
            int32_t m_nDefaultParamsStart = 0;
            functor(const FunctionType& func, int32_t nDefaultParamCount = 0, int32_t nDefaultParamStart = 0)
                : m_func(func)
                , m_nDefaultParamCount(nDefaultParamCount)
                , m_nDefaultParamsStart(nDefaultParamStart)
            {
            }
            functor(FunctionType&& func, int32_t nDefaultParamCount = 0, int32_t nDefaultParamStart = 0)
                : m_func(std::move(func))
                , m_nDefaultParamCount(nDefaultParamCount)
                , m_nDefaultParamsStart(nDefaultParamStart)
            {
            }

            ~functor() {}

            int32_t getDefaultArgsNum() { return m_nDefaultParamCount; }

            virtual int32_t apply(lua_State* L) override
            {
                TRY_LUA_TINKER_INVOKE()
                {
                    size_t           nArgsCount = lua_gettop(L);
                    constexpr size_t nArgsNeed  = sizeof...(Args);
                    push_upval_to_stack(L, nArgsCount, nArgsNeed, m_nDefaultParamCount, m_nDefaultParamsStart);
                    _invoke_function<RVal>(L, m_func);
                    return 1;
                }
                CATCH_LUA_TINKER_INVOKE()
                {
                    call_error(L, "lua fail to invoke functor");
                }
                return 0;
            }

            static int32_t invoke(lua_State* L)
            {
                TRY_LUA_TINKER_INVOKE()
                {
                    size_t           nArgsCount = lua_gettop(L);
                    constexpr size_t nArgsNeed  = sizeof...(Args);
                    push_upval_to_stack(L, nArgsCount, nArgsNeed);
                    _invoke<RVal>(L);
                    return 1;
                }
                CATCH_LUA_TINKER_INVOKE()
                {
                    call_error(L, "lua fail to invoke functor");
                }
                return 0;
            }

            template<typename T>
            static void _invoke(lua_State* L)
            {
                if constexpr(!std::is_void<T>::value)
                {
                    push_rv<RVal>(L, direct_invoke_func<1, RVal, FuncType, Args...>(std::forward<FuncType>(upvalue_<FuncType>(L)), L));
                }
                else
                    direct_invoke_func<1, RVal, FuncType, Args...>(std::forward<FuncType>(upvalue_<FuncType>(L)), L);
            }

            static int32_t invoke_function(lua_State* L)
            {
                TRY_LUA_TINKER_INVOKE()
                {
                    using FuncWarpType          = functor<RVal, Args...>;
                    FuncWarpType*    pFuncWarp  = upvalue_<FuncWarpType*>(L);
                    size_t           nArgsCount = lua_gettop(L);
                    constexpr size_t nArgsNeed  = sizeof...(Args);
                    push_upval_to_stack(L, nArgsCount, nArgsNeed);
                    _invoke_function<RVal>(L, pFuncWarp->m_func);
                    return 1;
                }
                CATCH_LUA_TINKER_INVOKE()
                {
                    call_error(L, "lua fail to invoke functor");
                }
                return 0;
            }

            template<typename T, typename F>
            static void _invoke_function(lua_State* L, F&& func)
            {
                if constexpr(!std::is_void<T>::value)
                {
                    push_rv<RVal>(L, direct_invoke_func<1, RVal, FunctionType, Args...>(std::forward<FunctionType>(func), L));
                }
                else
                    direct_invoke_func<1, RVal, FunctionType, Args...>(std::forward<FunctionType>(func), L);
            }
        };

        // destroyer
        template<typename T>
        int32_t destroyer(lua_State* L)
        {
            ((T*)lua_touserdata(L, 1))->~T();
            return 0;
        }

        template<typename T, typename... DEFAULT_ARGS>
        void _push_functor_invoke(lua_State* L, int32_t upval_num, T&& t, DEFAULT_ARGS&&... default_args)
        {
            constexpr size_t size_args = sizeof...(default_args);
            push<int32_t>(L, size_args);
            push_args(L, std::forward<DEFAULT_ARGS>(default_args)...);
            lua_pushcclosure(L, std::forward<T>(t), upval_num + 1 + size_args);
        }

        template<typename T>
        void _push_functor_invoke(lua_State* L, int32_t upval_num, T&& t)
        {
            lua_pushcclosure(L, std::forward<T>(t), upval_num);
        }

        // global function
        template<typename R, typename... ARGS, typename... DEFAULT_ARGS>
        void _push_functor(lua_State* L, R(func)(ARGS...), DEFAULT_ARGS&&... default_args)
        {
            using Functor_Warp = functor<R, ARGS...>;
            lua_pushlightuserdata(L, (void*)func);
            _push_functor_invoke(L, 1, &Functor_Warp::invoke, std::forward<DEFAULT_ARGS>(default_args)...);
        }

        template<typename R, typename... ARGS, typename... DEFAULT_ARGS>
        void _push_functor(lua_State* L, const std::function<R(ARGS...)>& func, DEFAULT_ARGS&&... default_args)
        {
            using Functor_Warp = functor<R, ARGS...>;
            new(lua_newuserdata(L, sizeof(Functor_Warp))) Functor_Warp(func);
            // register functor
            {
                lua_createtable(L, 0, 1);
                lua_pushstring(L, "__gc");
                lua_pushcclosure(L, &destroyer<Functor_Warp>, 0);
                lua_rawset(L, -3);
                lua_setmetatable(L, -2);
            }
            _push_functor_invoke(L, 1, &Functor_Warp::invoke_function, std::forward<DEFAULT_ARGS>(default_args)...);
        }

        template<typename R, typename... ARGS, typename... DEFAULT_ARGS>
        void _push_functor(lua_State* L, std::function<R(ARGS...)>&& func, DEFAULT_ARGS&&... default_args)
        {
            using Functor_Warp = functor<R, ARGS...>;
            new(lua_newuserdata(L, sizeof(Functor_Warp))) Functor_Warp(func);
            // register functor
            {
                lua_createtable(L, 0, 1);
                lua_pushstring(L, "__gc");
                lua_pushcclosure(L, &destroyer<Functor_Warp>, 0);
                lua_rawset(L, -3);
                lua_setmetatable(L, -2);
            }
            _push_functor_invoke(L, 1, &Functor_Warp::invoke_function, std::forward<DEFAULT_ARGS>(default_args)...);
        }

        template<typename overload_functor, typename... DEFAULT_ARGS>
        auto _push_functor(lua_State* L, overload_functor&& functor, DEFAULT_ARGS&&... default_args) ->
            typename std::enable_if<std::is_base_of<args_type_overload_functor_base, overload_functor>::value, void>::type
        {
            new(lua_newuserdata(L, sizeof(overload_functor))) overload_functor(std::forward<overload_functor>(functor));
            // register functor
            {
                lua_createtable(L, 0, 1);
                lua_pushstring(L, "__gc");
                lua_pushcclosure(L, &destroyer<overload_functor>, 0);
                lua_rawset(L, -3);
                lua_setmetatable(L, -2);
            }
            _push_functor_invoke(L, 1, &overload_functor::invoke_function, std::forward<DEFAULT_ARGS>(default_args)...);
        }

        template<typename F, typename... DefaultArgs>
        void _push_constructor(lua_State* L, F&& f, DefaultArgs&&... default_args)
        {
            if constexpr(!std::is_base_of<args_type_overload_functor_base, F>::value)
                _push_functor_invoke(L, 0, std::forward<F>(f), std::forward<DefaultArgs>(default_args)...);
            else
                _push_functor(L, std::forward<F>(f), std::forward<DefaultArgs>(default_args)...);
        }

    } // namespace detail

    // constructor
    template<typename T, typename... Args>
    struct constructor : public detail::functor_base
    {
        int32_t m_nDefaultParamCount  = 0;
        int32_t m_nDefaultParamsStart = 0;
        constructor(int32_t nDefaultParamCount = 0, int32_t nDefaultParamStart = 0)
            : m_nDefaultParamCount(nDefaultParamCount)
            , m_nDefaultParamsStart(nDefaultParamStart)
        {
        }

        int32_t getDefaultArgsNum() { return m_nDefaultParamCount; }

        virtual int32_t apply(lua_State* L) override
        {
            TRY_LUA_TINKER_INVOKE()
            {
                detail::push_upval_to_stack(L, lua_gettop(L) - 1, sizeof...(Args), m_nDefaultParamCount, m_nDefaultParamsStart);
                _invoke(L);
                return 1;
            }
            CATCH_LUA_TINKER_INVOKE()
            {
                call_error(L, "lua fail to invoke constructor");
            }
            return 0;
        }

        static int32_t invoke(lua_State* L)
        {
            TRY_LUA_TINKER_INVOKE()
            {
                detail::push_upval_to_stack(L, lua_gettop(L) - 1, sizeof...(Args), 1);
                _invoke(L);
                return 1;
            }
            CATCH_LUA_TINKER_INVOKE()
            {
                call_error(L, "lua fail to invoke constructor");
            }
            return 0;
        }

        static int32_t _invoke(lua_State* L)
        {
            new(lua_newuserdata(L, sizeof(detail::val2user<T>))) detail::val2user<T>(L, detail::class_tag<Args...>());
            detail::push_meta(L, detail::get_class_name<T>());
            lua_setmetatable(L, -2);

            return 1;
        }
    };

    template<typename Func, typename... DefaultArgs>
    void def(lua_State* L, const char* name, Func&& func, DefaultArgs&&... default_args)
    {
        detail::_push_functor(L, std::forward<Func>(func), std::forward<DefaultArgs>(default_args)...);
        lua_setglobal(L, name);
    }

    // global variable
    template<typename T>
    void set(lua_State* L, const char* name, T&& object)
    {
        detail::push(L, std::forward<T>(object));
        lua_setglobal(L, name);
    }

    template<typename T>
    T get(lua_State* L, const char* name)
    {
        lua_getglobal(L, name);
        return detail::pop<T>::apply(L);
    }

    template<typename T>
    void decl(lua_State* L, const char* name, T&& object)
    {
        set(L, name, std::forward<T>(object));
    }

    template<typename RVal>
    RVal dofile(lua_State* L, const char* filename)
    {

        if(luaL_loadfile(L, filename) != 0)
        {
            print_error(L, "%s", lua_tostring(L, -1));
            return detail::pop_nil<RVal>(L);
        }
        return call_stackfunc<RVal>(L);
    }

    template<typename RVal>
    RVal dostring(lua_State* L, const std::string& str, const char* name)
    {
        return dobuffer<RVal>(L, str.c_str(), str.size(), name);
    }

    template<typename RVal>
    RVal dobuffer(lua_State* L, const char* buff, size_t sz, const char* name)
    {
        if(luaL_loadbuffer(L, buff, sz, name?name:"lua_tinker::dobuffer") != 0)
        {
            print_error(L, "%s in buffer[%s]", lua_tostring(L, -1), buff);
            return detail::pop_nil<RVal>(L);
        }
        return call_stackfunc<RVal>(L);
    }

    // call lua func
    template<typename RVal, typename... Args>
    RVal call(lua_State* L, const char* name, Args&&... arg)
    {
        lua_getglobal(L, name);
        if(lua_isfunction(L, -1) == false)
        {
            lua_pop(L, 1); // pop func
            print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
            return detail::pop_nil<RVal>(L);
        }
        else
        {
            return call_stackfunc<RVal>(L, std::forward<Args>(arg)...);
        }
    }

    template<typename RVal, typename... Args>
    RVal call_stackfunc(lua_State* L, Args&&... arg)
    {
        // top = func
        lua_getglobal(L, ERROR_CALLBACK_NAME);
        // top = errfunc | func
        int32_t errfunc = lua_gettop(L) - 1;
        lua_insert(L, errfunc);
        // top = func | errfunc

        detail::push_args(L, std::forward<Args>(arg)...);

        if(lua_pcall(L, sizeof...(Args), detail::pop<RVal>::nresult, errfunc) != LUA_OK)
        {
            // stack have a nil string from on_error
            if constexpr(detail::pop<RVal>::nresult == 0)
            {
                // not need it, pop
                lua_pop(L, 1);
            }
            else if constexpr(detail::pop<RVal>::nresult > 1)
            {
                // push nil to pop result
                for(int32_t i = 0; i < detail::pop<RVal>::nresult - 1; i++)
                {
                    lua_pushnil(L);
                };
            }
            else
            {
                //==1, leave it for pop resuslt
            }
        }

        lua_remove(L, errfunc);
        return detail::pop<RVal>::apply(L);
    }

    namespace detail
    {
        // allow register a lua_CFuntion to control class
        inline void _push_class_functor(lua_State* L, lua_CFunction func) { lua_pushcclosure(L, func, 0); }

        template<typename T, typename R, typename... ARGS, typename... DEFAULT_ARGS>
        void _push_class_functor(lua_State* L, R (T::*func)(ARGS...), DEFAULT_ARGS&&... default_args)
        {
            using Functor_Warp = member_functor<false, T, R, ARGS...>;
            using FunctionType = R (T::*)(ARGS...);
            // register functor
            new(lua_newuserdata(L, sizeof(FunctionType))) FunctionType(func);
            _push_functor_invoke(L, 1, &Functor_Warp::invoke, std::forward<DEFAULT_ARGS>(default_args)...);
        }

        template<typename T, typename R, typename... ARGS, typename... DEFAULT_ARGS>
        void _push_class_functor(lua_State* L, R (T::*func)(ARGS...) const, DEFAULT_ARGS&&... default_args)
        {
            using Functor_Warp = member_functor<true, T, R, ARGS...>;
            using FunctionType = R (T::*)(ARGS...) const;
            // register functor
            new(lua_newuserdata(L, sizeof(FunctionType))) FunctionType(func);
            _push_functor_invoke(L, 1, &Functor_Warp::invoke, std::forward<DEFAULT_ARGS>(default_args)...);
        }

        template<typename T, typename R, typename... ARGS, typename... DEFAULT_ARGS>
        void _push_class_functor(lua_State* L, const std::function<R(T*, ARGS...)>& func, DEFAULT_ARGS&&... default_args)
        {
            using Functor_Warp = member_functor<false, T, R, ARGS...>;

            new(lua_newuserdata(L, sizeof(Functor_Warp))) Functor_Warp(func);
            // register metatable for gc
            {
                lua_createtable(L, 0, 1);
                lua_pushstring(L, "__gc");
                lua_pushcclosure(L, &destroyer<Functor_Warp>, 0);
                lua_rawset(L, -3);

                lua_setmetatable(L, -2);
            }

            _push_functor_invoke(L, 1, &Functor_Warp::invoke_function, std::forward<DEFAULT_ARGS>(default_args)...);
        }

        template<typename T, typename R, typename... ARGS, typename... DEFAULT_ARGS>
        void _push_class_functor(lua_State* L, std::function<R(T*, ARGS...)>&& func, DEFAULT_ARGS&&... default_args)
        {
            using Functor_Warp = member_functor<false, T, R, ARGS...>;

            new(lua_newuserdata(L, sizeof(Functor_Warp))) Functor_Warp(func);
            // register metatable for gc
            {
                lua_createtable(L, 0, 1);
                lua_pushstring(L, "__gc");
                lua_pushcclosure(L, &destroyer<Functor_Warp>, 0);
                lua_rawset(L, -3);

                lua_setmetatable(L, -2);
            }

            _push_functor_invoke(L, 1, &Functor_Warp::invoke_function, std::forward<DEFAULT_ARGS>(default_args)...);
        }

        template<typename overload_functor, typename... DEFAULT_ARGS>
        auto _push_class_functor(lua_State* L, overload_functor&& functor, DEFAULT_ARGS&&... default_args) ->
            typename std::enable_if<std::is_base_of<args_type_overload_functor_base, overload_functor>::value, void>::type
        {
            new(lua_newuserdata(L, sizeof(overload_functor))) overload_functor(std::forward<overload_functor>(functor));
            // register metatable for gc
            {
                lua_createtable(L, 0, 1);
                lua_pushstring(L, "__gc");
                lua_pushcclosure(L, &destroyer<overload_functor>, 0);
                lua_rawset(L, -3);

                lua_setmetatable(L, -2);
            }
            _push_functor_invoke(L, 1, &overload_functor::invoke_function, std::forward<DEFAULT_ARGS>(default_args)...);
        }

        template<typename T>
        int32_t _get_raw_ptr(lua_State* L)
        {
            std::shared_ptr<T> ptrT = read<std::shared_ptr<T>>(L, 1);
            push_rv<T*>(L, ptrT.get());
            return 1;
        }
    }; // namespace detail

    // namespace func
    template<typename Func, typename... DefaultArgs>
    void namespace_def(lua_State* L, const char* namespace_name, const char* name, Func&& func, DefaultArgs&&... default_args)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, namespace_name) == LUA_TTABLE)
        {
            // register functor
            lua_pushstring(L, name);
            _push_functor(L, std::forward<Func>(func), std::forward<DefaultArgs>(default_args)...);
            lua_rawset(L, -3);
        }
    }

    template<typename T>
    void namespace_set(lua_State* L, const char* namespace_name, const char* name, T&& object)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, namespace_name) == LUA_TTABLE)
        {
            // register functor
            lua_pushstring(L, name);
            push(L, std::forward<T>(object));
            lua_rawset(L, -3);
        }
    }

    template<typename T>
    T namespace_get(lua_State* L, const char* namespace_name, const char* name)
    {
        using namespace detail;
        push_meta(L, namespace_name);
        stack_obj namespace_meta = stack_obj::get_top(L);
        if(namespace_meta.is_table())
        {
            // register functor
            namespace_meta.rawget(name);
        }
        namespace_meta.remove();

        return pop<T>::apply(L);
    }

    // class alias
    template<typename T, typename Alias>
    void class_alias(lua_State* L, const char* name)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, get_class_name<T>()) != LUA_TTABLE)
        {
            return;
        }

        class_name<Alias>::name(name);
        lua_setglobal(L, name);

        if(push_meta(L, get_class_name<std::shared_ptr<T>>()) == LUA_TTABLE)
        {
            std::string strSharedName = (std::string(name) + S_SHARED_PTR_NAME);
            class_name<std::shared_ptr<Alias>>::name(strSharedName.c_str());
            lua_setglobal(L, strSharedName.c_str());
        }
    }

    // class init
    template<typename T>
    void class_add(lua_State* L, const char* name, bool bInitShared)
    {
        detail::class_name<T>::name(name);
        lua_createtable(L, 0, 4);

        lua_pushstring(L, "__name");
        lua_pushstring(L, name);
        lua_rawset(L, -3);

        lua_pushstring(L, "__index");
        lua_pushcclosure(L, detail::meta_get, 0);
        lua_rawset(L, -3);

        lua_pushstring(L, "__newindex");
        lua_pushcclosure(L, detail::meta_set, 0);
        lua_rawset(L, -3);

        lua_pushstring(L, "__gc");
        lua_pushcclosure(L, detail::destroyer<detail::UserDataWapper>, 0);
        lua_rawset(L, -3);

        lua_setglobal(L, name);

        if(bInitShared)
        {
            class_add_shared<T>(L, name);
        }
    }

    // class init
    template<typename T>
    void class_add_shared(lua_State* L, const char* name)
    {
        std::string strSharedName = (std::string(name) + S_SHARED_PTR_NAME);
        detail::class_name<std::shared_ptr<T>>::name(strSharedName.c_str());
        int32_t nReserveSize = 3;

#ifdef _ALLOW_SHAREDPTR_INVOKE
        nReserveSize = 6;
#endif

        lua_createtable(L, 0, nReserveSize);
        lua_pushstring(L, "__name");
        lua_pushstring(L, strSharedName.c_str());
        lua_rawset(L, -3);

        lua_pushstring(L, "__gc");
        lua_pushcclosure(L, detail::destroyer<detail::UserDataWapper>, 0);
        lua_rawset(L, -3);

#ifdef _ALLOW_SHAREDPTR_INVOKE
        lua_pushstring(L, "__index");
        lua_pushcclosure(L, detail::meta_get, 0);
        lua_rawset(L, -3);

        lua_pushstring(L, "__newindex");
        lua_pushcclosure(L, detail::meta_set, 0);
        lua_rawset(L, -3);

        lua_pushstring(L, "__parent");
        detail::push_meta(L, name);
        lua_rawset(L, -3);
#endif
        { // register _get_raw_ptr func
            lua_pushstring(L, "_get_raw_ptr");
            lua_pushcclosure(L, &detail::_get_raw_ptr<T>, 0);
            lua_rawset(L, -3);
        }

        lua_setglobal(L, strSharedName.c_str());
    }

    // Tinker Class Inheritance
    template<typename T, typename P>
    void class_inh(lua_State* L)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, get_class_name<T>()) == LUA_TTABLE)
        {
            stack_obj class_meta = stack_obj::get_top(L);
#ifndef LUATINKER_MULTI_INHERITANCE
            lua_pushstring(L, "__parent");
            push_meta(L, get_class_name<P>());
            class_meta.rawset(); // set class_meta["__parent"] = __parent
#else
            stack_obj parent_table = class_meta.rawget("__parent");
            if(parent_table.is_nil())
            {
                parent_table.remove();
                lua_pushstring(L, "__parent");
                push_meta(L, get_class_name<P>());
                class_meta.rawset(); // set class_meta["__parent"] = __parent
            }
            else
            {
                stack_obj parent_table = class_meta.rawget("__multi_parent");
                if(parent_table.is_nil())
                {
                    parent_table.remove();
                    lua_pushstring(L, "__multi_parent");
                    lua_createtable(L, 1, 0);
                    parent_table = stack_obj::get_top(L);
                    {
                        push_meta(L, get_class_name<P>());
                        parent_table.rawseti(1); // set __multi_parent[1]=table
                    }
                    class_meta.rawset(); // set class_meta["__multi_parent"] = __multi_parent
                }
                else
                {
                    int32_t nLen = parent_table.get_rawlen() + 1;
                    push_meta(L, get_class_name<P>());
                    parent_table.rawseti(nLen); // set __multi_parent[n]=table
                    parent_table.remove();      // pop __multi_parent table
                }
            }
#endif
        }

#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
        // add inheritance map
        detail::addInheritMap<T, P>(L);
#endif
    }

    template<typename T, typename C>
    void class_inner(lua_State* L, const char* name)
    {
        scope_inner(L, detail::get_class_name<T>(), name, detail::get_class_name<C>());
    }

    // Tinker Class Constructor
    template<typename T, typename F, typename... DefaultArgs>
    void class_con(lua_State* L, F&& func, DefaultArgs&&... default_args)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, get_class_name<T>()) == LUA_TTABLE)
        {
            lua_createtable(L, 0, 1);
            lua_pushstring(L, "__call");
            _push_constructor(L, std::forward<F>(func), std::forward<DefaultArgs>(default_args)...);
            lua_rawset(L, -3);
            lua_setmetatable(L, -2);
        }
    }

    // Tinker Class Functions
    template<typename T, typename Func, typename... DefaultArgs>
    void class_def(lua_State* L, const char* name, Func&& func, DefaultArgs&&... default_args)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, get_class_name<T>()) == LUA_TTABLE)
        {
            // register functor
            lua_pushstring(L, name);
            _push_class_functor(L, std::forward<Func>(func), std::forward<DefaultArgs>(default_args)...);
            lua_rawset(L, -3);
        }
    }
    template<typename T, typename Func, typename... DefaultArgs>
    void class_def_static(lua_State* L, const char* name, Func&& func, DefaultArgs&&... default_args)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, get_class_name<T>()) == LUA_TTABLE)
        {
            // register functor
            lua_pushstring(L, name);
            _push_functor(L, std::forward<Func>(func), std::forward<DefaultArgs>(default_args)...);
            lua_rawset(L, -3);
        }
    }

    namespace detail
    {
        // member variable
        struct var_base
        {
            virtual ~var_base(){};
            virtual void get(lua_State* L) = 0;
            virtual void set(lua_State* L) = 0;
        };

        template<typename T, typename V>
        struct mem_var : var_base
        {
            V T::*_var;
            mem_var(V T::*val)
                : _var(val)
            {
            }
            virtual void get(lua_State* L) override
            {
                CHECK_CLASS_PTR(T);
                push(L, _read_classptr_from_index1<T, true>(L)->*(_var));
            }
            virtual void set(lua_State* L) override
            {
                CHECK_CLASS_PTR(T);
                _read_classptr_from_index1<T, false>(L)->*(_var) = read<V>(L, 3);
            }
        };

        template<typename T, typename V>
        struct mem_readonly_var : var_base
        {
            V T::*_var;
            mem_readonly_var(V T::*val)
                : _var(val)
            {
            }
            virtual void get(lua_State* L) override
            {
                CHECK_CLASS_PTR(T);
                push(L, _read_classptr_from_index1<T, true>(L)->*(_var));
            }
            virtual void set(lua_State* L) override
            {
                call_error(L,"member is readonly.");
            }
        };

        template<typename V>
        struct static_mem_var : var_base
        {
            V* _var;
            static_mem_var(V* val)
                : _var(val)
            {
            }
            virtual void get(lua_State* L) override { push(L, *(_var)); }
            virtual void set(lua_State* L) override { *(_var) = read<V>(L, 3); }
        };

        template<typename V>
        struct static_readonly_mem_var : var_base
        {
            V* _var;
            static_readonly_mem_var(V* val)
                : _var(val)
            {
            }
            virtual void get(lua_State* L) override { push(L, *(_var)); }
            virtual void set(lua_State* L) override
            {
                call_error(L, "static_member is readonly.");
            }
        };

        template<typename T, typename GET_FUNC, typename SET_FUNC>
        struct member_property : var_base
        {
            GET_FUNC m_get_func = nullptr;
            SET_FUNC m_set_func = nullptr;
            member_property(GET_FUNC get_func, SET_FUNC set_func)
                : m_get_func(get_func)
                , m_set_func(set_func)
            {
            }

            virtual void get(lua_State* L) override { _get<GET_FUNC>(L); }

            virtual void set(lua_State* L) override { _set<SET_FUNC>(L); }

            template<typename FUNC>
            void _get(lua_State* L)
            {
                if constexpr(!std::is_null_pointer<FUNC>::value)
                {
                    CHECK_CLASS_PTR(T);
                    push(L, (_read_classptr_from_index1<T, true>(L)->*m_get_func)());
                }
                else
                {
                    call_error(L, "property didn't have get_func");
                }
            }

            template<typename FUNC>
            void _set(lua_State* L)
            {
                if constexpr(!std::is_null_pointer<FUNC>::value)
                {
                    CHECK_CLASS_PTR(T);
                    (_read_classptr_from_index1<T, false>(L)->*m_set_func)(read<typename function_traits<SET_FUNC>::template argv<0>::type>(L, 3));
                }
                else
                {
                    call_error(L, "property didn't have set_func");
                }
            }
        };
    } // namespace detail

    // Tinker Class Variables
    template<typename T, typename BASE, typename VAR>
    void class_mem(lua_State* L, const char* name, VAR BASE::*val)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, get_class_name<T>()) == LUA_TTABLE)
        {
            lua_pushstring(L, name);
            new(lua_newuserdata(L, sizeof(mem_var<BASE, VAR>))) mem_var<BASE, VAR>(val);
            lua_rawset(L, -3);
        }
    }

    // Tinker Class Variables
    template<typename T, typename BASE, typename VAR>
    void class_mem_readonly(lua_State* L, const char* name, VAR BASE::*val)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, get_class_name<T>()) == LUA_TTABLE)
        {
            lua_pushstring(L, name);
            new(lua_newuserdata(L, sizeof(mem_readonly_var<BASE, VAR>))) mem_readonly_var<BASE, VAR>(val);
            lua_rawset(L, -3);
        }
    }

    // Tinker Class Variables
    template<typename T, typename VAR>
    void class_mem_static(lua_State* L, const char* name, VAR* val)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, get_class_name<T>()) == LUA_TTABLE)
        {
            lua_pushstring(L, name);
            new(lua_newuserdata(L, sizeof(static_mem_var<VAR>))) static_mem_var<VAR>(val);
            lua_rawset(L, -3);
        }
    }

    template<typename T, typename VAR>
    void class_mem_static_readonly(lua_State* L, const char* name, VAR* val)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, get_class_name<T>()) == LUA_TTABLE)
        {
            lua_pushstring(L, name);
            new(lua_newuserdata(L, sizeof(static_readonly_mem_var<VAR>))) static_readonly_mem_var<VAR>(val);
            lua_rawset(L, -3);
        }
    }

    template<typename T, typename VAR>
    void class_var_static(lua_State* L, const char* name, VAR&& val)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, get_class_name<T>()) == LUA_TTABLE)
        {
            lua_pushstring(L, name);
            push(L, std::forward<VAR>(val));
            lua_rawset(L, -3);
        }
    }

    // Tinker Class Variables
    template<typename T, typename GET_FUNC, typename SET_FUNC>
    void class_property(lua_State* L, const char* name, GET_FUNC&& get_func, SET_FUNC&& set_func)
    {
        using namespace detail;
        stack_scope_exit scope_exit(L);
        if(push_meta(L, get_class_name<T>()) == LUA_TTABLE)
        {
            lua_pushstring(L, name);
            new(lua_newuserdata(L, sizeof(member_property<T, GET_FUNC, SET_FUNC>)))
                member_property<T, GET_FUNC, SET_FUNC>(std::forward<GET_FUNC>(get_func), std::forward<SET_FUNC>(set_func));
            lua_rawset(L, -3);
        }
    };

    namespace detail
    {
        // Table Object on Stack
        struct table_obj
        {
            table_obj(lua_State* L, int32_t index);
            ~table_obj();

            void inc_ref();
            void dec_ref();

            bool validate();

            template<typename Key>
            void _push_key(Key&& key)
            {
                if constexpr(std::is_convertible_v<Key, lua_Integer>)
                {
                    lua_pushinteger(m_L, key);
                }
                else
                {
                    lua_pushstring(m_L, key);
                }
            }

            template<typename T, typename Key>
            void set(Key&& key, T&& object)
            {
                if(validate())
                {
                    _push_key(std::forward<Key>(key));
                    detail::push(m_L, std::forward<T>(object));
                    lua_settable(m_L, m_index);
                }
            }

            template<typename RVal, typename Key>
            RVal get(Key&& key)
            {
                if(validate())
                {
                    _push_key(std::forward<Key>(key));
                    lua_gettable(m_L, m_index);
                    return detail::pop<RVal>::apply(m_L);
                }
                else
                {
                    return detail::pop_nil<RVal>(m_L);
                }
            }

            template<typename Key>
            bool get_to_stack(Key&& key)
            {
                if(validate())
                {
                    _push_key(std::forward<Key>(key));
                    lua_gettable(m_L, m_index);
                    return true;
                }
                else
                {
                    return false;
                }
            }

            template<typename RVal, typename Key, typename... Args>
            RVal call(Key&& key, Args&&... arg)
            {
                if(validate())
                {
                    _push_key(std::forward<Key>(key));
                    lua_gettable(m_L, m_index);
                    if(lua_isfunction(m_L, -1) == true)
                    {
                        return call_stackfunc<RVal>(m_L, std::forward<Args>(arg)...);
                    }
                    else
                    {
                        lua_pop(m_L, 1); // pop func
                        print_error(m_L, "lua_tinker::call() attempt to call table[`%d'] (not a function)", key);
                    }
                }

                // return nil
                return detail::pop_nil<RVal>(m_L);
            }

            size_t len() const
            {
                lua_len(m_L, m_index);
                stack_delay_pop _delay(m_L, 1);
                return (size_t)lua_tointeger(m_L, -1);
            }

            stack_obj get_stack_obj() { return stack_obj(m_L, m_index); }

            void release_owner()
            {
                m_pointer = nullptr;
                m_index   = 0;
            }

            lua_State*  m_L;
            int32_t     m_index;
            const void* m_pointer;
            int32_t     m_ref;
        };
    } // namespace detail

    // Table Object Holder
    struct table_onstack
    {
        table_onstack(lua_State* L);
        table_onstack(lua_State* L, int32_t index);
        table_onstack(lua_State* L, const char* name);
        table_onstack(const table_onstack& input);
        ~table_onstack();

        template<typename T, typename Key>
        void set(Key&& key, T&& object)
        {
            m_obj->set(std::forward<Key>(key), std::forward<T>(object));
        }

        template<typename T, typename Key>
        T get(Key&& key)
        {
            return m_obj->get<T>(std::forward<Key>(key));
        }

        template<typename Key>
        bool get_to_stack(Key&& key)
        {
            return m_obj->get_to_stack(std::forward<Key>(key));
        }

        template<typename RVal, typename Key, typename... Args>
        RVal call(Key&& key, Args&&... args)
        {
            return m_obj->call<RVal>(std::forward<Key>(key), std::forward<Args>(args)...);
        }

        size_t len() const { return m_obj->len(); }

        template<typename T>
        T to_container()
        {
            return detail::_readfromtable<T>(m_obj->m_L, m_obj->m_index);
        }

        void for_each(std::function<bool(int32_t key_idx, int32_t value_idx)> func)
        {

            detail::stack_obj      table = m_obj->get_stack_obj();
            detail::table_iterator it(table);
            while(it.hasNext())
            {
                if(func(it.key_idx(), it.value_idx()) == false)
                    return;
                it.moveNext();
            }
        }

        void release_owner() { m_obj->release_owner(); }

        detail::table_obj* m_obj = nullptr;
    };


    struct table_ref : public detail::lua_ref_base
    {
        using lua_ref_base::lua_ref_base;

        static table_ref make_table_ref(const table_onstack& ref_table)
        {
            if(ref_table.m_obj != nullptr && ref_table.m_obj->validate())
            {
                // copy table to top
                lua_pushvalue(ref_table.m_obj->m_L, ref_table.m_obj->m_index);
                // move top to registry
                int32_t reg_idx = luaL_ref(ref_table.m_obj->m_L, LUA_REGISTRYINDEX);

                return table_ref(ref_table.m_obj->m_L, reg_idx);
            }
            return table_ref();
        }
        static table_ref make_table_ref(lua_State* L, int32_t index)
        {
            if(lua_istable(L, index) == true)
            {
                // copy table to top
                lua_pushvalue(L, index);
                // move top to registry
                int32_t reg_idx = luaL_ref(L, LUA_REGISTRYINDEX);

                return table_ref(L, reg_idx);
            }
            return table_ref();
        }

        table_onstack push_table_to_stack() const
        {
            if(lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_regidx) == LUA_TTABLE)
            {
                return table_onstack(m_L, -1);
            }
            else
            {
                print_error(m_L, "lua_tinker::table_ref attempt to visit(not a table)");
                return table_onstack(m_L);
            }
        }
    };


    namespace detail
    {
        // stl helper
        template<typename T>
        int32_t meta_container_get(lua_State* L)
        {
            stack_obj class_obj(L, 1);
            stack_obj key_obj(L, 2);
            stack_obj class_meta = class_obj.get_metatable();
            stack_obj val_obj    = class_meta.rawget(key_obj);
            if(val_obj.is_nil())
            {
                // val = list[key]
                UserDataWapper* pWapper    = user2type<UserDataWapper*>(L, 1);
                T*              pContainer = (T*)(pWapper->m_p);
                if constexpr(is_map_like<T>::value)
                {
                    // k,v
                    auto it = pContainer->find(detail::read<typename T::key_type>(L, 2));
                    if(it == pContainer->end())
                    {
                        lua_pushnil(L);
                    }
                    else
                    {
                        detail::push(L, it->second);
                    }
                }
                else if constexpr(is_set_like<T>::value)
                {
                    // set
                    auto it = pContainer->find(detail::read<typename T::value_type>(L, 2));
                    if(it == pContainer->end())
                    {
                        lua_pushnil(L);
                    }
                    else
                    {
                        detail::push(L, *it);
                    }
                }
                else
                {
                    // vector

                    uint32_t key = detail::read<uint32_t>(L, 2);
                    key -= 1;
                    if(key > pContainer->size())
                    {
                        lua_pushnil(L);
                    }
                    else
                    {
                        detail::push(L, pContainer->at(key));
                    }
                }
            }
            return 1;
        }

        template<typename T>
        int32_t meta_container_set(lua_State* L)
        {
            // list[key] = val;
            UserDataWapper* pWapper    = user2type<UserDataWapper*>(L, 1);
            T*              pContainer = (T*)(pWapper->m_p);
#ifdef LUATINKER_USERDATA_CHECK_CONST
            if(pWapper->is_const())
            {
                call_error(L, "container is const");
                return 0;
            }
#endif
            if constexpr(is_map_like<T>::value)
            {
                // k,v
                (*pContainer)[detail::read<typename T::key_type>(L, 2)] = detail::read<typename T::mapped_type>(L, 3);
            }
            else if constexpr(is_set_like<T>::value)
            {
                // set
                pContainer->insert(detail::read<typename T::value_type>(L, 2));
            }
            else
            {
                // vector
                int32_t key = detail::read<int32_t>(L, 2);
                key -= 1;
                if(key > (int32_t)pContainer->size())
                {
                    call_error(L, "set to vector : %d out of range", key);
                    return 0;
                }
                else
                {
                    (*pContainer)[key] = detail::read<typename T::value_type>(L, 3);
                }
            }

            return 0;
        }

        template<typename T>
        int32_t meta_container_push(lua_State* L)
        {
            // list[key] = val;
            UserDataWapper* pWapper    = user2type<UserDataWapper*>(L, 1);
            T*              pContainer = (T*)(pWapper->m_p);
#ifdef LUATINKER_USERDATA_CHECK_CONST
            if(pWapper->is_const())
            {
                call_error(L, "container is const");
                return 0;
            }
#endif
            if constexpr(is_map_like<T>::value)
            {
                // k,v
                pContainer->emplace(detail::read<typename T::key_type>(L, 2), detail::read<typename T::mapped_type>(L, 3));
            }
            else if constexpr(is_set_like<T>::value)
            {
                // set<int>
                pContainer->emplace(detail::read<typename T::value_type>(L, 2));
            }
            else if constexpr(is_vector_like<T>::value)
            {
                // string
                // vector<int>
                pContainer->emplace_back(detail::read<typename T::value_type>(L, 2));
            }
            else
            {
                // array
            }

            return 0;
        }
		
		template<typename T>
        int32_t meta_container_erase_by_value(lua_State* L)
        {
            // list[key] = val;
            UserDataWapper* pWapper    = user2type<UserDataWapper*>(L, 1);
            T*              pContainer = (T*)(pWapper->m_p);
#ifdef LUATINKER_USERDATA_CHECK_CONST
            if(pWapper->is_const())
            {
                call_error(L, "container is const");
                return 0;
            }
#endif
            if constexpr(is_map_like<T>::value)
            {
                // k,v
                
            }
            else if constexpr(is_set_like<T>::value)
            {
                // set
                pContainer->erase(detail::read<typename T::value_type>(L, 2));
            }
            else if constexpr(is_vector_like<T>::value && has_equality<typename T::value_type>::value)
            {
                // vector
                pContainer->erase(std::find(pContainer->begin(), pContainer->end(), detail::read<typename T::value_type>(L, 2)));
            }
            else
            {
                // maybe std::array
            }

            return 0;
        }

		template<typename T>
        int32_t meta_container_erase_by_key(lua_State* L)
        {
            // list[key] = val;
            UserDataWapper* pWapper    = user2type<UserDataWapper*>(L, 1);
            T*              pContainer = (T*)(pWapper->m_p);
#ifdef LUATINKER_USERDATA_CHECK_CONST
            if(pWapper->is_const())
            {
                call_error(L, "container is const");
                return 0;
            }
#endif
            if constexpr(is_map_like<T>::value)
            {
                // k,v
                pContainer->erase(detail::read<typename T::key_type>(L, 2));
            }
            else if constexpr(is_set_like<T>::value)
            {
                // set
                pContainer->erase(detail::read<typename T::value_type>(L, 2));
            }
            else if constexpr(is_vector_like<T>::value)
            {
                // vector
                auto idx = detail::read<int32_t>(L, 2);
            	if(pContainer->size() > idx)
                	pContainer->erase(pContainer->begin() + idx);
            }
            else
            {
                // maybe std::array
            }

            return 0;
        }
		
        
        template<typename T>
        struct lua_iterator
        {
            using Iter = typename base_type<T>::iterator;

            lua_iterator(Iter beg, Iter end)
                : m_Iter(std::move(beg))
                , m_End(std::move(end))
            {
            }
            bool HasMore() const { return m_Iter != m_End; }
            void MoveNext()
            {
                m_Iter++;
                m_nMoveDistance++;
            }
            static int32_t Next(lua_State* L)
            {
                UserDataWapper*  pWapper = user2type<UserDataWapper*>(L, 1);
                lua_iterator<T>* pIter   = (lua_iterator<T>*)(pWapper->m_p);
                if(pIter->HasMore() == false)
                {
                    lua_pushnil(L);
                    return 1;
                }
                if constexpr(is_map_like<T>::value)
                {
                    detail::push(L, pIter->m_Iter->first);
                    detail::push(L, pIter->m_Iter->second);
                    pIter->MoveNext();
                    return 2;
                }
                else
                {
                    detail::push(L, pIter->m_nMoveDistance);
                    detail::push(L, *(pIter->m_Iter));
                    pIter->MoveNext();
                    return 2;
                }
            }
            size_t m_nMoveDistance = 1;
            Iter   m_Iter;
            Iter   m_End;
        };

        template<typename T>
        int32_t meta_container_make_range(lua_State* L)
        {
            UserDataWapper* pWapper    = user2type<UserDataWapper*>(L, 1);
            T*              pContainer = (T*)(pWapper->m_p);
            lua_pushcfunction(L, &lua_iterator<base_type<T>>::Next);
            detail::push(L, lua_iterator<decltype(*pContainer)>(pContainer->begin(), pContainer->end()));
            lua_pushnil(L);
            return 3;
        }

        template<typename T>
        int32_t meta_container_get_len(lua_State* L)
        {
            UserDataWapper* pWapper    = user2type<UserDataWapper*>(L, 1);
            T*              pContainer = (T*)(pWapper->m_p);
            detail::push(L, pContainer->size());
            return 1;
        }

        template<typename T>
        int32_t meta_container_to_table(lua_State* L)
        {
            UserDataWapper* pWapper    = user2type<UserDataWapper*>(L, 1);
            T*              pContainer = (T*)(pWapper->m_p);

            _pushtotable(L, *pContainer);
            return 1;
        }

        template<typename T>
        void add_container_mate(lua_State* L)
        {
            std::string t_class_name = get_class_name<base_type<T>>();
            std::string name = std::string("container_with_") + (t_class_name.empty()?"unknown": t_class_name);
            detail::class_name<base_type<T>>::name(name.c_str());
            lua_createtable(L, 0, 8);

            lua_pushstring(L, "__name");
            lua_pushstring(L, name.c_str());
            lua_rawset(L, -3);

            lua_pushstring(L, "__index");
            lua_pushcclosure(L, detail::meta_container_get<base_type<T>>, 0);
            lua_rawset(L, -3);

            lua_pushstring(L, "__newindex");
            lua_pushcclosure(L, detail::meta_container_set<base_type<T>>, 0);
            lua_rawset(L, -3);

            lua_pushstring(L, "__pairs");
            lua_pushcclosure(L, detail::meta_container_make_range<base_type<T>>, 0);
            lua_rawset(L, -3);

            lua_pushstring(L, "__len");
            lua_pushcclosure(L, detail::meta_container_get_len<base_type<T>>, 0);
            lua_rawset(L, -3);

            lua_pushstring(L, "to_table");
            lua_pushcclosure(L, detail::meta_container_to_table<base_type<T>>, 0);
            lua_rawset(L, -3);

            lua_pushstring(L, "push");
            lua_pushcclosure(L, detail::meta_container_push<base_type<T>>, 0);
            lua_rawset(L, -3);

            lua_pushstring(L, "erase_by_value");
            lua_pushcclosure(L, detail::meta_container_erase_by_value<base_type<T>>, 0);
            lua_rawset(L, -3);

            lua_pushstring(L, "erase_by_key");
            lua_pushcclosure(L, detail::meta_container_erase_by_key<base_type<T>>, 0);
            lua_rawset(L, -3);

            lua_pushstring(L, "__gc");
            lua_pushcclosure(L, detail::destroyer<detail::UserDataWapper>, 0);
            lua_rawset(L, -3);

            lua_setglobal(L, name.c_str());
        }
    } // namespace detail

} // namespace lua_tinker

typedef lua_tinker::table_onstack LuaTable;

// overload func
namespace lua_tinker
{

    // convert args to luatype store in longlong
    // so void(int32_t) = LUA_TNUMBER = 0x3, void(unsigned shart) = LUA_TNUMBER = 0x3, they have the same signature,
    // void(int32_t,a*) = LUA_TNUMBER,LUA_TUSERDATA = 0x73 , void(int64_t,b*) = LUA_TNUMBER,LUA_TUSERDATA = 0x73 , they
    // have the same signature, and use longlong to stroe,so we only support 16 args

    namespace detail
    {
        template<typename Func, std::size_t index>
        constexpr int32_t get_func_argv()
        {
            return _stack_help<typename function_traits<Func>::template argv<index>::type>::cover_to_lua_type();
        }

        template<typename Func, std::size_t args_num>
        struct count_all_func_argv
        {
            static constexpr const uint64_t result =
                ((get_func_argv<Func, (args_num - 1)>() << ((args_num - 1) * 4)) + count_all_func_argv<Func, (args_num - 1)>::result);
        };
        template<typename Func>
        struct count_all_func_argv<Func, 1>
        {
            static constexpr const uint64_t result = get_func_argv<Func, 0>();
        };

        template<typename Func>
        struct count_all_func_argv<Func, 0>
        {
            static constexpr const uint64_t result = 0;
        };

        template<typename T>
        struct function_signature
        {
            static constexpr const uint64_t m_sig = count_all_func_argv<T, function_traits<T>::argc>::result;
        };

        void    _set_signature_bit(uint64_t& sig, size_t idx, uint8_t c);
        uint8_t _get_signature_bit(const uint64_t& sig, size_t idx);

    }; // namespace detail

    struct args_type_overload_functor_base
    {
        typedef std::shared_ptr<detail::functor_base>     functor_base_ptr;
        typedef std::multimap<uint64_t, functor_base_ptr> overload_funcmap_t;
        typedef std::map<size_t, overload_funcmap_t>      overload_funcmap_by_argsnum_t;
        overload_funcmap_by_argsnum_t                     m_overload_funcmap;
        int32_t                                           m_nParamsOffset = 0;

        args_type_overload_functor_base() {}

        virtual ~args_type_overload_functor_base() { m_overload_funcmap.clear(); }
        args_type_overload_functor_base(args_type_overload_functor_base&& rht)
            : m_overload_funcmap(rht.m_overload_funcmap)
            , m_nParamsOffset(rht.m_nParamsOffset)
        {
            rht.m_overload_funcmap.clear();
        }

        void insert(size_t args_num, size_t default_args_num, uint64_t sig, functor_base_ptr&& ptr)
        {
            static constexpr uint64_t mask[] = {
                0,
                0xF,
                0xFF,
                0xFFF,
                0xFFFF,
                0xFFFFF,
                0xFFFFFF,
                0xFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFFF,
                0xFFFFFFFFFF,
                0xFFFFFFFFFFF,
                0xFFFFFFFFFFFF,
                0xFFFFFFFFFFFFF,
                0xFFFFFFFFFFFFFF,
                0xFFFFFFFFFFFFFFF,
                0xFFFFFFFFFFFFFFFF,
            };

            for(size_t i = 0; i <= default_args_num && i <= args_num; i++)
            {
                auto&   refMap  = m_overload_funcmap[args_num - i];
                int64_t new_sig = sig & mask[args_num - i];
                refMap.emplace(new_sig, ptr);
            }
        }

        int32_t apply(lua_State* L)
        {
            uint64_t sig          = 0;
            int32_t  nParamsCount = lua_gettop(L) - m_nParamsOffset;
            for(int32_t i = 0; i < nParamsCount; i++)
            {
                int32_t nType = detail::LType2ParamsType(L, i + m_nParamsOffset + 1);
                detail::_set_signature_bit(sig, i, nType);
            }
            auto& refMap = m_overload_funcmap[nParamsCount];
            auto  itFind = refMap.equal_range(sig);
            if(itFind.first == refMap.end())
            {
                // signature mismatch
                call_error(L, "function overload can't find %d args resolution ", nParamsCount);
                return 0;
            }
            else if(std::next(itFind.first) != itFind.second)
            {
                // signature mismatch
                std::string strSig;
                for(int32_t i = 0; i < nParamsCount; i++)
                {
                    uint8_t c = detail::_get_signature_bit(sig, i);
                    if(strSig.empty() == false)
                        strSig.push_back(',');
                    const char* pName = detail::OVERLOAD_PARAMTYPE_NAME[c];
                    strSig.append(pName);
                }
                call_error(L, "function(%s) overload resolution more than one", strSig.c_str());
                return 0;
            }
            else
            {
                return itFind.first->second->apply(L);
            }
        }

        static int32_t invoke_function(lua_State* L)
        {
            args_type_overload_functor_base* pFunctor = detail::upvalue_<args_type_overload_functor_base*>(L);
            return pFunctor->apply(L);
        }
    };

    struct args_type_overload_functor : public args_type_overload_functor_base
    {
        using args_type_overload_functor_base::args_type_overload_functor_base;
        template<typename... Args>
        args_type_overload_functor(Args&&... args)
        {
            push_to_map_help(std::forward<Args>(args)...);
        }
        void push_to_map_help() {}
        template<typename T, typename... Args>
        void push_to_map_help(T&& t, Args&&... args)
        {
            push_to_map_help(std::forward<T>(t));
            push_to_map_help(std::forward<Args>(args)...);
        }

        template<typename RVal, typename... Args>
        void push_to_map_help(detail::functor<RVal, Args...>* ptr)
        {
            constexpr int64_t sig = detail::function_signature<RVal(Args...)>::m_sig;
            insert(sizeof...(Args), ptr->getDefaultArgsNum(), sig, functor_base_ptr(ptr));
        }
    };
    struct args_type_overload_member_functor : public args_type_overload_functor_base
    {
        using args_type_overload_functor_base::args_type_overload_functor_base;

        template<typename... Args>
        args_type_overload_member_functor(Args&&... args)
        {
            push_to_map_help(std::forward<Args>(args)...);
            m_nParamsOffset = 1;
        }
        void push_to_map_help() {}
        template<typename T, typename... Args>
        void push_to_map_help(T&& t, Args&&... args)
        {
            push_to_map_help(std::forward<T>(t));
            push_to_map_help(std::forward<Args>(args)...);
        }

        template<bool bConst, typename CT, typename RVal, typename... Args>
        void push_to_map_help(detail::member_functor<bConst, CT, RVal, Args...>* ptr)
        {
            constexpr int64_t sig = detail::function_signature<RVal (CT::*)(Args...)>::m_sig;
            insert(sizeof...(Args), ptr->getDefaultArgsNum(), sig, functor_base_ptr(ptr));
        }
    };

    struct args_type_overload_constructor : public args_type_overload_functor_base
    {
        using args_type_overload_functor_base::args_type_overload_functor_base;

        template<typename... Args>
        args_type_overload_constructor(Args&&... args)
        {
            push_to_map_help(std::forward<Args>(args)...);
            m_nParamsOffset = 1;
        }
        void push_to_map_help() {}
        template<typename T, typename... Args>
        void push_to_map_help(T&& t, Args&&... args)
        {
            push_to_map_help(std::forward<T>(t));
            push_to_map_help(std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        void push_to_map_help(constructor<T, Args...>* ptr)
        {
            constexpr const int64_t sig = detail::function_signature<void(Args...)>::m_sig;
            insert(sizeof...(Args), ptr->getDefaultArgsNum(), sig, functor_base_ptr(ptr));
        }
    };

    template<typename RVal, typename... Args, typename... ExtArgs>
    detail::functor<RVal, Args...>* make_functor_ptr(RVal(func)(Args...), ExtArgs... exArgs)
    {
        return new detail::functor<RVal, Args...>(func, exArgs...);
    }

    template<typename RVal, typename... Args, typename... ExtArgs>
    detail::functor<RVal, Args...>* make_functor_ptr(const std::function<RVal(Args...)>& func, ExtArgs... exArgs)
    {
        return new detail::functor<RVal, Args...>(func, exArgs...);
    }

    template<typename RVal, typename... Args, typename... ExtArgs>
    detail::functor<RVal, Args...>* make_functor_ptr(std::function<RVal(Args...)>&& func, ExtArgs... exArgs)
    {
        return new detail::functor<RVal, Args...>(std::move(func), exArgs...);
    }

    template<typename CT, typename RVal, typename... Args, typename... ExtArgs>
    detail::member_functor<false, CT, RVal, Args...>* make_member_functor_ptr(RVal (CT::*func)(Args...), ExtArgs... exArgs)
    {
        return new detail::member_functor<false, CT, RVal, Args...>(func, exArgs...);
    }
    template<typename CT, typename RVal, typename... Args, typename... ExtArgs>
    detail::member_functor<true, CT, RVal, Args...>* make_member_functor_ptr(RVal (CT::*func)(Args...) const, ExtArgs... exArgs)
    {
        return new detail::member_functor<true, CT, RVal, Args...>(func, exArgs...);
    }

}; // namespace lua_tinker

#endif //_LUA_TINKER_H_
