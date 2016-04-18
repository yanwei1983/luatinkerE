#include<functional>
#include<vector>
#include<map>
#include<set>
#include<unordered_map>
#include<type_traits>
#include<iostream>
#include<utility>
#include<assert.h>
#include<memory>


#define export_lua


export_lua extern int g_c_int;
export_lua extern double g_c_double;
export_lua void gint_add1();
export_lua void gint_addint(int n);
export_lua void gint_addintptr(int* p);
export_lua void gint_addintref(const int& ref);
export_lua void gint_add_intref(int& ref, int n);
export_lua void g_addint_double(int n1, double n2);

export_lua int get_gint();
export_lua int& get_gintref();

export_lua int* get_gintptr();
export_lua double get_gdouble();



export_lua class ff_base
{
public:
	ff_base() {}
	virtual ~ff_base() {}

	export_lua int test_base_callfn(int n)
	{
		return n;
	}
};

export_lua class ff : public ff_base
{
public:
	export_lua ff(int a = 0) :m_val(a)
	{
		s_ref++;
	}
	ff(double a)
	{
		s_ref++;
	}
	export_lua ff(double, unsigned char, int a = 1) :m_val(a)
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
	export_lua bool test_memfn()
	{
		return true;
	}
	export_lua bool test_const() const
	{
		return true;
	}

	export_lua int add(int n)
	{
		return m_val += n;
	}

	export_lua int add_ffptr(ff* pff)
	{
		return m_val + pff->m_val;
	}

	export_lua int add_ffcref(const ff& rht)
	{
		return m_val + rht.m_val;
	}

	export_lua int test_overload(int n) const
	{
		return n;
	}

	export_lua int test_overload(int n, double d)
	{
		return n + (int)d;
	}

	export_lua int test_overload(int n1, int n2, double d)
	{
		return n1 + n2 + (int)d;
	}

	export_lua int test_default_params(int a, int b = 5, int c = 8)
	{
		return a + b - c;
	}


	export_lua int get_static_val(int v)
	{
		return s_val + v;
	}

	export_lua int getVal()const { return m_val; }
	export_lua void setVal(int v) { m_val = v; }

	export_lua int m_val = 0;
	export_lua static int s_val;
	export_lua static int s_ref;
};

export_lua ff* get_gff_ptr();
export_lua const ff& get_gff_cref();
export_lua const std::unordered_map<int, int>& push_hashmap();
export_lua const std::map<int, int>& push_map();
export_lua const std::set<int> & push_set();



export_lua const std::vector<int>& push_vector();
export_lua std::string push_string();
export_lua std::string connect_string(std::string str1, const std::string& str2, const std::string& str3);
export_lua const std::string& push_string_ref();


export_lua std::shared_ptr<ff> make_ff();
export_lua std::shared_ptr<ff> make_ff_to_lua();
export_lua std::weak_ptr<ff> make_ff_weak();


export_lua bool visot_ff(ff* pFF);
export_lua void visot_ff_ref(ff& refFF);
export_lua void visot_ff_const_ref(const ff& refFF);
export_lua bool visot_ff_shared(std::shared_ptr<ff> pFF);
export_lua bool visot_ff_weak(std::weak_ptr<ff> pWeakFF);


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

export_lua ff_nodef* make_ff_nodef();
export_lua bool visot_ff_nodef(ff_nodef* pFF);


export_lua std::shared_ptr<ff_nodef> make_ff_nodef_shared();
export_lua bool visot_ff_nodef_shared(std::shared_ptr<ff_nodef> pFF);


export_lua unsigned long long addUL(unsigned long long a, unsigned long long b);
export_lua long long Number2Interger(double v);


export_lua int test_overload_err(int n);
export_lua int test_overload_err(double d);



export_lua int test_overload(int n);
export_lua int test_overload(int n, double d);
export_lua int test_overload(int n1, int n2, double d);

export_lua int test_overload_default(int n, bool b);
export_lua int test_overload_default(int n1, int n2, bool b = true);
export_lua int test_overload_default(int n1, int n2, int n3, double d1=1.0, double d2 = 2.0, double d3 = 3.0, double d4 = 4.0, const std::string& refString = std::string("test"));

export_lua int test_lua_function(std::function<int(int)> func);
export_lua int test_lua_function_ref(const std::function<int(int)>& func);
export_lua std::function<int(int)> get_c_function();
export_lua void store_lua_function(std::function<int(int)> func);
export_lua int use_stored_lua_function();

export_lua int test_default_params(int a, int b = 5, int c = 8);


