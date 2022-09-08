#include "app.hpp"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;


Application::Application()
{
    cout << "Hello Application!" << endl;
}


Application::~Application()
{
    cout << "Goodbye Application!" << endl;
}