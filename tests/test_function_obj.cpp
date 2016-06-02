#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;



int test_lua_function(std::function<int(int)> func)
{
	return func(1);
}
int test_lua_function_ref(const std::function<int(int)>& func)
{
	return func(1);
}

std::function<int(int)> get_c_function()
{
	auto func = [](int v)->int
	{
		return v + 1;
	};
	return std::function<int(int)>(func);
}


//func must be release before lua close.....user_conctrl
std::function<int(int)> g_func_lua;
void store_lua_function(std::function<int(int)> func)
{
	g_func_lua = func;
}

int use_stored_lua_function()
{
	return g_func_lua(1);
}

void test_function_obj(lua_State* L)
{



	g_test_func_set["test_lua_stdfunction_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_stdfunction_1()
					return std_function_int_int(2,3);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return (2 + 3) == lua_tinker::call<int>(L, "test_lua_stdfunction_1");
	};
	g_test_func_set["test_lua_stdfunction_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_stdfunction_2()
					return std_function_int_bind88(2);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return (2 + 88) == lua_tinker::call<int>(L, "test_lua_stdfunction_2");
	};


	g_test_func_set["test_lua_funobj_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_1()
					local upval = 0;
					local localtest = function(intval)
						upval = upval+ intval;
						return upval;
					end
					;
					return 1 == test_lua_function(localtest);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_funobj_1");
	};

	g_test_func_set["test_lua_funobj_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_2()
					local upval = 0;
					local localtest = function(intval)
						upval = upval+ intval;
						return upval;
					end
					test_lua_function_ref(localtest);
					return 2 == test_lua_function_ref(localtest);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_funobj_2");
	};

	g_test_func_set["test_lua_funobj_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_3()
					local upval = 0;
					local localtest = function (intval)
						upval = upval+ intval;
						return upval;
					end
					return 1 == test_lua_function(function (intval)
														return localtest(intval);
														end);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_funobj_3");
	};

	g_test_func_set["test_lua_funobj_4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_4()
					local localtest = function (intval)
						return intval +1;
					end
					store_lua_function(localtest);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::call<void>(L, "test_lua_funobj_4");
		return g_func_lua != nullptr;
	};

	g_test_func_set["test_lua_funobj_5"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_5()
					local ret = use_stored_lua_function();
					ret = ret + use_stored_lua_function();
					return ret == 4;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return  lua_tinker::call<bool>(L, "test_lua_funobj_5");;
	};


	g_test_func_set["test_lua_funobj_6"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_6()
					local c_func = get_c_function();
					return c_func(1) == 2;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_lua_funobj_6");
	};

	g_test_func_set["test_lua_funobj_7"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_7()
					local localtest = function (intval)
						return intval +1;
					end
					return localtest;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		std::function<int(int)> func = lua_tinker::call<decltype(func)>(L, "test_lua_funobj_7");
		return 7 == func(6);
	};

	g_test_func_set["test_lua_funobj_8"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_8()
					local c_func = get_c_function();
					local pTest = TestCon();
					return pTest:TestFuncObj(c_func) == (7+1);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_lua_funobj_8");
	};


}