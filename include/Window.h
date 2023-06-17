#pragma once

class GUI;

class Window
{
private:
    /* data */
    GUI *gui;

protected:
    bool closing = false;
    int activeTasks = 0;

public:
    const char *name;
    Window(const char *name);
    void setGui(GUI *gui);
    virtual void show() = 0;
    virtual void close()
    {
        closing = true;
    }
    virtual bool isClosed() { return closing && (activeTasks == 0); }
};
