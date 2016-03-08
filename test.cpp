#define _CRT_SECURE_NO_WARNINGS
#include<functional>
#include<vector>
#include<map>
#include<set>
#include<unordered_map>
#include<type_traits>
#include<iostream>

#include "lua_tinker.h"

void test()
{
	std::cout << "test" << std::endl;
}
void test1(int n) 
{
	std::cout << "test1(" << n << ")" << std::endl;
}
int test2(int n) 
{
	std::cout << "test2(" << n << ")" << std::endl;
	return 0; 
}
class ff
{
public:
	ff(int a = 0) :m_val(a) 
	{
		std::cout << "ff::ff(int a)" << std::endl;
	}
	ff(const ff& rht) :m_val(rht.m_val) {}
	ff(ff&& rht) :m_val(rht.m_val) {}

	~ff()
	{
		std::cout << "ff::~ff()" << std::endl;
	}
	void test() 
	{
		std::cout << "ff::test" << std::endl;
	}
	void test_const() const
	{
		std::cout << "ff::test" << std::endl;
	}
	void test1(int n ) 
	{
		std::cout << "ff::test1("<< n <<")" << std::endl;
	}
	int test2(int n)
	{
		std::cout << "ff::test2(" << n << ")" << std::endl;

		return 0; 
	}
	int test3(int n,ff* pff) 
	{ 
		std::cout << "ff::test3(" << n<<","<< (int)pff << ")" << std::endl;

		return 0;
	}
	void test4(ff* rht)
	{
		std::cout << "ff::test4(" << (ptrdiff_t)rht << ")" << std::endl;
	}
	void test5(const ff& rht)
	{
		std::cout << "ff::test5(" << (ptrdiff_t)&rht << ")" << std::endl;
	}


	int m_val;
};
ff g_ff;
ff* test3() 
{
	std::cout << "test3" << std::endl;
	return &g_ff; 
}

ff test4()
{
	std::cout << "test4" << std::endl;
	return g_ff;
}
ff& test5()
{
	std::cout << "test5" << std::endl;
	return g_ff;
}

std::unordered_map<int, int> g_testhashmap = { { 1,1 },{ 3,2 },{ 5,3 },{ 7,4 } };
const std::unordered_map<int, int>& push_hashmap()
{
	return g_testhashmap;
}

std::map<int, int> g_testmap = { {1,1},{3,2},{5,3},{7,4} };
const std::map<int,int>& push_map()
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

std::string g_teststring = "test";
std::string push_string()
{
	return g_teststring;
}

void read_lua_string(std::string t)
{
	std::cout << "read_lua_string(" << t << ")" << std::endl;
}

const std::string& push_string_ref()
{
	return g_teststring;
}

void read_lua_string_ref(const std::string& t)
{
	std::cout << "read_lua_string_ref(" << t << ")" << std::endl;
}

std::shared_ptr<ff> make_ff()
{
	return std::shared_ptr<ff>(new ff);
}

void visot_ff(std::shared_ptr<ff> pFF)
{
	if (pFF)
	{
		std::cout << "visot_ff(" << pFF->m_val << ")" << std::endl;
	}
}

class ff_nodef
{
public:
	ff_nodef(int n = 0):m_val(n) {}
	ff_nodef(const ff_nodef& rht) :m_val(rht.m_val) {}
	ff_nodef(ff_nodef&& rht) :m_val(rht.m_val) {}

	~ff_nodef()
	{
		std::cout << "ff_nodef::~ff_nodef()" << std::endl;
	}

	int m_val;
};

std::shared_ptr<ff_nodef> g_ff_nodef;
std::shared_ptr<ff_nodef> make_ff_nodef()
{
	if (!g_ff_nodef)
	{
		g_ff_nodef = std::shared_ptr<ff_nodef>(new ff_nodef);
	}
	return g_ff_nodef;
}

void visot_ff_nodef(std::shared_ptr<ff_nodef> pFF)
{
	if (pFF)
	{
		pFF->m_val++;
		std::cout << "visot_ff_nodef(" << pFF->m_val << ")" << std::endl;
	}
}

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	lua_tinker::init(L);

	lua_tinker::class_add<lua_State>(L, "lua_State");
	lua_tinker::set<lua_State*>(L, "thos_lua", L);


	lua_tinker::def(L, "test",&test);
	lua_tinker::def(L, "test1", &test1);
	lua_tinker::def(L, "test2", &test2);
	lua_tinker::def(L, "test3", &test3);
	lua_tinker::def(L, "test4", &test4);
	lua_tinker::def(L, "test5", &test5);
	lua_tinker::def(L, "push_string", &push_string);
	lua_tinker::def(L, "push_string_ref", &push_string_ref);
	lua_tinker::def(L, "read_lua_string", &read_lua_string);
	lua_tinker::def(L, "read_lua_string_ref", &read_lua_string_ref);
	lua_tinker::def(L, "push_map", &push_map);
	lua_tinker::def(L, "push_vector", &push_vector);
	lua_tinker::def(L, "push_set", &push_set);
	lua_tinker::def(L, "push_hashmap", &push_hashmap);

	lua_tinker::def(L, "make_ff", &make_ff);
	lua_tinker::def(L, "visot_ff", &visot_ff);
	lua_tinker::def(L, "make_ff_nodef", &make_ff_nodef);
	lua_tinker::def(L, "visot_ff_nodef", &visot_ff_nodef);


	lua_tinker::class_add<ff>(L, "ff");
	lua_tinker::class_add<std::string>(L, "string");

	//lua_tinker::class_con<ff>(L, lua_tinker::constructor<ff>::invoke);
	lua_tinker::class_con<ff>(L, lua_tinker::constructor<ff,int>::invoke);

	lua_tinker::class_def<ff>(L, "test", &ff::test);
	lua_tinker::class_def<ff>(L, "test_const", &ff::test_const);
	lua_tinker::class_def<ff>(L, "test1", &ff::test1);
	lua_tinker::class_def<ff>(L, "test2", &ff::test2);
	lua_tinker::class_def<ff>(L, "test3", &ff::test3);
	lua_tinker::class_def<ff>(L, "test4", &ff::test4);
	lua_tinker::class_def<ff>(L, "test5", &ff::test5);
	lua_tinker::class_mem<ff>(L, "m_val", &ff::m_val);




	std::string luabuf =
		"g_int = 100; \n"
		"function lua_test()"
		"	local pFFShared = make_ff_nodef();"
		"	visot_ff_nodef(pFFShared);"
		"	local pFF = test4();"
		"	test();"
		"	test1(2);"
		"	test2(3);"
		"	local pFF = test3();"
		"	pFF:test();"
		"	pFF:test_const();"
		"	pFF:test1(3);"
		"	pFF:test2(45);"
		"	pFF:test3(123,pFF);"
		"	pFF:test3(123,nil);"
		"	pFF:test3(123,0);"
		"	local luaFF = ff();"
		"	test1(luaFF.m_val);"
		"	luaFF.m_val = 2;"
		"	test1(luaFF.m_val);"
		"	local luaFF1 = ff(1);"
		"	test1(luaFF1.m_val);"
		"	local luaFF2 = luaFF1"
		"	test1(luaFF2.m_val);"
		"	luaFF2:test3(321,luaFF1);"
		"	luaFF2:test4(luaFF1);"
		"	luaFF2:test5(luaFF1);"
		
		"	local pFFref = test5()"
		"	pFFref:test4(luaFF1);"

		"	local string = push_string();"
		"	read_lua_string(string);"
		"	local string_ref = push_string_ref();"
		"	read_lua_string_ref(string_ref);"
		
		"	local map_table = push_map();"
		"	print(\"print map_table\")"
		"	for k,v in pairs(map_table) do"
		"		print(k);"
		"		print(v);"
		"	end""\n"
		"	print(\"print hashmap_table\")"
		"	local hashmap_table = push_hashmap();"
		"	for k,v in pairs(hashmap_table) do"
		"		print(k);"
		"		print(v);"
		"	end""\n"
		"	print(\"print vector_table\")"
		"	local vector_table = push_vector();"
		"	for idx,v in ipairs(vector_table) do"
		"		print(idx);"
		"		print(v);"
		"	end""\n"
		"	print(\"print set_table\")"
		"	local set_table = push_set();"
		"	for idx,v in ipairs(set_table) do"
		"		print(idx);"
		"		print(v);"
		"	end""\n"
		
		"end"
		"\n"
		"function lua_test2(n)"
		"	test1(n)"
		"	return n+1;"
		"end";
	lua_tinker::dostring(L, luabuf.c_str());
	int a = lua_tinker::get<int>(L, "g_int");
	printf("%d\n",a);
	lua_tinker::call<void>(L, "lua_test");
	int b = lua_tinker::call<int>(L, "lua_test2", 1);
	printf("%d\n", b);

	//lua_gc(L, LUA_GCSTEP, 1);
	lua_gc(L, LUA_GCCOLLECT, 0);
	lua_close(L);

    return 0;
}

