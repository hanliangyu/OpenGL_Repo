//
// Created by Han on 2022/12/3.
//

#include <iostream>
#include <vector>
#include "glad/glad.h"
#include "windowManager.h"
#include "Shader.h"

static unsigned int WindowWidth = 1000;
static unsigned int WindowHeight = 800;

int main(){
    float OL1, OL2, OL3, IL1;
    std::cout << "Input OL1: \n";
    std::cin >> OL1;
    std::cout << "Input OL2: \n";
    std::cin >> OL2;
    std::cout << "Input OL3: \n";
    std::cin >> OL3;
    std::cout << "Input IL1: \n";
    std::cin >> IL1;

    std::string title = "OL1: " + std::to_string(OL1) + " OL2: " + std::to_string(OL2) + " OL3: " + std::to_string(OL3)
            + " IL1: " + std::to_string(IL1);

    glfwWindowManager windowManager (WindowWidth, WindowHeight, title);
    auto window = windowManager.get();

    // Initialise glad functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialise ShaderManger
    std::string vertexFile = "/Users/han/CLionProjects/OpenGl_Repo/shaders/tess_vs.vs.glsl";
    std::string fragmentFile = "/Users/han/CLionProjects/OpenGl_Repo/shaders/tess_fs.fs.glsl";
    std::string tcsFile = "/Users/han/CLionProjects/OpenGl_Repo/shaders/tess_control_shader.tcs.glsl";
    std::string tesFile = "/Users/han/CLionProjects/OpenGl_Repo/shaders/tess_eval_shader.tes.glsl";
    ShaderManager shaderManager(vertexFile, fragmentFile, tcsFile, tesFile);

    // Configure the shaders
    shaderManager.configureVertexShader();
    shaderManager.configureFragmentShader();
    shaderManager.configureTCSShader();
    shaderManager.configureTESShader();
    shaderManager.configureShaderProgram();

    GLint maxTessLevel;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    std::cout << "Max available tessellation level supported: " << maxTessLevel << "\n";
    std::cout << "Tessellation Levels: OL1=" << OL1 << " OL2=" << OL2 << " OL3=" << OL3 << " IL1=" << IL1 << "\n";

    // Define the geometry and followed by the vertex color (first three are vertex coordinates, followed by color)
    std::vector<float> colored_triangle {
            -0.5f, -0.5f, 0.0f, // left
            0.5f, -0.5f, 0.0f, // right
            0.0f,  0.5f, 0.0f  // top
    };
    // OL1, OL2, OL3, IL
    std::vector<float> tess_factors {
            OL1, OL2, OL3, IL1
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // activate attribute 0

    glPatchParameteri(GL_PATCH_VERTICES, 3);

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    while (!glfwWindowShouldClose(window))
    {
        windowManager.processInput(); // check for key event
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // this is a state setting function
        glClear(GL_COLOR_BUFFER_BIT); // clear the screen with the color defined on top

        // Set time-varying color
        auto shaderProgram = shaderManager.getShaderProgram();
        glUseProgram(shaderProgram); // Need to run the program before setting Uniform

        int tess_factor = glGetUniformLocation(shaderProgram, "TessLevelFactors"); // Get the attribute location
        glUniform4f(tess_factor, tess_factors[0], tess_factors[1], tess_factors[2], tess_factors[3]);

        glBindVertexArray(VAO);
        glDrawArrays(GL_PATCHES, 0, 3);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

}

