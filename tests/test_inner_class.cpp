#include "lua_tinker.h"
#include"test.h"


LUA_TEST(inner_class)
{
	g_test_func_set["test_lua_inner_class1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_inner_class1()
					return 1 == ff.inner.test_static_func(1);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_inner_class1");
	};
	g_test_func_set["test_lua_inner_class2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_inner_class2()
					return ff.visit_inner_ptr(ff.get_inner_ptr());
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_inner_class2");
	};


}