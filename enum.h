#ifndef ENUM_STRINGS_H
#define ENUM_STRINGS_H

/**
 * @file enum_strings.h
 * @author Sergey Klevtsov
 */

#include <string>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <utility>
#include <array>

/**
 * @brief Associate a list of string names with enumeration values.
 * @param ENUM the enumeration type
 * @param ... list of names (C-string literals)
 *
 * Conditions (not enforced but won't work correctly if violated):
 *  - the macro must be called at namespace scope the enumeration type is defined in
 *  - the number and order of string arguments passed must match the enum values
 *  - enumeration constants must not have custom values assigned
 *
 *  If the enumeration has a special constant named @p END, it should be
 *  the last one, and its value will be used to determine the number
 *  of enum values and checked at compile time against number of strings.
 */

template <typename R, typename P, size_t N, size_t... I>
constexpr std::array<R, N> to_array_impl(P (&a)[N], std::index_sequence<I...>) noexcept
{
    return {{a[I]...}};
}

template <typename T, size_t N>
constexpr std::array<T, N> to_array(T (&a)[N]) noexcept
{
    return to_array_impl<std::remove_cv_t<T>, T, N>(a, std::make_index_sequence<N>{});
}

template <typename R, typename P, size_t N, size_t... I>
constexpr std::array<R, N> to_array_impl(P (&&a)[N], std::index_sequence<I...>) noexcept
{
    return {{std::move(a[I])...}};
}

template <typename T, size_t N>
constexpr std::array<T, N> to_array(T (&&a)[N]) noexcept
{
    return to_array_impl<std::remove_cv_t<T>, T, N>(std::move(a), std::make_index_sequence<N>{});
}

#define ENUM_STRINGS(E, ...)                                          \
    static_assert(std::is_enum<E>::value, "Not an enumeration type"); \
                                                                      \
    template <>                                                       \
    struct EnumMetaInfo<E>                                            \
    {                                                                 \
        static decltype(to_array<std::string>({__VA_ARGS__})) Info()  \
        {                                                             \
            return to_array<std::string>({__VA_ARGS__});              \
        }                                                             \
    };                                                                \
    inline std::ostream &operator<<(std::ostream &os, const E &e)     \
    {                                                                 \
        os << enum_to_string(e);                                      \
        return os;                                                    \
    }                                                                 \
    inline std::istream &operator>>(std::istream &is, E &e)           \
    {                                                                 \
        std::string s;                                                \
        is >> s;                                                      \
        e = enum_from_string<E>(s);                                   \
        return is;                                                    \
    }

template <typename T>
struct EnumMetaInfo
{
    static std::array<std::string, 0> Info()
    {
        return std::array<std::string, 0>{};
    }
};

template <typename E>
std::string enum_to_string(const E &e)
{
    using base_type = std::underlying_type_t<E>;
    auto const &string_list = EnumMetaInfo<E>::Info();
    const auto index = static_cast<base_type>(e);
    const auto max_size = string_list.max_size();
    // todo : anyway to do static check. assert(index >= max_size, "Error, enum value is not in range");
    if (index >= static_cast<base_type>(max_size))
    {
        return std::string{};
    }
    return string_list[index];
}

template <typename E>
E enum_from_string(const std::string &s)
{
    auto const &string_list = EnumMetaInfo<E>::Info();
    const auto max_size = string_list.max_size();
    size_t n = 0;
    while (n < max_size && string_list[n] != s)
    {
        ++n;
    }
    // todo: error handling
    return n == max_size ? E{} : static_cast<E>(n);
}

#endif // ENUM_STRINGS_H