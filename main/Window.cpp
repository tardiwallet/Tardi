#include "Window.h"
#include "GUI.h"

Window::Window(const char *name)
{
    this->name = name;
}

void Window::setGui(GUI *gui)
{
    assert(gui->isInitialized());
    gui = gui;
}
