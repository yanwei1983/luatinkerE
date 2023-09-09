# luatinkerE

[![Join the chat at https://gitter.im/yanwei1983/luatinkerE](https://badges.gitter.im/yanwei1983/luatinkerE.svg)](https://gitter.im/yanwei1983/luatinkerE?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.org/yanwei1983/luatinkerE.svg?branch=master)](https://travis-ci.org/yanwei1983/luatinkerE)
[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)

LUA to C++ Binding Library "lua_tinker" expand for c++14 and lua 5.3

## why luatinker"E"?

0xE=14, \^_^  
E=Expand，we add a lot of feature to luatinker

we use Variadic Template and index_sequence, SFINAE enable_if and type_traits, tuple, function, forward_ref, decltype(auto), and more in c++11/14

add support when use c++11

complied with vc2015,g++4.8,g++5.0,clang3.6

QQ group:518398988
## new feature

***
* Functions imported to Lua now support any number of parameters
* read / push element now support std::string,std::function,lua_function_ref, std::shared_ptr, and stl container
* Support for inexact matching function overload of up to 16 parameters (generate signature after downcast of C++ type to Lua type), support for normal function, class member function, constructor
* Support the matching of function default parameters and overloaded functions with default parameters
* Remove Int64 related functions, and use luainterager of lua5.3 instead
* Support to return multiple return values when calling Lua function and wrap them with tuple
* Support to open type consistency check and constant class member function check through macro definition
* Support to allow registered shares through macro definition_ PTR object calls class member function
* Support class static function registration
* Support class static variable registration
* Support property registration
* It supports inheritance of multiple parent classes. When searching, search in order and depth first
* Support to register function / variable / enumeration / class in namespace
* Support for registering nested classes
* Support to extend registered classes in Lua
* Support registering a new class in Lua to inherit the registered class


***




### detail info 
* move code implementation to namespace detail
* use Variadic Template to modify old func "functor/memberfunctor/constructor/call "    
* use enable_if to hides a function overload  
* use stack_help class to handle read/push function  
* remove int64 like function just use luaInteger in lua5.3  
* stl container can push a table to lua/ read a table from lua  
* can pop tuple from lua to warp multi-return value  
* use weak_ptr to hold a shared_obj in lua, to avoid lua control c++ object's lifetime  
* when push a shared_ptr who only has 1 refcount(a r-reference) ,will use lua to hold shared obj, let lua to control c++ object's lifetime  
* do not allow shared_ptr auto convert to raw_ptr, registered shared_ptr can inovke default registered member func "_get_raw_ptr()" to get raw_ptr  
* class_add function adds argument bInitShared to register a class objects's shared_ptr  
* unregistered shared_ptr objects using the default same metatable to gc  
* can register a std::function obj through function_warp  
* can read/push a lua_function_ref<RVal> with luafunction(use LUA_REGISTRYINDEX), lua_function_ref is a ref-count obj, when all lua_function_ref was released,lua_function will unref from lua regtable, if invoke function when lua was closed will throw a std::runtime_error  
* can read/push a std::function obj warp for lua_function_ref<RVal>      
* can def _ALLOW_SHAREDPTR_INVOKE to allow shared_ptr to invoke member_func   
* call register_lua_close_callback reg a callback func, when lua close it will be callback  
* allow to push a const obj/ref/pointer, but it will lost const qualifier, plz used carefully 
* define macro LUATINKER_USERDATA_HOLD_CONST will store const info(bool) in userdata,will check when call member function,will check lost const qualifier when read   
* args_type_overload_functor/member_functor/constructor was simple convert c++ params list to luatype, then stored in int64 as a function signature, do non-exact matching when function name invoked  
* add function class_def_static for register class static member function,use class.foo() to invoke, plz don't use class:foo()  
* add function class_property for register get/set function for a member_name  
* add function class_static_mem for register class static ver  
* add function class_mem_readonly for register class const ver
* add function class_mem_readonly for register class const static ver
* when call def/class_def/class_def_static/class_con function,can push params's default value. when invoke in lua, if params not enough, will use default values   
* overload like function allow add default params, manual generation is not recommended, plz use autogen tools "export2lua"  
* define macro LUATINKER_MULTI_INHERITANCE，will allow call class_inh multi-times. sequence searching when invoke,depth-first, spend more time than single inheritance
* add function namespace_add for register a namespace
* add function namespace_def for register a function in namespace
* add function namespace_set/get for register a ver or enum in namespace
* add function scope_inner relate between two metatable，getmetatable(scope_global_name)[name] = getmetatable(global_name), to implement namespace and inner class 's relationship
* can use lua_create_class(class_name,base_name) in lua to register a new class inhert base


