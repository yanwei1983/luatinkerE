#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;

void test_member_func(lua_State* L)
{



	g_test_func_set["test_lua_member_func_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_1()
					local pFF = get_gff_ptr();
					return pFF:test_memfn();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_func_1");
	};
	g_test_func_set["test_lua_member_func_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_2()
					local pFF = get_gff_cref();
					return pFF:test_const();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_func_2");
	};
	g_test_func_set["test_lua_member_func_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_3()
					local pFF = get_gff_ptr();
					return pFF:test_const();	--ptr->const member
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_func_3");
	};
#ifdef LUATINKER_USERDATA_CHECK_CONST
	g_test_func_set["test_lua_member_func_4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_4()
					local pFF = get_gff_cref();
					pFF:test_memfn();	--error const_ptr -> member_func
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::set_error_callback(L, [](lua_State *L) -> int
		{
			std::string errinfo(lua_tostring(L, -1));
			if (errinfo != "const class_ptr ff can't invoke non-const member func.")
			{
				lua_tinker::on_error(L);
			}
			return 0;
		});
		lua_tinker::call<void>(L, "test_lua_member_func_4");
		lua_tinker::set_error_callback(L, &lua_tinker::on_error);

		return true;

	};
#endif
	g_test_func_set["test_lua_member_func_5"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_5()
					local pFF = get_gff_ptr();
					pFF:setVal(55);
					return pFF:getVal();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 55 == lua_tinker::call<int>(L, "test_lua_member_func_5");
	};

	g_test_func_set["test_lua_member_func_6"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_6()
					local pFF = get_gff_ptr();
					local ff = ff(44);
					pFF:setVal(0);
					return pFF:add_ffptr(ff);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 44 == lua_tinker::call<int>(L, "test_lua_member_func_6");
	};;



}