# luatinkerE
lua_tinker for c++14 and lua 5.3

##why luatinker"E"?

0xE=14, ^_^

##what modifyed

使用变参模板重写 functor/memberfunctor/constructor/call 函数来支持多参数    
use Variadic Template to modify old func "functor/memberfunctor/constructor/call "



使用enable_if机制实现 函数条件重载   
use enable_if to hides a function overload 



通过stack_help类实现read/push函数  
use stack_help class to handle read/push function



移除int64相关函数，使用lua5.3的luaInterager来替代  
remove int64 like function just use luaInteger in lua5.3



加入stl容器类向lua导出一个table的功能  
stl container can push a table to lua 



使用weak_ptr来存储导出到lua的shared_ptr对象  
use weak_ptr to hold a shared_obj in lua



可以向lua注册一个std::function对象（通过functor/memberfunctor warp类）  
can register a std::function obj through function_warp



通过定义 _ALLOW_SHAREDPTR_INVOKE 可以允许shared_ptr对象调用类成员函数  
can def _ALLOW_SHAREDPTR_INVOKE to allow shared_ptr to invoke member_func

