# luatinkerE

[![Join the chat at https://gitter.im/yanwei1983/luatinkerE](https://badges.gitter.im/yanwei1983/luatinkerE.svg)](https://gitter.im/yanwei1983/luatinkerE?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.org/yanwei1983/luatinkerE.svg?branch=master)](https://travis-ci.org/yanwei1983/luatinkerE)
[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)

[english](README_english.md)

LUA-C++ 绑定库"lua_tinker" 扩展为支持c++14和lua 5.3

## 为啥要叫 luatinker"E"?

0xE=14, \^_^  
E=Expand，我们为luatinker增加了大量的新特性

我们使用了 c++11/14 中大量的新特新: 可变模板参数，整数序列，基于enable_if的SFINAE,tuple,function对象, forward_ref, decltype(auto)

master版本最低支持c++11
c++17分支使用if constexpr来优化代码的可读性

complied with vc2015,g++4.8,g++5.0,clang3.6

QQ群:518398988
## new feature 新特性一览

* 导入lua的函数，现在支持任意多的参数
* 支持的read/push的元素类型增加std::string,std::function,lua_function_ref,stl容器,std::shared_ptr对象
* 支持最多16个参数的非精确匹配的函数重载(c++类型downcast到lua类型后生成签名)， 支持普通函数、类成员函数、构造函数
* 支持函数默认参数及带默认参数的重载函数的匹配
* 移除int64相关函数，使用lua5.3的luaInterager来替代  
* 支持调用lua函数时返回多个返回值用tuple包裹  
* 支持通过宏定义打开类型一致性检查和常量类成员函数检查
* 支持通过宏定义允许已注册的shared_ptr对象调用类成员函数    
* 支持类静态函数注册
* 支持类静态变量注册
* 支持property注册
* 支持继承多个父类，查找时根据顺序依次查找，深度优先
* 支持注册namespace及内的函数/变量/枚举/类
* 支持注册嵌套的类
* 支持在lua中扩展已经注册的类
* 支持在lua中注册一个新类继承已经注册的类

***



***




### detail info 详细功能说明
* 将内部实现移动到detail名字空间
* 使用变参模板重写 functor/memberfunctor/constructor/call 函数来支持多参数  
* 使用enable_if机制实现 函数条件重载  
* 通过stack_help类实现read/push函数  
* 移除int64相关函数，使用lua5.3的luaInterager来替代  
* 加入stl容器类向lua导入/导出一个table的功能  
* 可以从lua中返回多个返回值用tuple包裹  
* 使用weak_ptr来存储导出到lua的shared_ptr对象来避免lua控制c++对象生命周期  
* 当push到lua的shared_ptr只有1个引用时(一个右值引用)，使用lua来储存shared_ptr对象，由lua控制该对象生命周期
* 不允许shared_ptr对象到raw_ptr的自动转换,已注册shared_ptr可以通过默认注册的成员函数_get_raw_ptr()来获取一个raw_ptr  
* class_add函数增加参数bInitShared来注册导出类对应的shared_ptr对象  
* 未注册shared_ptr对象统一使用默认metatable来GC  
* 可以向lua注册一个std::function对象（通过functor/memberfunctor warp类）  
* 可以read/push一个lua_function_ref<RVal>来对应lua内部的function(使用LUA_REGISTRYINDEX), lua_function_ref是一个引用计数对象, 全部释放后将会从lua中unref,如果lua关闭后再调用该对象将产生一个std::runtime_error  
* 可以read/push一个std::function对象来包裹 lua_function_ref<RVal>    
* 通过定义宏 _ALLOW_SHAREDPTR_INVOKE 可以允许已注册的shared_ptr对象调用类成员函数    
* 通过调用register_lua_close_callback注册回调函数，当lua关闭时回调  
* 允许向lua导出常量对象，但是会丢失常量限定符，请注意 
* 定义宏 LUATINKER_USERDATA_CHECK_CONST 将会储存const信息(bool)到userdata中，当调用成员函数时会检查是否允许调用,同时read是会检查是否发生丢失常量限定符的问题  
* 定义宏 LUATINKER_USERDATA_CHECK_TYPEINFO 将会储存typeidx信息(size_t)到userdata中，当read时将进行类型一致性检查  
* 调试模式下自动打开LUATINKER_USERDATA_CHECK_CONST和LUATINKER_USERDATA_CHECK_TYPEINFO 进行额外的检查  
* args_type_overload_functor/member_functor/constructor是简单的将c++参数列表转换为luatype后存储到int64作为函数签名，运行时进行非精确匹配  
* 新增class_def_static函数可以注册类静态函数,使用class.foo()来调用，不要使用class:foo() 
* 新增class_property函数可以为一个名字注册一对get/set函数  
* 新增class_mem_static函数可以注册类静态变量
* 新增class_mem_readonly注册只读变量(const变量) 
* 新增class_mem_static_readonly注册只读静态变量(const变量)  
* 允许调用def/class_def/class_def_static/class_con时同时加入参数默认值，当lua中invoke时，如果参数不足，会使用参数默认值  
* overload相关函数允许加入参数默认值，但不推荐人工生成，请使用自动化生成工具export2lua  
* 定义宏LUATINKER_MULTI_INHERITANCE，将会允许继承多个父类，查找时根据顺序依次查找，深度优先，比单次继承是多了一个继承表遍历的过程
* 通过namespace_add注册一个namespace
* 通过namespace_def注册一个namespace中的函数
* 通过namespace_set/get 注册一个namespace中的变量或枚举
* 通过scope_inner关联meta表，getmetatable(scope_global_name)[name] = getmetatable(global_name),来实现namespace, inner class的关联
* 通过在lua中调用lua_create_class(class_name,base_name)来注册一个新的类继承base



## 范例

### 全局函数
```
lua_tinker::def("func_name", &func);
```
##### 从lua调用c++函数

普通全局函数被注册在lua全局表中，所以你可以直接调用
```
func_name(xxx,xxx,xxx)
```

##### 从c++调用lua函数
```
ret_val_t ret = lua_tinker::call<ret_val_t>("func_name", param1, param2);
```

### 全局变量
```
//设置一个变量到lua全局表
lua_tinker::set("var_name", &var);
//lua全局表读取一个变量
var_t var = lua_tinker::get<var_t>("var_name");
```

### 注册一个类
```
lua_tinker::class_add<class_t>("class_name");
```
##### 构造函数
```
lua_tinker::class_con<class_t>(&class_t::class_t);
//这样你就能在lua中通过同名函数的方式直接构造一个class对象
```

```
local class_instance = class_t(param1);
```

##### 成员函数
```
lua_tinker::class_def<class_t>("func_name", &class_t::func);
```

lua
```
class_instance:func();
```

##### 成员变量
```
lua_tinker::class_mem<class_t>("member_name", &class_t::member);
```

lua
```
if class_instance.member == 99 then
    class_instance.member = 88;
endif
```

##### 成员常量
```
lua_tinker::class_mem_readonly<class_t>("member_name", &class_t::member);
```

lua
```
function test_lua_member_readonly_1()
    local pFF1 = ff(1);
    pFF1.m_member_readonly = 88; --error readonly
end
```

##### 静态成员变量
```
lua_tinker::class_mem_static<class_t>("member_name", &class_t::member);
```
lua
```
function test_lua_member_static()
    local pFF1 = ff(1);
    pFF1.s_val = 901;
    local pFF2 = ff(2);
    return pFF2.s_val == 901;
end
```
##### 静态成员常量
```
lua_tinker::class_mem_static_readonly<class_t>("member_name", &class_t::member);
```

##### 类内定义的枚举成员常量
```
lua_tinker::class_var_static<ff>(L, "ENUM_1", ff::ENUM_1);
```
lua
```
function test_lua_var_static()
    return ff.ENUM_1;
end
```

##### 通过一对get/set实现property

```
lua_tinker::class_property<ff>(L, "m_prop", &ff::getVal, &ff::setVal);
lua_tinker::class_property<ff>(L, "m_prop_readonly", &ff::getVal, nullptr);
```
lua
```
function test_lua_member_readonly_2()
    local pFF1 = ff(1);
    local val = pFF1.m_prop_readonly; 
    pFF1.m_prop_readonly = 88; --error readonly
end
```

##### 继承
```
lua_tinker::class_add<ff_base>(L, "ff_base");
lua_tinker::class_add<ff_other>(L, "ff_other");
lua_tinker::class_add<ff>(L, "ff");

lua_tinker::class_inh<ff, ff_base>(L);
lua_tinker::class_inh<ff, ff_other>(L);
```
这样就ff对象就能在lua中访问基类注册的函数
多继承需要打开宏开关

##### 在lua中继承一个c++类
```
lua_create_class("ff_inlua","ff")
function ff_inlua:test(x)
    return self:getVal() + x;
end
function test_lua_inhert_lua()
    local local_ff = ff_inlua(1);
    return local_ff:test(3) == (1+3);
end
```
### tuple
```
std::tuple<int, int> push_tuple()
{
    return std::make_tuple(7,10);
}
bool test_tuple(std::tuple<int, int> tuple)
{
    return std::get<0>(tuple) == 8 && std::get<1>(tuple) == 9;
}
lua_tinker::def("push_tuple",&push_tuple);
lua_tinker::def("test_tuple",&test_tuple);
```
lua
```
local tuple_table = push_tuple();
return tuple_table[1] == 7 and tuple_table[2] == 10;
```
```
local tuple_table = {8, 9};
return test_tuple(tuple_table);
```

### 多返回值
```
function test_lua_multireturn()
    return 1,2.0,3,4.0,"5"
end
```
c++
```
int c = 0;
double d = 0.0;
char e = 0;
float f = 0.0;
std::string g;
std::tie(c, d, e, f, g) = lua_tinker::call< std::tuple<int, double, char, float, std::string> >(L, "test_lua_multireturn");

return c == 1 && d == 2.0 && e == 3 && f == 4.0 && g == "5";
```


### stl容器
```

std::vector<int> g_testvec = { 2,4,6,8,10 };
std::vector<int> push_vector()
{
    return g_testvec;
}

std::vector<int>& push_vector_ref()
{
    return g_testvec;
}

const std::vector<int>& push_vector_const()
{
    return g_testvec;
}


lua_tinker::def("push_vector", &push_vector);
lua_tinker::def("push_vector_ref", &push_vector_ref);
lua_tinker::def("push_vector_const", &push_vector_const);
```

lua
```
local vec_copy = push_vector_const();
--遍历vector
local total_k = 0;
local total_v = 0;
for k,v in pairs(vec_copy) do
    total_k = total_k + k;
    total_v = total_v + v;
end
--转换为table后再遍历table
local vec_table = vec_copy:to_table();
local total_k1 = 0;
local total_v1 = 0;
for k,v in pairs(vec_table) do
    total_k1 = total_k1 + k;
    total_v1 = total_v1 + v;
end

local container_ref = push_vector_ref();
container_ref:push(15);
--container_ref={2,4,6,8,10,15}
container_ref:erase(6);
--container_ref={2,4,8,10,15}



```


### function对象
```
int test_lua_function(std::function<int(int)> func)
{
    return func(1);
}
int test_lua_function_ref(const std::function<int(int)>& func)
{
    return func(1);
}

std::function<int(int)> get_cpp_function()
{
    auto func = [](int v)->int
    {
        return v + 1;
    };
    return std::function<int(int)>(func);
}

lua_tinker::def("test_lua_function", &test_lua_function);
lua_tinker::def("test_lua_function_ref", &test_lua_function_ref);
lua_tinker::def("get_c_function", &get_c_function);

std::function<int(int, int)> func = [](int k, int j)->int {	return k + j; };
lua_tinker::def(L, "std_function_int_int", func);	//can hold function

std::function<int(int)> func_c = std::bind(func, std::placeholders::_1, 88);
lua_tinker::def(L, "std_function_int_bind88", func_c);
```

lua
```
local c_func = get_cpp_function();
return c_func(1) == 2;
```

##### 把lua函数当成function对象传递给c++
```
local upval = 0;
local localtest = function(intval)
    upval = upval+ intval;
    return upval;
end

return 1 == test_lua_function(localtest);
```

### lua_function_ref在c++层持有lua函数对象

lua
```
function test_lua_luafunction_ref_1()
    local upval = 0;
    local localtest = function (intval)
        upval = upval + intval;
        return upval;
    end
    return localtest;
end
```
c++
```
lua_tinker::lua_function_ref<int> lua_func = 
                   lua_tinker::call<decltype(lua_func)>(L, "test_lua_luafunction_ref_1");
//执行第一次
lua_func(7);
//执行第二次
return 14 == lua_func(7);
```


### table_onstack 用来访问栈上的table对象
lua
```
function test_lua_table_iter1()
    return {
            [1]={key=1,value="a"},
            [2]={key=2,value="b"},
            };
end
```

c++
```
lua_tinker::table_onstack table = lua_tinker::call<decltype(table)>(L, "test_lua_table_iter1");
lua_tinker::table_onstack t1 = table.get<lua_tinker::table_onstack>(1);
if(t1.get<int>("key") != 1 || t1.get<std::string>("value") != "a")
    return false;
lua_tinker::table_onstack t2 = table.get<lua_tinker::table_onstack>(2);
if(t2.get<int>("key") != 2 || t2.get<std::string>("value") != "b")
    return false;
```

### table_ref 在c++层长期持有一个table对象
lua
```
g_test_table_ref={[1]=10, test1=5,};
```

c++
```
lua_tinker::table_ref g_table_ref;
lua_tinker::table_onstack table(L, "g_test_table_ref");
g_table_ref = lua_tinker::table_ref::make_table_ref(table);

//每次使用table_ref必须将他push到栈上才能使用 table_ref本身不是一个table
lua_tinker::table_onstack refTable = g_table_ref.push_table_to_stack();
refTable.get<int>(1) == 10;
```

### 全局函数/成员函数/构造函数都支持 默认参数和函数重载
### 函数重载
```
int test_overload(int n)
{
    return n;
}

int test_overload(double d)
{
    return int(d);
}


int test_overload(int n, double d)
{
    return n + (int)d;
}

int test_overload(int n1, int n2, double d)
{
    return n1 + n2 + (int)d;
}

lua_tinker::def(L, "test_overload", lua_tinker::args_type_overload_functor(
        lua_tinker::make_functor_ptr((int(*)(int))(&test_overload)),
        lua_tinker::make_functor_ptr((int(*)(double))(&test_overload)),
        lua_tinker::make_functor_ptr((int(*)(int, double))(&test_overload)),
        lua_tinker::make_functor_ptr((int(*)(int, int, double))(&test_overload))));
```

lua
```
test_overload(1) == 1;
test_overload(2.0) == 2;
test_overload(1,2.0) == 3;
test_overload(1,2,3.0) == 6;
```


### 默认参数
```
int test_overload_default(int n, bool b)
{
    return n;
}

int test_overload_default(int n1, int n2, bool b = true)
{
    return n1 + n2;
}


int test_overload_default(int n1, int n2, int n3, double d1 = 1.0, double d2 = 2.0, double d3 = 3.0, double d4 = 4.0, const std::string &refString = std::string("test"))
{
    return n1 + n2;
}

lua_tinker::def(L, "test_overload_default", lua_tinker::args_type_overload_functor(
        lua_tinker::make_functor_ptr((int(*)(int, bool))(&test_overload_default)),
        lua_tinker::make_functor_ptr((int(*)(int, int, bool))(&test_overload_default), 1 /*default_args_count*/, 1 /*default_args_start*/),
        lua_tinker::make_functor_ptr((int(*)(int, int, int, double, double, double, double, const std::string &))(&test_overload_default), 5 /*default_args_count*/, 2 /*default_args_start*/)), 
        true, 1.0, 2.0, 3.0, 4.0, std::string("test"));
```


```
test_overload_default(1,2) == 3;
test_overload_default(1,2,false) == 3;
test_overload_default(1,2,3) == 3;
```


### namespace
##### 通过namespace_add注册一个namespace
`lua_tinker::namespace_add(L, "NS_TEST");`
##### 通过namespace_def注册一个namespace中的函数
`lua_tinker::namespace_def(L, "NS_TEST", "test_function_in_namespace", &NS_TEST::test_function_in_namespace);`
##### 通过namespace_set/get 注册一个namespace中的变量或枚举
`lua_tinker::namespace_set(L, "NS_TEST", "ENUM_1", NS_TEST::ENUM_1);`
##### 通过scope_inner关联meta表，getmetatable(scope_global_name)[name] = getmetatable(global_name),来实现namespace, inner class的关联
```
namespace NS_TEST
{
    namespace NS_INNER
    {
        struct Test
        {
            struct Iterator
            {
                enum class ENUM_T
                {
                    ENUM_1 = 8,
                    ENUM_2 = 6,
                    ENUM_3 = 2,
                };
            };
        };
    }
}
```

```
lua_tinker::scope_inner(L, "NS_TEST::NS_INNER::Test::Iterator", "ENUM_T", "NS_TEST::NS_INNER::Test::Iterator::ENUM_T");
lua_tinker::scope_inner(L, "NS_TEST::NS_INNER::Test", "Iterator", "NS_TEST::NS_INNER::Test::Iterator");
lua_tinker::scope_inner(L, "NS_TEST::NS_INNER", "Test", "NS_TEST::NS_INNER::Test");
lua_tinker::scope_inner(L, "NS_TEST", "NS_INNER", "NS_TEST::NS_INNER");
```