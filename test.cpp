// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include<functional>
#include<vector>
#include<map>
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
	ff()
		:m_val(0)
	{
	}
	ff(int a) :m_val(a) 
	{
	}
	void test() 
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

	int m_val;
};
ff g_ff;
ff* test3() 
{
	std::cout << "test3" << std::endl;
	return &g_ff; 
}

#include "lua_tinker.h"

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


	lua_tinker::class_add<ff>(L, "ff");

	lua_tinker::class_con<ff>(L, lua_tinker::constructor<ff>);
	lua_tinker::class_con<ff>(L, lua_tinker::constructor<ff,int>);

	lua_tinker::class_def<ff>(L, "test", &ff::test);
	lua_tinker::class_def<ff>(L, "test1", &ff::test1);
	lua_tinker::class_def<ff>(L, "test2", &ff::test2);
	lua_tinker::class_def<ff>(L, "test3", &ff::test3);
	lua_tinker::class_mem<ff>(L, "m_val", &ff::m_val);




	std::string luabuf =
		"g_int = 100;"
		"function lua_test()"
		"	test();"
		"	test1(2);"
		"	test2(3);"
		"	local pFF = test3();"
		"	pFF:test();"
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




    return 0;
}

