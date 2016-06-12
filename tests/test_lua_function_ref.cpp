#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;

lua_tinker::lua_function_ref<int> g_lua_func_ref;

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


	g_test_func_set["test_lua_luafunction_ref_3"] = [L]()->bool
	{
		std::string luabuf =
			R"( g_test_func_table={test=1,};
				function g_test_func_table.func1(val)
					return val + 1;
				end;
				
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::table_onstack func_table(L, "g_test_func_table");
		lua_tinker::lua_function_ref<int> lua_func = func_table.get<decltype(lua_func)>("func1");
		return 7 == lua_func(6);
	};



		

	g_test_func_set["test_lua_luafunction_ref_6"] = [L]()->bool
	{
		std::string luabuf =
			R"(
				test_lua_luafunction_ref_6 = {
					[1]=10, test1=5,
					testFunc1 = function(param1)
						return {
							[1]=3, test2=22,
							testFunc2 = function()
								return param1;
							end,
						};
					end,
				};
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::table_onstack table(L, "test_lua_luafunction_ref_6");
		lua_tinker::lua_function_ref<lua_tinker::table_ref> my_func_ref = table.get<decltype(my_func_ref)>("testFunc1");

		lua_tinker::table_ref tt_ref = my_func_ref("test_upval");
		lua_tinker::table_onstack tt = tt_ref.push_table_to_stack();

		lua_tinker::lua_function_ref<std::string> ref = tt.get<decltype(ref)>("testFunc2");
		return tt.get<int>(1) == 3 && ref() == "test_upval";
	};


	g_test_func_set["test_lua_luafunction_ref_7"] = [L]()->bool
	{
		std::string luabuf =
			R"(
				test_lua_luafunction_ref_7 = {
					[1]=10, test1=5,
					testFunc1 = function(param1)
						return {
							[1]=3, test2=22,
							testFunc2 = function()
								return {
									{key1="key1_1", key2="key1_2", key3="key1_3"},
									{key1="key2_1", key2="key2_2", key3="key2_3"},
								};
							end,
						};
					end,
				};
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::table_onstack table(L, "test_lua_luafunction_ref_7");
		lua_tinker::lua_function_ref<lua_tinker::table_onstack> my_func_ref = table.get<decltype(my_func_ref)>("testFunc1");

		lua_tinker::table_onstack tt = my_func_ref("test_upval");

		lua_tinker::lua_function_ref<lua_tinker::table_onstack> fun2 = tt.get<decltype(fun2)>("testFunc2");
		lua_tinker::table_onstack tt2 = fun2();
		lua_tinker::table_onstack tt2_1 = tt2.get<decltype(tt2_1)>(1);
		std::string key1_1 = tt2_1.get<std::string>("key1");

		std::vector<std::map<std::string, std::string>> datamap = tt2.convertto<decltype(datamap)>();
		

		return key1_1 == "key1_1" && datamap[0]["key2"] == "key1_2" && datamap[1]["key3"] == "key2_3";
	};



	std::string luabuf =
		R"( 	function g_lua_func_test(val)
					return val + 1;
				end;
				
			)";
	lua_tinker::dostring(L, luabuf.c_str());
	g_test_func_set["test_lua_luafunction_ref_4"] = [L]()->bool
	{
		if (g_lua_func_ref.empty())
		{
			g_lua_func_ref = lua_tinker::get<decltype(g_lua_func_ref)>(L, "g_lua_func_test");
			lua_tinker::register_lua_close_callback(L, [](lua_State* L)
															{ g_lua_func_ref.reset(); });
		}

		return g_lua_func_ref(6) == 7;
	};

	g_test_func_set["test_lua_luafunction_ref_5"] = [L]()->bool
	{
		if (g_lua_func_ref.empty())
		{
			g_lua_func_ref = lua_tinker::get<decltype(g_lua_func_ref)>(L, "g_lua_func_test");
			lua_tinker::register_lua_close_callback(L, [](lua_State* L)
														{ g_lua_func_ref.reset(); });
		}
		return g_lua_func_ref(8) == 9;
	};
}