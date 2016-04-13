#ifndef __LUA_TINKER_OVERLOAD_FUNC_H
#define __LUA_TINKER_OVERLOAD_FUNC_H
#include"lua_tinker.h"
#include<map>
#include<memory>
#include<assert.h>
namespace lua_tinker
{

	//convert args to luatype store in longlong
	//so void(int) = LUA_TNUMBER = 0x3, void(double) = LUA_TNUMBER = 0x3, they have the same signature,
	//void(int,a*) = LUA_TNUMBER,LUA_TUSERDATA = 0x73 , void(long long,b*) = LUA_TNUMBER,LUA_TUSERDATA = 0x73 , they have the same signature,
	//and use longlong to stroe,so we only support 16 args

	namespace detail
	{
		template<typename Func, std::size_t index>
		constexpr int get_func_argv()
		{
			return _stack_help<typename function_traits<Func>::template argv<index>::type>::cover_to_lua_type();
		}

		template<typename Func, std::size_t args_num>
		struct count_all_func_argv
		{
			static constexpr const long long result = ((get_func_argv<Func, (args_num - 1)>() << ((args_num - 1) * 4)) + count_all_func_argv<Func, (args_num - 1)>::result);
		};
		template<typename Func>
		struct count_all_func_argv<Func, 1>
		{
			static constexpr const long long result = get_func_argv<Func, 0>();
		};

		template<typename Func>
		struct count_all_func_argv<Func, 0>
		{
			static constexpr const long long result = 0;
		};

		template<typename T>
		struct function_signature
		{
			static constexpr const long long m_sig = count_all_func_argv<T, function_traits<T>::argc>::result;
		};
		


		void _set_signature(long long& sig, size_t idx, unsigned char c)
		{
			if (idx > sizeof(sig) * 2)
				return;
			sig = (sig & ~(0xF << (idx * 4))) | ((c & 0xF) << (idx * 4));
		}



		template <typename RVal, typename ... Args>
		decltype(auto) make_functor_ptr(RVal(func)(Args...))
		{
			using Functor_Warp = functor<RVal, Args...>;
			return new Functor_Warp(func);
		}

		template<typename CT, typename RVal, typename ... Args>
		decltype(auto) make_member_functor_ptr(RVal(CT::*func)(Args...))
		{
			using Functor_Warp = member_functor<false, CT, RVal, Args...>;
			return new Functor_Warp(func);
		}
		template<typename CT, typename RVal, typename ... Args>
		decltype(auto) make_member_functor_ptr(RVal(CT::*func)(Args...)const)
		{
			using Functor_Warp = member_functor<true, CT, RVal, Args...>;
			return new Functor_Warp(func);
		}
	};

	struct args_type_overload_functor_base
	{
		typedef std::map<long long, std::shared_ptr<detail::functor_base> > overload_funcmap_t;
		overload_funcmap_t m_overload_funcmap;
		int m_nParamsOffset = 0;

		args_type_overload_functor_base()
		{
		}

		virtual ~args_type_overload_functor_base()
		{
			m_overload_funcmap.clear();
		}
		args_type_overload_functor_base(args_type_overload_functor_base&& rht)
			:m_nParamsOffset(rht.m_nParamsOffset)
			, m_overload_funcmap(rht.m_overload_funcmap)
		{
			rht.m_overload_funcmap.clear();
		}


		int apply(lua_State* L)
		{
			long long sig = 0;
			int nParamsCount = lua_gettop(L) - m_nParamsOffset;
			for (int i = 0; i < nParamsCount; i++)
			{
				detail::_set_signature(sig, i, lua_type(L, i + m_nParamsOffset + 1));
			}
			auto itFind = m_overload_funcmap.find(sig);
			if (itFind != m_overload_funcmap.end())
				return itFind->second->apply(L);
			else
			{
				//signature mismatch
				lua_pushfstring(L, "overload function args mismatch");
				lua_error(L);
				return -1;
			}
		}

		static int invoke_function(lua_State* L)
		{
			args_type_overload_functor_base* pFunctor = detail::upvalue_<args_type_overload_functor_base*>(L);
			return pFunctor->apply(L);
		}
	};


	struct args_type_overload_functor : public args_type_overload_functor_base
	{
		template<typename ...Args>
		args_type_overload_functor(Args&&... args)
		{
			emplace(std::forward<Args&&>(args)...);
		}

		template<typename F>
		void emplace(F f)
		{
			constexpr long long sig = detail::function_signature<F>::m_sig;
			m_overload_funcmap.emplace(sig, std::shared_ptr<detail::functor_base>(detail::make_functor_ptr(f)));
		}
		template<typename T,typename... Args>
		void emplace(T f, Args...args)
		{
			emplace(f); emplace(args...);
		}
	};
	struct args_type_overload_member_functor : public args_type_overload_functor_base
	{
		template<typename ...Args>
		args_type_overload_member_functor(Args&&... args)
		{
			emplace(std::forward<Args&&>(args)...);
			m_nParamsOffset = 1;
		}

		template<typename F>
		void emplace(F&& f)
		{
			constexpr long long sig = detail::function_signature<F>::m_sig;
			m_overload_funcmap.emplace(sig, std::shared_ptr<detail::functor_base>(detail::make_member_functor_ptr(f)) );
		}
		template<typename T, typename... Args>
		void emplace(T f, Args...args)
		{
			emplace(f); emplace(args...);
		}
	};



	struct args_type_overload_constructor : public args_type_overload_functor_base
	{
		template<typename ...Args>
		args_type_overload_constructor(Args&&...)
		{
			emplace<Args...>::apply(m_overload_funcmap);
			m_nParamsOffset = 1;
		}

		template<typename T, typename... Args>
		struct emplace
		{
			static void apply(overload_funcmap_t& map)
			{
				emplace<T>::apply(map);
				emplace<Args...>::apply(map);
			}
		};

		template<typename T, typename ...Args>
		struct emplace< constructor<T,Args...> >
		{
			static void apply(overload_funcmap_t& map)
			{
				constexpr const long long sig = detail::function_signature<void(Args...)>::m_sig;
				map.emplace(sig, std::shared_ptr<detail::functor_base>(new constructor<T, Args...>()));
			}
		};
			
		

	};

	namespace detail
	{
		template<typename overload_functor>
		auto _push_functor(lua_State* L, overload_functor&& functor)->
			typename std::enable_if<std::is_base_of<args_type_overload_functor_base, overload_functor>::value, void>::type
		{
			new(lua_newuserdata(L, sizeof(overload_functor))) overload_functor(std::forward<overload_functor>(functor));
			//register functor
			{
				lua_newtable(L);
				lua_pushstring(L, "__gc");
				lua_pushcclosure(L, &destroyer<overload_functor>, 0);
				lua_rawset(L, -3);
				lua_setmetatable(L, -2);
			}

			lua_pushcclosure(L, &overload_functor::invoke_function, 1);
		}

		template<typename overload_functor>
		auto _push_calss_functor(lua_State* L, overload_functor&& functor)->
			typename std::enable_if<std::is_base_of<args_type_overload_functor_base, overload_functor>::value, void>::type
		{
			new(lua_newuserdata(L, sizeof(overload_functor))) overload_functor(std::forward<overload_functor>(functor));
			//register metatable for gc
			{
				lua_newtable(L);
				lua_pushstring(L, "__gc");
				lua_pushcclosure(L, &destroyer<overload_functor>, 0);
				lua_rawset(L, -3);

				lua_setmetatable(L, -2);
			}
			lua_pushcclosure(L, &overload_functor::invoke_function, 1);
		}
	}









};

#endif//__LUA_TINKER_OVERLOAD_FUNC_H