#include "window.hpp"

kirana::window::Window::Window(string name, int width, int height)
    : name{name}, width{width}, height{height}
{
}

kirana::window::Window::~Window()
{
}