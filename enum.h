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

using stringlist = std::vector<std::string>;

#define ENUM_STRINGS(E, ...)                                    \
    static_assert(std::is_enum<E>::value,                       \
                  "Not an enumeration type");                   \
                                                                \
    inline std::vector<std::string> const &_get_enum_strings(E) \
    {                                                           \
        static stringlist ss{__VA_ARGS__};                      \
        return ss;                                              \
    }                                                           \
                                                                \
    inline std::ostream &                                       \
    operator<<(std::ostream &os, E const e)                     \
    {                                                           \
        os << ::enum_strings::to_string(e);                     \
        return os;                                              \
    }                                                           \
                                                                \
    inline std::istream &                                       \
    operator>>(std::istream &is, E &e)                          \
    {                                                           \
        std::string s;                                          \
        is >> s;                                                \
        e = ::enum_strings::from_string<E>(s);                  \
        return is;                                              \
    }

namespace enum_strings
{
    template <typename E>
    inline std::string to_string(E const e)
    {
        using base_type = std::underlying_type_t<E>;
        const auto &strings = _get_enum_strings(E{});
        auto const index = static_cast<base_type>(e);
        if (index >= static_cast<base_type>(strings.size()))
        {
            throw std::invalid_argument("Invalid value " + std::to_string(index) + ". "
                                                                                   "Valid range is 0.." +
                                        std::to_string(strings.size() - 1));
        }
        return strings[index];
    }

    template <typename E>
    inline E from_string(std::string const &s)
    {
        const auto &strings = _get_enum_strings(E{});
        std::size_t n = 0;
        while (n < strings.size() && strings[n] != s)
        {
            ++n;
        }
        if (n == strings.size())
        {
            throw std::invalid_argument("'" + s + "' is not a valid string representation of this type");
        }
        auto const e = static_cast<E>(n);
        return e;
    }

} // namespace enum_strings

template <typename R, typename P, size_t N, size_t... I>
constexpr std::array<R, N> to_array_impl(P (&a)[N], std::index_sequence<I...>) noexcept
{
    return {{a[I]...}};
}

template <typename T, size_t N>
constexpr std::array<T, N> to_array(T (&a)[N]) noexcept
{
    return to_array_impl < std:: : remove_cv_t<T>, T, N > (a, std::make_index_sequence<N>{})
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

#endif // ENUM_STRINGS_H