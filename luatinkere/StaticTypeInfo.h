#ifndef STATICTYPEINFO_H
#define STATICTYPEINFO_H

#include <string_view>

// based on
// https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/56766138#56766138

namespace static_type_info
{

    template<class T>
    constexpr std::string_view rawTypeName()
    {
#if defined(_MSC_VER)
        return __FUNCSIG__;
#else
        return __PRETTY_FUNCTION__;
#endif
    }

    namespace probe
    {
        constexpr auto   probeRawTypeName   = rawTypeName<double>();
        constexpr size_t probeRawTypeLength = std::string_view("double").size();
        constexpr size_t prefixLength       = probeRawTypeName.find("double");
        static_assert(prefixLength != std::string_view::npos, "cannot extract typename from function signature");
        constexpr size_t suffixLength = probeRawTypeName.size() - prefixLength - probeRawTypeLength;
    } // namespace probe

    using TypeName = std::string_view;

    template<class T>
    constexpr TypeName getTypeName()
    {
        std::string_view name = rawTypeName<T>();
        return std::string_view(name.data() + probe::prefixLength, name.size() - probe::prefixLength - probe::suffixLength);
    }

} // namespace static_type_info

#include <stdint.h>

namespace static_type_info
{

    // FNV1a c++11 constexpr compile time hash functions, 32 and 64 bit
    // str should be implicitly convertible std::string_view, value should be left out
    // e.g hash_32_fnv1a_const("example")
    // code license: public domain or equivalent
    // post: https://notes.underscorediscovery.com/constexpr-fnv1a/

    constexpr uint32_t val_32_const   = 0x811c9dc5;
    constexpr uint32_t prime_32_const = 0x1000193;
    constexpr uint64_t val_64_const   = 0xcbf29ce484222325;
    constexpr uint64_t prime_64_const = 0x100000001b3;

    inline constexpr uint32_t hash_32_fnv1a_const(std::string_view str, const uint32_t value = val_32_const) noexcept
    {
        return (str.size() == 0) ? value
                                 : hash_32_fnv1a_const(std::string_view(str.data() + 1, str.size() - 1), (value ^ uint32_t(str[0])) * prime_32_const);
    }

    inline constexpr uint64_t hash_64_fnv1a_const(std::string_view str, const uint64_t value = val_64_const) noexcept
    {
        return (str.size() == 0) ? value
                                 : hash_64_fnv1a_const(std::string_view(str.data() + 1, str.size() - 1), (value ^ uint64_t(str[0])) * prime_64_const);
    }

} // namespace static_type_info

#ifndef STATIC_TYPE_INFO_USE_MEMBER_POINTER
// enable the new implementation using member pointers on tested compilers
// gcc currently fails as member pointers are not considered constexpr
#if defined(__clang__) || defined(_MSC_VER) || defined(EMSCRIPTEN)
#define STATIC_TYPE_INFO_USE_MEMBER_POINTER true
#else
#define STATIC_TYPE_INFO_USE_MEMBER_POINTER false
#endif
#endif

#if STATIC_TYPE_INFO_USE_MEMBER_POINTER

namespace static_type_info
{

    namespace detail
    {
        template<typename T>
        struct IDGenerator
        {
            static const void* const id;
        };

        template<typename T>
        const void* const IDGenerator<T>::id = nullptr;
    } // namespace detail

    using TypeIndex = decltype(&detail::IDGenerator<void>::id);

    template<class T>
    constexpr TypeIndex getTypeIndex()
    {
        return &detail::IDGenerator<T>::id;
    }

} // namespace static_type_info

#else

namespace static_type_info
{

    using TypeIndex = uint64_t;

    template<class T>
    constexpr TypeIndex getTypeIndex()
    {
        return hash_64_fnv1a_const(getTypeName<T>());
    }

} // namespace static_type_info

#endif

namespace static_type_info
{

    struct TypeID
    {
        TypeName  name  = TypeName();
        TypeIndex index = TypeIndex();

        constexpr bool operator==(const TypeID& other) const { return index == other.index; }

        constexpr bool operator!=(const TypeID& other) const { return index != other.index; }

        constexpr bool operator<(const TypeID& other) const { return index < other.index; }
    };

    template<class T>
    constexpr TypeID getTypeID()
    {
        return TypeID{getTypeName<T>(), getTypeIndex<T>()};
    }

} // namespace static_type_info

namespace std
{
    template<>
    struct hash<static_type_info::TypeID>
    {
        hash<static_type_info::TypeIndex> hasher;

        size_t operator()(const static_type_info::TypeID& id) const { return hasher(id.index); }
    };
} // namespace std

#endif /* STATICTYPEINFO_H */
