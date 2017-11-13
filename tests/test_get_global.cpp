#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;

void test_get_gloabl(lua_State* L)
{
	g_test_func_set["test_lua_getval"] = [L]()->bool
	{
		std::string luabuf =
			R"(
				g_int=100;
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 100 == lua_tinker::get<int>(L, "g_int");

	};

	g_test_func_set["test_lua_getval_err"] = [L]()->bool
	{
		return 0 == lua_tinker::get<int>(L, "g_int_notdefine");

	};


	g_test_func_set["test_lua_gettable_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(
				g_ChargePrizeList = 
				{
					[1] = {charge = 1000, itemtype=1,},
					[2] = {charge = 3000, itemtype=2,},
					[3] = {charge = 6000, itemtype=3,},
					[4] = {charge = 10000, itemtype=4,},
					[5] = {charge = 30000, itemtype=5,},
					[6] = {charge = 50000, itemtype=5,},
				};
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		std::map<int, std::map<std::string, int> > test_map = lua_tinker::get< decltype(test_map) >(L, "g_ChargePrizeList");
		std::vector<std::map<std::string, int> > test_vec = lua_tinker::get< decltype(test_vec) >(L, "g_ChargePrizeList");

		return (test_map[3]["charge"] == 6000) && (test_vec[5]["charge"] == 50000);
	};
}