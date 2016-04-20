# luatinkerE
LUA to C++ Binding Library "lua_tinker" expand for c++14 and lua 5.3

##why luatinker"E"?

0xE=14, ^_^  
E=Expand，we add a lot of feature to luatinker

we use Variadic Template and index_sequence, SFINAE enable_if and type_traits, tuple, function, forward_ref, decltype(auto), and more in c++11/14


complied with vc2015,gcc5.3,clang3.8


##new feature 新特性一览

* 导入lua的函数，现在支持任意多的参数
* 支持的read/push的元素类型增加std::string,std::function,lua_function_ref,stl容器,std::shared_ptr对象
* 支持最多16个参数的非精确匹配的函数重载(c++类型downcast到lua类型后生成签名)
* 支持函数默认参数及带默认参数的重载函数的匹配
* 移除int64相关函数，使用lua5.3的luaInterager来替代  
* 支持调用lua函数时返回多个返回值用tuple包裹  
* 支持通过宏定义打开类型一致性检查和常量类成员函数检查
* 支持通过宏定义允许已注册的shared_ptr对象调用类成员函数    
* 支持类静态变量注册
* 支持property注册

***



***




###detail info 详细功能说明
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
* 新增class_static_mem函数可以注册类静态变量  
* 允许调用def/class_def/class_def_static/class_con时同时加入参数默认值，当lua中invoke时，如果参数不足，会使用参数默认值  
* overload相关函数允许加入参数默认值，但不推荐人工生成，请使用自动化生成工具export2lua  

***

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
* define macro LUATINKER_USERDATA_CHECK_TYPEINFO will stroe typeidx info(size_t) in userdata,will do type consistency check when read  
* on debug mode will auto define LUATINKER_USERDATA_CHECK_CONST and LUATINKER_USERDATA_CHECK_TYPEINFO for extra check  
* args_type_overload_functor/member_functor/constructor was simple convert c++ params list to luatype, then stored in int64 as a function signature, do non-exact matching when function name invoked  
* add function class_def_static for register class static member function,use class.foo() to invoke, plz don't use class:foo()  
* add function class_property for register get/set function for a member_name  
* add function class_static_mem for register class static ver  
* when call def/class_def/class_def_static/class_con function,can push params's default value. when invoke in lua, if params not enough, will use default values   
* overload like function allow add default params, manual generation is not recommended, plz use autogen tools "export2lua"  
