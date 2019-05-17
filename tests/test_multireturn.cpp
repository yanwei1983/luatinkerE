#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;
std::tuple<int, int> push_tuple()
{
	return std::make_tuple(7,10);
}

bool test_tuple(std::tuple<int, int> tuple)
{
	return std::get<0>(tuple) == 8 && std::get<1>(tuple) == 9;
}

void test_multireturn(lua_State* L)
{
	g_test_func_set["test_lua_multireturn"] = [L]()->bool
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

	g_test_func_set["test_lua_multireturn_err"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_multireturn_err()
					error("this is my test error");
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::set_error_callback([](lua_State *L) -> int
		{
			std::string errinfo(lua_tostring(L, -1));
			if (errinfo != "[string \"lua_tinker::dobuffer()\"]:2: this is my test error")
			{
				lua_tinker::on_error(L);
			}
			return 0;
		});


		int c = 0;
		double d = 0.0;
		char e = 0;
		float f = 0.0;
		std::string g;
		std::tie(c, d, e, f, g) = lua_tinker::call< std::tuple<int, double, char, float, std::string> >(L, "test_lua_multireturn_err");
		lua_tinker::set_error_callback(&lua_tinker::on_error);

		return true;
	};
	
	
	
	g_test_func_set["test_push_tuple"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_push_tuple()
					local tuple_table = push_tuple();
					return tuple_table[1] == 7 and tuple_table[2] == 10;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_push_tuple");
	};
	
	g_test_func_set["test_get_tuple"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_get_tuple()
					local tuple_table = {8, 9};
					return test_tuple(tuple_table);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		return lua_tinker::call<bool>(L, "test_get_tuple");
	};

}
