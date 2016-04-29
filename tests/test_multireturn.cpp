#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;

void test_multireturn(lua_State* L)
{
	g_test_func_set["test_lua_multireturn"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_multireturn()
					return 1,2.0,3,4.0,"5"
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		int c = 0;
		double d = 0.0;
		char e = 0;
		float f = 0.0;
		std::string g;
		std::tie(c, d, e, f, g) = lua_tinker::call< std::tuple<int, double, char, float, std::string> >(L, "test_lua_multireturn");

		return c == 1 && d == 2.0 && e == 3 && f == 4.0 && g == "5";
	};

}