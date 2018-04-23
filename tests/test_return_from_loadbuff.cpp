#include "lua_tinker.h"
#include"test.h"
extern std::map<std::string, std::function<bool()> > g_test_func_set;


void test_return_from_loadbuff(lua_State* L)
{


	g_test_func_set["test_return_from_loadbuff1"] = [L]()->bool
	{
		std::string luabuf =
			R"(return "aaa"
			)";
		std::string rval = lua_tinker::dostring<std::string>(L, luabuf.c_str());
		return rval == "aaa";
	};
	

	g_test_func_set["test_return_from_loadbuff2"] = [L]()->bool
	{
		std::string luabuf =
			R"(return {
						  ["default"] = {
								ver = "2.1",
							},

						  ["vv"] = {
								id = "55"
							}
						}
			)";
		std::map<std::string,std::map<std::string,std::string> > rval = lua_tinker::dostring<decltype(rval)>(L, luabuf.c_str());
		std::string v1 = rval["default"]["ver"];
		std::string v2 = rval["vv"]["id"];
		return v1 == "2.1" && v2 == "55";
	};

	g_test_func_set["test_return_from_loadbuff3"] = [L]()->bool
	{
		std::string luabuf =
			R"(return {
						  ["default"] = {
								ver = "2.1",
							},

						  ["vv"] = {
								id = 55
							}
						}
			)";
		
			lua_tinker::table_onstack rval = lua_tinker::dostring<decltype(rval)>(L, luabuf);
			lua_tinker::table_onstack t1 = rval.get<decltype(t1)>("default");
			std::string v1 = t1.get<std::string>("ver");
			lua_tinker::table_onstack t2 = rval.get<decltype(t2)>("vv");
			int v2 = t2.get<int>("id");
			return v1 == "2.1" && v2 == 55;

	};

}