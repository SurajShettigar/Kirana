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