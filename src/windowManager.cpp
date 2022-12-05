#include "windowManager.h"

void glfwWindowManager::_framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0, width, height);
}

glfwWindowManager::glfwWindowManager(int windowWidth, int windowHeight, std::string applicationTitle)
    : _windowHeight(windowHeight), _windowWidth(windowWidth), _applicationTitle(applicationTitle){
    // initialise
    if(!glfwInit()){
        throw GLFW_NOT_INITIALIZED;
    }

    // set the GL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // need this one for Mac

    // initialise window
    _window = glfwCreateWindow(_windowWidth, _windowHeight, _applicationTitle.data(), nullptr, nullptr);
    if (!_window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(_window);

    glfwSetFramebufferSizeCallback(_window, &glfwWindowManager::_framebuffer_resize_callback);

}

void glfwWindowManager::processInput() {
    if(glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(_window, 1);
    }
}

GLFWwindow * glfwWindowManager::get() {
    return _window;
}

