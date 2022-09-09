#ifndef APP_HPP
#define APP_HPP

namespace kirana
{
class Application
{
  private:
    Application(const Application &app) = delete;
    Application &operator=(const Application &app) = delete;

  public:
    Application();
    ~Application();
};
} // namespace kirana

#endif