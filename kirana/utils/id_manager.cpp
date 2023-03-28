#include "id_manager.hpp"

#include <string>

kirana::utils::ID kirana::utils::IDManager::generateUniqueID(
    std::string preferredValue) const
{
    if (preferredValue.empty())
        preferredValue = "Empty";

    auto it = m_idCounter.find(preferredValue);
    if (it == m_idCounter.end())
    {
        ID id{preferredValue};
        m_idCounter[id] = 0;
        return id;
    }
    it->second++;
    preferredValue = it->first.value + "_" + std::to_string(it->second);
    return ID(preferredValue);
}