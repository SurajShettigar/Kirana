#ifndef KIRANA_UTILS_ID_MANAGER_HPP
#define KIRANA_UTILS_ID_MANAGER_HPP

#include "id.hpp"
#include <unordered_map>

namespace kirana::utils
{
class IDManager
{
  public:
    IDManager(const IDManager &idManager) = delete;

    inline static IDManager &get()
    {
        static IDManager instance;
        return instance;
    }

    ID generateUniqueID(std::string preferredValue) const;
  private:
    IDManager() = default;
    ~IDManager() = default;

    mutable std::unordered_map<ID, size_t> m_idCounter;
};
} // namespace kirana::utils

#endif