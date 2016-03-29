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
	std::function<void(int)> func = [](int k) {std::cout << k; };
	lua_tinker::def(L, "test_p_int1", func);
	auto func_a = [](int k) {std::cout << k; };
	lua_tinker::def(L, "test_p_int2", func_a);
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
	
	
	
	
	std::string luabuf =
		R"(g_int = 100;
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

			local pFFShared =  make_ff();
			visot_ff(pFFShared);
			--visot_ff_weak(pFFShared);	--error shared_ptr to weak_ptr
			local pFFWeak = make_ff_weak();
			--visot_ff(pFFWeak);		--error weak_ptr to shared_ptr
			visot_ff_weak(pFFWeak);

							local pFF_nodef_Shared = make_ff_nodef_shared();
			visot_ff_nodef_shared(pFF_nodef_Shared);
			local pFF_nodef = make_ff_nodef();
			visot_ff_nodef(pFF_nodef);
			visot_ff_nodef_shared(pFF_nodef_Shared);
			local pFF = test_v_ffv();

			test_fun();
			test_p_int(2);
			test_vint_p_int(3);

			local local_int = test_vintr_err();
			test_vint_p_int(local_int);
			test_p_intr(local_int,1);
			test_vint_p_int(local_int);
			test_p_intr(local_int,1);
			test_vint_p_int(local_int);

			local pFF = test_v_ff();
			pFF:test_memfn();
			pFF:test_const();
			pFF:test_p_int(3);
			pFF:test_vint_p_int(45);
			pFF:test_vint_p_int_ff(123,pFF);
			pFF:test_vint_p_int_ff(123,nil);
			pFF:test_vint_p_int_ff(123,0);

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
		
			local pFFref = test_v_ffr()
			pFFref:test_vint_p_int(luaFF1);

			local string = push_string();
			read_lua_string(string);
			local string_ref = push_string_ref();
			read_lua_string_ref(string_ref);
		
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
		
		function lua_test2(n)
			test_p_int(n)
			return n+1;
		end
		)";
	lua_tinker::dostring(L, luabuf.c_str());
	int a = lua_tinker::get<int>(L, "g_int");
	printf("%d\n", a);
	lua_tinker::call<void>(L, "lua_test");
	int b = lua_tinker::call<int>(L, "lua_test2", 1);
	printf("%d\n", b);

	//lua_gc(L, LUA_GCSTEP, 1);
	lua_gc(L, LUA_GCCOLLECT, 0);
	lua_close(L);

	return 0;
}

