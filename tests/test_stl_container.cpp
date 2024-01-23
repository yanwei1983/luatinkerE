#include "lua_tinker.h"
#include"test.h"





std::unordered_map<int, int> g_testhashmap = { { 1,1 },{ 3,2 },{ 5,3 },{ 7,4 } };
std::unordered_map<int, int> push_hashmap()
{
	return g_testhashmap;
}

std::map<int, int> g_testmap = { { 1,1 },{ 3,2 },{ 5,3 },{ 7,4 } };
std::map<int, int> push_map()
{
	return g_testmap;
}
const std::map<int, int>& push_map_ref()
{
	return g_testmap;
}

std::set<int> g_testset = { 1,3,5,7,9 };
std::set<int> push_set()
{
	return g_testset;
}


std::vector<int> g_testvec = { 2,4,6,8,10 };
std::vector<int> push_vector()
{
	return g_testvec;
}

const std::vector<int>& push_vector_const()
{
	return g_testvec;
}

std::string vector_join_const(const std::vector<int>& vec, const std::string& sep)
{
	std::string result;
	for(auto v : vec)
	{
		if(result.empty() == false)
			result+=sep;
		result += std::to_string(v);
	}
	return result;
}

const std::vector<SampleStruct> &push_vector_struct_const()
{
	static std::vector<SampleStruct> test_v = { {1,2}, {3,4}};
	return test_v;
}

bool test_vector_struct_const(const std::vector<SampleStruct> & st)
{
	return (st.size() == 2) && (st[0].a == 1) && (st[0].b == 2)&& (st[1].a == 3)&& (st[1].b == 4);
}

LUA_TEST(stl_container)
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
		if (mapval.empty())
			return false;
		for (const auto& v : g_testmap)
		{
			if (mapval[v.first] != v.second)
				return false;
		}
		return true;
	};

	g_test_func_set["test_lua_map_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_map_2()
					local map_table = push_map_ref();
					local pTest = TestCon();
					pTest:ChangeDataMapRef(map_table);
					return pTest:getDataMap();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		std::map<int, int> mapval = lua_tinker::call<decltype(mapval)>(L, "test_lua_map_2");
		if (mapval.empty())
			return false;
		for (const auto& v : g_testmap)
		{
			if (mapval[v.first] != v.second)
				return false;
		}
		return true;
	};

	g_test_func_set["test_lua_map_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_map_3()
					local map_table = push_map(); 
					local pTest = TestCon();
					local pTest2 = TestCon();
					pTest:ChangeDataMap(map_table);	
					pTest2:ChangeDataMapPtr(pTest:getDataMapPtr());
					return pTest2:getDataMapRef();		
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		const std::map<int, int>& mapval = lua_tinker::call<decltype(mapval)>(L, "test_lua_map_3");
		if (mapval.empty())
			return false;
		for (const auto& v : g_testmap)
		{
			auto itfind = mapval.find(v.first);
			if(itfind != mapval.end())
			if (itfind->second != v.second)
				return false;
		}
		return true;
	};

	g_test_func_set["test_lua_map_4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_map_4()
					local map_table = push_map():to_table(); --map to lua_table
					local pTest = TestCon();
					local pTest2 = TestCon();
					pTest:ChangeDataMap(map_table); --lua_table to map
					pTest2:ChangeDataMap(pTest:getDataMapRef());
					return pTest2:getDataMapRef();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		const std::map<int, int>* pMap = lua_tinker::call<decltype(pMap) >(L, "test_lua_map_4");
		if (pMap == nullptr)
			return false;

		std::map<int, int> mapval = *pMap;
		if (mapval.empty())
			return false;
		for (const auto& v : g_testmap)
		{
			if (mapval[v.first] != v.second)
				return false;
		}
		return true;
	};

	g_test_func_set["test_lua_map_5"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_map_5(nTotalK, nTotalV)
					local map_ref = push_map_ref();
					local total_k = 0;
					local total_v = 0;
					for k,v in pairs(map_ref) do
						total_k = total_k + k;
						total_v = total_v + v;
					end
					local map_table = map_ref:to_table();
					local total_k1 = 0;
					local total_v1 = 0;
					for k,v in pairs(map_table) do
						total_k1 = total_k1 + k;
						total_v1 = total_v1 + v;
					end
					return total_k == total_k1 and total_k == nTotalK and total_v == total_v1 and total_v == nTotalV;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		int nTotalK =0;
		int nTotalV =0;
		for(const auto&[k,v] : push_map_ref())
		{
			nTotalK += k;
			nTotalV += v;
		}

		return lua_tinker::call<bool>(L, "test_lua_map_5", nTotalK, nTotalV);
	};

	g_test_func_set["test_lua_map_6"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_map_6()
					local map_ref = push_map_ref();
					map_ref[2] = 5;
					return map_ref[2] == 5;
				end
			)";
		
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_map_6");
	};
	
	g_test_func_set["test_lua_map_7"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_map_7()
					local map_ref = push_map_ref();
					map_ref:push(10,15);
					return map_ref[10] == 15;
				end
			)";
		
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_map_7");
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
		std::unordered_map<int, int> mapval = lua_tinker::call<decltype(mapval)>(L, "test_lua_hashmap");
		for (const auto& v : g_testhashmap)
		{
			if (mapval[v.first] != v.second)
				return false;
		}
		return true;
	};

	g_test_func_set["test_lua_hashmap_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_hashmap_1()
					local map_ref = push_map_ref();
					--{ 1,1 },{ 3,2 },{ 5,3 },{ 7,4 } 
					return map_ref[1] == 1 and map_ref[3] == 2 and map_ref[5] == 3 and map_ref[7] == 4;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());		
		return lua_tinker::call<bool>(L, "test_lua_hashmap_1");
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
		std::set<int> val = lua_tinker::call<decltype(val)>(L, "test_lua_set");
		if (val.empty())
			return false;
		for (const auto& v : val)
		{
			if (g_testset.find(v) == g_testset.end())
				return false;
		}
		return true;
	};

	g_test_func_set["test_lua_set_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_set_2()
					local testSet = push_set();
					local pTest = TestCon();
					pTest:ChangeDataSet(testSet);
					return pTest:getDataSet();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		std::set<int> val = lua_tinker::call<decltype(val)>(L, "test_lua_set_2");
		if (val.empty())
			return false;
		for (const auto& v : val)
		{
			if (g_testset.find(v) == g_testset.end())
				return false;
		}
		return true;
	};
	g_test_func_set["test_lua_set_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_set_3()
					local testSet = push_set();
					local pTest = TestCon();
					local pTest2 = TestCon();
					pTest:ChangeDataSet(testSet);
					pTest2:ChangeDataSetRef(pTest:getDataSetRef());
					return pTest2:getDataSet();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		std::set<int> val = lua_tinker::call<decltype(val)>(L, "test_lua_set_3");
		if (val.empty())
			return false;
		for (const auto& v : val)
		{
			if (g_testset.find(v) == g_testset.end())
				return false;
		}
		return true;
	};

	g_test_func_set["test_lua_set_4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_set_4(nTotalK,nTotalV)
					local set_copy = push_set();
					local total_k = 0;
					local total_v = 0;
					for k,v in pairs(set_copy) do
						total_k = total_k + k;
						total_v = total_v + v;
					end
					local set_table = set_copy:to_table();
					local total_k1 = 0;
					local total_v1 = 0;
					for k,v in pairs(set_table) do
						total_k1 = total_k1 + k;
						total_v1 = total_v1 + v;
					end
					--totalk=1+2+3+4+5 --total_v=1+3+5+7+9
					return total_k == total_k1 and total_k == nTotalK and total_v == total_v1 and total_v == nTotalV;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		int nTotalK =0;
		int nTotalV =0;
		int i = 1;
		for(auto it = g_testset.begin(); it != g_testset.end(); it++)
		{
			nTotalK += i;
			nTotalV += *it;
			i++;
		}
		return lua_tinker::call<bool>(L, "test_lua_set_4",nTotalK,nTotalV);
	};

	g_test_func_set["test_lua_set_5"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_set_5()
					local map_copy = push_set();
					map_copy:push(15);
					return map_copy;
				end
			)";
		
		lua_tinker::dostring(L, luabuf.c_str());
		const std::set<int>& val =  lua_tinker::call<decltype(val)>(L, "test_lua_set_5");
		return val.find(15) != val.end();
	};

	g_test_func_set["test_lua_set_6"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_set_6()
					local map_copy = push_set();
					map_copy:push(15);
					map_copy:erase_by_value(15);
					return map_copy;
				end
			)";
		
		lua_tinker::dostring(L, luabuf.c_str());
		const std::set<int>& val =  lua_tinker::call<decltype(val)>(L, "test_lua_set_6");
		return val.find(15) == val.end();
	};

	g_test_func_set["test_lua_set_7"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_set_7(pTest)
					local testSet = pTest.m_DataSet;
					testSet:push(1);
					return testSet:size();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		TestCon test;
		test.m_DataSet.insert(77);
		size_t data_size = lua_tinker::call<size_t>(L, "test_lua_set_7", &test);
		
		return data_size == test.m_DataSet.size() && data_size == 2;
	};

	g_test_func_set["test_lua_set_8"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_set_8(pTest)
					pTest.m_DataSet:push(1);
					return pTest.m_DataSet:size();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		TestCon test;
		test.m_DataSet.insert(77);
		size_t data_size = lua_tinker::call<size_t>(L, "test_lua_set_8", &test);
		
		return data_size == test.m_DataSet.size() && data_size == 2;
	};

	g_test_func_set["test_lua_set_9"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_set_9(pTest)
					local table = pTest:DataMapToTable();
					local count = 0;
					for k,v in pairs(table) do
						count= count+1;
					end
					return count;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		TestCon test;
		test.m_DataMap.emplace(77,3);
		test.m_DataMap.emplace(88,4);
		size_t data_size = lua_tinker::call<size_t>(L, "test_lua_set_9", &test);
		
		return data_size == test.m_DataMap.size() && data_size == 2;
	};

	g_test_func_set["test_lua_set_10"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_set_10(pTest)
					local table = {[77]=1, [88]=2};
					pTest:TableToDataMap(table);
					return pTest:getDataMapRef():size();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		TestCon test;
		size_t data_size = lua_tinker::call<size_t>(L, "test_lua_set_10", &test);
		
		return data_size == test.m_DataMap.size() && data_size == 2;
	};

	g_test_func_set["test_lua_vec"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_vec()
					local testSet = push_vector();
					return testSet;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		std::vector<int> val = lua_tinker::call<decltype(val)>(L, "test_lua_vec");
		if (val.empty())
			return false;
		for (size_t i = 0; i < g_testvec.size(); i++)
		{
			if (g_testvec[i] != val[i])
				return false;
		}
		return true;
	};
	
	g_test_func_set["test_lua_vec1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_vec1()
					local testSet = push_vector_const();
					local str = vector_join_const(testSet,",");
					return str;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		std::string val = lua_tinker::call<decltype(val)>(L, "test_lua_vec1");
		
		return val == "2,4,6,8,10";
	};


	g_test_func_set["test_lua_vec_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_vec_2(nTotalK, nTotalV)
					local vec_copy = push_vector_const();
					local total_k = 0;
					local total_v = 0;
					for k,v in pairs(vec_copy) do
						total_k = total_k + k;
						total_v = total_v + v;
					end
					local vec_table = vec_copy:to_table();
					local total_k1 = 0;
					local total_v1 = 0;
					for k,v in pairs(vec_table) do
						total_k1 = total_k1 + k;
						total_v1 = total_v1 + v;
					end
					--totalk=1+2+3+4+5 --total_v=2+4+6+8+10
					return total_k == total_k1 and total_k == nTotalK and total_v == total_v1 and total_v == nTotalV;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		int nTotalK =0;
		int nTotalV =0;
		int i = 1;
		for(auto it = g_testvec.begin(); it != g_testvec.end(); it++)
		{
			nTotalK += i;
			nTotalV += *it;
			i++;
		}
		return lua_tinker::call<bool>(L, "test_lua_vec_2", nTotalK, nTotalV);
	};


	g_test_func_set["test_lua_vec_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_vec_3()
					local container_copy = push_vector();
					container_copy:push(15);
					return container_copy;
				end
			)";
		
		lua_tinker::dostring(L, luabuf.c_str());
		const std::vector<int>& val =  lua_tinker::call<decltype(val)>(L, "test_lua_vec_3");
		return std::find(val.begin(), val.end(), 15) != val.end();
	};

	g_test_func_set["test_lua_vec_4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_vec_4()
					local container_copy = push_vector();
					container_copy:push(15);
					container_copy:erase_by_value(15);
					return container_copy;
				end
			)";
		
		lua_tinker::dostring(L, luabuf.c_str());
		const std::vector<int>& val =  lua_tinker::call<decltype(val)>(L, "test_lua_vec_4");
		return std::find(val.begin(), val.end(), 15) == val.end();
	};

	g_test_func_set["test_lua_vec_5"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_vec_5()
					local container_ref = push_vector_struct_const();
					return test_vector_struct_const(container_ref);
				end
			)";
		
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_vec_5");
	};
}
