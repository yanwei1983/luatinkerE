# luatinkerE
lua_tinker for c++14 and lua 5.3

##why luatinker"E"?

0xE=14, ^_^


we use Variadic Template and index_sequence, SFINAE enable_if and type_traits, tuple, function, forward_ref, decltype(auto), and more in c++11/14


complied with vc2015,gcc5.3,clang3.8


##what modifyed

使用变参模板重写 functor/memberfunctor/constructor/call 函数来支持多参数  
使用enable_if机制实现 函数条件重载  
通过stack_help类实现read/push函数  
移除int64相关函数，使用lua5.3的luaInterager来替代  
加入stl容器类向lua导入/导出一个table的功能  
可以从lua中返回多个返回值用tuple包裹  
使用weak_ptr来存储导出到lua的shared_ptr对象来避免lua控制c++对象生命周期
class_add函数增加参数bInitShared来注册导出类对应的shared_ptr对象
未注册shared_ptr对象统一使用默认metatable来GC
可以向lua注册一个std::function对象（通过functor/memberfunctor warp类）  
通过定义 _ALLOW_SHAREDPTR_INVOKE 可以允许已注册的shared_ptr对象调用类成员函数  
分支args_num_overload是简单的将c++参数列表转换为luatype后存储到int64作为函数签名，运行时进行非精确匹配


use Variadic Template to modify old func "functor/memberfunctor/constructor/call "  
use enable_if to hides a function overload  
use stack_help class to handle read/push function  
remove int64 like function just use luaInteger in lua5.3  
stl container can push a table to lua/ read a table from lua  
can pop tuple from lua to warp multi-return value  
use weak_ptr to hold a shared_obj in lua, to avoid lua control c++ object's lifetime
class_add function adds argument bInitShared to register a class objects's shared_ptr
Unregistered shared_ptr objects using the default same metatable to gc
can register a std::function obj through function_warp  
can def _ALLOW_SHAREDPTR_INVOKE to allow shared_ptr to invoke member_func  
branches "args_num_overload" was simple convert c++ params list to luatype, then stored in int64 as a function signature, do non-exact matching when function name invoked

