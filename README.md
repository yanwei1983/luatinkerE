# luatinkerE
lua_tinker for c++14 and lua 5.3

##why luatinker"E"?

0xE=14, ^_^


we use Variadic Template and index_sequence, SFINAE enable_if and type_traits, tuple, function, forward_ref, decltype(auto), and more in c++11/14


complied with vc2015,gcc5.3,clang3.8


##what modifyed

1.使用变参模板重写 functor/memberfunctor/constructor/call 函数来支持多参数  
2.使用enable_if机制实现 函数条件重载  
3.通过stack_help类实现read/push函数  
4.移除int64相关函数，使用lua5.3的luaInterager来替代  
5.加入stl容器类向lua导入/导出一个table的功能  
6.可以从lua中返回多个返回值用tuple包裹  
7.使用weak_ptr来存储导出到lua的shared_ptr对象来避免lua控制c++对象生命周期  
8.class_add函数增加参数bInitShared来注册导出类对应的shared_ptr对象  
9.未注册shared_ptr对象统一使用默认metatable来GC  
10.可以向lua注册一个std::function对象（通过functor/memberfunctor warp类）  
11.可以向read/push一个std::function对象来对应luafunction(使用LUA_REGISTRYINDEX和内嵌的lambda)，引用的std::function对象全部释放后自动从lua中unref, 如果lua关闭后再调用该对象将产生一个std::exception  
12.通过定义宏 _ALLOW_SHAREDPTR_INVOKE 可以允许已注册的shared_ptr对象调用类成员函数，不允许shared_ptr对象到raw_ptr的自动转换  
13.通过调用register_lua_close_callback注册回调函数，当lua关闭时回调  
14.允许向lua导出常量对象，但是会丢失常量限定符，请注意 
15.定义宏 LUATINKER_USERDATA_CHECK_CONST 将会储存const信息(bool)到userdata中，当调用成员函数时会检查是否允许调用,同时read是会检查是否发生丢失常量限定符的问题  
16.定义宏 LUATINKER_USERDATA_CHECK_TYPEINFO 将会储存typeidx信息(size_t)到userdata中，当read时将进行类型一致性检查  
17.调试模式下自动打开LUATINKER_USERDATA_CHECK_CONST和LUATINKER_USERDATA_CHECK_TYPEINFO 进行额外的检查  
18.头文件lua_tinker_overload_func.h 是简单的将c++参数列表转换为luatype后存储到int64作为函数签名，运行时进行非精确匹配  
19.新增class_property函数可以为一个名字注册一对get/set函数

1.use Variadic Template to modify old func "functor/memberfunctor/constructor/call "  
2.use enable_if to hides a function overload  
3.use stack_help class to handle read/push function  
4.remove int64 like function just use luaInteger in lua5.3  
5.stl container can push a table to lua/ read a table from lua  
6.can pop tuple from lua to warp multi-return value  
7.use weak_ptr to hold a shared_obj in lua, to avoid lua control c++ object's lifetime  
8.class_add function adds argument bInitShared to register a class objects's shared_ptr  
9.unregistered shared_ptr objects using the default same metatable to gc  
10.can register a std::function obj through function_warp  
11.can read/push a function obj with luafunction(use LUA_REGISTRYINDEX and in-function lambda)  ,when all std::function obj was released,lua_function will unref from lua regtable, if invoke function when lua was closed will throw a std::exception  
12.can def _ALLOW_SHAREDPTR_INVOKE to allow shared_ptr to invoke member_func  
13.call register_lua_close_callback reg a callback func, when lua close it will be callback  
14.allow to push a const obj/ref/pointer, but it will lost const qualifier, plz used carefully 
15.define macro LUATINKER_USERDATA_HOLD_CONST will store const info(bool) in userdata,will check when call member function,will check lost const qualifier when read   
16.define macro LUATINKER_USERDATA_CHECK_TYPEINFO will stroe typeidx info(size_t) in userdata,will do type consistency check when read  
17.on debug mode will auto define LUATINKER_USERDATA_CHECK_CONST and LUATINKER_USERDATA_CHECK_TYPEINFO for extra check  
18.header file "lua_tinker_overload_func.h" was simple convert c++ params list to luatype, then stored in int64 as a function signature, do non-exact matching when function name invoked  
19.add function class_property for register get/set function for a member_name  

