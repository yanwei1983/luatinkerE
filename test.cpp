#define _CRT_SECURE_NO_WARNINGS
#include<functional>
#include<vector>
#include<map>
#include<set>
#include<unordered_map>
#include<type_traits>
#include<iostream>
#include<utility>
#include "lua_tinker.h"
#include "lua_tinker_overload_func.h"
void test_fun()
{
	std::cout << "test_fun" << std::endl;
}
void test_p_int(int n)
{
	std::cout << "test_p_int(" << n << ")" << std::endl;
}
int test_vint_p_int(int n)
{
	std::cout << "test_vint_p_int(" << n << ")" << std::endl;
	return n;
}
int g_int = 0;
int& test_vintr_err()
{
	std::cout << "test_vintr(" << g_int << ")" << std::endl;
	return g_int;
}

void test_p_intr(int& n,int v)
{
	n+=v;
	std::cout << "test_p_intr(" << n << ")" << std::endl;
}

class ff
{
public:
	ff(int a = 0, double b = 0, unsigned char c = 0) :m_val(a)
	{
		std::cout << "ff::ff(int a)" << std::endl;
	}
	ff(const ff& rht) :m_val(rht.m_val)
	{
		std::cout << "ff::copy constructor" << std::endl;
	}
	ff(ff&& rht) = default;
	/*ff(ff&& rht) :m_val(rht.m_val)
	{
		std::cout << "ff::move constructor" << std::endl;
	}*/
	ff& operator=(const ff& rht)
	{
		if (&rht != this)
		{
			m_val = rht.m_val;
		}
		std::cout << "ff::operator =" << std::endl;

		return *this;
	}

	~ff()
	{
		std::cout << "ff::~ff()" << std::endl;
	}
	void test_memfn()
	{
		std::cout << "ff::test_memfn" << std::endl;
	}
	void test_const() const
	{
		std::cout << "ff::test" << std::endl;
	}
	void test_p_int(int n)
	{
		std::cout << "ff::test_p_int(" << n << ")" << std::endl;
	}
	int test_vint_p_int(int n)
	{
		std::cout << "ff::test_vint_p_int(" << n << ")" << std::endl;

		return 0;
	}
	int test_vint_p_int_ff(int n, ff* pff)
	{
		std::cout << "ff::test_vint_p_int_ff(" << n << "," << (ptrdiff_t)pff << ")" << std::endl;

		return 0;
	}
	void test_p_ff(ff* rht)
	{
		std::cout << "ff::test_p_ff(" << (ptrdiff_t)rht << ")" << std::endl;
	}
	void test_p_ffcr(const ff& rht)
	{
		std::cout << "ff::test_p_ffcr(" << (ptrdiff_t)&rht << ")" << std::endl;
	}

	int test_overload(int)
	{
		std::cout << "ff::test_overload(int)" << std::endl;
		return 0;
	}

	int test_overload(int, double)
	{
		std::cout << "ff::test_overload(int,double)" << std::endl;
		return 0;
	}

	int test_overload(int, int, double)
	{
		std::cout << "ff::test_overload(int,int, double)" << std::endl;
		return 0;
	}


	int m_val;
};
ff g_ff;
ff* test_v_ff()
{
	std::cout << "test_v_ff" << std::endl;
	return &g_ff;
}

ff test_v_ffv()
{
	std::cout << "test_v_ffv" << std::endl;
	return g_ff;
}
ff& test_v_ffr()
{
	std::cout << "test_v_ffr" << std::endl;
	return g_ff;
}

std::unordered_map<int, int> g_testhashmap = { { 1,1 },{ 3,2 },{ 5,3 },{ 7,4 } };
const std::unordered_map<int, int>& push_hashmap()
{
	return g_testhashmap;
}

std::map<int, int> g_testmap = { {1,1},{3,2},{5,3},{7,4} };
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

std::shared_ptr<ff> g_ff_shared;
std::shared_ptr<ff> make_ff()
{
	if (!g_ff_shared)
	{
		g_ff_shared = std::shared_ptr<ff>(new ff);
	}
	return g_ff_shared;
}

std::weak_ptr<ff> make_ff_weak()
{
	return std::weak_ptr<ff>(g_ff_shared);
}


void visot_ff(std::shared_ptr<ff> pFF)
{
	if (pFF)
	{
		std::cout << "visot_ff(" << pFF->m_val << ")" << std::endl;
	}
}

void visot_ff_weak(std::weak_ptr<ff> pWeakFF)
{
	if (pWeakFF.expired() == false)
	{
		std::shared_ptr<ff> pFF = pWeakFF.lock();
		if (pFF)
		{
			std::cout << "visot_ff(" << pFF->m_val << ")" << std::endl;
		}
	}
	
}

class ff_nodef
{
public:
	ff_nodef(int n = 0) :m_val(n) {}
	ff_nodef(const ff_nodef& rht) :m_val(rht.m_val) {}
	ff_nodef(ff_nodef&& rht) :m_val(rht.m_val) {}

	~ff_nodef()
	{
		std::cout << "ff_nodef::~ff_nodef()" << std::endl;
	}

	int m_val;
};

ff_nodef g_ff_nodef;
ff_nodef* make_ff_nodef()
{
	return &g_ff_nodef;
}

void visot_ff_nodef(ff_nodef* pFF)
{
	if (pFF)
	{
		pFF->m_val++;
		std::cout << "visot_ff_nodef(" << pFF->m_val << ")" << std::endl;
	}
}

std::shared_ptr<ff_nodef> g_ff_nodef_shared;
std::shared_ptr<ff_nodef> make_ff_nodef_shared()
{
	if (!g_ff_nodef_shared)
	{
		g_ff_nodef_shared = std::shared_ptr<ff_nodef>(new ff_nodef);
	}
	return g_ff_nodef_shared;
}

void visot_ff_nodef_shared(std::shared_ptr<ff_nodef> pFF)
{
	if (pFF)
	{
		pFF->m_val++;
		std::cout << "visot_ff_nodef_shared(" << pFF->m_val << ")" << std::endl;
	}
}

unsigned long long addUL(unsigned long long a, unsigned long long b)
{
	return a + b;
}

void print_ul(unsigned long long a)
{
	std::cout << "print_ul(" << a << ")" << std::endl;
}

long long Number2Interger(double v)
{
	return (long long)(v);
}


int test_overload(int)
{
	std::cout << "test_overload(int)" << std::endl;
	return 0;
}

int test_overload(int,double)
{
	std::cout << "test_overload(int,double)" << std::endl;
	return 0;
}

int test_overload(int,int, double)
{
	std::cout << "test_overload(int,int, double)" << std::endl;
	return 0;
}

int test_lua_function(std::function<int(int)> func)
{
	std::cout << "test_lua_function" << std::endl;
	return func(1);
}

std::function<int(int)> get_c_function()
{
	std::cout << "get_c_function" << std::endl;

	auto func = [](int v)->int
	{
		std::cout << "get_c_function_invoke" << std::endl;

		return v + 1;
	};
	return std::function<int(int)>(func);
}
int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	lua_tinker::init(L);

	lua_tinker::class_add<lua_State>(L, "lua_State");
	lua_tinker::set<lua_State*>(L, "thos_lua", L);


	lua_tinker::def(L, "Number2Interger", &Number2Interger);
	lua_tinker::def(L, "test_fun", &test_fun);
	lua_tinker::def(L, "test_p_int", &test_p_int);
	
	{
		std::function<void(int,int)> func = [](int k,int j) {	std::cout << "test_p_int1(" << k <<"," << j << ")" << std::endl;};
		lua_tinker::def(L, "test_p_int1", func);	//can hold function

		std::function<void(int,int)> func_a = [](int k,int j) {std::cout << "test_p_int2(" << k << "," << j << ")" << std::endl; };
		lua_tinker::def(L, "test_p_int2", func_a);  //can use F::operator() -> function

		//auto func_c = std::bind(func, std::placeholders::_1, 88); // can not use F::operator ->
		std::function<void(int)> func_c = std::bind(func, std::placeholders::_1, 88);
		
		lua_tinker::def(L, "test_p_int3", func_c);
	}
	lua_tinker::def(L, "test_vint_p_int", &test_vint_p_int);
	lua_tinker::def(L, "test_vintr_err", &test_vintr_err);
	lua_tinker::def(L, "test_p_intr", &test_p_intr);
	lua_tinker::def(L, "test_v_ff", &test_v_ff);
	lua_tinker::def(L, "test_v_ffv", &test_v_ffv);
	lua_tinker::def(L, "test_v_ffr", &test_v_ffr);


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
	lua_tinker::def(L, "make_ff_weak", &make_ff_weak);
	lua_tinker::def(L, "visot_ff_weak", &visot_ff_weak);

	lua_tinker::def(L, "make_ff_nodef", &make_ff_nodef);
	lua_tinker::def(L, "visot_ff_nodef", &visot_ff_nodef);
	lua_tinker::def(L, "make_ff_nodef_shared", &make_ff_nodef_shared);
	lua_tinker::def(L, "visot_ff_nodef_shared", &visot_ff_nodef_shared);

	lua_tinker::def(L, "addUL", &addUL);
	lua_tinker::def(L, "print_ul", &print_ul);
	lua_tinker::def(L, "test_lua_function", &test_lua_function);
	lua_tinker::def(L, "get_c_function", &get_c_function);


	lua_tinker::def(L, "test_overload", lua_tinker::args_type_overload_functor((int(*)(int)) (&test_overload),
																		(int(*)(int, double))(&test_overload),
																		(int(*)(int, int, double))(&test_overload)));
	lua_tinker::class_add<ff>(L, "ff", true);

	//lua_tinker::class_con<ff>(L, lua_tinker::constructor<ff>::invoke);
	lua_tinker::class_con<ff>(L, lua_tinker::constructor<ff, int, double, unsigned char>::invoke);

	lua_tinker::class_def<ff>(L, "test_memfn", &ff::test_memfn);
	lua_tinker::class_def<ff>(L, "test_const", &ff::test_const);
	lua_tinker::class_def<ff>(L, "test_p_int", &ff::test_p_int);
	lua_tinker::class_def<ff>(L, "test_vint_p_int", &ff::test_vint_p_int);
	lua_tinker::class_def<ff>(L, "test_vint_p_int_ff", &ff::test_vint_p_int_ff);
	lua_tinker::class_def<ff>(L, "test_p_ff", &ff::test_p_ff);
	lua_tinker::class_def<ff>(L, "test_p_ffcr", &ff::test_p_ffcr);
	lua_tinker::class_mem<ff>(L, "m_val", &ff::m_val);

	lua_tinker::class_def<ff>(L, "test_overload",
									lua_tinker::args_type_overload_member_functor(
										(int(ff::*)(int)) (&ff::test_overload),
										(int(ff::*)(int, double))(&ff::test_overload),
										(int(ff::*)(int, int, double))(&ff::test_overload)));


	
	std::string luabuf =
R"(		g_int = 100;
		function lua_test()
			local ul_a = addUL(0x8000000000000000, 1);
			print_ul(ul_a);
			print(ul_a);
			local ul_b = Number2Interger(2^63)+0x1;
			print_ul(ul_b);
			print(ul_b);
			local ul_c = ul_a + ul_b;
			print_ul(ul_c);
			print(ul_c);
		end
		function lua_test11()
			local pFFShared =  make_ff();
			visot_ff(pFFShared);
			--visot_ff_weak(pFFShared);	--error shared_ptr to weak_ptr
			local pFFWeak = make_ff_weak();
			--visot_ff(pFFWeak);		--error weak_ptr to shared_ptr
			visot_ff_weak(pFFWeak);
			pFFShared:test_memfn();	--need define _ALLOW_SHAREDPTR_INVOKE
			pFFShared.m_val = 77;
		end
		function lua_test12()
			local pFF_nodef_Shared = make_ff_nodef_shared();
			visot_ff_nodef_shared(pFF_nodef_Shared);
			local pFF_nodef = make_ff_nodef();
			visot_ff_nodef(pFF_nodef);
			visot_ff_nodef_shared(pFF_nodef_Shared);
			local pFF = test_v_ffv();
		end
		function lua_test13()
			test_fun();
			test_p_int(2);
			test_vint_p_int(3);
			test_p_int1(1,2);
			test_p_int2(3,4);
			test_p_int3(5);
		end
		function lua_test14()
			local local_int = test_vintr_err();
			test_vint_p_int(local_int);
			test_p_intr(local_int,1);
			test_vint_p_int(local_int);
			test_p_intr(local_int,1);
			test_vint_p_int(local_int);
		end
		function lua_test15()
			local pFF = test_v_ff();
			pFF:test_memfn();
			pFF:test_const();
			pFF:test_p_int(3);
			pFF:test_vint_p_int(45);
			pFF:test_vint_p_int_ff(123,pFF);
			pFF:test_vint_p_int_ff(123,nil);
			pFF:test_vint_p_int_ff(123,0);
		end
		function lua_test16()
			local luaFF = ff(1,2,3);
			test_p_int(luaFF.m_val);
			luaFF.m_val = 2;
			test_p_int(luaFF.m_val);
			local luaFF1 = ff(1,2,3);
			test_p_int(luaFF1.m_val);
			local luaFF2 = luaFF1
			test_p_int(luaFF2.m_val);
			luaFF2:test_vint_p_int_ff(321,luaFF1);
			luaFF2:test_p_ff(luaFF1);
			luaFF2:test_p_ffcr(luaFF1);
		end
		function lua_test17()
			local luaFF = ff(1,2,3);
			luaFF:test_overload(luaFF:test_overload(1),luaFF:test_overload(1,0.0),1);
			test_overload(1,test_overload(1,0.0), test_overload(1));
		end
		function lua_test18()
			local luaFF1 = ff(1,2,3);
			local pFFref = test_v_ffr()
			pFFref:test_vint_p_int(luaFF1);
			local string = push_string();
			read_lua_string(string);
			local string_ref = push_string_ref();
			read_lua_string_ref(string_ref);
		end
		function lua_test19()
			local map_table = push_map();
			print("print map_table")
			for k,v in pairs(map_table) do
				print(string.format("[%d]=%d",k,v));
			end
			print("print hashmap_table")
			local hashmap_table = push_hashmap();
			for k,v in pairs(hashmap_table) do
				print(string.format("[%d]=%d",k,v));
			end
			print("print vector_table")
			local vector_table = push_vector();
			for idx,v in ipairs(vector_table) do
				print(string.format("[%d]=%d",idx,v));
			end
			print("print set_table")
			local set_table = push_set();
			for idx,v in ipairs(set_table) do
				print(string.format("[%d]=%d",idx,v));
			end
		end
		function lua_test20()
			local function localtest(intval)
				print("localtest():");
				print(intval);
				return intval +1;
			end
			print( test_lua_function(localtest) );
			test_lua_function( function (intval)
									return localtest(intval);
								end);
			local c_func = get_c_function();
			c_func(1);
		end
		
		function lua_test2(n)
			test_p_int(n)
			return n+1;
		end

		g_ChargePrizeList = 
		{
			[1] = {charge = 1000, itemtype=1,},
			[2] = {charge = 3000, itemtype=2,},
			[3] = {charge = 6000, itemtype=3,},
			[4] = {charge = 10000, itemtype=4,},
			[5] = {charge = 30000, itemtype=5,},
			[6] = {charge = 50000, itemtype=5,},
		};

		function lua_test3()
			return 1,2.0,3,4.0,"5"
		end

		)";
	lua_tinker::dostring(L, luabuf.c_str());
	int a = lua_tinker::get<int>(L, "g_int");
	printf("%d\n", a);
	lua_tinker::call<void>(L, "lua_test");
	lua_tinker::call<void>(L, "lua_test11");
	lua_tinker::call<void>(L, "lua_test12");
	lua_tinker::call<void>(L, "lua_test13");
	lua_tinker::call<void>(L, "lua_test14");
	lua_tinker::call<void>(L, "lua_test15");
	lua_tinker::call<void>(L, "lua_test16");
	lua_tinker::call<void>(L, "lua_test17");
	lua_tinker::call<void>(L, "lua_test18");
	lua_tinker::call<void>(L, "lua_test19");
	lua_tinker::call<void>(L, "lua_test20");
	int b = lua_tinker::call<int>(L, "lua_test2", 1);
	int c = 0;
	double d = 0.0;
	char e = 0;
	float f = 0.0;
	std::string g;
	std::tie(c, d, e, f, g) = lua_tinker::call< std::tuple<int, double, char, float, std::string> >(L, "lua_test3");
	printf("%d\n", b);

	std::vector<std::map<std::string, int> > test_vec = lua_tinker::get< decltype(test_vec) >(L, "g_ChargePrizeList");
	std::map<int, std::map<std::string, int> > test_map = lua_tinker::get< decltype(test_map) >(L, "g_ChargePrizeList");

	//lua_gc(L, LUA_GCSTEP, 1);
	lua_gc(L, LUA_GCCOLLECT, 0);
	lua_close(L);

	return 0;
}

