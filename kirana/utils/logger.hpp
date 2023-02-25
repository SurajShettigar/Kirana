#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <memory>

namespace spdlog {
class logger;
}

namespace kirana::utils
{
enum class LogSeverity
{
    trace = 0,
    debug = 1,
    info = 2,
    warning = 3,
    error = 4,
    fatal = 5
};

class Logger
{
  private:
    LogSeverity m_minSeverity;
    std::shared_ptr<spdlog::logger> m_current;

    Logger();
    ~Logger() = default;

  public:
    Logger(const Logger &logger) = delete;

    static Logger &get()
    {
        static Logger instance;
        return instance;
    }

    inline LogSeverity getMinSeverity() const
    {
        return m_minSeverity;
    }
    void setMinSeverity(LogSeverity severity);

    void log(const char *channel = "DEFAULT",
             LogSeverity severity = LogSeverity::debug,
             const std::string &message = "");
};
} // namespace kirana::utils
#endif