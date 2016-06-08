#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;

lua_tinker::table_ref g_table_ref;
void test_lua_table_ref(lua_State* L)
{

	std::string luabuf =
		R"(g_test_table_ref={[1]=10, test1=5,};
			)";
	lua_tinker::dostring(L, luabuf.c_str());

	g_test_func_set["test_lua_table_ref_1"] = [L]()->bool
	{
		if (g_table_ref.empty())
		{
			lua_tinker::table_onstack table(L, "g_test_table_ref");
			g_table_ref = lua_tinker::table_ref::make_table_ref(table);
		}

		lua_tinker::table_onstack refTable = g_table_ref.push_table_to_stack();
		return refTable.get<int>(1) == 10;
	};


	g_test_func_set["test_lua_table_ref_2"] = [L]()->bool
	{
		if (g_table_ref.empty())
		{
			lua_tinker::table_onstack table(L, "g_test_table_ref");
			g_table_ref = lua_tinker::table_ref::make_table_ref(table);
		}

		lua_tinker::table_onstack refTable = g_table_ref.push_table_to_stack();
		return refTable.get<int>("test1") == 5;
	};
}