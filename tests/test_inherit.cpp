#include "lua_tinker.h"

extern std::map<std::string, std::function<bool()> > g_test_func_set;

void test_inherit(lua_State* L)
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

#ifdef LUATINKER_MULTI_INHERITANCE
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

#endif



}