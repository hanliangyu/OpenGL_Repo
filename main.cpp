#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "windowManager.h"

static int VIEW_HEIGHT = 600;
static int VIEW_WIDTH = 800;

unsigned int configureVertexShader(){
    // Here, the GLSL is wrapped inside a string, and the shader does nothing at all but to pass through the data
    const char * vertexShaderSource = "#version 410 core\n" // version of GLSL (from 330 onwards GL and GLSL versions are synced)
                                      "layout (location = 0) in vec3 aPos;\n" // location here needs to be defined, will be referenced later
                                      "void main()\n"
                                      "{\n"
                                      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" // no transformation at all
                                      "}\0";
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    return vertexShader;
}

unsigned int configureFragmentShader(){
    const char * fragmentShaderSource = "#version 410 core\n" // For GLES shaders, the shader version ends with es
                                        "out vec4 FragColor;\n"
                                        "void main(){\n"
                                        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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
    // Here we use an abstraction to hide most of the set-up boiler plates for glfw
    glfwWindowManager windowManager (VIEW_WIDTH, VIEW_HEIGHT);
    auto window = windowManager.get();

    /* GLAD call to load all the functions from GPU driver */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";

    /*************************** Vertex Shader ****************************/
    // Now that the vertex data is stored on the GPU and managed by the VBO, we continue to create a vertex shader
    // Vertex shaders are compulsory to be defined for all GL applications and is written in GLSL (shader language)

    unsigned int vertexShader = configureVertexShader();

    // check if the vertex shader has been compiled successfully
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // retrieve the status of the shader compilation

    if(!success){
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "VERTEX SHADER COMPILATION ERROR: " << infoLog << "\n";
    }

    /*************************** Fragment Shader ***************************/
    // Fragment shader is all about calculating the color output of the pixels
    // Here to keep things simple, the shape is filled with the same color everywhere using RGBA convention

    unsigned int fragmentShader = configureFragmentShader();

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        //memset(infoLog, 0, 512);
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "FRAGMENT SHADER COMPILATION ERROR: " << infoLog << "\n";
    }

    /*************************** Shader Program ***************************/
    // Now that we have the two shaders, we link the multiple shaders to produce an integrated shader program
    // The output of one shader will be the input to the next. Errors will occur if these do not match
    unsigned int shaderProgram = configureShaderProgram(vertexShader, fragmentShader);

    // Check for any linking error
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        //memset(infoLog, 0, 512);
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "SHADER LINKING ERROR: " << infoLog << "\n";
    }

    // After linking we no longer need the two individual shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /*************************** Set up the Vertex Buffer Object and VAO ***************************/
    // Define a vertex input
    std::vector<float> vertices {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
    };

    // Vertex Buffer Object (VBO)
    // Able to store large amount of vertex data in GPU memory. Advantage of using VBO is that we can send over
    // large amount of data at once to the GPU and store it there. Fast access can be done after that from the GPU.
    // OpenGL has many types of buffer objects and the type of buffer for VBO is GL_ARRAY_BUFFER

    // Vertex Array Object (VAO)
    // Normally the flow for drawing objects goes like this:
    // glBindBuffer -> glBufferData -> glVertexAttribPointer -> glEnableVertexAttribArray -> glUseProgram -> plot
    // And this needs to be repeated every time we want to draw an object (cumbersome)
    // Vertex Array Object helps with this by storing:
    // 1. all the configurations from glVertexAttribPointer
    // 2. Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
    // 3. Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer.
    // Thus we only need to switch on the relevant VAO when we want to draw certain object

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); // vertex array object, explained later
    glGenBuffers(1, &VBO); // the index corresponding to the buffer will be returned in VBO
    glBindVertexArray(VAO); // Now all the settings will be linked to this VAO

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the buffer to buffer type GL_ARRAY_BUFFER
    // After this all the calls related to GL_ARRAY_BUFFER will be configuring the VBO linked

    // The next function copies the data to the GPU, GL_STATIC_DRAW indicates the data is set once and used repeatedly
    // Take note the second argument here: we need the actual size in memory in bytes
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Now that we have the shader pipeline linked together, OpenGL still needs to know how it should handle
    // the vertex data and how to connect to the shaders' attribute

    // As the position data are stored as doubles (64 bits / 8 bytes)
    // The first argument is related to the location defined in GLSL in the vertex shader (location==0)
    // Second argument: size of vertex attribute
    // Third argument: data format
    // Forth argument: data normalisation
    // Fifth argument: offset to where the data begins (In this case is 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // tells opengl how to interpret vertex data
    glEnableVertexAttribArray(0); // enable the vertex attribute (location is 0)

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind

    glBindVertexArray(0); // unbind VAO -> useful when we have multiple VAOs

    /*************************** Element Buffer Objects ***************************/
    // When we try to draw multiple triangles that are linked together, declaring each of the vertices of the triangles
    // become inefficient as there will be overlaps. Element Buffer Object provides us a way to efficiently declare the
    // vertices (only the unique ones), together with an index array to tell openGL how to connect these vertices
    std::vector<float> rev_vertices {
            0.5f,  0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left
    };

    std::vector<int> indices {
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
    };

    // Here we create a new set of buffers
    unsigned int VAO_rec, VBO_rec, EBO;
    glGenVertexArrays(1, &VAO_rec);
    glGenBuffers(1, &VBO_rec);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO_rec); // all configurations will be saved inside VAO_rec

    glBindBuffer(GL_ARRAY_BUFFER, VBO_rec);
    glBufferData(GL_ARRAY_BUFFER, rev_vertices.size()*sizeof(float), rev_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // type here is different
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    glBindVertexArray(0); // unbind VAO -> useful when we have multiple VAOs

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wire-frame mode

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        windowManager.processInput();
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // this is a state setting function
        glClear(GL_COLOR_BUFFER_BIT); // clear the screen with the color defined on top

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO_rec); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        // Here the application uses a double buffer approach
        // This prevents issues like flickering when a single buffer is being updated from happening
        // The front buffer contains the final output image that is shown at the screen,
        // while all the rendering commands draw to the back buffer
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;


}
