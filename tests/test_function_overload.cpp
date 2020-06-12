#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;




void test_overload_err(const char* n)
{
}


void test_overload_err(const std::string& n)
{
}

int test_overload(int n)
{
	return n;
}

int test_overload(double d)
{
	return int(d);
}


int test_overload(int n, double d)
{
	return n + (int)d;
}

int test_overload(int n1, int n2, double d)
{
	return n1 + n2 + (int)d;
}



int test_overload_default(int n, bool b)
{
	return n;
}

int test_overload_default(int n1, int n2, bool b)
{
	return n1 + n2;
}


int test_overload_default(int n1, int n2, int n3, double d1/*=1.0*/, double d2 /*= 2.0*/, double d3 /*= 3.0*/, double d4 /*= 4.0*/, const std::string& refString /*= std::string("test")*/)
{
	return n1 + n2;
}


void test_overloadfunc(lua_State* L)
{

	g_test_func_set["test_lua_coverloadfunc_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_coverloadfunc_1()
					return test_overload(1) == 1;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_coverloadfunc_1");
	};
	g_test_func_set["test_lua_coverloadfunc_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_coverloadfunc_2()
					return test_overload(1,2.0) == 3;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_coverloadfunc_2");
	};
	g_test_func_set["test_lua_coverloadfunc_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_coverloadfunc_3()
					return test_overload(1,2,3.0) == 6;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_coverloadfunc_3");
	};
	g_test_func_set["test_lua_coverloadfunc_4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_coverloadfunc_4()
					return test_overload_default(1,2) == 3;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_coverloadfunc_4");
	};
	g_test_func_set["test_lua_coverloadfunc_5"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_coverloadfunc_5()
					return test_overload_default(1,true) == 1;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_coverloadfunc_5");
	};
	g_test_func_set["test_lua_coverloadfunc_6"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_coverloadfunc_6()
					return test_overload_default(1,2,false) == 3;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_coverloadfunc_6");
	};
	g_test_func_set["test_lua_coverloadfunc_7"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_coverloadfunc_7()
					return test_overload_default(1,2,3) == 3;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_coverloadfunc_7");
	};

	g_test_func_set["test_lua_coverloadfunc_err_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_coverloadfunc_err_1()
					test_overload_err("1");
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::set_error_callback(L, [](lua_State *L) -> int
		{
			std::string errinfo(lua_tostring(L, -1));
			if (errinfo != "function(CLT_STRING) overload resolution more than one")
			{
				lua_tinker::on_error(L);
			}
			return 0;
		});
		lua_tinker::call<void>(L, "test_lua_coverloadfunc_err_1");
		lua_tinker::set_error_callback(L, &lua_tinker::on_error);

		return true;

	};



	g_test_func_set["test_lua_member_overloadfunc_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overloadfunc_1()
					local pFF = get_gff_ptr();
					return pFF:test_overload(1);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 1 == lua_tinker::call<int>(L, "test_lua_member_overloadfunc_1");
	};
	g_test_func_set["test_lua_member_overloadfunc_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overloadfunc_2()
					local pFF = get_gff_ptr();
					return pFF:test_overload(1,2.0);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 3 == lua_tinker::call<int>(L, "test_lua_member_overloadfunc_2");
	};
	g_test_func_set["test_lua_member_overloadfunc_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overloadfunc_3()
					local pFF = get_gff_ptr();
					return pFF:test_overload(1,2,3.0);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 6 == lua_tinker::call<int>(L, "test_lua_member_overloadfunc_3");
	};
	g_test_func_set["test_lua_member_overload_con_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overload_con_1()
					local ff = ff(1.1,2,5);
					return ff.m_val == 5;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_overload_con_1");
	};
	g_test_func_set["test_lua_member_overload_con_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overload_con_2()
					local ff = ff(1);
					return ff.m_val == 1;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_overload_con_2");
	};
	g_test_func_set["test_lua_member_overload_con_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overload_con_3()
					local ff = ff();
					return ff.m_val == 0;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_overload_con_3");
	};


}