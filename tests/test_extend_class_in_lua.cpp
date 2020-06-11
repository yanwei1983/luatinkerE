#include "lua_tinker.h"
#include"test.h"


LUA_TEST(extend_class_in_lua)
{
	g_test_func_set["test_lua_extend_class_in_lua"] = [L]()->bool
	{
		std::string luabuf =
			R"( function ff:f_in_lua(x)
				   return self:getVal() + x;
				end
				function test_lua_extend_class_in_lua()
					local local_ff = ff();
					return local_ff:f_in_lua(3) == (local_ff:getVal() + 3);
				end
				
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_lua_extend_class_in_lua");
	};
}