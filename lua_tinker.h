// lua_tinker.h
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright (c) 2005-2007 Kwon-il Lee (zupet@hitel.net)
// 
// please check Licence.txt file for licence and legal issues. 

#if !defined(_LUA_TINKER_H_)
#define _LUA_TINKER_H_

#include <new>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <typeinfo>
#include <type_traits>
#include"lua.hpp"
#include"type_traits_ext.h" 
#include<memory>

#ifdef LUA_CALL_CFUNC_NEED_ALL_PARAM
#define LUA_CHECK_HAVE_THIS_PARAM(L,index) if(lua_isnone(L,index)){lua_pushfstring(L, "need argument %d to call cfunc", index);lua_error(L);}
#define LUA_CHECK_HAVE_THIS_PARAM_AND_NOT_NIL(L,index) if(lua_isnoneornil(L,index)){lua_pushfstring(L, "need argument %d to call cfunc", index);lua_error(L);}
#else
#define LUA_CHECK_HAVE_THIS_PARAM(L,index)
#define LUA_CHECK_HAVE_THIS_PARAM_AND_NOT_NIL(L,index)
#endif

#define CHECK_CLASS_PTR(T) {if(lua_isnoneornil(L,1)){lua_pushfstring(L, "class_ptr %s is nil or none", lua_tinker::class_name<base_type<T>>::name());lua_error(L);} }

#define TRY_LUA_TINKER_INVOKE() try
#define CATCH_LUA_TINKER_INVOKE() catch(...)

namespace lua_tinker
{
	static const char* S_SHARED_PTR_NAME = "__shared_ptr";
    // init LuaTinker
    void    init(lua_State *L);

    void    init_s64(lua_State *L);
    void    init_u64(lua_State *L);
	void	init_shared_ptr(lua_State *L);

    // string-buffer excution
    void    dofile(lua_State *L, const char *filename);
    void    dostring(lua_State *L, const char* buff);
    void    dobuffer(lua_State *L, const char* buff, size_t sz);
    
    // debug helpers
    void    enum_stack(lua_State *L);
    int     on_error(lua_State *L);
    void    print_error(lua_State *L, const char* fmt, ...);

	template<typename T>
	using base_type = typename std::remove_cv<typename std::remove_reference<typename std::remove_pointer<T>::type>::type>::type;

    // dynamic type extention
    struct lua_value
    {
        virtual void to_lua(lua_State *L) = 0;
    };

    // type trait
    template<typename T> struct class_name;
    struct table;

	// lua stack help to read/push
	template<typename T>
	struct _stack_help
	{
		static T _read(lua_State *L, int index) { return lua2type<T>(L, index); }
		static void _push(lua_State *L, T ret) { type2lua<T>(L, std::forward<T>(ret)); }
	};

	template<>
	struct _stack_help<char*>
	{
		static char* _read(lua_State *L, int index);
		static void  _push(lua_State *L, char* ret);
	};

	template<>
	struct _stack_help<const char*>
	{
		static const char* _read(lua_State *L, int index);
		static void  _push(lua_State *L, const char* ret);

	};

	template<>
	struct _stack_help<char>
	{
		static char _read(lua_State *L, int index);
		static void  _push(lua_State *L, char ret);

	};

	template<>
	struct _stack_help<unsigned char>
	{
		static unsigned char _read(lua_State *L, int index);
		static void  _push(lua_State *L, unsigned char ret);

	};

	template<>
	struct _stack_help<short>
	{
		static short _read(lua_State *L, int index);
		static void  _push(lua_State *L, short ret);

	};

	template<>
	struct _stack_help<unsigned short>
	{
		static unsigned short _read(lua_State *L, int index);
		static void  _push(lua_State *L, unsigned short ret);

	};

	template<>
	struct _stack_help<int>
	{
		static int _read(lua_State *L, int index);
		static void  _push(lua_State *L, int ret);

	};

	template<>
	struct _stack_help<unsigned int>
	{
		static unsigned int _read(lua_State *L, int index);
		static void  _push(lua_State *L, unsigned int ret);

	};

	template<>
	struct _stack_help<long>
	{
		static long _read(lua_State *L, int index);
		static void  _push(lua_State *L, long ret);

	};

	template<>
	struct _stack_help< unsigned long>
	{
		static  unsigned long _read(lua_State *L, int index);
		static void  _push(lua_State *L, unsigned long ret);

	};

	template<>
	struct _stack_help<float>
	{
		static float _read(lua_State *L, int index);
		static void  _push(lua_State *L, float ret);

	};

	template<>
	struct _stack_help< double>
	{
		static  double _read(lua_State *L, int index);
		static void  _push(lua_State *L, double ret);

	};

	template<>
	struct _stack_help<std::string>
	{
		static std::string _read(lua_State *L, int index);
		static void _push(lua_State *L, std::string ret);

	};

	//if want read const std::string& , we read a string obj
	template<>
	struct _stack_help<const std::string&>
	{
		static std::string _read(lua_State *L, int index);
		static void _push(lua_State *L, const std::string& ret);

	};

	// from lua
	// param to pointer
	template<typename T>
	typename std::enable_if<std::is_pointer<T>::value, T>::type void2type(void* ptr)
	{
		return (base_type<T>*)ptr;
	}
	//to reference
	template<typename T>
	typename std::enable_if<std::is_reference<T>::value, base_type<T>&>::type void2type(void* ptr)
	{
		return *(base_type<T>*)ptr;
	}

	//to val
	template<typename T>
	typename std::enable_if<!is_shared_ptr<T>::value && !std::is_pointer<T>::value && !std::is_reference<T>::value, base_type<T>>::type void2type(void* ptr)
	{
		return *(base_type<T>*)ptr;
	}

	
	//to shared_ptr, use weak_ptr to hold it
	template<typename T>
	typename std::enable_if<is_shared_ptr<T>::value, T>::type void2type(void* ptr)
	{

		return ((std::weak_ptr<get_shared_t<T>>*)ptr)->lock();
	}
	//template<typename T>
	//typename std::enable_if<is_shared_ptr<T>::value, T>::type void2type(void* ptr)
	//{

	//	return *(T*)ptr;
	//}

	//userdata to T，T*，T&
	template<typename T>
	T user2type(lua_State *L, int index)
	{
		return void2type<T>(lua_touserdata(L, index));
	}
	
	//get userdata from lua 
	template<typename T>
	typename std::enable_if<!std::is_enum<T>::value && !std::is_pointer<T>::value, T>::type lua2type(lua_State *L, int index)
	{
		if (!lua_isuserdata(L, index))
		{
			lua_pushfstring(L, "can't convert argument %d to class %s", index, class_name< base_type<T> >::name());
			lua_error(L);
		}
		
		user* puser = user2type<user*>(L, index);
		if (puser->checktype<T>() == false)
		{
			lua_pushfstring(L, "can't convert argument %d to class %s", index, class_name< base_type<T> >::name());
			lua_error(L);
		}

		return void2type<T>(puser->m_p);
	}

	//get userdata ptr from lua, can handle nil an 0
	template<typename T>
	typename std::enable_if<std::is_pointer<T>::value, T>::type lua2type(lua_State *L, int index)
	{
		if (lua_isnoneornil(L, index))
		{
			return void2type<T>(nullptr);
		}
		else if (lua_isnumber(L, index) && lua_tonumber(L, index) == 0)
		{
			return void2type<T>(nullptr);
		}
		else if (!lua_isuserdata(L, index))
		{
			lua_pushfstring(L, "can't convert argument %d to class %s", index, class_name< base_type<T> >::name());
			lua_error(L);
		}

		user* puser = user2type<user*>(L, index);
		if (puser->checktype<T>() == false)
		{
			lua_pushfstring(L, "can't convert argument %d to class %s", index, class_name< base_type<T> >::name());
			lua_error(L);
		}

		return void2type<T>(puser->m_p);
	}

	//get enum from lua
	template<typename T>
	typename std::enable_if<std::is_enum<T>::value,T>::type lua2type(lua_State *L, int index)
	{
		return (T)(int)lua_tonumber(L, index);
	}

 	//read_weap
	template<typename T>
	decltype(auto) read(lua_State *L, int index)
	{
#ifdef LUA_CALL_CFUNC_NEED_ALL_PARAM
		if (std::is_pointer<T>)
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

	//read_weap
	template<typename T>
	decltype(auto) read_nocheck(lua_State *L, int index)
	{
		return _stack_help<T>::_read(L, index);
	}

	enum userdata_holder_t
	{
		userdata_val,
		userdata_ref,
		userdata_ptr,
		userdata_sharedptr,
	};
	//userdata holder
	struct user
	{
		user(void* p) : m_p(p) {}
		virtual ~user() {}
		void* m_p;
		virtual userdata_holder_t gettype() = 0;





		template<typename T>
		typename std::enable_if<std::is_pointer<T>::value, bool>::type checktype()
		{
			return gettype() == userdata_ptr;
		}

		template<typename T>
		typename std::enable_if<std::is_reference<T>::value, bool>::type checktype()
		{
			return gettype() == userdata_ref;
		}

		template<typename T>
		typename std::enable_if<is_shared_ptr<T>::value, bool>::type checktype()
		{
			return gettype() == userdata_sharedptr;
		}

		template<typename T>
		typename std::enable_if<!std::is_pointer<T>::value && !std::is_reference<T>::value && !is_shared_ptr<T>::value, bool>::type checktype()
		{
			return gettype() == userdata_val;
		}
	};

	template<typename T>
	struct val2user : user
	{
		val2user() : user(new T) { nType = }
		val2user(const T& t) : user(new T(t)) {}
		val2user(T&& t) : user(new T(t)) {}

		template<typename Tup,typename = typename std::enable_if<is_tuple<Tup>::value, void>::type >
		val2user(Tup&& tup) : val2user(std::forward<Tup>(tup), std::make_index_sequence<std::tuple_size<typename std::decay<Tup>::type>::value>{}) {}
		template<typename Tup, size_t ...index>
		val2user(Tup&& tup, std::index_sequence<index...>) : user(new T(std::get<index>(std::forward<Tup>(tup))...)) {}

		~val2user() { delete ((T*)m_p); }

		virtual userdata_holder_t gettype() override { return userdata_val; }
	};

	template<typename T>
	struct ptr2user : user
	{
		ptr2user(T* t) : user((void*)t) {}
		virtual userdata_holder_t gettype() override { return userdata_ptr; }
	};

	template<typename T>
	struct ref2user : user
	{
		ref2user(T& t) : user(&t) {}
		virtual userdata_holder_t gettype() override { return userdata_ref; }
	};

	template<typename T>
	struct sharedptr2user : user
	{
		sharedptr2user(const std::shared_ptr<T>& rht) :m_holder(rht), user(&m_holder) {}
		//use weak_ptr to hold it
		~sharedptr2user() { m_holder.reset(); }

		std::weak_ptr<T> m_holder;
		virtual userdata_holder_t gettype() override { return userdata_sharedptr; }
	};

	// to lua
	// userdata pointer to lua 
	template<typename T>
	typename std::enable_if<std::is_pointer<T>::value, void>::type object2lua(lua_State *L, T input)
	{
		if (input) new(lua_newuserdata(L, sizeof(ptr2user<base_type<T>>))) ptr2user<base_type<T>>(input); else lua_pushnil(L);
	}
	// userdata reference to lua
	template<typename T>
	typename std::enable_if<std::is_reference<T>::value, void>::type object2lua(lua_State *L, T input)
	{
		new(lua_newuserdata(L, sizeof(ref2user<T>))) ref2user<T>(input);
	}
	// userdata val to lua
	template<typename T>
	typename std::enable_if<!std::is_pointer<T>::value && !std::is_reference<T>::value, void>::type object2lua(lua_State *L, T input)
	{
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(input);
	}

	// shared_ptr to lua 
	template<typename T>
	void sharedobject2lua(lua_State *L, std::shared_ptr<T> input)
	{
		if (input) new(lua_newuserdata(L, sizeof(sharedptr2user<T>))) sharedptr2user<T>(input); else lua_pushnil(L);
	}

	//obj to lua
	template<typename T>
	typename std::enable_if<!std::is_enum<T>::value && !is_shared_ptr<T>::value && !is_container<T>::value, void>::type type2lua(lua_State *L, T val)
	{
		object2lua(L, std::forward<T>(val));
		push_meta(L, class_name<base_type<T>>::name());
		lua_setmetatable(L, -2);
	}


	//k,v container to lua
	template<typename T >
	static typename std::enable_if<is_associative_container<T>::value, void>::type _push_container(lua_State *L, const T& ret)
	{
		lua_newtable(L);
		for (auto it = ret.begin(); it != ret.end(); it++)
		{
			push(L, it->first);
			push(L, it->second);
			lua_settable(L, -3);
		}
	}

	//t container to lua
	template<typename T >
	static typename std::enable_if<!is_associative_container<T>::value, void>::type _push_container(lua_State *L, const T& ret)
	{
		lua_newtable(L);
		auto it = ret.begin();
		for (int i = 1; it != ret.end(); it++, i++)
		{
			push(L, i);
			push(L, *it);
			lua_settable(L, -3);
		}
	}

	//stl container push to lua table
	template<typename T >
	typename std::enable_if<is_container<T>::value, void>::type type2lua(lua_State *L, T ret) 
	{
		_push_container<T>(L, ret); 
	}

	//shared_ptr to lua
	template<typename T>
	typename std::enable_if<is_shared_ptr<T>::value, void>::type type2lua(lua_State *L, T val)
	{
		sharedobject2lua(L, val);
		//use RowT's meta
		//push_meta(L, class_name<get_shared_t<T>>::name());
		push_meta(L, S_SHARED_PTR_NAME);

		lua_setmetatable(L, -2);
	}

	//enum to lua
	template<typename T>
	typename std::enable_if<std::is_enum<T>::value, void>::type type2lua(lua_State *L, T val)
	{
		lua_pushnumber(L, (int)val);
	}

	// get value from cclosure
	template<typename T>
	T upvalue_(lua_State *L)
	{
		return user2type<T>(L, lua_upvalueindex(1));
	}

    // push value_list to lua stack 
	template<typename T,typename ...Args>
	void push_args(lua_State *L, T ret, Args...args) { push<T>(L, std::forward<T>(ret)); push_args<Args...>(L, std::forward<Args>(args)...); }
	template<typename T, typename ...Args>
	void push_args(lua_State *L, T ret) { push<T>(L, std::forward<T>(ret)); }


	//push warp
	template<typename T>
	void push(lua_State *L, T ret)
	{
		return _stack_help<T>::_push(L, std::forward<T>(ret));
	}

    // pop a value from lua stack
    template<typename T>  
    T pop(lua_State *L) { T t = read_nocheck<T>(L, -1); lua_pop(L, 1); return t; }
    
    template<>  void    pop(lua_State *L);
    template<>  table   pop(lua_State *L);


	//invoke func
	template<typename Func, typename Tup, std::size_t... index>
	decltype(auto) invoke_helper(Func&& func, Tup&& tup, std::index_sequence<index...>)
	{
		return std::invoke(func, std::get<index>(std::forward<Tup>(tup))...);
	}

	template<typename Func, typename Tup>
	decltype(auto) invoke_func(Func&& func, Tup&& tup)
	{
		constexpr auto Size = std::tuple_size<typename std::decay<Tup>::type>::value;
		return invoke_helper(std::forward<Func>(func),
			std::forward<Tup>(tup),
			std::make_index_sequence<Size>{});
	}

	//template<typename Func, typename ...Args>
	//decltype(auto) invoke_func(Func func, Args...args)
	//{
	//	return func(args...);
	//}

	template<typename Func>
	decltype(auto) invoke_func(Func func)
	{
		return func();
	}


	//make params to tuple
	template<typename...T>
	struct ParamHolder
	{
		typedef std::tuple<typename std::remove_cv<typename std::remove_reference<T>::type>::type...> type;
	};
	template<typename...T>
	using ParamHolder_T = typename ParamHolder<T...>::type;


	template <int nIdxParams, typename... T, std::size_t... N>
	ParamHolder_T<T...> _get_args_help(lua_State *L, std::index_sequence<N...>)
	{
		return std::forward<ParamHolder_T<T...>>(ParamHolder_T<T...>{ read<T>(L, N + nIdxParams)... });
	}

	template <int nIdxParams, typename... T>
	ParamHolder_T<T...> _get_args_help(lua_State *L)
	{
		constexpr std::size_t num_args = sizeof...(T);
		return _get_args_help<nIdxParams, T...>(L, std::make_index_sequence<num_args>());
	}

	////ParamHolder no hold const T&, only hold T
	//template <int nIdxParams, typename... T>
	//ParamHolder_T<T...> _get_args(lua_State *L)
	//{
	//	return _get_args_help<nIdxParams, typename std::remove_cv<typename std::remove_reference<T>::type>::type...>(L);
	//}
	template <int nIdxParams, typename... T>
	ParamHolder_T<T...> _get_args(lua_State *L)
	{
		return _get_args_help<nIdxParams, T...>(L);
	}
	//functor
	template <typename RVal, typename CT, typename ... Args>
	struct member_functor
	{

		static int invoke(lua_State *L)
		{
			CHECK_CLASS_PTR(CT);
			TRY_LUA_TINKER_INVOKE()
			{
				_invoke<RVal>(L);
				return 1;
			}
			CATCH_LUA_TINKER_INVOKE()
			{
				lua_pushfstring(L, "lua fail to invoke functor");
				lua_error(L);
			}
			return 0;
			
		}

		template<typename T>
		static typename std::enable_if<!std::is_void<T>::value, void>::type _invoke(lua_State *L)
		{
			push(L, std::forward<RVal>(invoke_func(upvalue_<RVal(CT::*)(Args...)>(L), _get_args<1, CT*, Args...>(L))));
		}

		template<typename T>
		static typename std::enable_if<std::is_void<T>::value, void>::type _invoke(lua_State *L)
		{
			invoke_func(upvalue_<RVal(CT::*)(Args...)>(L), _get_args<1, CT*, Args...>(L));
		}
	};


	template<typename RVal, typename CT>
	struct member_functor<RVal, CT>
	{
		static int invoke(lua_State *L)
		{
			CHECK_CLASS_PTR(CT);
			TRY_LUA_TINKER_INVOKE()
			{
				_invoke<RVal>(L);
				return 1;
			}
			CATCH_LUA_TINKER_INVOKE()
			{
				lua_pushfstring(L, "lua fail to invoke functor");
				lua_error(L);
			}
			return 0;
		}

		template<typename T>
		static typename std::enable_if<!std::is_void<T>::value, void>::type _invoke(lua_State *L)
		{
			push(L, std::forward<RVal>(invoke_func(upvalue_<RVal(CT::*)()>(L), _get_args<1, CT*>(L))));
		}

		template<typename T>
		static typename std::enable_if<std::is_void<T>::value, void>::type _invoke(lua_State *L)
		{
			invoke_func(upvalue_<void(CT::*)()>(L), _get_args<1, CT*>(L));
		}
	};




	template <typename RVal, typename ... Args>
	struct functor
	{

		static int invoke(lua_State *L)
		{
			TRY_LUA_TINKER_INVOKE()
			{
				_invoke<RVal>(L);
				return 1;
			}
			CATCH_LUA_TINKER_INVOKE()
			{
				lua_pushfstring(L, "lua fail to invoke functor");
				lua_error(L);
			}
			return 0;
		}



		template<typename T>
		static typename std::enable_if<!std::is_void<T>::value, void>::type _invoke(lua_State *L)
		{
			push(L, std::forward<RVal>(invoke_func(upvalue_<RVal(*)(Args...)>(L), _get_args<1, Args...>(L))));
		}

		template<typename T>
		static typename std::enable_if<std::is_void<T>::value, void>::type _invoke(lua_State *L)
		{
			invoke_func(upvalue_<RVal(*)(Args...)>(L), _get_args<1, Args...>(L));
		}
	};


	template<typename RVal>
	struct functor<RVal>
	{
		static int invoke(lua_State *L)
		{
			TRY_LUA_TINKER_INVOKE()
			{
				_invoke<RVal>(L);
				return 1;
			}
			CATCH_LUA_TINKER_INVOKE()
			{
				lua_pushfstring(L, "lua fail to invoke functor");
				lua_error(L);
			}
			return 0;
		}

		template<typename T>
		static typename std::enable_if<!std::is_void<T>::value, void>::type _invoke(lua_State *L)
		{
			push(L, std::forward<RVal>(invoke_func( upvalue_<RVal(*)()>(L) ) ));
		}

		template<typename T>
		static typename std::enable_if<std::is_void<T>::value, void>::type _invoke(lua_State *L)
		{
			invoke_func(upvalue_<void(*)()>(L));
		}
	};

	//functor push
	template<typename RVal, typename T, typename ... Args>
	void push_functor(lua_State *L, RVal(T::*func)(Args...))
	{
		(void)func;
		lua_pushcclosure(L, &member_functor<RVal, T, Args...>::invoke, 1);
	}

	template<typename RVal, typename T>
	void push_functor(lua_State *L, RVal(T::*func)())
	{
		(void)func;
		lua_pushcclosure(L, &member_functor<RVal, T>::invoke, 1);
	}

	template<typename RVal, typename T, typename ... Args>
	void push_functor(lua_State *L, RVal(T::*func)(Args...)const)
	{
		(void)func;
		lua_pushcclosure(L, &member_functor<RVal, T, Args...>::invoke, 1);
	}

	template<typename RVal, typename T>
	void push_functor(lua_State *L, RVal(T::*func)()const)
	{
		(void)func;
		lua_pushcclosure(L, &member_functor<RVal, T>::invoke, 1);
	}

	template<typename RVal, typename ... Args>
	void push_functor(lua_State *L, RVal(*func)(Args...))
	{
		(void)func;
		lua_pushcclosure(L, &functor<RVal, Args...>::invoke, 1);
	}

	template<typename RVal>
	void push_functor(lua_State *L, RVal(*func)())
	{
		(void)func;
		lua_pushcclosure(L, &functor<RVal>::invoke, 1);
	}


    // member variable
    struct var_base
    {
        virtual ~var_base() {};
        virtual void get(lua_State *L) = 0;
        virtual void set(lua_State *L) = 0;
    };

    template<typename T, typename V>
    struct mem_var : var_base
    {
        V T::*_var;
        mem_var(V T::*val) : _var(val) {}
		void get(lua_State *L) { CHECK_CLASS_PTR(T); push(L, read<T*>(L, 1)->*(_var)); }
        void set(lua_State *L) { CHECK_CLASS_PTR(T); read<T*>(L,1)->*(_var) = read<V>(L, 3);  
		}
    };

    // constructor
	template<typename T, typename ...Args>
	struct constructor
	{
		static int invoke(lua_State *L)
		{
			new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(_get_args<2, Args...>(L));
			push_meta(L, class_name<base_type<T>>::name());
			lua_setmetatable(L, -2);

			return 1;
		}
	};
	

	template<typename T>
	struct constructor<T>
	{
		static int invoke(lua_State *L)
		{
			new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>();
			push_meta(L, class_name<base_type<T>>::name());
			lua_setmetatable(L, -2);

			return 1;
		}
	};
	

    // destroyer
    template<typename T>
    int destroyer(lua_State *L) 
    { 
        ((user*)lua_touserdata(L, 1))->~user();
        return 0;
    }
	int destroyer_shared_ptr(lua_State *L);

    // global function
    template<typename F> 
    void def(lua_State* L, const char* name, F func)
    { 
        lua_pushlightuserdata(L, (void*)func);
        push_functor(L, func);
        lua_setglobal(L, name);
    }

    // global variable
    template<typename T>
    void set(lua_State* L, const char* name, T object)
    {
        push(L, object);
        lua_setglobal(L, name);
    }

    template<typename T>
    T get(lua_State* L, const char* name)
    {
        lua_getglobal(L, name);
        return pop<T>(L);
    }

    template<typename T>
    void decl(lua_State* L, const char* name, T object)
    {
        set(L, name, object);
    }

    // call lua func
    template<typename RVal>
    RVal call(lua_State* L, const char* name)
    {
        lua_pushcclosure(L, on_error, 0);
        int errfunc = lua_gettop(L);

        lua_getglobal(L, name);
        if(lua_isfunction(L,-1))
        {
            lua_pcall(L, 0, 1, errfunc);
        }
        else
        {
            print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
        }

        lua_remove(L, errfunc);
        return pop<RVal>(L);
    }

    template<typename RVal, typename ...Args>
    RVal call(lua_State* L, const char* name, Args... arg)
    {
      
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_getglobal(L,name);
		if(lua_isfunction(L,-1))
		{
			push_args(L, arg...);
			
			if(lua_pcall(L, sizeof...(Args), 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	} // }

    // class helper
    int meta_get(lua_State *L);
    int meta_set(lua_State *L);
    void push_meta(lua_State *L, const char* name);

    // class init
    template<typename T>
    void class_add(lua_State* L, const char* name) 
    { 
        class_name<T>::name(name);

        lua_newtable(L);

        lua_pushstring(L, "__name");
        lua_pushstring(L, name);
        lua_rawset(L, -3);

        lua_pushstring(L, "__index");
        lua_pushcclosure(L, meta_get, 0);
        lua_rawset(L, -3);

        lua_pushstring(L, "__newindex");
        lua_pushcclosure(L, meta_set, 0);
        lua_rawset(L, -3);

        lua_pushstring(L, "__gc");
        lua_pushcclosure(L, destroyer<T>, 0);
        lua_rawset(L, -3);

        lua_setglobal(L, name);
    }

    // Tinker Class Inheritence
    template<typename T, typename P>
    void class_inh(lua_State* L)
    {
        push_meta(L, class_name<T>::name());
        if(lua_istable(L, -1))
        {
            lua_pushstring(L, "__parent");
            push_meta(L, class_name<P>::name());
            lua_rawset(L, -3);
        }
        lua_pop(L, 1);
    }

    // Tinker Class Constructor
    template<typename T, typename F>
    void class_con(lua_State* L,F func)
    {
        push_meta(L, class_name<T>::name());
        if(lua_istable(L, -1))
        {
            lua_newtable(L);
            lua_pushstring(L, "__call");
            lua_pushcclosure(L, func, 0);
            lua_rawset(L, -3);
            lua_setmetatable(L, -2);
        }
        lua_pop(L, 1);
    }

    // Tinker Class Functions
    template<typename T, typename F>
    void class_def(lua_State* L, const char* name, F func) 
    { 
        push_meta(L, class_name<T>::name());
        if(lua_istable(L, -1))
        {
            lua_pushstring(L, name);
            new(lua_newuserdata(L,sizeof(F))) F(func);
			push_functor(L, func);
            lua_rawset(L, -3);
        }
        lua_pop(L, 1);
    }

    // Tinker Class Variables
    template<typename T, typename BASE, typename VAR>
    void class_mem(lua_State* L, const char* name, VAR BASE::*val) 
    { 
        push_meta(L, class_name<T>::name());
        if(lua_istable(L, -1))
        {
            lua_pushstring(L, name);
            new(lua_newuserdata(L,sizeof(mem_var<BASE,VAR>))) mem_var<BASE,VAR>(val);
            lua_rawset(L, -3);
        }
        lua_pop(L, 1);
    }

    template<typename T>
    struct class_name
    {
        // global name
        static const char* name(const char* name = NULL)
        {
            static char temp[256] = "";
            if (name != NULL) strncpy(temp, name, sizeof(temp)-1);
            return temp;
        }
    };

    // Table Object on Stack
    struct table_obj
    {
        table_obj(lua_State* L, int index);
        ~table_obj();

        void inc_ref();
        void dec_ref();

        bool validate();

        template<typename T>
        void set(const char* name, T object)
        {
            if(validate())
            {
                lua_pushstring(m_L, name);
                push(m_L, object);
                lua_settable(m_L, m_index);
            }
        }

        template<typename T>
        T get(const char* name)
        {
            if(validate())
            {
                lua_pushstring(m_L, name);
                lua_gettable(m_L, m_index);
            }
            else
            {
                lua_pushnil(m_L);
            }

            return pop<T>(m_L);
        }

        template<typename T>
        T get(int num)
        {
            if(validate())
            {
                lua_pushinteger(m_L, num);
                lua_gettable(m_L, m_index);
            }
            else
            {
                lua_pushnil(m_L);
            }

            return pop<T>(m_L);
        }

        lua_State*      m_L;
        int             m_index;
        const void*     m_pointer;
        int             m_ref;
    };

    // Table Object Holder
    struct table
    {
        table(lua_State* L);
        table(lua_State* L, int index);
        table(lua_State* L, const char* name);
        table(const table& input);
        ~table();

        template<typename T>
        void set(const char* name, T object)
        {
            m_obj->set(name, object);
        }

        template<typename T>
        T get(const char* name)
        {
            return m_obj->get<T>(name);
        }

        template<typename T>
        T get(int num)
        {
            return m_obj->get<T>(num);
        }

        table_obj*      m_obj;
    };

} // namespace lua_tinker

typedef lua_tinker::table LuaTable;

#endif //_LUA_TINKER_H_
