#define _CRT_SECURE_NO_WARNINGS
#include<functional>
#include<vector>
#include<map>
#include<set>
#include<unordered_map>
#include<type_traits>
#include<iostream>
#include<utility>
#include<assert.h>
#include "lua_tinker.h"
#include "test.h"




int ff::s_val = 0;
int ff::s_ref = 0;
const int ff::s_const_val;
int ff_nodef::s_ref = 0;


ff g_ff;
ff* get_gff_ptr()
{
	return &g_ff;
}

const ff& get_gff_cref()
{
	return g_ff;
}








int NS_TEST::test_function_in_namespace(int a)
{
	return a;
}

void export_to_lua_manual(lua_State* L)
{

	{
		std::function<int(int, int)> func = [](int k, int j)->int {	return k + j; };
		lua_tinker::def(L, "std_function_int_int", func);	//can hold function
		std::function<int(int)> func_c = std::bind(func, std::placeholders::_1, 88);
		lua_tinker::def(L, "std_function_int_bind88", func_c);
	}

	lua_tinker::class_property<ff>(L, "m_prop", &ff::getVal, &ff::setVal);
	lua_tinker::class_property<ff>(L, "m_prop_readonly", &ff::getVal, nullptr);
}

extern std::function<int(int)> g_func_lua;
extern lua_tinker::table_ref g_table_ref;

void on_lua_close(lua_State* L)
{
	g_func_lua = nullptr;
	g_table_ref.reset();
	std::cout << "on_lua_close" << std::endl;
}

std::map<std::string, std::function<bool()> > g_test_func_set;


int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	lua_tinker::init(L);


	extern void export_to_lua_auto(lua_State* L);




	export_to_lua_auto(L);
	export_to_lua_manual(L);

	lua_tinker::register_lua_close_callback(L, lua_tinker::Lua_Close_CallBack_Func(on_lua_close));




	
	


	

	





	extern void test_lua_intoptest(lua_State* L);

	extern void test_class_member(lua_State* L);
	extern void test_default_params(lua_State* L);
	extern void test_extend_class_in_lua(lua_State* L);
	extern void test_function_obj(lua_State* L);
	extern void test_overloadfunc(lua_State* L);
	extern void test_get_gloabl(lua_State* L);
	extern void test_gloabl_func(lua_State* L);
	extern void test_inherit(lua_State* L);
	extern void test_inherit_in_lua(lua_State* L);
	extern void test_inner_class(lua_State* L);
	extern void test_int64(lua_State* L);
	extern void test_luafunction_ref(lua_State* L);
	extern void test_lua_table_ref(lua_State* L);
	extern void test_member_func(lua_State* L);
	extern void test_multireturn(lua_State* L);
	extern void test_namespace(lua_State* L);
	extern void test_sharedptr(lua_State* L);
	extern void test_stl_container(lua_State* L);
	extern void test_string(lua_State* L);


	test_lua_intoptest(L);

	test_class_member(L);
	test_default_params(L);
	test_extend_class_in_lua(L);
	test_function_obj(L);
	test_overloadfunc(L);
	test_get_gloabl(L);
	test_gloabl_func(L);
	test_inherit(L);
	test_inherit_in_lua(L);
	test_inner_class(L);
	test_int64(L);
	test_luafunction_ref(L);
	test_lua_table_ref(L);
	test_member_func(L);
	test_multireturn(L);
	test_namespace(L);
	test_sharedptr(L);
	test_stl_container(L);
	test_string(L);


	int nError = 0;
	for (const auto& v : g_test_func_set)
	{
		const auto& func = v.second;
		bool result = func();
		if (lua_gettop(L) != 0)
		{
			printf("unit test stack leak: %s error\n", v.first.c_str());
			nError++;
		}
		if (result == false)
		{
			printf("unit test: %s error\n", v.first.c_str());
			nError++;
		}
	}




	//lua_gc(L, LUA_GCSTEP, 1);
	lua_gc(L, LUA_GCCOLLECT, 0);
	if(ff::s_ref != 3) //g_ff,g_ff_shared,in lua:g_ffshared
		nError++;

	std::string luabuf =
		R"( g_ffshared = nil;
			)";
	lua_tinker::dostring(L, luabuf.c_str());

	lua_gc(L, LUA_GCCOLLECT, 0);
	if(ff::s_ref != 2) //g_ff,g_ff_shared
		nError++;

	lua_close(L);

	if(g_func_lua != nullptr)
		nError++;
	//g_func_lua(1); //access lua_State after lua_close will crash
	return nError;
}

