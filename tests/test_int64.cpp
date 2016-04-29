#include "lua_tinker.h"
#include"test.h"

extern std::map<std::string, std::function<bool()> > g_test_func_set;



unsigned long long addUL(unsigned long long a, unsigned long long b)
{
	return a + b;
}

long long Number2Interger(double v)
{
	return (long long)(v);
}


void test_int64(lua_State* L)
{


	g_test_func_set["test_lua_int64_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_int64_1()
					local ul_a = addUL(0x8000000000000000, 1);
					return ul_a;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return (0x8000000000000000 + 1) == lua_tinker::call<unsigned long long>(L, "lua_test_int64_1");

	};
	g_test_func_set["test_lua_int64_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_int64_2()
					local ul_b = Number2Interger(2^63)+0x1;
					return ul_b;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return (0x8000000000000001) == lua_tinker::call<unsigned long long>(L, "lua_test_int64_2");
	};
	g_test_func_set["test_lua_int64_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_int64_3()
					local ul_a = addUL(0x8000000000000000, 1);
					local ul_b = Number2Interger(2^63)+0x1;
					return ul_a + ul_b;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		static constexpr const unsigned long long result = (unsigned long long)(0x8000000000000001) + ((unsigned long long)(0x8000000000000000) + 1);
		return result == lua_tinker::call<unsigned long long>(L, "lua_test_int64_3");
	};


}