#include "app.hpp"

using kirana::Application;

int main(int argc, char **argv)
{
    Application &app = Application::get();
    app.run();
    return 0;
}