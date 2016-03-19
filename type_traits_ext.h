#ifndef type_traits_ext_h
#define type_traits_ext_h

#include<memory>
#include<string>
#include<tuple>



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
	typedef typename T type;
};
template<typename T>
using get_shared_t = typename get_shared<T>::type;




template< class T > struct remove_r_reference { typedef T type; };

template< class T > struct remove_r_reference<T&&> { typedef T type; };







#endif
