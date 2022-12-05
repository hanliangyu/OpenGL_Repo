#ifndef OPENGL_DEMO_WINDOWMANAGER_H
#define OPENGL_DEMO_WINDOWMANAGER_H

#include <iostream>
#include <string>
#include <GLFW/glfw3.h>

// Helper class to simplify the setting up of GLFW window

class glfwWindowManager{
public:
    explicit glfwWindowManager(int windowWidth = 800, int windowHeight = 600, std::string applicationTitle = "Demo");
    void processInput();
    GLFWwindow* get(); // get window
private:
    static void _framebuffer_resize_callback(GLFWwindow* window, int width, int height);
    GLFWwindow* _window = nullptr;
    int _windowHeight;
    int _windowWidth;
    std::string _applicationTitle;
};

#endif //OPENGL_DEMO_WINDOWMANAGER_H
