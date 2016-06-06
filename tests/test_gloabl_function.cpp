#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;

int g_c_int = 0;
double g_c_double = 0.0;
void gint_add1()
{
	g_c_int++;
}
void gint_addint(int n)
{
	g_c_int += n;
}
void gint_addintptr(int* p)
{
	g_c_int += *p;
}
void gint_addintptr_const(const int* p)
{
	g_c_int += *p;
}
void gint_addintref(const int& ref)
{
	g_c_int += ref;
}
void gint_add_intref(int& ref, int n)
{
	ref += n;
}

void g_addint_double(int n1, double n2)
{
	g_c_int += n1;
	g_c_double += n2;
}

int get_gint()
{
	return g_c_int;
}
int& get_gintref()
{
	return g_c_int;
}
const int& get_gintref_const()
{
	return g_c_int;
}
int* get_gintptr()
{
	return &g_c_int;
}
const int* get_gintptr_const()
{
	return &g_c_int;
}
double get_gdouble()
{
	return g_c_double;
}

void test_gloabl_func(lua_State* L)
{
	g_test_func_set["test_lua_cfunc_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_1()
					gint_add1();
					return get_gint();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 0;
		return 1 == lua_tinker::call<int>(L, "test_lua_cfunc_1");
	};
	g_test_func_set["test_lua_cfunc_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_2()
					gint_addint(1);
					return get_gint();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 0;
		return 1 == lua_tinker::call<int>(L, "test_lua_cfunc_2");
	};
	g_test_func_set["test_lua_cfunc_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_3()
					gint_addintref( get_gintref() );
					return get_gint();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 1;
		return 2 == lua_tinker::call<int>(L, "test_lua_cfunc_3");
	};
	g_test_func_set["test_lua_cfunc_4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_4()
					gint_addintptr( get_gintptr() );
					return get_gint();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 1;
		return 2 == lua_tinker::call<int>(L, "test_lua_cfunc_4");
	};
	g_test_func_set["test_lua_cfunc_5"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_5()
					gint_add_intref( get_gintref(), 1);
					return get_gint();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 1;
		return 2 == lua_tinker::call<int>(L, "test_lua_cfunc_5");
	};
	g_test_func_set["test_lua_cfunc_6"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_6()
					g_addint_double( 1, 1.0);
					return (get_gint() == 1 and get_gdouble() == 1.0);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 0;
		g_c_double = 0.0;
		return  lua_tinker::call<bool>(L, "test_lua_cfunc_6");
	};

	g_test_func_set["test_lua_cfunc_7"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_7()
					gint_addintptr_const( get_gintptr_const() );
					return get_gint() == 2;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 1;
		return  lua_tinker::call<bool>(L, "test_lua_cfunc_7");
	};

	g_test_func_set["test_lua_cfunc_8"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_8()
					gint_addintref( get_gintref_const() );
					return get_gint();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 1;
		return 2 == lua_tinker::call<int>(L, "test_lua_cfunc_8");
	};


}