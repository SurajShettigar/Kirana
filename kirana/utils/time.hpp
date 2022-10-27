#ifndef TIME_HPP
#define TIME_HPP

#include <chrono>

namespace kirana::utils
{
class Time
{
  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startPoint;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_currentPoint;
    std::chrono::duration<double> m_elapsedTime;
    std::chrono::duration<double> m_deltaTime;
    double m_avgDeltaTime[3]{0.0, 0.0, 0.0};

    Time()
    {
        m_startPoint = std::chrono::high_resolution_clock::now();
        m_currentPoint = std::chrono::high_resolution_clock::now();
    };
    ~Time() = default;

  public:
    Time(const Time &time) = delete;
    static Time &get()
    {
        static Time instance;
        return instance;
    }

    void update()
    {
        m_deltaTime =
            std::chrono::high_resolution_clock::now() - m_currentPoint;
        m_currentPoint = std::chrono::high_resolution_clock::now();
        m_elapsedTime = m_currentPoint - m_startPoint;
        m_avgDeltaTime[0] = m_avgDeltaTime[1];
        m_avgDeltaTime[1] = m_avgDeltaTime[2];
        m_avgDeltaTime[2] = m_deltaTime.count();
    }

    /// Time since the start of the application in seconds.
    [[nodiscard]] inline double getTimeSinceStartup() const
    {
        return m_elapsedTime.count();
    }
    /// Time taken for previous frame in seconds.
    [[nodiscard]] inline float getDeltaTime() const
    {
        return static_cast<float>(m_deltaTime.count());
    }

    /// Frames per second
    [[nodiscard]] inline uint32_t getFPS() const
    {
        return static_cast<uint32_t>(
            3.0 / (m_avgDeltaTime[0] + m_avgDeltaTime[1] + m_avgDeltaTime[2]));
    }
};
} // namespace kirana::utils
#endif