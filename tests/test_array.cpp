#include "lua_tinker.h"
#include"test.h"
#include<array>
#include<vector>



TEST_ARRAY_T g_array_int = {1,2,3,4,5};

const TEST_ARRAY_T& push_array()
{
    return g_array_int;
}


bool read_array(const TEST_ARRAY_T& array)
{
    for(int i = 0; i < 5; i++)
    {
        if(array[i] != g_array_int[i])
        {
            return false;
        }
    }
    return true;
}

LUA_TEST(array)
{
    g_test_func_set["test_lua_array_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_array_1(array)
                    local array_table = push_array();
					return read_array(array_table)
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
        
		return lua_tinker::call<bool>(L, "test_lua_array_1",g_array_int);
	};
    g_test_func_set["test_lua_array_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_array_2(array)
                    local array_table = push_array();
                    return array_table[2] == 2;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
        
		return lua_tinker::call<bool>(L, "test_lua_array_2",g_array_int);
	};
    g_test_func_set["test_lua_array_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_array_3(array)
                    local array_table = push_array();
                    local test_table = array_table:to_table();
                    return test_table[2] == 2;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
        
		return lua_tinker::call<bool>(L, "test_lua_array_3",g_array_int);
	};

}