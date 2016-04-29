#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;


std::unordered_map<int, int> g_testhashmap = { { 1,1 },{ 3,2 },{ 5,3 },{ 7,4 } };
const std::unordered_map<int, int>& push_hashmap()
{
	return g_testhashmap;
}

std::map<int, int> g_testmap = { { 1,1 },{ 3,2 },{ 5,3 },{ 7,4 } };
const std::map<int, int>& push_map()
{
	return g_testmap;
}

std::set<int> g_testset = { 1,2,3,4,5 };
const std::set<int> & push_set()
{
	return g_testset;
}


std::vector<int> g_testvec = { 1,2,3,4,5 };
const std::vector<int>& push_vector()
{
	return g_testvec;
}


void test_stl_container(lua_State* L)
{



	g_test_func_set["test_lua_map"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_map()
					local map_table = push_map();
					return map_table;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		std::map<int, int> mapval = lua_tinker::call<decltype(mapval)>(L, "test_lua_map");
		for (const auto& v : g_testmap)
		{
			if (mapval[v.first] != v.second)
				return false;
		}
		return true;
	};

	g_test_func_set["test_lua_hashmap"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_hashmap()
					local map_table = push_hashmap();
					return map_table;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		std::map<int, int> mapval = lua_tinker::call<decltype(mapval)>(L, "test_lua_hashmap");
		for (const auto& v : g_testhashmap)
		{
			if (mapval[v.first] != v.second)
				return false;
		}
		return true;
	};

	g_test_func_set["test_lua_set"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_set()
					local testSet = push_set();
					return testSet;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		std::vector<int> val = lua_tinker::call<decltype(val)>(L, "test_lua_set");
		for (const auto& v : val)
		{
			if (g_testset.find(v) == g_testset.end())
				return false;
		}
		return true;
	};



	g_test_func_set["test_lua_vec"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_vec()
					local testSet = push_set();
					return testSet;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		std::vector<int> val = lua_tinker::call<decltype(val)>(L, "test_lua_vec");
		for (size_t i = 0; i < g_testvec.size(); i++)
		{
			if (g_testvec[i] != val[i])
				return false;
		}
		return true;
	};

}