#include "lua_tinker.h"
#include"test.h"


std::string g_teststring = "test";
std::string push_string()
{
	return g_teststring;
}

std::string connect_string(std::string str1, const std::string& str2, const std::string& str3)
{
	return str1 + str2 + str3;
}

const std::string& push_string_ref()
{
	return g_teststring;
}

LUA_TEST(string)
{


	g_test_func_set["test_lua_string_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_string_1()
					return push_string();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return g_teststring == lua_tinker::call<std::string>(L, "test_lua_string_1");
	};
	g_test_func_set["test_lua_string_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_string_2(string)
					return connect_string(push_string(), push_string_ref(), string);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return (g_teststring + g_teststring + g_teststring) == lua_tinker::call<std::string>(L, "test_lua_string_2", g_teststring);
	};



}