#include "lua_tinker.h"
#include"test.h"





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

std::shared_ptr<ff> pass_shared_from_lua(std::shared_ptr<ff> val)
{
	return val;
}


std::weak_ptr<ff> make_ff_weak()
{
	return std::weak_ptr<ff>(g_ff_shared);
}

bool visot_ffbase(ff_base* pFF)
{
	if (pFF)
	{
		return true;
	}
	return false;
}

bool visot_ff_other_baseA(ff_other_baseA* pFF)
{
	if (pFF)
	{
		return true;
	}
	return false;
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

}
void visot_ff_const_ref(const ff& refFF)
{

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

std::shared_ptr<int> g_shared_int;

const std::shared_ptr<int>& get_shared_int()
{
	if (!g_shared_int)
	{
		g_shared_int = std::shared_ptr<int>(new int(7));
	}
	return g_shared_int;
}

int visit_shared_int(std::shared_ptr<int> shared_int)
{
	if (shared_int)
	{
		return *shared_int;
	}
	return 0;
}

int visit_shared_int_constref(const std::shared_ptr<int>& shared_int)
{
	if (shared_int)
	{
		return *shared_int;
	}
	return 0;
}

LUA_TEST(sharedptr)
{
	g_test_func_set["test_lua_shared_1"] = [L]()->bool
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

	g_test_func_set["test_lua_shared_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_shared_2()
					local pFFShared =  make_ff();
					visot_ff(pFFShared);		--error sharedptr->raw_ptr				
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());

		lua_tinker::set_error_callback(L, [](lua_State *L) -> int
		{
			std::string errinfo(lua_tostring(L, -1));
			if (errinfo != "can't convert argument 1 from ff__shared_ptr to class ff")
			{
				lua_tinker::on_error(L);
			}
			return 0;
		});

		lua_tinker::call<void>(L, "lua_test_shared_2");
		lua_tinker::set_error_callback(L, &lua_tinker::on_error);
		return true;

	};
	g_test_func_set["test_lua_shared_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function lua_test_shared_3()
					local pFFShared =  make_ff();
					visot_ff_weak(pFFShared);	--error shared_ptr to weak_ptr
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::set_error_callback(L, [](lua_State *L) -> int
		{
			std::string errinfo(lua_tostring(L, -1));
			if (errinfo != "can't convert argument 1 from ff__shared_ptr to class ")
			{
				lua_tinker::on_error(L);
			}
			return 0;
		});
		lua_tinker::call<void>(L, "lua_test_shared_3");
		lua_tinker::set_error_callback(L, &lua_tinker::on_error);

		return true;

	};


	g_test_func_set["test_lua_shared_4"] = [L]()->bool
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

	g_test_func_set["test_lua_shared_5"] = [L]()->bool
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

	g_test_func_set["test_lua_shared_6"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_shared_6()
					local shared_int = get_shared_int();
					return 7 == visit_shared_int_constref(shared_int);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_shared_6") && g_shared_int.use_count() == 1;
	};

	g_test_func_set["test_lua_shared_7"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_shared_7()
					local shared_int = get_shared_int();
					return 7 == visit_shared_int(shared_int);
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_shared_7") && g_shared_int.use_count() == 1;
	};

	g_test_func_set["test_lua_shared_8"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_shared_8()
					local shared_ff = make_ff_to_lua();
					shared_ff.m_val = 77;

					local shared_ff_copy = pass_shared_from_lua(shared_ff);
					local raw_ff_copy = shared_ff_copy:_get_raw_ptr();
					local raw_ff = shared_ff:_get_raw_ptr();


					return raw_ff_copy.m_val == 77;
				end
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		return  lua_tinker::call<bool>(L, "test_lua_shared_8");
	};

	g_test_func_set["test_lua_weak_1"] = [L]()->bool
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


	g_test_func_set["test_lua_weak_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_weak_2()
					local pFFWeak = make_ff_weak();
					visot_ff(pFFWeak);		--error weak_ptr to shared_ptr
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::set_error_callback(L, [](lua_State *L) -> int
		{
			std::string errinfo(lua_tostring(L, -1));
			if (errinfo != "can't convert argument 1 to class ff")
			{
				lua_tinker::on_error(L);
			}
			return 0;
		});
		lua_tinker::call<void>(L, "test_lua_weak_2");
		lua_tinker::set_error_callback(L, &lua_tinker::on_error);

		return true;

	};

	g_test_func_set["test_lua_shared_invoke_1"] = [L]()->bool
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
	g_test_func_set["test_lua_shared_invoke_2"] = [L]()->bool
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
	g_test_func_set["test_lua_shared_invoke_3"] = [L]()->bool
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



	g_test_func_set["test_lua_hold_shared_ptr"] = [L]()->bool
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


	g_test_func_set["test_lua_nodef_shared_1"] = [L]()->bool
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
	g_test_func_set["test_lua_nodef_shared_2"] = [L]()->bool
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

	g_test_func_set["test_lua_nodef_shared_3"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_nodef_shared_3()
					local pFF_nodef = make_ff_nodef();
					visot_ff_nodef_shared(pFF_nodef);	--raw_ptr to shared_ptr
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::set_error_callback(L, [](lua_State *L) -> int
		{
			std::string errinfo(lua_tostring(L, -1));
			if (errinfo != "shared_ptr _read 1 to class __shared_ptr,nil or not shared")
			{
				lua_tinker::on_error(L);
			}
			return 0;
		});
		lua_tinker::call<void>(L, "test_lua_nodef_shared_3");
		lua_tinker::set_error_callback(L, &lua_tinker::on_error);

		return true;

	};

	g_test_func_set["test_lua_nodef_shared_4"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_nodef_shared_4()
					local pFF_nodef_Shared = make_ff_nodef_shared();
					visot_ff_nodef(pFF_nodef_Shared);	--shared_ptr to raw_ptr
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::set_error_callback(L, [](lua_State *L) -> int
		{
			std::string errinfo(lua_tostring(L, -1));
			if (errinfo != "can't convert argument 1 to class ")
			{
				lua_tinker::on_error(L);
			}
			return 0;
		});
		lua_tinker::call<void>(L, "test_lua_nodef_shared_4");
		lua_tinker::set_error_callback(L, &lua_tinker::on_error);

		return true;

	};


}