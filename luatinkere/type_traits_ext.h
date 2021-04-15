#ifndef type_traits_ext_h
#define type_traits_ext_h

#include <functional>
#include <memory>
#include <string>
#include <tuple>

#define CAT_CLASS_HAS_TYPEDEF(typedef_)                                               \
    template<typename T>                                                              \
    class has_##typedef_                                                              \
    {                                                                                 \
        template<class C>                                                             \
        static void check(typename std::decay<typename C::typedef_>::type*) noexcept; \
        template<class C>                                                             \
        static void check(...) noexcept(false);                                       \
                                                                                      \
    public:                                                                           \
        enum                                                                          \
        {                                                                             \
            value = noexcept(check<T>(nullptr))                                       \
        };                                                                            \
    };

#define CAT_CLASS_HAS_MEMBER(member)                                                  \
    template<typename T>                                                              \
    class has_##member                                                                \
    {                                                                                 \
        template<class C>                                                             \
        static void check(typename std::decay<decltype(&C::member)>::type*) noexcept; \
        template<class C>                                                             \
        static void check(...) noexcept(false);                                       \
                                                                                      \
    public:                                                                           \
        enum                                                                          \
        {                                                                             \
            value = noexcept(check<T>(nullptr))                                       \
        };                                                                            \
    };

//////////////////////////////////////////////////////////////////////////////////
//
// boolean type
//

template<bool value>
using bool_type = std::integral_constant<bool, value>;

template<typename Trait, bool V = Trait::value>
struct not_type;

template<typename Trait>
struct not_type<Trait, false> : std::true_type
{
};

template<typename Trait>
struct not_type<Trait, true> : std::false_type
{
};

//////////////////////////////////////////////////////////////////////////////////
//
// generic type traits...
//

namespace details
{
    CAT_CLASS_HAS_TYPEDEF(value_type);
    CAT_CLASS_HAS_TYPEDEF(key_type);
    CAT_CLASS_HAS_TYPEDEF(mapped_type);
    CAT_CLASS_HAS_TYPEDEF(container_type);

    CAT_CLASS_HAS_TYPEDEF(pointer);
    CAT_CLASS_HAS_TYPEDEF(const_pointer);
    CAT_CLASS_HAS_TYPEDEF(reference);
    CAT_CLASS_HAS_TYPEDEF(const_reference);
    CAT_CLASS_HAS_TYPEDEF(iterator);
    CAT_CLASS_HAS_TYPEDEF(const_iterator);
    CAT_CLASS_HAS_TYPEDEF(reverse_iterator);
    CAT_CLASS_HAS_TYPEDEF(const_reverse_iterator);
    CAT_CLASS_HAS_TYPEDEF(size_type);
    CAT_CLASS_HAS_TYPEDEF(difference_type);

    CAT_CLASS_HAS_TYPEDEF(function_type);
    CAT_CLASS_HAS_TYPEDEF(return_type);
    CAT_CLASS_HAS_TYPEDEF(arity_value);
    CAT_CLASS_HAS_TYPEDEF(allocator_type);

} // namespace details

template<typename T>
struct has_value_type : bool_type<details::has_value_type<T>::value>
{
};

template<typename t>
struct has_key_type : bool_type<details::has_key_type<t>::value>
{
};

template<typename t>
struct has_mapped_type : bool_type<details::has_mapped_type<t>::value>
{
};

template<typename t>
struct has_container_type : bool_type<details::has_container_type<t>::value>
{
};

template<typename T>
struct has_pointer : bool_type<details::has_pointer<T>::value>
{
};

template<typename T>
struct has_const_pointer : bool_type<details::has_const_pointer<T>::value>
{
};

template<typename T>
struct has_reference : bool_type<details::has_reference<T>::value>
{
};

template<typename T>
struct has_const_reference : bool_type<details::has_const_reference<T>::value>
{
};

template<typename T>
struct has_iterator : bool_type<details::has_iterator<T>::value>
{
};

template<typename T>
struct has_const_iterator : bool_type<details::has_const_iterator<T>::value>
{
};

template<typename T>
struct has_reverse_iterator : bool_type<details::has_reverse_iterator<T>::value>
{
};

template<typename T>
struct has_const_reverse_iterator : bool_type<details::has_const_reverse_iterator<T>::value>
{
};

template<typename T>
struct has_size_type : bool_type<details::has_size_type<T>::value>
{
};

template<typename T>
struct has_difference_type : bool_type<details::has_difference_type<T>::value>
{
};

template<typename t>
struct has_allocator_type : bool_type<details::has_allocator_type<t>::value>
{
};

namespace notstd
{

    struct left_shift
    {

        template<class L, class R>
        constexpr auto operator()(L&& l, R&& r) const noexcept(noexcept(std::forward<L>(l) << std::forward<R>(r)))
            -> decltype(std::forward<L>(l) << std::forward<R>(r))
        {
            return std::forward<L>(l) << std::forward<R>(r);
        }
    };

    struct right_shift
    {

        template<class L, class R>
        constexpr auto operator()(L&& l, R&& r) const noexcept(noexcept(std::forward<L>(l) >> std::forward<R>(r)))
            -> decltype(std::forward<L>(l) >> std::forward<R>(r))
        {
            return std::forward<L>(l) >> std::forward<R>(r);
        }
    };

} // namespace notstd




namespace stdext
{
    namespace detail
    {
        template<class T>
        struct is_reference_wrapper : std::false_type
        {
        };
        template<class U>
        struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type
        {
        };
        // template <class T>
        // constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

        template<class Base, class T, class Derived, class... Args>
        auto INVOKE(T Base::* pmf,
                    Derived&& ref,
                    Args&&... args) noexcept(noexcept((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...)))
            -> std::enable_if_t<std::is_function<T>::value && std::is_base_of<Base, std::decay_t<Derived>>::value,
                                decltype((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...))>
        {
            return (std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...);
        }

        template<class Base, class T, class RefWrap, class... Args>
        auto INVOKE(T Base::* pmf,
                    RefWrap&& ref,
                    Args&&... args) noexcept(noexcept((ref.get().*pmf)(std::forward<Args>(args)...)))
            -> std::enable_if_t<std::is_function<T>::value && is_reference_wrapper<std::decay_t<RefWrap>>::value,
                                decltype((ref.get().*pmf)(std::forward<Args>(args)...))>

        {
            return (ref.get().*pmf)(std::forward<Args>(args)...);
        }

        template<class Base, class T, class Pointer, class... Args>
        auto INVOKE(T Base::*pmf, Pointer&& ptr, Args&&... args) noexcept(noexcept(((*std::forward<Pointer>(ptr)).*
                                                                                    pmf)(std::forward<Args>(args)...)))
            -> std::enable_if_t<std::is_function<T>::value && !is_reference_wrapper<std::decay_t<Pointer>>::value &&
                                    !std::is_base_of<Base, std::decay_t<Pointer>>::value,
                                decltype(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...))>
        {
            return ((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...);
        }

        template<class Base, class T, class Derived>
        auto INVOKE(T Base::*pmd, Derived&& ref) noexcept(noexcept(std::forward<Derived>(ref).*pmd))
            -> std::enable_if_t<!std::is_function<T>::value && std::is_base_of<Base, std::decay_t<Derived>>::value,
                                decltype(std::forward<Derived>(ref).*pmd)>
        {
            return std::forward<Derived>(ref).*pmd;
        }

        template<class Base, class T, class RefWrap>
        auto INVOKE(T Base::*pmd, RefWrap&& ref) noexcept(noexcept(ref.get().*pmd))
            -> std::enable_if_t<!std::is_function<T>::value && is_reference_wrapper<std::decay_t<RefWrap>>::value,
                                decltype(ref.get().*pmd)>
        {
            return ref.get().*pmd;
        }

        template<class Base, class T, class Pointer>
        auto INVOKE(T Base::*pmd, Pointer&& ptr) noexcept(noexcept((*std::forward<Pointer>(ptr)).*pmd))
            -> std::enable_if_t<!std::is_function<T>::value && !is_reference_wrapper<std::decay_t<Pointer>>::value &&
                                    !std::is_base_of<Base, std::decay_t<Pointer>>::value,
                                decltype((*std::forward<Pointer>(ptr)).*pmd)>
        {
            return (*std::forward<Pointer>(ptr)).*pmd;
        }

        template<class F, class... Args>
        auto INVOKE(F&& f, Args&&... args) noexcept(noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
            -> std::enable_if_t<!std::is_member_pointer<std::decay_t<F>>::value,
                                decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
        {
            return std::forward<F>(f)(std::forward<Args>(args)...);
        }

        
    } // namespace detail

    template<class F, class... ArgTypes>
    auto invoke(F&& f, ArgTypes&&... args)
        // exception specification for QoI
        noexcept(noexcept(detail::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...)))
            -> decltype(detail::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...))
    {
        return detail::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...);
    }

    // Conforming C++14 implementation (is also a valid C++11 implementation):
    namespace detail
    {
        template<typename AlwaysVoid, typename, typename...>
        struct invoke_result
        {
        };
        template<typename F, typename... Args>
        struct invoke_result<decltype(void(detail::INVOKE(std::declval<F>(), std::declval<Args>()...))), F, Args...>
        {
            using type = decltype(detail::INVOKE(std::declval<F>(), std::declval<Args>()...));
        };
    };

    template<class F, class... ArgTypes>
    struct invoke_result : detail::invoke_result<void, F, ArgTypes...>
    {
    };
    template<class F, class... ArgTypes>
    using invoke_result_t = typename invoke_result<F, ArgTypes...>::type;
}; // namespace stdext

template<class T, class R, class... Args>
std::is_convertible<stdext::invoke_result_t<T, Args...>, R> is_invokable_test(int);

template<class T, class R, class... Args>
std::false_type is_invokable_test(...);

template<class T, class R, class... Args>
using is_invokable = decltype(is_invokable_test<T, R, Args...>(0));

template<class T, class R, class... Args>
constexpr auto is_invokable_v = is_invokable<T, R, Args...>::value;

template<class L, class R = L>
using has_equality = is_invokable<std::equal_to<>, bool, L, R>;
template<class L, class R = L>
constexpr auto has_equality_v = has_equality<L, R>::value;

template<class L, class R = L>
using has_inequality = is_invokable<std::not_equal_to<>, bool, L, R>;
template<class L, class R = L>
constexpr auto has_inequality_v = has_inequality<L, R>::value;

template<class L, class R = L>
using has_less_than = is_invokable<std::less<>, bool, L, R>;
template<class L, class R = L>
constexpr auto has_less_than_v = has_less_than<L, R>::value;

template<class L, class R = L>
using has_less_equal = is_invokable<std::less_equal<>, bool, L, R>;
template<class L, class R = L>
constexpr auto has_less_equal_v = has_less_equal<L, R>::value;

template<class L, class R = L>
using has_greater = is_invokable<std::greater<>, bool, L, R>;
template<class L, class R = L>
constexpr auto has_greater_v = has_greater<L, R>::value;

template<class L, class R = L>
using has_greater_equal = is_invokable<std::greater_equal<>, bool, L, R>;
template<class L, class R = L>
constexpr auto has_greater_equal_v = has_greater_equal<L, R>::value;

template<class L, class R = L>
using has_bit_xor = is_invokable<std::bit_xor<>, bool, L, R>;
template<class L, class R = L>
constexpr auto has_bit_xor_v = has_bit_xor<L, R>::value;

template<class L, class R = L>
using has_bit_or = is_invokable<std::bit_or<>, bool, L, R>;
template<class L, class R = L>
constexpr auto has_bit_or_v = has_bit_or<L, R>::value;

template<class L, class R = L>
using has_left_shift = is_invokable<notstd::left_shift, L, L, R>;
template<class L, class R = L>
constexpr auto has_left_shift_v = has_left_shift<L, R>::value;

template<class L, class R = L>
using has_right_shift = is_invokable<notstd::right_shift, L, L, R>;
template<class L, class R = L>
constexpr auto has_right_shift_v = has_right_shift<L, R>::value;

//////////////////////////////////////////////////////////////////////////////////
//
// is_container
//

template<typename T>
struct is_container : bool_type<details::has_value_type<T>::value && details::has_reference<T>::value &&
                                details::has_const_reference<T>::value && details::has_iterator<T>::value &&
                                details::has_const_iterator<T>::value && details::has_pointer<T>::value &&
                                details::has_const_pointer<T>::value && details::has_size_type<T>::value &&
                                details::has_difference_type<T>::value>
{
};

//////////////////////////////////////////////////////////////////////////////////
//
// is_associative_container
//

template<typename T>
struct is_associative_container
    : bool_type<is_container<T>::value && details::has_key_type<T>::value && details::has_mapped_type<T>::value>
{
};

template<typename T>
struct is_map_like
    : bool_type<is_associative_container<T>::value>
{
};

template<typename T>
struct is_set_like
    : bool_type<!is_associative_container<T>::value && has_key_type<T>::value>
{
};

template<typename T>
struct is_vector_or_array_like
    : bool_type<!is_associative_container<T>::value && !has_key_type<T>::value>
{
};

template<typename T>
struct is_array_like
    : bool_type<!is_associative_container<T>::value && !has_key_type<T>::value && !has_allocator_type<T>::value>
{
};

template<typename T>
struct is_vector_like
    : bool_type<!is_associative_container<T>::value && !has_key_type<T>::value && has_allocator_type<T>::value>
{
};



//////////////////////////////////////////////////////////////////////////////////
//
// is_tuple
//

template<typename T>
struct is_tuple : bool_type<false>
{
};

template<typename... Ti>
struct is_tuple<std::tuple<Ti...>> : bool_type<true>
{
};

//////////////////////////////////////////////////////////////////////////////////
//
// is_shared_ptr
//

template<typename T>
struct is_shared_ptr : bool_type<false>
{
};

template<typename T>
struct is_shared_ptr<std::shared_ptr<T>> : bool_type<true>
{
};

template<typename T>
struct get_shared
{
};

template<typename T>
struct get_shared<std::shared_ptr<T>>
{
    using type = T;
};

template<typename T>
using get_shared_t = typename get_shared<T>::type;

template<class T>
struct remove_r_reference
{
    typedef T type;
};

template<class T>
struct remove_r_reference<T&&>
{
    typedef T type;
};

#if __cplusplus < 201402L
namespace std
{
    template<bool B, class T = void>
    using enable_if_t = typename enable_if<B, T>::type;

    template<class T>
    using decay_t = typename decay<T>::type;

    template<typename T, T... ints>
    struct integer_sequence
    {
    };

    template<typename T, T N, typename = void>
    struct make_integer_sequence_impl
    {
        template<typename>
        struct tmp;

        template<T... Prev>
        struct tmp<integer_sequence<T, Prev...>>
        {
            using type = integer_sequence<T, Prev..., N - 1>;
        };

        using type = typename tmp<typename make_integer_sequence_impl<T, N - 1>::type>::type;
    };

    template<typename T, T N>
    struct make_integer_sequence_impl<T, N, typename std::enable_if<N == 0>::type>
    {
        using type = integer_sequence<T>;
    };

    template<typename T, T N>
    using make_integer_sequence = typename make_integer_sequence_impl<T, N>::type;

    template<size_t... _Vals>
    using index_sequence = integer_sequence<size_t, _Vals...>;

    template<size_t _Size>
    using make_index_sequence = make_integer_sequence<size_t, _Size>;

    // TEMPLATE CLASS _Cat_base
    template<bool _Val>
    struct _Cat_base : integral_constant<bool, _Val>
    { // base class for type predicates
    };

    template<class _Ty>
    struct is_null_pointer : _Cat_base<is_same<typename remove_cv<_Ty>::type, nullptr_t>::value>
    { // determine whether _Ty is nullptr_t
    };
} // namespace std
#endif //#if __cplusplus != 201402L



template<class _FUNC>
struct function_traits : function_traits<decltype(&_FUNC::operator())>
{
};

template<class _RET, class... _ARGS>
struct function_traits<_RET(_ARGS...)>
{
    typedef _RET return_type;
    using _CALLTYPE = _RET(_ARGS...);

    static constexpr const size_t args_num = sizeof...(_ARGS);
    static constexpr const size_t argc     = sizeof...(_ARGS);
    template<size_t i>
    struct argv
    {
        typedef typename std::tuple_element<i, std::tuple<_ARGS...>>::type type;
    };
};

template<class _RET, class... _ARGS>
struct function_traits<_RET (*)(_ARGS...)> : function_traits<_RET(_ARGS...)>
{
};

template<class _RET, class _OBJ, class... _ARGS>
struct function_traits<_RET (_OBJ::*)(_ARGS...)> : function_traits<_RET(_ARGS...)>
{
};

template<class _RET, class _OBJ, class... _ARGS>
struct function_traits<_RET (_OBJ::*)(_ARGS...) const> : function_traits<_RET(_ARGS...)>
{
};

template<class _RET, class _OBJ, class... _ARGS>
struct function_traits<_RET (_OBJ::*)(_ARGS...) volatile> : function_traits<_RET(_ARGS...)>
{
};

template<class _RET, class _OBJ, class... _ARGS>
struct function_traits<_RET (_OBJ::*)(_ARGS...) const volatile> : function_traits<_RET(_ARGS...)>
{
};

template<class _SIGNATURE>
struct function_traits<std::function<_SIGNATURE>> : function_traits<_SIGNATURE>
{
};

template<class _FUNC>
struct function_traits<_FUNC&> : function_traits<_FUNC>
{
};

template<class _FUNC>
struct function_traits<const _FUNC&> : function_traits<_FUNC>
{
};

template<class _FUNC>
struct function_traits<volatile _FUNC&> : function_traits<_FUNC>
{
};

template<class _FUNC>
struct function_traits<const volatile _FUNC&> : function_traits<_FUNC>
{
};

template<class _FUNC>
struct function_traits<_FUNC&&> : function_traits<_FUNC>
{
};

template<class _FUNC>
struct function_traits<const _FUNC&&> : function_traits<_FUNC>
{
};

template<class _FUNC>
struct function_traits<volatile _FUNC&&> : function_traits<_FUNC>
{
};

template<class _FUNC>
struct function_traits<const volatile _FUNC&&> : function_traits<_FUNC>
{
};

template<typename T>
using base_type = typename std::remove_cv<typename std::remove_reference<typename std::remove_pointer<T>::type>::type>::type;

#endif
