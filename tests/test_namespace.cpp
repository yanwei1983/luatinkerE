#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;

void test_namespace(lua_State* L)
{
	g_test_func_set["test_lua_namespace1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_namespace1()
					return 11 == NS_TEST.test_function_in_namespace(11);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_namespace1");
	};
	g_test_func_set["test_lua_namespace2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_namespace2()
					local pTest = NS_TEST.NS_INNER.Test.getStaticFunc();
					return pTest:IsEqual(pTest:getIterator());
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_namespace2");
	};

	g_test_func_set["test_lua_namespace3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_namespace3()
					local pTest = NS_TEST.NS_INNER.Test.getStaticFunc();
					return pTest:getIterator().ENUM_T.ENUM_1;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return NS_TEST::NS_INNER::Test::Iterator::ENUM_T::ENUM_1 == lua_tinker::call<NS_TEST::NS_INNER::Test::Iterator::ENUM_T>(L, "test_lua_namespace3");
	};

	g_test_func_set["test_lua_namespace4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_namespace4()
					return NS_TEST.ENUM_1;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return NS_TEST::ENUM_1 == lua_tinker::call<NS_TEST::ENUM_T>(L, "test_lua_namespace4");
	};



}