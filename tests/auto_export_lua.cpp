//this file was auto generated, plz don't modify it
#include "lua_tinker.h"
#include "test.h"
void export_to_lua_auto(lua_State* L)
{
	lua_tinker::def(L, "Number2Interger", &Number2Interger);
	lua_tinker::def(L, "addUL", &addUL);
	lua_tinker::def(L, "connect_string", &connect_string);
	lua_tinker::def(L, "g_addint_double", &g_addint_double);
	lua_tinker::def(L, "get_c_function", &get_c_function);
	lua_tinker::def(L, "get_gdouble", &get_gdouble);
	lua_tinker::def(L, "get_gff_cref", &get_gff_cref);
	lua_tinker::def(L, "get_gff_ptr", &get_gff_ptr);
	lua_tinker::def(L, "get_gint", &get_gint);
	lua_tinker::def(L, "get_gintptr", &get_gintptr);
	lua_tinker::def(L, "get_gintptr_const", &get_gintptr_const);
	lua_tinker::def(L, "get_gintref", &get_gintref);
	lua_tinker::def(L, "get_gintref_const", &get_gintref_const);
	lua_tinker::def(L, "get_shared_int", &get_shared_int);
	lua_tinker::def(L, "gint_add1", &gint_add1);
	lua_tinker::def(L, "gint_add_intref", &gint_add_intref);
	lua_tinker::def(L, "gint_addint", &gint_addint);
	lua_tinker::def(L, "gint_addintptr", &gint_addintptr);
	lua_tinker::def(L, "gint_addintptr_const", &gint_addintptr_const);
	lua_tinker::def(L, "gint_addintref", &gint_addintref);
	lua_tinker::def(L, "make_ff", &make_ff);
	lua_tinker::def(L, "make_ff_nodef", &make_ff_nodef);
	lua_tinker::def(L, "make_ff_nodef_shared", &make_ff_nodef_shared);
	lua_tinker::def(L, "make_ff_to_lua", &make_ff_to_lua);
	lua_tinker::def(L, "make_ff_weak", &make_ff_weak);
	lua_tinker::def(L, "pass_shared_from_lua", &pass_shared_from_lua);
	lua_tinker::def(L, "push_hashmap", &push_hashmap);
	lua_tinker::def(L, "push_map", &push_map);
	lua_tinker::def(L, "push_map_ref", &push_map_ref);
	lua_tinker::def(L, "push_set", &push_set);
	lua_tinker::def(L, "push_string", &push_string);
	lua_tinker::def(L, "push_string_ref", &push_string_ref);
	lua_tinker::def(L, "push_vector", &push_vector);
	lua_tinker::def(L, "push_vector_const", &push_vector_const);
	lua_tinker::def(L, "vector_join_const", &vector_join_const);
	lua_tinker::def(L, "store_lua_function", &store_lua_function);
	lua_tinker::def(L, "test_default_params", &test_default_params, 5, 8);
	lua_tinker::def(L, "test_lua_function", &test_lua_function);
	lua_tinker::def(L, "test_lua_function_ref", &test_lua_function_ref);
	lua_tinker::def(L, "test_overload", lua_tinker::args_type_overload_functor(
		lua_tinker::make_functor_ptr((int(*)(int))(&test_overload)),
		lua_tinker::make_functor_ptr((int(*)(int, double))(&test_overload)),
		lua_tinker::make_functor_ptr((int(*)(int, int, double))(&test_overload))));
	lua_tinker::def(L, "test_overload_default", lua_tinker::args_type_overload_functor(
		lua_tinker::make_functor_ptr((int(*)(int, bool))(&test_overload_default)),
		lua_tinker::make_functor_ptr((int(*)(int, int, bool))(&test_overload_default), 1 /*default_args_count*/, 1 /*default_args_start*/),
		lua_tinker::make_functor_ptr((int(*)(int, int, int, double, double, double, double, const std::string &))(&test_overload_default), 5 /*default_args_count*/, 2 /*default_args_start*/)), true, 1.0, 2.0, 3.0, 4.0, std::string("test"));
	lua_tinker::def(L, "test_overload_err", lua_tinker::args_type_overload_functor(
		lua_tinker::make_functor_ptr((void(*)(const char *))(&test_overload_err)),
		lua_tinker::make_functor_ptr((void(*)(const std::string &))(&test_overload_err))));
	lua_tinker::def(L, "use_stored_lua_function", &use_stored_lua_function);
	lua_tinker::def(L, "visit_shared_int", &visit_shared_int);
	lua_tinker::def(L, "visit_shared_int_constref", &visit_shared_int_constref);
	lua_tinker::def(L, "visot_ff", &visot_ff);
	lua_tinker::def(L, "visot_ff_const_ref", &visot_ff_const_ref);
	lua_tinker::def(L, "visot_ff_nodef", &visot_ff_nodef);
	lua_tinker::def(L, "visot_ff_nodef_shared", &visot_ff_nodef_shared);
	lua_tinker::def(L, "visot_ff_other_baseA", &visot_ff_other_baseA);
	lua_tinker::def(L, "visot_ff_ref", &visot_ff_ref);
	lua_tinker::def(L, "visot_ff_shared", &visot_ff_shared);
	lua_tinker::def(L, "visot_ff_weak", &visot_ff_weak);
	lua_tinker::def(L, "visot_ffbase", &visot_ffbase);
	lua_tinker::set(L, "g_c_double", g_c_double);
	lua_tinker::set(L, "g_c_int", g_c_int);
	lua_tinker::class_add<IntOpTest>(L, "IntOpTest", true);
	lua_tinker::class_def<IntOpTest>(L, "__mul", &IntOpTest::operator*);
	lua_tinker::class_def<IntOpTest>(L, "__add", &IntOpTest::operator+);
	lua_tinker::class_def<IntOpTest>(L, "__sub", &IntOpTest::operator-);
	lua_tinker::class_def<IntOpTest>(L, "__idiv", &IntOpTest::operator/);
	lua_tinker::class_def<IntOpTest>(L, "__lt", &IntOpTest::operator<);
	lua_tinker::class_def<IntOpTest>(L, "__le", &IntOpTest::operator<=);
	lua_tinker::class_def<IntOpTest>(L, "__eq", &IntOpTest::operator==);
	lua_tinker::class_con<IntOpTest>(L, lua_tinker::constructor<IntOpTest, int>::invoke);
	lua_tinker::class_mem<IntOpTest>(L, "m_n", &IntOpTest::m_n);
	lua_tinker::namespace_add(L, "NS_TEST");
	lua_tinker::namespace_def(L, "NS_TEST", "test_function_in_namespace", &NS_TEST::test_function_in_namespace);
	lua_tinker::namespace_set(L, "NS_TEST", "ENUM_1", NS_TEST::ENUM_1);
	lua_tinker::namespace_set(L, "NS_TEST", "ENUM_2", NS_TEST::ENUM_2);
	lua_tinker::namespace_set(L, "NS_TEST", "ENUM_3", NS_TEST::ENUM_3);
	lua_tinker::namespace_add(L, "NS_TEST::NS_INNER");
	lua_tinker::class_add<NS_TEST::NS_INNER::Test>(L, "NS_TEST::NS_INNER::Test", true);
	lua_tinker::class_def<NS_TEST::NS_INNER::Test>(L, "IsEqual", &NS_TEST::NS_INNER::Test::IsEqual);
	lua_tinker::class_def<NS_TEST::NS_INNER::Test>(L, "getIterator", &NS_TEST::NS_INNER::Test::getIterator);
	lua_tinker::class_def_static<NS_TEST::NS_INNER::Test>(L, "getStaticFunc", &NS_TEST::NS_INNER::Test::getStaticFunc);
	lua_tinker::class_mem<NS_TEST::NS_INNER::Test>(L, "m_iter", &NS_TEST::NS_INNER::Test::m_iter);
	lua_tinker::class_add<NS_TEST::NS_INNER::Test::Iterator>(L, "NS_TEST::NS_INNER::Test::Iterator", true);
	lua_tinker::namespace_add(L, "NS_TEST::NS_INNER::Test::Iterator::ENUM_T");
	lua_tinker::namespace_set(L, "NS_TEST::NS_INNER::Test::Iterator::ENUM_T", "ENUM_1", NS_TEST::NS_INNER::Test::Iterator::ENUM_T::ENUM_1);
	lua_tinker::namespace_set(L, "NS_TEST::NS_INNER::Test::Iterator::ENUM_T", "ENUM_2", NS_TEST::NS_INNER::Test::Iterator::ENUM_T::ENUM_2);
	lua_tinker::namespace_set(L, "NS_TEST::NS_INNER::Test::Iterator::ENUM_T", "ENUM_3", NS_TEST::NS_INNER::Test::Iterator::ENUM_T::ENUM_3);
	lua_tinker::scope_inner(L, "NS_TEST::NS_INNER::Test::Iterator", "ENUM_T", "NS_TEST::NS_INNER::Test::Iterator::ENUM_T");
	lua_tinker::scope_inner(L, "NS_TEST::NS_INNER::Test", "Iterator", "NS_TEST::NS_INNER::Test::Iterator");
	lua_tinker::scope_inner(L, "NS_TEST::NS_INNER", "Test", "NS_TEST::NS_INNER::Test");
	lua_tinker::scope_inner(L, "NS_TEST", "NS_INNER", "NS_TEST::NS_INNER");
	lua_tinker::class_add<TestCon>(L, "TestCon", true);
	lua_tinker::class_def<TestCon>(L, "ChangeDataMap", &TestCon::ChangeDataMap);
	lua_tinker::class_def<TestCon>(L, "ChangeDataMapPtr", &TestCon::ChangeDataMapPtr);
	lua_tinker::class_def<TestCon>(L, "ChangeDataMapRef", &TestCon::ChangeDataMapRef);
	lua_tinker::class_def<TestCon>(L, "ChangeDataSet", &TestCon::ChangeDataSet);
	lua_tinker::class_def<TestCon>(L, "ChangeDataSetRef", &TestCon::ChangeDataSetRef);
	lua_tinker::class_def<TestCon>(L, "TestFuncObj", &TestCon::TestFuncObj);
	lua_tinker::class_def<TestCon>(L, "getDataMap", &TestCon::getDataMap);
	lua_tinker::class_def<TestCon>(L, "getDataMapPtr", &TestCon::getDataMapPtr);
	lua_tinker::class_def<TestCon>(L, "getDataMapRef", &TestCon::getDataMapRef);
	lua_tinker::class_def<TestCon>(L, "getDataSet", &TestCon::getDataSet);
	lua_tinker::class_def<TestCon>(L, "getDataSetRef", &TestCon::getDataSetRef);
	lua_tinker::class_def<TestCon>(L, "getStr", &TestCon::getStr);
	lua_tinker::class_con<TestCon>(L, lua_tinker::constructor<TestCon, float, const char *, int>::invoke, -1.0f, "aa", 7);
	lua_tinker::class_mem<TestCon>(L, "m_fVal", &TestCon::m_fVal);
	lua_tinker::class_mem<TestCon>(L, "m_nVal", &TestCon::m_nVal);
	lua_tinker::class_mem<TestCon>(L, "m_str", &TestCon::m_str);
	lua_tinker::class_add<ff>(L, "ff", true);
	lua_tinker::class_def<ff>(L, "add", &ff::add);
	lua_tinker::class_def<ff>(L, "add_ffcref", &ff::add_ffcref);
	lua_tinker::class_def<ff>(L, "add_ffptr", &ff::add_ffptr);
	lua_tinker::class_def<ff>(L, "getVal", &ff::getVal);
	lua_tinker::class_def_static<ff>(L, "get_inner_ptr", &ff::get_inner_ptr);
	lua_tinker::class_def<ff>(L, "get_static_val", &ff::get_static_val);
	lua_tinker::class_def<ff>(L, "setVal", &ff::setVal);
	lua_tinker::class_def<ff>(L, "test_const", &ff::test_const);
	lua_tinker::class_def<ff>(L, "test_default_params", &ff::test_default_params, 5, 8);
	lua_tinker::class_def<ff>(L, "test_memfn", &ff::test_memfn);
	lua_tinker::class_def<ff>(L, "test_overload", lua_tinker::args_type_overload_member_functor(
		lua_tinker::make_member_functor_ptr((int(ff::*)(int)const)(&ff::test_overload)),
		lua_tinker::make_member_functor_ptr((int(ff::*)(int, double))(&ff::test_overload)),
		lua_tinker::make_member_functor_ptr((int(ff::*)(int, int, double))(&ff::test_overload))));
	lua_tinker::class_def_static<ff>(L, "visit_inner_ptr", &ff::visit_inner_ptr);
	lua_tinker::class_con<ff>(L, lua_tinker::args_type_overload_constructor(
		new lua_tinker::constructor<ff, double, unsigned char, int>(1 /*default_args_count*/, 1 /*default_args_start*/),
		new lua_tinker::constructor<ff, int>(1 /*default_args_count*/, 2 /*default_args_start*/)), 1, 0);
	lua_tinker::class_mem_readonly<ff>(L, "m_const_val", &ff::m_const_val);
	lua_tinker::class_mem<ff>(L, "m_val", &ff::m_val);
	lua_tinker::class_mem_static_readonly<ff>(L, "s_const_val", &ff::s_const_val);
	lua_tinker::class_mem_static<ff>(L, "s_ref", &ff::s_ref);
	lua_tinker::class_mem_static<ff>(L, "s_val", &ff::s_val);
	lua_tinker::class_var_static<ff>(L, "ENUM_1", ff::ENUM_1);
	lua_tinker::class_var_static<ff>(L, "ENUM_2", ff::ENUM_2);
	lua_tinker::class_var_static<ff>(L, "ENUM_3", ff::ENUM_3);
	lua_tinker::class_add<ff::inner>(L, "ff::inner", true);
	lua_tinker::class_def<ff::inner>(L, "test_func", &ff::inner::test_func);
	lua_tinker::class_def_static<ff::inner>(L, "test_static_func", &ff::inner::test_static_func);
	lua_tinker::scope_inner(L, "ff", "inner", "ff::inner");
	lua_tinker::class_add<ff_base>(L, "ff_base", true);
	lua_tinker::class_def<ff_base>(L, "test_base_callfn", &ff_base::test_base_callfn);
	lua_tinker::class_add<ff_other>(L, "ff_other", true);
	lua_tinker::class_add<ff_other_base>(L, "ff_other_base", true);
	lua_tinker::class_def<ff_other_base>(L, "test_other_callfn", &ff_other_base::test_other_callfn);
	lua_tinker::class_add<ff_other_baseA>(L, "ff_other_baseA", true);
	lua_tinker::class_add<ff_other_baseB>(L, "ff_other_baseB", true);
	lua_tinker::class_inh<ff, ff_base>(L);
	lua_tinker::class_inh<ff, ff_other>(L);
	lua_tinker::class_inh<ff_other, ff_other_baseA>(L);
	lua_tinker::class_inh<ff_other, ff_other_baseB>(L);
	lua_tinker::class_inh<ff_other_baseB, ff_other_base>(L);


}
