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

	g_test_func_set["test_lua_table_add_new"] = [L]()->bool
	{
		lua_tinker::table_onstack table(L, "test_lua_table_add_new");
		return true;
	};

	g_test_func_set["test_lua_table_iter_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_table_iter1()
					return {
							{key=1,value="a"},
							{key=2,value="b"},
							};
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::table_onstack table = lua_tinker::call<decltype(table)>(L, "test_lua_table_iter1");
		lua_tinker::table_onstack t1 = table.get<lua_tinker::table_onstack>(1);
		if(t1.get<int>("key") != 1 || t1.get<std::string>("value") != "a")
			return false;
		lua_tinker::table_onstack t2 = table.get<lua_tinker::table_onstack>(2);
		if(t2.get<int>("key") != 2 || t2.get<std::string>("value") != "b")
			return false;



		
		return true;
	};


	g_test_func_set["test_lua_table_iter_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_table_iter2()
					return {
							{key=1,value="a"},
							{key=2,value="b"},
							};
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::table_onstack table = lua_tinker::call<decltype(table)>(L, "test_lua_table_iter2");
		bool bFail = false;
		table.for_each([L, &bFail](int key_idx, int value_idx)
		{
			lua_tinker::detail::stack_obj table(L,value_idx);
			if(table.is_table() == true)
			{
				//table_onstack will remove table from stack, so push it to top;
				table.push_top();
				lua_tinker::table_onstack t(L, lua_gettop(L));
				if((t.get<int>("key") == 1 && t.get<std::string>("value") == "a") ||
				   (t.get<int>("key") == 2 && t.get<std::string>("value") == "b") )
				{
					return true;
				}
				else
				{
					bFail = true;
					return false;
				}
			}


			return true;
		});



		
		return !bFail;
	};

}