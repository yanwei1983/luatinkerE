#include "lua_tinker.h"
#include "test.h"


LUA_TEST(inherit)
{
	g_test_func_set["test_lua_inherit_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_inherit_1()
					local local_ff =  ff();
					return visot_ffbase(local_ff);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_inherit_1");
	};
	g_test_func_set["test_lua_inherit_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_inherit_3()
					local pFF = get_gff_ptr();
					return pFF:test_base_callfn(1) == 1;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_inherit_3");
	};


	g_test_func_set["test_lua_inherit_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_inherit_2()
					local local_ff =  ff();
					return visot_ff_other_baseA(local_ff);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_inherit_2");
	};



	g_test_func_set["test_lua_inherit_4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_inherit_4()
					local pFF = get_gff_ptr();
					return pFF:test_other_callfn(1) == 1;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_inherit_4");
	};
	g_test_func_set["test_lua_inherit_5"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_inherit_5()
					local pFF = get_gff_ptr();
					pFF:no_name(1);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::set_error_callback(L, [](lua_State *L) -> int
		{
			std::string errinfo(lua_tostring(L, -1));
			if (errinfo != "can't find 'no_name' class variable. (forgot registering class variable ?)")
			{
				lua_tinker::on_error(L);
			}
			return 0;
		});

		lua_tinker::call<void>(L, "test_lua_inherit_5");
		lua_tinker::set_error_callback(L, &lua_tinker::on_error);
		return true;
	};


	g_test_func_set["test_lua_inherit_6"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_inherit_6(ff)
					return ff:get_ff_other() + ff:get_ff_other_base() + ff:get_ff_other_baseB();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		
		ff_other test;
		test.set_ff_other(1);
		test.set_ff_other_base(2);
		test.set_ff_other_baseB(3);
	
		return lua_tinker::call<int>(L, "test_lua_inherit_6", &test) == 6;
	};
	g_test_func_set["test_lua_inherit_7"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_inherit_7()
					local ff = ff_other();
					ff:set_ff_other(1);
					ff:set_ff_other_base(2);
					ff:set_ff_other_baseB(3);
					return ff:get_ff_other() + ff:get_ff_other_base() + ff:get_ff_other_baseB();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		
	
		return lua_tinker::call<int>(L, "test_lua_inherit_7") == 6;
	};
	g_test_func_set["test_lua_inherit_8"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_inherit_8()
					local ff = ff_other();
					return ff:get_ff_other() + ff:get_ff_other_base() + ff:get_ff_other_baseB();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		
	
		return lua_tinker::call<int>(L, "test_lua_inherit_8") == 0;
	};




}