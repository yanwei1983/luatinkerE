#include "lua_tinker.h"

extern std::map<std::string, std::function<bool()> > g_test_func_set;

int test_default_params(int a, int b, int c)
{
	return a + b - c;
}


void test_default_params(lua_State* L)
{
	g_test_func_set["lua_test_default_params1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_default_params1()
					return test_default_params(7);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  (7 + 5 - 8) == lua_tinker::call<int>(L, "lua_test_default_params1");
	};
	g_test_func_set["lua_test_default_params2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_default_params2()
					return test_default_params(7,3);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  (7 + 3 - 8) == lua_tinker::call<int>(L, "lua_test_default_params2");
	};
	g_test_func_set["lua_test_default_params3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_default_params3()
					local pFF = ff();
					return pFF:test_default_params(7);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  (7 + 5 - 8) == lua_tinker::call<int>(L, "lua_test_default_params3");
	};
	g_test_func_set["lua_test_default_params4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_default_params4()
					local pFF = ff();
					return pFF:test_default_params(7,3);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  (7 + 3 - 8) == lua_tinker::call<int>(L, "lua_test_default_params4");
	};

	g_test_func_set["lua_test_con_default_params_0"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_con_default_params_0()
					local pTest = TestCon();
					return pTest.m_nVal == 7 and pTest:getStr() == "aa" and pTest.m_fVal == -1.0;
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  true == lua_tinker::call<bool>(L, "lua_test_con_default_params_0");
	};

	g_test_func_set["lua_test_con_default_params_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_con_default_params_1()
					local pTest = TestCon(-2.0);
					return pTest.m_nVal == 7 and pTest:getStr() == "aa" and pTest.m_fVal == -2.0;
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  true == lua_tinker::call<bool>(L, "lua_test_con_default_params_1");
	};

	g_test_func_set["lua_test_con_default_params_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_con_default_params_2()
					local pTest = TestCon(-2.0,"tt");
					return pTest.m_nVal == 7 and pTest:getStr() == "tt" and pTest.m_fVal == -2.0;
					
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  true == lua_tinker::call<bool>(L, "lua_test_con_default_params_2");
	};

	g_test_func_set["lua_test_con_default_params_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_con_default_params_3()
					local pTest = TestCon(-2.0, "tt", 1);
					return pTest.m_nVal == 1 and pTest:getStr() == "tt" and pTest.m_fVal == -2.0;
					
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  true == lua_tinker::call<bool>(L, "lua_test_con_default_params_3");
	};
}