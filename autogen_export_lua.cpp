//this file was auto generated, plz don't modify it
#include "lua_tinker.h"
#include "test.h"
void export_to_lua_auto(lua_State* L)
{
lua_tinker::def(L, "Number2Interger",&Number2Interger);
lua_tinker::def(L, "addUL",&addUL);
lua_tinker::def(L, "connect_string",&connect_string);
lua_tinker::def(L, "g_addint_double",&g_addint_double);
lua_tinker::def(L, "get_c_function",&get_c_function);
lua_tinker::def(L, "get_gdouble",&get_gdouble);
lua_tinker::def(L, "get_gff_cref",&get_gff_cref);
lua_tinker::def(L, "get_gff_ptr",&get_gff_ptr);
lua_tinker::def(L, "get_gint",&get_gint);
lua_tinker::def(L, "get_gintptr",&get_gintptr);
lua_tinker::def(L, "get_gintref",&get_gintref);
lua_tinker::def(L, "gint_add1",&gint_add1);
lua_tinker::def(L, "gint_add_intref",&gint_add_intref);
lua_tinker::def(L, "gint_addint",&gint_addint);
lua_tinker::def(L, "gint_addintptr",&gint_addintptr);
lua_tinker::def(L, "gint_addintref",&gint_addintref);
lua_tinker::def(L, "make_ff",&make_ff);
lua_tinker::def(L, "make_ff_nodef",&make_ff_nodef);
lua_tinker::def(L, "make_ff_nodef_shared",&make_ff_nodef_shared);
lua_tinker::def(L, "make_ff_to_lua",&make_ff_to_lua);
lua_tinker::def(L, "make_ff_weak",&make_ff_weak);
lua_tinker::def(L, "push_hashmap",&push_hashmap);
lua_tinker::def(L, "push_map",&push_map);
lua_tinker::def(L, "push_set",&push_set);
lua_tinker::def(L, "push_string",&push_string);
lua_tinker::def(L, "push_string_ref",&push_string_ref);
lua_tinker::def(L, "push_vector",&push_vector);
lua_tinker::def(L, "store_lua_function",&store_lua_function);
lua_tinker::def(L, "test_default_params",&test_default_params, 5, 8);
lua_tinker::def(L, "test_lua_function",&test_lua_function);
lua_tinker::def(L, "test_lua_function_ref",&test_lua_function_ref);
lua_tinker::def(L, "test_overload", lua_tinker::args_type_overload_functor(
	lua_tinker::make_functor_ptr((int(*)(int))(&test_overload)), 
	lua_tinker::make_functor_ptr((int(*)(int, double))(&test_overload)), 
	lua_tinker::make_functor_ptr((int(*)(int, int, double))(&test_overload))));
lua_tinker::def(L, "test_overload_default", lua_tinker::args_type_overload_functor(
	lua_tinker::make_functor_ptr((int(*)(int, bool))(&test_overload_default)), 
	lua_tinker::make_functor_ptr((int(*)(int, int, bool))(&test_overload_default),1 /*default_args_count*/, 1 /*default_args_start*/ ), 
	lua_tinker::make_functor_ptr((int(*)(int, int, int, double, double, double, double, const std::string &))(&test_overload_default),5 /*default_args_count*/, 2 /*default_args_start*/ )), true, 1.0, 2.0, 3.0, 4.0, std::string("test"));
lua_tinker::def(L, "test_overload_err", lua_tinker::args_type_overload_functor(
	lua_tinker::make_functor_ptr((int(*)(int))(&test_overload_err)), 
	lua_tinker::make_functor_ptr((int(*)(double))(&test_overload_err))));
lua_tinker::def(L, "use_stored_lua_function",&use_stored_lua_function);
lua_tinker::def(L, "visot_ff",&visot_ff);
lua_tinker::def(L, "visot_ff_const_ref",&visot_ff_const_ref);
lua_tinker::def(L, "visot_ff_nodef",&visot_ff_nodef);
lua_tinker::def(L, "visot_ff_nodef_shared",&visot_ff_nodef_shared);
lua_tinker::def(L, "visot_ff_ref",&visot_ff_ref);
lua_tinker::def(L, "visot_ff_shared",&visot_ff_shared);
lua_tinker::def(L, "visot_ff_weak",&visot_ff_weak);
lua_tinker::set(L,"g_c_double",g_c_double);
lua_tinker::set(L,"g_c_int",g_c_int);
lua_tinker::class_add<ff_base>(L, "ff_base",true);
lua_tinker::class_def<ff_base>(L, "test_base_callfn",&ff_base::test_base_callfn);
lua_tinker::class_add<ff>(L, "ff",true);
lua_tinker::class_inh<ff,ff_base>(L);
lua_tinker::class_def<ff>(L, "add",&ff::add);
lua_tinker::class_def<ff>(L, "add_ffcref",&ff::add_ffcref);
lua_tinker::class_def<ff>(L, "add_ffptr",&ff::add_ffptr);
lua_tinker::class_def<ff>(L, "getVal",&ff::getVal);
lua_tinker::class_def<ff>(L, "get_static_val",&ff::get_static_val);
lua_tinker::class_def<ff>(L, "setVal",&ff::setVal);
lua_tinker::class_def<ff>(L, "test_const",&ff::test_const);
lua_tinker::class_def<ff>(L, "test_default_params",&ff::test_default_params, 5, 8);
lua_tinker::class_def<ff>(L, "test_memfn",&ff::test_memfn);
lua_tinker::class_def<ff>(L, "test_overload", lua_tinker::args_type_overload_member_functor(
	lua_tinker::make_member_functor_ptr((int(ff::*)(int)const)(&ff::test_overload)), 
	lua_tinker::make_member_functor_ptr((int(ff::*)(int, double))(&ff::test_overload)), 
	lua_tinker::make_member_functor_ptr((int(ff::*)(int, int, double))(&ff::test_overload))));
lua_tinker::class_con<ff>(L,lua_tinker::args_type_overload_constructor(
	new lua_tinker::constructor<ff, int>(1 /*default_args_count*/, 1 /*default_args_start*/ ), 
	new lua_tinker::constructor<ff, double, unsigned char, int>(1 /*default_args_count*/, 2 /*default_args_start*/ )), 0, 1);
lua_tinker::class_mem<ff>(L,"m_val",&ff::m_val);
lua_tinker::class_mem_static<ff>(L,"s_ref",&ff::s_ref);
lua_tinker::class_mem_static<ff>(L,"s_val",&ff::s_val);
}
