#ifndef __LUA_TINKER_OVERLOAD_FUNC_H
#define __LUA_TINKER_OVERLOAD_FUNC_H
#include"lua_tinker.h"
#include<map>

namespace lua_tinker
{


	struct function_signature
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
		constexpr static long long get_all_func_argv(Func)
		{
			return count_all_func_argv<Func, function_traits<Func>::argc>::result;
		}

		template<typename RVal, typename ... Args>
		function_signature(RVal(*func)(Args...))
			:m_sig(get_all_func_argv(func))
		{
		}
		template<typename RVal, typename CT, typename ... Args>
		function_signature(RVal(CT::*func)(Args...))
			: m_sig(get_all_func_argv(func))
		{
		}

		function_signature()
		{}

		bool operator <(const function_signature& rht) const
		{
			return m_sig < rht.m_sig;
		}

		void set(size_t idx, unsigned char c)
		{
			if (idx > sizeof(m_sig) * 2)
				return;
			m_sig = (m_sig & ~(0xF << (idx * 4))) | ((c & 0xF) << (idx * 4));
		}
		long long m_sig = 0;
	};

	struct args_type_overload_functor_base
	{

		std::map<function_signature, functor_base*> m_overload_funcmap;
		int m_nParamsOffset = 0;

		args_type_overload_functor_base()
		{
		}

		virtual ~args_type_overload_functor_base()
		{
			for (auto it : m_overload_funcmap)
			{
				delete it.second;
			}
		}
		args_type_overload_functor_base(args_type_overload_functor_base&& rht)
			:m_nParamsOffset(rht.m_nParamsOffset)
			, m_overload_funcmap(rht.m_overload_funcmap)
		{
			rht.m_overload_funcmap.clear();
		}


		int apply(lua_State* L)
		{
			function_signature sig;
			int nParamsCount = lua_gettop(L) - m_nParamsOffset;
			for (int i = 0; i < nParamsCount; i++)
			{
				sig.set(i, lua_type(L, i + m_nParamsOffset + 1));
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
		template<typename ...F>
		args_type_overload_functor(F ... f)
		{
			m_overload_funcmap = { { function_signature(f), make_functor_ptr(f) }... };
		}
	};
	struct args_type_overload_member_functor : public args_type_overload_functor_base
	{
		template<typename ...F>
		args_type_overload_member_functor(F ... f)
		{
			m_overload_funcmap = { { function_signature(f), make_member_functor_ptr(f) }... };
			m_nParamsOffset = 1;
		}
	};











};

#endif//__LUA_TINKER_OVERLOAD_FUNC_H