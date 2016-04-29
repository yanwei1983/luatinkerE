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



}