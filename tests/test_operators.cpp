#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;

void test_lua_intoptest(lua_State* L)
{


	g_test_func_set["test_lua_intoptest1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_intoptest1()
					local Intop1 = IntOpTest(1);
					local Intop2 = IntOpTest(2);
					local Intop3 = Intop1 + Intop2;
					return Intop3.m_n == (1+2);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_lua_intoptest1");
	};

	g_test_func_set["test_lua_intoptest2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_intoptest2()
					local Intop1 = IntOpTest(7);
					local Intop2 = IntOpTest(4);
					local Intop3 = Intop1 - Intop2;
					return Intop3.m_n == (7-4);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_lua_intoptest2");
	};

	g_test_func_set["test_lua_intoptest3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_intoptest3()
					local Intop1 = IntOpTest(3);
					local Intop2 = IntOpTest(5);
					local Intop3 = Intop1 * Intop2;
					return Intop3.m_n == (3*5);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_lua_intoptest3");
	};

	g_test_func_set["test_lua_intoptest4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_intoptest4()
					local Intop1 = IntOpTest(99);
					local Intop2 = IntOpTest(3);
					local Intop3 = Intop1 // Intop2;
					return Intop3.m_n == (99//3);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_lua_intoptest4");
	};

	g_test_func_set["test_lua_intoptest5"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_intoptest5()
					local Intop1 = IntOpTest(99);
					local Intop2 = IntOpTest(3);
					return (Intop1 < Intop2) == (99 < 3);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_lua_intoptest5");
	};

	g_test_func_set["test_lua_intoptest6"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_intoptest6()
					local Intop1 = IntOpTest(4);
					local Intop2 = IntOpTest(8);
					return (Intop1 <= Intop2) == (4 <= 8);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_lua_intoptest6");
	};

	g_test_func_set["test_lua_intoptest7"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_intoptest7()
					local Intop1 = IntOpTest(8);
					local Intop2 = IntOpTest(8);
					return Intop1 == Intop2;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_lua_intoptest7");
	};
}