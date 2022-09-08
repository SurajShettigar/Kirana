#ifndef APP_HPP
#define APP_HPP

class Application
{
private:
    Application(const Application& app) = delete;
    Application& operator=(const Application& app) = delete;
public:
    Application();
    ~Application();
};

#endif