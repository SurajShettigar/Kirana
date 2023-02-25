#include "logger.hpp"

#include "constants.h"

#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


kirana::utils::Logger::Logger() : m_minSeverity{LogSeverity::debug}
{
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    const spdlog::sinks_init_list sinks = {consoleSink};

    m_current = std::make_shared<spdlog::logger>(constants::APP_NAME, sinks.begin(),
                                                 sinks.end());
    m_current->set_level(static_cast<spdlog::level::level_enum>(m_minSeverity));
    spdlog::set_default_logger(m_current);
}

void kirana::utils::Logger::setMinSeverity(LogSeverity severity)
{
    m_minSeverity = severity;
    m_current->set_level(static_cast<spdlog::level::level_enum>(m_minSeverity));
}

void kirana::utils::Logger::log(const char *channel, LogSeverity severity,
                                const std::string &message)
{
    spdlog::log(static_cast<spdlog::level::level_enum>(severity), "[{}]  {}",
                channel, message);
}