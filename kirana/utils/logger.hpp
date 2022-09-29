#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/attributes.hpp>

namespace kirana::utils
{
typedef boost::log::trivial::severity_level LogSeverity;
typedef boost::log::attributes::mutable_constant<const char *> LogAttribute;
typedef boost::log::sources::severity_channel_logger_mt<LogSeverity, const char *>
    SCLogger;

class Logger
{
  private:
    LogAttribute m_channel;
    LogSeverity m_minSeverity;
    SCLogger m_current;

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
             const char *message = "");

    void log(const char *channel = "DEFAULT",
             LogSeverity severity = LogSeverity::debug,
             const std::string &message = "");
};
} // namespace kirana::utils
#endif