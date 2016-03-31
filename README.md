# luatinkerE
lua_tinker for c++14 and lua 5.3

##why luatinker"E"?

0xE=14, ^_^

##what modifyed

use Variadic Template to modify old func "functor/memberfunctor/constructor/call "

use enable_if to hides a function overload 

use stack_help class to handle read/push function

remove int64 like function just use luaInteger in lua5.3

stl container can push a table to lua 

use weak_ptr to hold a shared_obj in lua

can register a std::function obj through function_warp

can def _ALLOW_SHAREDPTR_INVOKE to allow shared_ptr to invoke func

