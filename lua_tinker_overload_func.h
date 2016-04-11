#ifndef __LUA_TINKER_OVERLOAD_FUNC_H
#define __LUA_TINKER_OVERLOAD_FUNC_H
#include"lua_tinker.h"
#include<map>
#include<memory>
#include<assert.h>
namespace lua_tinker
{


	namespace detial
	{
		template<typename Func, std::size_t index>
		constexpr static int get_func_argv()
		{
			return _stack_help<typename function_traits<Func>::template argv<index>::type>::cover_to_lua_type();
		}

		template<typename Func, std::size_t args_num>
		struct count_all_func_argv
		{
			constexpr static const long long result = ((get_func_argv<Func, (args_num - 1)>() << ((args_num - 1) * 4)) + count_all_func_argv<Func, (args_num - 1)>::result);
		};
		template<typename Func>
		struct count_all_func_argv<Func, 1>
		{
			constexpr static const long long result = get_func_argv<Func, 0>();
		};

		template<typename Func>
		struct count_all_func_argv<Func, 0>
		{
			constexpr static const long long result = 0;
		};

		template<typename Func>
		constexpr static long long get_all_func_argv()
		{
			return count_all_func_argv<Func, function_traits<Func>::argc>::result;
		}

		template<typename T>
		struct function_signature
		{
			static constexpr const long long m_sig = get_all_func_argv<T>();
		};
		


		void _set_signature(long long& sig, size_t idx, unsigned char c)
		{
			if (idx > sizeof(sig) * 2)
				return;
			sig = (sig & ~(0xF << (idx * 4))) | ((c & 0xF) << (idx * 4));
		}
	};

	struct args_type_overload_functor_base
	{
		typedef std::map<long long, std::shared_ptr<functor_base> > overload_funcmap_t;
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
				detial::_set_signature(sig, i, lua_type(L, i + m_nParamsOffset + 1));
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
			args_type_overload_functor_base* pFunctor = upvalue_<args_type_overload_functor_base*>(L);
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
			bool bInsertd = false;
			std::tie(std::ignore, bInsertd) = m_overload_funcmap.emplace(detial::function_signature<F>::m_sig, std::shared_ptr<functor_base>(make_functor_ptr(f)));
			assert(bInsertd == true);
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
			bool bInsertd = false;
			std::tie(std::ignore, bInsertd) = m_overload_funcmap.emplace(detial::function_signature<F>::m_sig, std::shared_ptr<functor_base>(make_member_functor_ptr(f)) );
			assert(bInsertd == true);
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
				bool bInsertd = false;
				std::tie(std::ignore, bInsertd) = map.emplace(detial::function_signature<void(Args...)>::m_sig, std::shared_ptr<functor_base>(new constructor<T, Args...>()));
				assert(bInsertd == true);
			}
		};
			
		

	};











};

#endif//__LUA_TINKER_OVERLOAD_FUNC_H