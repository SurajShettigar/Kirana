#ifndef EVENT_HPP
#define EVENT_HPP

#include <functional>
#include <unordered_map>

namespace kirana::utils
{
template <typename... Args> class Event
{
    using Callback = std::function<void(Args...)>;

  private:
    uint32_t m_listenerID = 0;
    std::unordered_map<uint32_t, Callback> m_callbacks;

  public:
    Event() : m_listenerID{0} {};
    ~Event() = default;

    uint32_t addListener(const Callback &c)
    {
        m_callbacks[m_listenerID] = c;
        return m_listenerID++;
    }

    void removeListener(uint32_t callbackID)
    {
        m_callbacks.erase(callbackID);
    }

    void operator()(Args... args) const
    {
        for (const auto &[id, callback] : m_callbacks)
        {
            callback(args...);
        }
    }
};
} // namespace kirana::utils

#endif