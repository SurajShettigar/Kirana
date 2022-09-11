#include "app.hpp"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

kirana::Application::Application()
{
    cout << "Hello Application!" << endl;
}

kirana::Application::~Application()
{
    cout << "Goodbye Application!" << endl;
}

void kirana::Application::init()
{
    m_windowManager.init();
    m_windowManager.setOnAllWindowsClosedListener(
        [=]() { m_isRunning = false; });

    m_windowManager.createWindow();
    m_windowManager.createWindow("New Window", 640, 360);
    m_isRunning = true;
}

void kirana::Application::update()
{
    m_windowManager.update();
}

void kirana::Application::render()
{
}

void kirana::Application::clean()
{
    m_windowManager.clean();
    m_isRunning = false;
}

void kirana::Application::run()
{
    init();

    while (m_isRunning)
    {
        update();
        render();
    }

    clean();
}
