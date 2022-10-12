#include "logger.hpp"

#include <boost/core/null_deleter.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/support/date_time.hpp>
#include <iostream>

typedef boost::log::sinks::synchronous_sink<
    boost::log::sinks::text_ostream_backend>
    TextSink;
namespace keywords = boost::log::keywords;
namespace expression = boost::log::expressions;

kirana::utils::Logger::Logger()
    : m_channel{LogAttribute("DEFAULT")}, m_minSeverity{LogSeverity::debug},
      m_current{SCLogger(keywords::severity = m_minSeverity,
                         keywords::channel = "DEFAULT",
                         keywords::format = "[%TimeStamp%]: %Message%")}
{
    boost::log::add_common_attributes();
    m_current.add_attribute("Channel", m_channel);
    setMinSeverity(m_minSeverity);

    boost::shared_ptr<std::ostream> m_stream(&std::cout, boost::null_deleter());
    boost::shared_ptr<TextSink> m_sink(boost::make_shared<TextSink>());

    m_sink->locked_backend()->add_stream(m_stream);
    m_sink->set_formatter(
        expression::stream
        << " ["
        << expression::format_date_time<boost::posix_time::ptime>(
               "TimeStamp", "%Y-%m_current-%d %H:%M:%S:%f")
        << "]"
        << " [" << boost::log::trivial::severity << "]"
        << " [" << expression::attr<const char *>("Channel") << "]"
        << " " << expression::smessage);

    boost::log::core::get()->add_sink(m_sink);
}

void kirana::utils::Logger::setMinSeverity(LogSeverity severity)
{
    m_minSeverity = severity;
    boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                        severity);
}

void kirana::utils::Logger::log(const char *channel, LogSeverity severity,
                                const char *message)
{
    m_channel.set(channel);
    BOOST_LOG_CHANNEL_SEV(m_current, channel, severity) << message;
}

void kirana::utils::Logger::log(const char *channel, LogSeverity severity,
                                const std::string &message)
{
    log(channel, severity, message.c_str());
}