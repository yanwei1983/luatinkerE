#ifndef type_traits_ext_h
#define type_traits_ext_h

#include<memory>
#include<string>
#include<tuple>
#include<functional>


#define CAT_CLASS_HAS_TYPEDEF(typedef_) \
    template <typename T> \
    class has_ ## typedef_ \
    { \
        template <class C> static void check(typename std::decay<typename C::typedef_>::type *) noexcept; \
        template <class C> static void check(...) noexcept(false); \
    public: \
    \
        enum { value = noexcept(check<T>(nullptr)) }; \
    };

#define CAT_CLASS_HAS_MEMBER(member) \
    template <typename T> \
    class has_ ## member \
    { \
        template <class C> static void check(typename std::decay<typename C::member>::type *) noexcept; \
        template <class C> static void check(...) noexcept(false); \
    public: \
    \
        enum { value = noexcept(check<T>(nullptr)) }; \
    };


//////////////////////////////////////////////////////////////////////////////////
//
// boolean type
//

template <bool value>
using bool_type = std::integral_constant<bool, value>;

template <typename Trait, bool V = Trait::value> struct not_type;

template <typename Trait>
struct not_type<Trait, false> : std::true_type { };

template <typename Trait>
struct not_type<Trait, true> : std::false_type { };


//////////////////////////////////////////////////////////////////////////////////
//
// generic type traits...
//

namespace details
{
	CAT_CLASS_HAS_MEMBER(value_type);
	CAT_CLASS_HAS_MEMBER(key_type);
	CAT_CLASS_HAS_MEMBER(mapped_type);
	CAT_CLASS_HAS_MEMBER(container_type);

	CAT_CLASS_HAS_MEMBER(pointer);
	CAT_CLASS_HAS_MEMBER(const_pointer);
	CAT_CLASS_HAS_MEMBER(reference);
	CAT_CLASS_HAS_MEMBER(const_reference);
	CAT_CLASS_HAS_MEMBER(iterator);
	CAT_CLASS_HAS_MEMBER(const_iterator);
	CAT_CLASS_HAS_MEMBER(reverse_iterator);
	CAT_CLASS_HAS_MEMBER(const_reverse_iterator);
	CAT_CLASS_HAS_MEMBER(size_type);
	CAT_CLASS_HAS_MEMBER(difference_type);

	CAT_CLASS_HAS_TYPEDEF(function_type);
	CAT_CLASS_HAS_TYPEDEF(return_type);
	CAT_CLASS_HAS_MEMBER(arity_value);
}


template <typename T>
struct has_value_type
	: bool_type<details::has_value_type<T>::value>
{ };

template <typename t>
struct has_key_type
	: bool_type<details::has_key_type<t>::value>
{ };

template <typename t>
struct has_mapped_type
	: bool_type<details::has_mapped_type<t>::value>
{ };

template <typename t>
struct has_container_type
	: bool_type<details::has_container_type<t>::value>
{ };

template <typename T>
struct has_pointer
	: bool_type<details::has_pointer<T>::value>
{ };

template <typename T>
struct has_const_pointer
	: bool_type<details::has_const_pointer<T>::value>
{ };

template <typename T>
struct has_reference
	: bool_type<details::has_reference<T>::value>
{ };

template <typename T>
struct has_const_reference
	: bool_type<details::has_const_reference<T>::value>
{ };

template <typename T>
struct has_iterator
	: bool_type<details::has_iterator<T>::value>
{ };

template <typename T>
struct has_const_iterator
	: bool_type<details::has_const_iterator<T>::value>
{ };

template <typename T>
struct has_reverse_iterator
	: bool_type<details::has_reverse_iterator<T>::value>
{ };

template <typename T>
struct has_const_reverse_iterator
	: bool_type<details::has_const_reverse_iterator<T>::value>
{ };

template <typename T>
struct has_size_type
	: bool_type<details::has_size_type<T>::value>
{ };

template <typename T>
struct has_difference_type
	: bool_type<details::has_difference_type<T>::value>
{ };


//////////////////////////////////////////////////////////////////////////////////
//
// is_container
//

template <typename T>
struct is_container
	: bool_type<details::has_value_type<T>::value &&
	details::has_reference<T>::value &&
	details::has_const_reference<T>::value &&
	details::has_iterator<T>::value &&
	details::has_const_iterator<T>::value &&
	details::has_pointer<T>::value &&
	details::has_const_pointer<T>::value &&
	details::has_size_type<T>::value &&
	details::has_difference_type<T>::value>
{ };


//////////////////////////////////////////////////////////////////////////////////
//
// is_associative_container
//

template <typename T>
struct is_associative_container
	: bool_type<is_container<T>::value &&
	details::has_key_type<T>::value &&
	details::has_mapped_type<T>::value>
{ };


//////////////////////////////////////////////////////////////////////////////////
//
// is_tuple
//

template <typename T>
struct is_tuple : bool_type<false>
{ };

template <typename ...Ti>
struct is_tuple<std::tuple<Ti...>> : bool_type<true>
{ };


//////////////////////////////////////////////////////////////////////////////////
//
// is_shared_ptr
//

template <typename T>
struct is_shared_ptr : bool_type<false>
{ };

template <typename T>
struct is_shared_ptr<std::shared_ptr<T>> : bool_type<true>
{ };


template <typename T>
struct get_shared
{
};

template <typename T>
struct get_shared<std::shared_ptr<T>>
{
	using type = T;
};

template<typename T>
using get_shared_t = typename get_shared<T>::type;




template< class T > struct remove_r_reference { typedef T type; };

template< class T > struct remove_r_reference<T&&> { typedef T type; };

namespace stdext
{
	namespace detail {
		template <class T>
		struct is_reference_wrapper : std::false_type {};
		template <class U>
		struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};
		//template <class T>
		//constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

		template <class Base, class T, class Derived, class... Args>
		auto INVOKE(T Base::*pmf, Derived&& ref, Args&&... args)
			noexcept(noexcept((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...)))
			-> std::enable_if_t<std::is_function<T>::value &&
			std::is_base_of<Base, std::decay_t<Derived>>::value,
			decltype((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...))>
		{
			return (std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...);
		}

		template <class Base, class T, class RefWrap, class... Args>
		auto INVOKE(T Base::*pmf, RefWrap&& ref, Args&&... args)
			noexcept(noexcept((ref.get().*pmf)(std::forward<Args>(args)...)))
			-> std::enable_if_t<std::is_function<T>::value &&
			is_reference_wrapper<std::decay_t<RefWrap>>::value,
			decltype((ref.get().*pmf)(std::forward<Args>(args)...))>

		{
			return (ref.get().*pmf)(std::forward<Args>(args)...);
		}

		template <class Base, class T, class Pointer, class... Args>
		auto INVOKE(T Base::*pmf, Pointer&& ptr, Args&&... args)
			noexcept(noexcept(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...)))
			-> std::enable_if_t<std::is_function<T>::value &&
			!is_reference_wrapper<std::decay_t<Pointer>>::value &&
			!std::is_base_of<Base, std::decay_t<Pointer>>::value,
			decltype(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...))>
		{
			return ((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...);
		}

		template <class Base, class T, class Derived>
		auto INVOKE(T Base::*pmd, Derived&& ref)
			noexcept(noexcept(std::forward<Derived>(ref).*pmd))
			-> std::enable_if_t<!std::is_function<T>::value &&
			std::is_base_of<Base, std::decay_t<Derived>>::value,
			decltype(std::forward<Derived>(ref).*pmd)>
		{
			return std::forward<Derived>(ref).*pmd;
		}

		template <class Base, class T, class RefWrap>
		auto INVOKE(T Base::*pmd, RefWrap&& ref)
			noexcept(noexcept(ref.get().*pmd))
			-> std::enable_if_t<!std::is_function<T>::value &&
			is_reference_wrapper<std::decay_t<RefWrap>>::value,
			decltype(ref.get().*pmd)>
		{
			return ref.get().*pmd;
		}

		template <class Base, class T, class Pointer>
		auto INVOKE(T Base::*pmd, Pointer&& ptr)
			noexcept(noexcept((*std::forward<Pointer>(ptr)).*pmd))
			-> std::enable_if_t<!std::is_function<T>::value &&
			!is_reference_wrapper<std::decay_t<Pointer>>::value &&
			!std::is_base_of<Base, std::decay_t<Pointer>>::value,
			decltype((*std::forward<Pointer>(ptr)).*pmd)>
		{
			return (*std::forward<Pointer>(ptr)).*pmd;
		}

		template <class F, class... Args>
		auto INVOKE(F&& f, Args&&... args)
			noexcept(noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
			-> std::enable_if_t<!std::is_member_pointer<std::decay_t<F>>::value,
			decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
		{
			return std::forward<F>(f)(std::forward<Args>(args)...);
		}
	} // namespace detail

	template< class F, class... ArgTypes >
	auto invoke(F&& f, ArgTypes&&... args)
		// exception specification for QoI
		noexcept(noexcept(detail::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...)))
		-> decltype(detail::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...))
	{
		return detail::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...);
	}
};






template <class _FUNC>
struct function_traits : function_traits<decltype(&_FUNC::operator())> {
};

template <class _RET, class ..._ARGS>
struct function_traits<_RET(_ARGS...)> {
	typedef _RET return_type;
	using _CALLTYPE = _RET(_ARGS...);
	
	static constexpr const size_t args_num = sizeof...(_ARGS);
	static constexpr const size_t argc = sizeof...(_ARGS);
	template <size_t i> struct argv {
		typedef typename std::tuple_element<i, std::tuple<_ARGS...> >::type type;
	};
};

template <class _RET, class ..._ARGS>
struct function_traits<_RET(*)(_ARGS...)> : function_traits<_RET(_ARGS...)> {
};

template <class _RET, class _OBJ, class ..._ARGS>
struct function_traits<_RET(_OBJ::*)(_ARGS...)> : function_traits<_RET(_ARGS...)> {
};

template <class _RET, class _OBJ, class ..._ARGS>
struct function_traits<_RET(_OBJ::*)(_ARGS...) const> : function_traits<_RET(_ARGS...)> {
};

template <class _RET, class _OBJ, class ..._ARGS>
struct function_traits<_RET(_OBJ::*)(_ARGS...) volatile> : function_traits<_RET(_ARGS...)> {
};

template <class _RET, class _OBJ, class ..._ARGS>
struct function_traits<_RET(_OBJ::*)(_ARGS...) const volatile> : function_traits<_RET(_ARGS...)> {
};

template <class _SIGNATURE>
struct function_traits<std::function<_SIGNATURE> > : function_traits<_SIGNATURE> {
};

template <class _FUNC>
struct function_traits<_FUNC &> : function_traits<_FUNC> {
};

template <class _FUNC>
struct function_traits<const _FUNC &> : function_traits<_FUNC> {
};

template <class _FUNC>
struct function_traits<volatile _FUNC &> : function_traits<_FUNC> {
};

template <class _FUNC>
struct function_traits<const volatile _FUNC &> : function_traits<_FUNC> {
};

template <class _FUNC>
struct function_traits<_FUNC &&> : function_traits<_FUNC> {
};

template <class _FUNC>
struct function_traits<const _FUNC &&> : function_traits<_FUNC> {
};

template <class _FUNC>
struct function_traits<volatile _FUNC &&> : function_traits<_FUNC> {
};

template <class _FUNC>
struct function_traits<const volatile _FUNC &&> : function_traits<_FUNC> {
};


#endif
