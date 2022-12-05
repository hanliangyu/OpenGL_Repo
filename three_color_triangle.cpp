#include <iostream>
#include <vector>
#include "glad/glad.h"
#include "windowManager.h"
#include "Shader.h"

// Here we associate each vertex of the triangle with a unique color by encoding the color information
// inside the VBO. Modifications are then made to how we set the vertex attribute to incorporate the different stride

// Even though we only specified color for the three vertices, the whole triangle is filled with color throughout.
// This is called the fragment interpolation, for which the fragment shader interpolates the color in-between the
// vertices (linear interpolation)

static unsigned int WindowWidth = 1000;
static unsigned int WindowHeight = 800;

int main(){
    glfwWindowManager windowManager (WindowWidth, WindowHeight, "Three Color Triangle");
    auto window = windowManager.get();

    // Initialise glad functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialise ShaderManger
    std::string vertexFile = "/Users/han/CLionProjects/OpenGl_Repo/shaders/three_color_triangle_vertex";
    std::string fragmentFile = "/Users/han/CLionProjects/OpenGl_Repo/shaders/three_color_triangle_fragment";
    ShaderManager shaderManager(vertexFile, fragmentFile);

    // Configure the shaders
    shaderManager.configureVertexShader();
    shaderManager.configureFragmentShader();
    shaderManager.configureShaderProgram();

    // Define the geometry and followed by the vertex color (first three are vertex coordinates, followed by color)
    std::vector<float> colored_triangle {
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    // Configure VBO, VAO (For simple triangle we omit EBO)
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Upload vertex data onto VBO
    glBufferData(GL_ARRAY_BUFFER, colored_triangle.size()*sizeof(float), colored_triangle.data(), GL_STATIC_DRAW);

    // Set the first attribute -> vertex position -> Take note the stride is 6 now as position data is added
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // activate attribute 0

    // Set the second attribute -> vertex color -> take note of the offset at the end (offset by the first 3 pos value)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // activate attribute 1

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    glBindVertexArray(0); // unbind VAO -> useful when we have multiple VAOs

    while (!glfwWindowShouldClose(window))
    {
        windowManager.processInput(); // check for key event
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // this is a state setting function
        glClear(GL_COLOR_BUFFER_BIT); // clear the screen with the color defined on top

        // Set time-varying color
        glUseProgram(shaderManager.getShaderProgram()); // Need to run the program before setting Uniform
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

}