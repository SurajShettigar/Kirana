#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <iostream>

namespace kirana::utils
{
typedef boost::log::attributes::mutable_constant<const char *> LogAttribute;
typedef boost::log::trivial::severity_level LogSeverity;
typedef boost::log::sources::severity_channel_logger<LogSeverity, const char *>
    SCLogger;
namespace keywords = boost::log::keywords;
namespace expression = boost::log::expressions;

class Logger
{
  private:
    LogAttribute m_channel;
    LogSeverity m_minSeverity = LogSeverity::debug;
    SCLogger m_current;

    Logger()
        : m_channel{LogAttribute("DEFAULT")}, m_minSeverity{LogSeverity::debug},
          m_current{SCLogger(keywords::severity = m_minSeverity,
                             keywords::channel = "DEFAULT")}
    {
        boost::log::add_common_attributes();
        m_current.add_attribute("Channel", m_channel);
        setMinSeverity(m_minSeverity);
    }

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
    inline void setMinSeverity(LogSeverity severity)
    {
        m_minSeverity = severity;
        boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                            severity);
    }

    inline void log(const char *channel = "DEFAULT",
                    LogSeverity severity = LogSeverity::debug,
                    const char *message = "")
    {
        m_channel.set(channel);
        BOOST_LOG_CHANNEL_SEV(m_current, channel, severity) << message;
    }
};
} // namespace kirana::utils
#endif