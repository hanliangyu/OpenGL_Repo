#include <iostream>
#include <vector>
#include "glad/glad.h"
#include "windowManager.h"

// Here we create 2 triangles that change color using uniforms

static unsigned int WindowWidth = 1000;
static unsigned int WindowHeight = 800;

unsigned int configureVertexShader(){
    // Use the same shader
    const char * vertexShaderSource = "#version 330 core\n" // version of GLSL (from 330 onwards GL and GLSL versions are synced)
                                      "layout (location = 0) in vec3 aPos;\n" // location here needs to be defined, will be referenced later
                                      "void main()\n"
                                      "{\n"
                                      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" // no transformation at all
                                      "}\0";
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    return vertexShader;
}

unsigned int configureFragmentShader(){
    const char * fragmentShaderSource = "#version 330 core\n" // For GLES shaders, the shader version ends with es
                                        "out vec4 FragColor;\n"
                                        "uniform vec4 outColor;\n" // Here we define a uniform
                                        "void main(){\n"
                                        "   FragColor = outColor;\n"
                                        "}\n\0";;
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    return fragmentShader;
}

unsigned int configureShaderProgram(unsigned int vertexShader, unsigned int fragShader){
    unsigned int shaderProgram = glCreateProgram();

    // Then attach the two shaders
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    return shaderProgram;
}

int main(){
    // Initialise window
    glfwWindowManager windowManager (WindowWidth, WindowHeight, "Double Triangles");
    auto window = windowManager.get(); // Get the GLFWwindow object

    // Initialise glad functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Configure the shaders
    auto vertexShader = configureVertexShader();

    auto fragmentShader = configureFragmentShader();

    auto shaderProgram = configureShaderProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Define the geometry
    std::vector<float> triangle_vertices{
            -0.5f ,  0.5f, 0.0f,
            -0.75f, -0.5f, 0.0f,
            -0.25f, -0.5f, 0.0f,
             0.5f ,  0.5f, 0.0f,
             0.25f, -0.5f, 0.0f,
             0.75f, -0.5f, 0.0f
    };

    // Define linking order
    std::vector<int>index{
            0, 1, 2, // First triangle
            3, 4, 5 // Second triangle
    };

    // Configure VBO and VAO and EBO
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, triangle_vertices.size()*sizeof(float), triangle_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // type here is different
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size()*sizeof(int), index.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    glBindVertexArray(0); // unbind VAO -> useful when we have multiple VAOs

    while (!glfwWindowShouldClose(window))
    {
        windowManager.processInput();
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // this is a state setting function
        glClear(GL_COLOR_BUFFER_BIT); // clear the screen with the color defined on top

        // Set time-varying color
        glUseProgram(shaderProgram); // Need to run the program before setting Uniform
        float currentTime = glfwGetTime();
        float greenValue = (sin(currentTime) / 2.0f) + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "outColor"); // Get the attribute location
        // As the OpenGL APIs are built with C, different versions of the function are available:
        // glUniform4f, glUniform4i, glUniform4ui, glUniform4fv etc..
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); // RGBA

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
}