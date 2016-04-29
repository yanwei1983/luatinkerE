#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;

void test_luafunction_ref(lua_State* L)
{
	g_test_func_set["test_lua_luafunction_ref_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_luafunction_ref_1()
					local upval = 0;
					local localtest = function (intval)
						upval = upval + intval;
						return upval;
					end
					return localtest;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		lua_tinker::lua_function_ref<int> lua_func = lua_tinker::call<decltype(lua_func)>(L, "test_lua_luafunction_ref_1");
		lua_func(7);
		return 14 == lua_func(7);
	};
	g_test_func_set["test_lua_luafunction_ref_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_luafunction_ref_2_1()
					local upval = 0;
					local localtest = function (intval)
						upval = upval + intval;
						return upval;
					end
					return localtest;
				end
				function test_lua_luafunction_ref_2_2(func)
					func(7);
					return func(7);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		lua_tinker::lua_function_ref<int> lua_func = lua_tinker::call<decltype(lua_func)>(L, "test_lua_luafunction_ref_2_1");
		return 14 == lua_tinker::call<int>(L, "test_lua_luafunction_ref_2_2", lua_func);
	};



}