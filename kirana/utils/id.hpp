#ifndef KIRANA_UTILS_ID_HPP
#define KIRANA_UTILS_ID_HPP

#include <functional>

namespace kirana::utils
{
struct ID
{
    std::string value;

    ID() = default;
    ID(std::string id) : value{std::move(id)} {};

    operator std::string() const
    {
        return value;
    }

    std::string operator()() const
    {
        return value;
    }

    bool operator==(const ID &id) const
    {
        return value == id.value;
    }

    [[nodiscard]] inline bool empty() const
    {
        return value.empty();
    }
};
} // namespace kirana::utils

// hashing function for IDs
template <> struct std::hash<kirana::utils::ID>
{
    std::size_t operator()(kirana::utils::ID const &id) const noexcept
    {
        return std::hash<std::string>{}(id.value);
    }
};

#endif