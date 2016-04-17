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

int g_c_int = 0;
double g_c_double = 0.0;
void gint_add1()
{
	g_c_int++;
}
void gint_addint(int n)
{
	g_c_int += n;
}
void gint_addintptr(int* p)
{
	g_c_int += *p;
}
void gint_addintref(const int& ref)
{
	g_c_int += ref;
}
void gint_add_intref(int& ref, int n)
{
	ref += n;
}

void g_addint_double(int n1,double n2)
{
	g_c_int += n1;
	g_c_double += n2;
}

int get_gint()
{
	return g_c_int;
}
int& get_gintref()
{
	return g_c_int;
}
int* get_gintptr()
{
	return &g_c_int;
}
double get_gdouble()
{
	return g_c_double;
}


class ff_base
{
public:
	ff_base() {}
	virtual ~ff_base() {}

	int test_base_callfn(int n)
	{
		return n;
	}
};

class ff : public ff_base
{
public:
	ff()
	{
		s_ref++;
	}
	ff(int a) :m_val(a)
	{
		s_ref++;
	}
	ff(double a)
	{
		s_ref++;
	}
	ff(double, unsigned char, int a) :m_val(a)
	{
		s_ref++;
	}
	ff(const ff& rht) :m_val(rht.m_val)
	{
		s_ref++;
	}
	ff(ff&& rht) = default;

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
		s_ref--;
	}
	bool test_memfn()
	{
		return true;
	}
	bool test_const() const
	{
		return true;
	}

	int add(int n)
	{
		return m_val += n;
	}

	int add_ffptr(ff* pff)
	{
		return m_val + pff->m_val;
	}
	
	int add_ffcref(const ff& rht)
	{
		return m_val + rht.m_val;
	}

	int test_overload(int n) const
	{
		return n;
	}

	int test_overload(int n, double d)
	{
		return n+(int)d;
	}

	int test_overload(int n1, int n2, double d)
	{
		return n1+n2+ (int)d;
	}

	int test_default_params(int a, int b, int c)
	{
		return a + b - c;
	}


	static int get_static_val(int v)
	{
		return s_val+v;
	}

	int getVal()const { return m_val; }
	void setVal(int v) { m_val = v; }

	int m_val = 0;
	static int s_val;
	static int s_ref;
};
int ff::s_val;
int ff::s_ref;

ff g_ff;
ff* get_gff_ptr()
{
	return &g_ff;
}

const ff& get_gff_cref()
{
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

std::string connect_string(std::string str1,const std::string& str2, const std::string& str3)
{
	return str1 + str2 + str3;
}

const std::string& push_string_ref()
{
	return g_teststring;
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

std::shared_ptr<ff> make_ff_to_lua()
{
	return std::shared_ptr<ff>(new ff);
}

std::weak_ptr<ff> make_ff_weak()
{
	return std::weak_ptr<ff>(g_ff_shared);
}

bool visot_ff(ff* pFF)
{
	if (pFF)
	{
		return true;
	}
	return false;
}

void visot_ff_ref(ff& refFF)
{
	std::cout << "visot_ff(" << refFF.m_val << ")" << std::endl;
}
void visot_ff_const_ref(const ff& refFF)
{
	std::cout << "visot_ff(" << refFF.m_val << ")" << std::endl;
}

bool visot_ff_shared(std::shared_ptr<ff> pFF)
{
	if (pFF)
	{
		return true;
	}
	return false;
}

bool visot_ff_weak(std::weak_ptr<ff> pWeakFF)
{
	if (pWeakFF.expired() == false)
	{
		std::shared_ptr<ff> pFF = pWeakFF.lock();
		if (pFF)
		{
			return true;
		}
	}
	return false;
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

bool visot_ff_nodef(ff_nodef* pFF)
{
	if (pFF)
	{
		return true;
	}
	return false;
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

bool visot_ff_nodef_shared(std::shared_ptr<ff_nodef> pFF)
{
	if (pFF)
	{
		return true;
	}
	return false;
}

unsigned long long addUL(unsigned long long a, unsigned long long b)
{
	return a + b;
}

long long Number2Interger(double v)
{
	return (long long)(v);
}


int test_overload(int n)
{
	return n;
}

int test_overload(int n,double d)
{
	return n+ (int)d;
}

int test_overload(int n1,int n2, double d)
{
	return n1+n2+ (int)d;
}

int test_lua_function(std::function<int(int)> func)
{
	return func(1);
}
int test_lua_function_ref(const std::function<int(int)>& func)
{
	return func(1);
}

std::function<int(int)> get_c_function()
{
	auto func = [](int v)->int
	{
		return v + 1;
	};
	return std::function<int(int)>(func);
}


//func must be release before lua close.....user_conctrl
std::function<int(int)> g_func_lua;
void store_lua_function(std::function<int(int)> func)
{
	g_func_lua = func;
}

int use_stored_lua_function()
{
	return g_func_lua(1);
}

void on_lua_close(lua_State* L)
{
	g_func_lua = nullptr;
	std::cout << "on_lua_close" << std::endl;
}

int test_default_params(int a, int b, int c)
{
	return a + b - c;
}

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	lua_tinker::init(L);

	lua_tinker::def(L, "addUL", &addUL);
	lua_tinker::def(L, "Number2Interger", &Number2Interger);


	lua_tinker::def(L, "gint_add1", &gint_add1);
	lua_tinker::def(L, "gint_addint", &gint_addint);
	lua_tinker::def(L, "gint_addintptr", &gint_addintptr);
	lua_tinker::def(L, "gint_addintref", &gint_addintref);
	lua_tinker::def(L, "gint_add_intref", &gint_add_intref);
	lua_tinker::def(L, "g_addint_double", &g_addint_double);
	lua_tinker::def(L, "get_gint", &get_gint);
	lua_tinker::def(L, "get_gintref", &get_gintref);
	lua_tinker::def(L, "get_gintptr", &get_gintptr);
	lua_tinker::def(L, "get_gdouble", &get_gdouble);


	{
		std::function<int(int, int)> func = [](int k, int j)->int {	return k + j; };
		lua_tinker::def(L, "std_function_int_int", func);	//can hold function
		std::function<int(int)> func_c = std::bind(func, std::placeholders::_1, 88);
		lua_tinker::def(L, "std_function_int_bind88", func_c);
	}

	lua_tinker::def(L, "get_gff_ptr", &get_gff_ptr);
	lua_tinker::def(L, "get_gff_cref", &get_gff_cref);

	lua_tinker::def(L, "make_ff", &make_ff);
	lua_tinker::def(L, "make_ff_to_lua", &make_ff_to_lua);
	lua_tinker::def(L, "make_ff_weak", &make_ff_weak);
	lua_tinker::def(L, "visot_ff", &visot_ff);
	lua_tinker::def(L, "visot_ff_ref", &visot_ff_ref);
	lua_tinker::def(L, "visot_ff_const_ref", &visot_ff_const_ref);
	lua_tinker::def(L, "visot_ff_weak", &visot_ff_weak);


	lua_tinker::def(L, "push_string", &push_string);
	lua_tinker::def(L, "push_string_ref", &push_string_ref);
	lua_tinker::def(L, "connect_string", &connect_string);

	lua_tinker::def(L, "push_map", &push_map);
	lua_tinker::def(L, "push_vector", &push_vector);
	lua_tinker::def(L, "push_set", &push_set);
	lua_tinker::def(L, "push_hashmap", &push_hashmap);


	lua_tinker::def(L, "make_ff_nodef", &make_ff_nodef);
	lua_tinker::def(L, "visot_ff_nodef", &visot_ff_nodef);
	lua_tinker::def(L, "make_ff_nodef_shared", &make_ff_nodef_shared);
	lua_tinker::def(L, "visot_ff_nodef_shared", &visot_ff_nodef_shared);
	lua_tinker::def(L, "make_ff", &make_ff);
	lua_tinker::def(L, "visot_ff_shared", &visot_ff_shared);
	lua_tinker::def(L, "visot_ff", &visot_ff);

	lua_tinker::def(L, "test_lua_function", &test_lua_function);
	lua_tinker::def(L, "test_lua_function_ref", &test_lua_function_ref);
	lua_tinker::def(L, "get_c_function", &get_c_function);
	lua_tinker::def(L, "store_lua_function", &store_lua_function);
	lua_tinker::def(L, "use_stored_lua_function", &use_stored_lua_function);
	lua_tinker::def(L, "test_default_params", &test_default_params, 5, 8);


	lua_tinker::def(L, "test_overload", lua_tinker::args_type_overload_functor( (int(*)(int)) (&test_overload),
																				(int(*)(int, double))(&test_overload),
																				(int(*)(int, int, double))(&test_overload) ) );
	lua_tinker::class_add<ff_base>(L, "ff_base", true);
	lua_tinker::class_add<ff>(L, "ff", true);
	lua_tinker::class_inh<ff,ff_base>(L);

	lua_tinker::class_def<ff_base>(L, "test_base_callfn", &ff_base::test_base_callfn);

	//lua_tinker::class_con<ff>(L, lua_tinker::constructor<ff>::invoke);
	//lua_tinker::class_con<ff>(L, lua_tinker::constructor<ff, int, double, unsigned char>::invoke);
	lua_tinker::class_con<ff>(L, lua_tinker::args_type_overload_constructor( lua_tinker::constructor<ff>(),
																			 lua_tinker::constructor<ff, int>(),
																			 //lua_tinker::constructor<ff, double>(),
																			 lua_tinker::constructor<ff, double, unsigned char, int>()) );



	lua_tinker::class_def<ff>(L, "test_memfn", &ff::test_memfn);
	lua_tinker::class_def<ff>(L, "test_const", &ff::test_const);
	lua_tinker::class_def<ff>(L, "add", &ff::add);
	lua_tinker::class_def<ff>(L, "add_ffptr", &ff::add_ffptr);
	lua_tinker::class_def<ff>(L, "add_ffcref", &ff::add_ffcref);
	lua_tinker::class_def<ff>(L, "getVal", &ff::getVal);
	lua_tinker::class_def<ff>(L, "setVal", &ff::setVal);
	lua_tinker::class_def<ff>(L, "test_default_params", &ff::test_default_params, 5, 8);

	lua_tinker::class_def_static<ff>(L, "get_static_val", &ff::get_static_val);

	lua_tinker::class_mem<ff>(L, "m_val", &ff::m_val);
	lua_tinker::class_property<ff>(L, "m_prop", &ff::getVal, &ff::setVal);
	lua_tinker::class_property<ff>(L, "m_prop_readonly", &ff::getVal, nullptr);
	lua_tinker::class_static_mem<ff>(L, "s_val", &ff::s_val);

	lua_tinker::class_def<ff>(L, "test_overload",
									lua_tinker::args_type_overload_member_functor(	(int(ff::*)(int)const) (&ff::test_overload),
																					(int(ff::*)(int, double))(&ff::test_overload),
																					(int(ff::*)(int, int, double))(&ff::test_overload)));


	lua_tinker::register_lua_close_callback(L, lua_tinker::Lua_Close_CallBack_Func(on_lua_close) );
	
	
	std::map<std::string, std::function<bool()> > test_func_set;


	test_func_set["test_lua_getval"] = [L]()->bool
	{
		std::string luabuf =
			R"(
				g_int=100;
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 100 == lua_tinker::get<int>(L, "g_int");

	};

	test_func_set["test_lua_int64_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_int64_1()
					local ul_a = addUL(0x8000000000000000, 1);
					return ul_a;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return (0x8000000000000000+1) == lua_tinker::call<unsigned long long>(L, "lua_test_int64_1");

	};
	test_func_set["test_lua_int64_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_int64_2()
					local ul_b = Number2Interger(2^63)+0x1;
					return ul_b;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return (0x8000000000000001) == lua_tinker::call<unsigned long long>(L, "lua_test_int64_2");
	};
	test_func_set["test_lua_int64_3"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_int64_3()
					local ul_a = addUL(0x8000000000000000, 1);
					local ul_b = Number2Interger(2^63)+0x1;
					return ul_a + ul_b;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		static constexpr const unsigned long long result = (unsigned long long)(0x8000000000000001) + ((unsigned long long)(0x8000000000000000) + 1);
		return result == lua_tinker::call<unsigned long long>(L, "lua_test_int64_3");
	};
	test_func_set["lua_test_default_params1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_default_params1()
					return test_default_params(7);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  (7+5-8) == lua_tinker::call<int>(L, "lua_test_default_params1");
	};
	test_func_set["lua_test_default_params2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_default_params2()
					return test_default_params(7,3);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  (7 + 3 - 8) == lua_tinker::call<int>(L, "lua_test_default_params2");
	};
	test_func_set["lua_test_default_params3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_default_params3()
					local pFF = ff();
					return pFF:test_default_params(7);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  (7 + 5 - 8) == lua_tinker::call<int>(L, "lua_test_default_params3");
	};
	test_func_set["lua_test_default_params4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_default_params4()
					local pFF = ff();
					return pFF:test_default_params(7,3);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  (7 + 3 - 8) == lua_tinker::call<int>(L, "lua_test_default_params4");
	};
	test_func_set["test_lua_shared_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_shared_1()
					local pFFShared =  make_ff();
					return visot_ff_shared(pFFShared);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "lua_test_shared_1");
	};
#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO
	test_func_set["test_lua_shared_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_shared_2()
					local pFFShared =  make_ff();
					visot_ff(pFFShared);		--error sharedptr->raw_ptr				
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		printf("error: error sharedptr->raw_ptr \n");
		try
		{
			lua_tinker::call_throw<void>(L, "lua_test_shared_2");
			return false;
		}
		catch (lua_tinker::lua_call_err&)
		{
			return true;
		}
	};
	test_func_set["test_lua_shared_3"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_shared_3()
					local pFFShared =  make_ff();
					visot_ff_weak(pFFShared);	--error shared_ptr to weak_ptr
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		printf("error: error shared_ptr to weak_ptr \n");
		try
		{
			lua_tinker::call_throw<void>(L, "lua_test_shared_3");
			return false;
		}
		catch (lua_tinker::lua_call_err&)
		{
			return true;
		}
	};
#endif

	test_func_set["test_lua_shared_4"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_shared_4(shared_ptr)
					return shared_ptr;
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		std::shared_ptr<ff> testshared(new ff(0));
		std::shared_ptr<ff> ffshared = lua_tinker::call< std::shared_ptr<ff> >(L, "test_lua_shared_4", testshared);
		return ffshared == testshared;
	};

	test_func_set["test_lua_shared_5"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_shared_5(shared_ptr)
					return shared_ptr;
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		std::shared_ptr<ff> ffshared = lua_tinker::call< std::shared_ptr<ff> >(L, "test_lua_shared_5", std::shared_ptr<ff>(new ff(0)));
		return ffshared.use_count() == 2;
	};

	test_func_set["test_lua_weak_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_weak_1()
					local pFFWeak = make_ff_weak();
					return visot_ff_weak(pFFWeak);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "lua_test_weak_1");
	};

#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO

	test_func_set["test_lua_weak_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_weak_2()
					local pFFWeak = make_ff_weak();
					visot_ff(pFFWeak);		--error weak_ptr to shared_ptr
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		printf("error: error weak_ptr to shared_ptr \n");
		try
		{
			lua_tinker::call_throw<void>(L, "test_lua_weak_2");
			return false;
		}
		catch (lua_tinker::lua_call_err&)
		{
			return true;
		}
	};
#endif

	test_func_set["test_lua_shared_invoke_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_shared_invoke_1()
					local pFFShared =  make_ff();
					return pFFShared:test_memfn();	--need define _ALLOW_SHAREDPTR_INVOKE
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_shared_invoke_1");
	};
	test_func_set["test_lua_shared_invoke_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_shared_invoke_2()
					local pFFShared =  make_ff();
					pFFShared.m_val = 77;
					return pFFShared.m_val == 77;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_shared_invoke_2");
	};
	test_func_set["test_lua_shared_invoke_3"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_shared_invoke_3()
					local pFFShared =  make_ff();
					local raw_pff = pFFShared:_get_raw_ptr();
					raw_pff.m_val = 88;
					return raw_pff.m_val == 88;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_shared_invoke_3");
	};
	test_func_set["test_lua_member_property"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_property()
					local pFFShared =  make_ff();
					local raw_pff = pFFShared:_get_raw_ptr();
					raw_pff.m_prop = 88;
					return raw_pff.m_prop == 88;
				end	
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_property");
	};
	test_func_set["test_lua_member_readonly_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_readonly_1()
					local pFF1 = ff(1);
					return pFF1.m_prop_readonly;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 1 == lua_tinker::call<int>(L, "test_lua_member_readonly_1");
	};
	test_func_set["test_lua_member_readonly_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_readonly_2()
					local pFF1 = ff(1);
					pFF1.m_prop_readonly = 88; --error readonly
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		printf("error: visit porp readonly \n");
		try
		{
			lua_tinker::call_throw<void>(L, "test_lua_member_readonly_2");
			return false;
		}
		catch (lua_tinker::lua_call_err&)
		{
			return true;
		}
	};
	test_func_set["test_lua_hold_shared_ptr"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_hold_shared_ptr()
					local pFF1 = make_ff_to_lua();
					g_ffshared = make_ff_to_lua();
				end
			)";
		lua_gc(L, LUA_GCCOLLECT, 0);
		int ref_count_old = ff::s_ref;
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::call<void>(L, "test_lua_hold_shared_ptr");
		int ref_count_new = ff::s_ref;
		lua_gc(L, LUA_GCCOLLECT, 0);
		int ref_count_new_gc = ff::s_ref;
		return (ref_count_old + 2) == ref_count_new		//pFF1 + 1,  g_ffshared+1
			&& (ref_count_old + 1) == ref_count_new_gc;	//g_ffshared+1
	};
	

	test_func_set["test_lua_nodef_shared_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_nodef_shared_1()
					local pFF_nodef_Shared = make_ff_nodef_shared();
					return visot_ff_nodef_shared(pFF_nodef_Shared);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_nodef_shared_1");
	};
	test_func_set["test_lua_nodef_shared_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_nodef_shared_2()
					local pFF_nodef = make_ff_nodef();
					return visot_ff_nodef(pFF_nodef);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_nodef_shared_2");
	};
#ifdef LUATINKER_USERDATA_CHECK_TYPEINFO

	test_func_set["test_lua_nodef_shared_3"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_nodef_shared_3()
					local pFF_nodef = make_ff_nodef();
					visot_ff_nodef_shared(pFF_nodef);	--raw_ptr to shared_ptr
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		printf("error: shared_ptr to raw_ptr \n");
		try
		{
			lua_tinker::call_throw<void>(L, "test_lua_nodef_shared_3");
			return false;
		}
		catch (lua_tinker::lua_call_err&)
		{
			return true;
		}
	};

	test_func_set["test_lua_nodef_shared_4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_nodef_shared_4()
					local pFF_nodef_Shared = make_ff_nodef_shared();
					visot_ff_nodef(pFF_nodef_Shared);	--shared_ptr to raw_ptr
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		printf("error: shared_ptr to raw_ptr \n");
		try
		{
			lua_tinker::call_throw<void>(L, "test_lua_nodef_shared_4");
			return false;
		}
		catch (lua_tinker::lua_call_err&)
		{
			return true;
		}
	};
#endif

	test_func_set["test_lua_cfunc_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_1()
					gint_add1();
					return get_gint();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 0;
		return 1 == lua_tinker::call<int>(L, "test_lua_cfunc_1");
	};
	test_func_set["test_lua_cfunc_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_2()
					gint_addint(1);
					return get_gint();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 0;
		return 1 == lua_tinker::call<int>(L, "test_lua_cfunc_2");
	};
	test_func_set["test_lua_cfunc_3"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_3()
					gint_addintref( get_gintref() );
					return get_gint();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 1;
		return 2 == lua_tinker::call<int>(L, "test_lua_cfunc_3");
	};
	test_func_set["test_lua_cfunc_4"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_4()
					gint_addintptr( get_gintptr() );
					return get_gint();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 1;
		return 2 == lua_tinker::call<int>(L, "test_lua_cfunc_4");
	};
	test_func_set["test_lua_cfunc_5"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_5()
					gint_add_intref( get_gintref(), 1);
					return get_gint();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 1;
		return 2 == lua_tinker::call<int>(L, "test_lua_cfunc_5");
	};
	test_func_set["test_lua_cfunc_6"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_cfunc_6()
					g_addint_double( 1, 1.0);
					return (get_gint() == 1 and get_gdouble() == 1.0);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 0;
		g_c_double = 0.0;
		return  lua_tinker::call<bool>(L, "test_lua_cfunc_6");
	};
	test_func_set["test_lua_coverloadfunc_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_coverloadfunc_1()
					return test_overload(1) == 1;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 0;
		g_c_double = 0.0;
		return  lua_tinker::call<bool>(L, "test_lua_coverloadfunc_1");
	};
	test_func_set["test_lua_coverloadfunc_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_coverloadfunc_2()
					return test_overload(1,2.0) == 3;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 0;
		g_c_double = 0.0;
		return  lua_tinker::call<bool>(L, "test_lua_coverloadfunc_2");
	};
	test_func_set["test_lua_coverloadfunc_3"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_coverloadfunc_3()
					return test_overload(1,2,3.0) == 6;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		g_c_int = 0;
		g_c_double = 0.0;
		return  lua_tinker::call<bool>(L, "test_lua_coverloadfunc_3");
	};

	test_func_set["test_lua_stdfunction_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_stdfunction_1()
					return std_function_int_int(2,3);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return (2+3) == lua_tinker::call<int>(L, "test_lua_stdfunction_1");
	};
	test_func_set["test_lua_stdfunction_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_stdfunction_2()
					return std_function_int_bind88(2);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return (2+88) == lua_tinker::call<int>(L, "test_lua_stdfunction_2");
	};

	test_func_set["test_lua_member_func_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_1()
					local pFF = get_gff_ptr();
					return pFF:test_memfn();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_func_1");
	};
	test_func_set["test_lua_member_func_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_2()
					local pFF = get_gff_cref();
					return pFF:test_const();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_func_2");
	};
	test_func_set["test_lua_member_func_3"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_3()
					local pFF = get_gff_ptr();
					return pFF:test_const();	--ptr->const member
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_func_3");
	};
#ifdef LUATINKER_USERDATA_CHECK_CONST
	test_func_set["test_lua_member_func_4"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_4()
					local pFF = get_gff_cref();
					pFF:test_memfn();	--error const_ptr -> member_func
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		printf("error:const_ptr -> member_func \n");
		try
		{
			lua_tinker::call_throw<void>(L, "test_lua_member_func_4");
			return false;
		}
		catch (lua_tinker::lua_call_err&)
		{
			return true;
		}
	};
#endif
	test_func_set["test_lua_member_func_5"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_5()
					local pFF = get_gff_ptr();
					pFF:setVal(55);
					return pFF:getVal();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 55 == lua_tinker::call<int>(L, "test_lua_member_func_5");
	};

	test_func_set["test_lua_member_func_6"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_6()
					local pFF = get_gff_ptr();
					local ff = ff(44);
					pFF:setVal(0);
					return pFF:add_ffptr(ff);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 44 == lua_tinker::call<int>(L, "test_lua_member_func_6");
	};
	test_func_set["test_lua_member_func_7"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_func_6()
					local pFF = get_gff_ptr();
					return pFF:test_base_callfn(1) == 1;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_func_6");
	};
	test_func_set["test_lua_member_overloadfunc_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overloadfunc_1()
					local pFF = get_gff_ptr();
					return pFF:test_overload(1);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 1 == lua_tinker::call<int>(L, "test_lua_member_overloadfunc_1");
	};
	test_func_set["test_lua_member_overloadfunc_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overloadfunc_2()
					local pFF = get_gff_ptr();
					return pFF:test_overload(1,2.0);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 3 == lua_tinker::call<int>(L, "test_lua_member_overloadfunc_2");
	};
	test_func_set["test_lua_member_overloadfunc_3"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overloadfunc_3()
					local pFF = get_gff_ptr();
					return pFF:test_overload(1,2,3.0);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return 6 == lua_tinker::call<int>(L, "test_lua_member_overloadfunc_3");
	};
	test_func_set["test_lua_member_overload_con_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overload_con_1()
					local ff = ff(1.1,2,5);
					return ff.m_val == 5;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_overload_con_1");
	};
	test_func_set["test_lua_member_overload_con_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overload_con_2()
					local ff = ff(1);
					return ff.m_val == 1;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_overload_con_2");
	};
	test_func_set["test_lua_member_overload_con_3"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_member_overload_con_3()
					local ff = ff();
					return ff.m_val == 0;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_member_overload_con_3");
	};

	test_func_set["test_lua_string_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_string_1()
					return push_string();
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return g_teststring == lua_tinker::call<std::string>(L, "test_lua_string_1");
	};
	test_func_set["test_lua_string_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_string_2(string)
					return connect_string(push_string(), push_string_ref(), string);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return (g_teststring+ g_teststring+ g_teststring) == lua_tinker::call<std::string>(L, "test_lua_string_2", g_teststring);
	};

	test_func_set["test_lua_map"]  = [L]()->bool
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

	test_func_set["test_lua_hashmap"]  = [L]()->bool
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

	test_func_set["test_lua_set"]  = [L]()->bool
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



	test_func_set["test_lua_vec"]  = [L]()->bool
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

	test_func_set["test_lua_funobj_1"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_1()
					local upval = 0;
					local localtest = function(intval)
						upval = upval+ intval;
						return upval;
					end
					;
					return 1 == test_lua_function(localtest);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_funobj_1");
	};

	test_func_set["test_lua_funobj_2"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_2()
					local upval = 0;
					local localtest = function(intval)
						upval = upval+ intval;
						return upval;
					end
					test_lua_function_ref(localtest);
					return 2 == test_lua_function_ref(localtest);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_funobj_2");
	};

	test_func_set["test_lua_funobj_3"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_3()
					local upval = 0;
					local localtest = function (intval)
						upval = upval+ intval;
						return upval;
					end
					return 1 == test_lua_function(function (intval)
														return localtest(intval);
														end);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		return lua_tinker::call<bool>(L, "test_lua_funobj_3");
	};

	test_func_set["test_lua_funobj_4"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_4()
					local localtest = function (intval)
						return intval +1;
					end
					store_lua_function(localtest);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::call<void>(L, "test_lua_funobj_4");
		return g_func_lua != nullptr;
	};

	test_func_set["test_lua_funobj_5"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_5()
					local ret = use_stored_lua_function();
					ret = ret + use_stored_lua_function();
					return ret == 4;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		
		return  lua_tinker::call<bool>(L, "test_lua_funobj_5");;
	};


	test_func_set["test_lua_funobj_6"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_6()
					local c_func = get_c_function();
					return c_func(1) == 2;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_lua_funobj_6");
	};

	test_func_set["test_lua_funobj_7"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_funobj_7()
					local localtest = function (intval)
						return intval +1;
					end
					return localtest;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		std::function<int(int)> func = lua_tinker::call<decltype(func)>(L, "test_lua_funobj_7");
		return 7 == func(6);
	};

	test_func_set["test_lua_luafunction_ref_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_luafunction_ref_1()
					local upval = 0;
					local localtest = function (intval)
						upval = upval + intval;
						return upval;
					end
					return localtest;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		lua_tinker::lua_function_ref<int> lua_func = lua_tinker::call<decltype(lua_func)>(L, "test_lua_luafunction_ref_1");
		lua_func(7);
		return 14 == lua_func(7);
	};
	test_func_set["test_lua_luafunction_ref_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_luafunction_ref_2_1()
					local upval = 0;
					local localtest = function (intval)
						upval = upval + intval;
						return upval;
					end
					return localtest;
				end
				function test_lua_luafunction_ref_2_2(func)
					func(7);
					return func(7);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		lua_tinker::lua_function_ref<int> lua_func = lua_tinker::call<decltype(lua_func)>(L, "test_lua_luafunction_ref_2_1");
		return 14 == lua_tinker::call<int>(L, "test_lua_luafunction_ref_2_2", lua_func);
	};

	test_func_set["test_lua_gettable_1"]  = [L]()->bool
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


	test_func_set["test_lua_multireturn"]  = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_multireturn()
					return 1,2.0,3,4.0,"5"
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		int c = 0;
		double d = 0.0;
		char e = 0;
		float f = 0.0;
		std::string g;
		std::tie(c, d, e, f, g) = lua_tinker::call< std::tuple<int, double, char, float, std::string> >(L, "test_lua_multireturn");

		return c == 1 && d == 2.0 && e == 3 && f == 4.0 && g == "5";
	};





	for (const auto& v : test_func_set)
	{
		const auto& func = v.second;
		bool result = func();
		if (result == false)
		{
			printf("unit test: %s error\n", v.first.c_str());
		}
	}

	

	//func must be release before lua close.....user_conctrl
	//g_func_lua = nullptr;

	//lua_gc(L, LUA_GCSTEP, 1);
	lua_gc(L, LUA_GCCOLLECT, 0);
	assert(ff::s_ref == 3); //g_ff,g_ff_shared,in lua:g_ffshared
	std::string luabuf =
		R"( g_ffshared = nil;
			)";
	lua_tinker::dostring(L, luabuf.c_str());

	lua_gc(L, LUA_GCCOLLECT, 0);
	assert(ff::s_ref == 2); //g_ff,g_ff_shared
	lua_close(L);

	assert(g_func_lua == nullptr);
	//g_func_lua(1); //access lua_State after lua_close will crash
	return 0;
}

