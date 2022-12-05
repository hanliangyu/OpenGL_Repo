#include <iostream>
#include <string>
#include <vector>
#include "Shader.h"
#include "windowManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Here we will add texture to a box that is made from 2 triangles
const int windowWidth = 800;
const int windowHeight = 600;

float ratio = 0.2;
unsigned int shader_index; // run time determined

// key callback - to be used by GLFW to adjust ratio at runtime
void ratio_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UP && ratio <= 0.9){
        ratio += 0.1;
        glUniform1f(glGetUniformLocation(shader_index, "ratio"), ratio); // update the value of the uniform
    }
    if (key == GLFW_KEY_DOWN && ratio >= 0.1){
        ratio -= 0.1;
        glUniform1f(glGetUniformLocation(shader_index, "ratio"), ratio);
    }
}

// Define two simple shaders
// Here in order for the shader to use the texture, we define as the input the coordinates of the texture at each vertex
const char* vertexShaderSource = "#version 330 core\n" // version of GLSL (from 330 onwards GL and GLSL versions are synced)
                                 "layout (location = 0) in vec3 aPos;\n" // location here needs to be defined, will be referenced later
                                 "layout (location = 1) in vec3 aColor;\n"
                                 "layout (location = 2) in vec2 aTexCoord;\n" // texture coordinate as input
                                 "out vec3 ourColor;\n"
                                 "out vec2 TexCoord;\n" // pass on the texture coordinate to the fragment shader
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos, 1.0);\n" // no transformation at all
                                 "   ourColor = aColor;\n" // vertex color
                                 "   TexCoord = aTexCoord;\n"
                                 "}\0";

const char* fragmentShaderSource =  "#version 330 core\n"
                                    "in vec3 ourColor;\n"
                                    "in vec2 TexCoord;\n"
                                    "out vec4 FragColor;\n"
                                    // the sampler is used to link the texture to fragment shader
                                    // each of the sampler maps the texture to the display using the texture coordinate
                                    "uniform sampler2D texture1;\n"
                                    "uniform sampler2D texture2;\n"
                                    "uniform float ratio;\n"
                                    "void main(){\n"
                                    // the mix function combines two textures and the ratio at the end is applied to the first texture
                                    // Then we can simply multiply the color to impose the color on top

                                    // Here we try to flip one of the texture using some vector operations
                                    "   FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(1.0-TexCoord[0], TexCoord[1])), ratio)*vec4(ourColor, 1.0);\n"
                                    "}\n\0";

int main(){
    // Set up the window
    glfwWindowManager windowManager (windowWidth, windowHeight, "Box with texture");
    auto window = windowManager.get();

    // Set up GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set up shaders
    ShaderManager shaderManager{};
    shaderManager.addVertexSource(vertexShaderSource);
    shaderManager.addFragmentSource(fragmentShaderSource);
    shaderManager.configureVertexShader();
    shaderManager.configureFragmentShader();
    shaderManager.configureShaderProgram();

    shader_index = shaderManager.getShaderProgram();
    glfwSetKeyCallback(window, ratio_key_callback);

    /*--------------- Setting up buffers ----------------*/
    // As usual, we have vertex buffer object, vertex array object and element buffer object to send the data to GPU.
    // The only difference this time is that we have the extra texture coordinate inside the vector. The offset and
    // stride values will be different when we define the attributes

    // Here we set the texture coordinates to be between [0, 2]. As the actual texture coordinates only go from 0 to 1
    // This effectively means that the object becomes bigger than the texture. Depending on the texture wrapping
    // setup, different effects can be used to fill in the additional space
    std::vector<float> textured_rec {
            // vertex               color               texture
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f, // top right
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
            -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 2.0f // top left
    };
    // Then the index order to plot the triangles
    std::vector<int> index {
            0, 1, 2,
            2, 3, 0
    };
    // Set up all the buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, textured_rec.size()*sizeof(float), textured_rec.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // type here is different
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size()*sizeof(int), index.data(), GL_STATIC_DRAW);

    // Here we need change it a bit as we have the extra texture data
    // For the vertex data only the stride changes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // For the color data we need offset by 3 bytes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // For the texture coord attribute, it only takes 2 bytes instead of 3
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

//    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
//    glBindVertexArray(0); // unbind VAO -> useful when we have multiple VAOs

    /*--------------- Setting Texture ----------------*/
    // generate the textures, each texture represents one picture loaded
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1); // store the handle inside the variable
    glBindTexture(GL_TEXTURE_2D, texture1);
    // Set the texture wrapping / filtering options
    // glTexParameteri has several options:
    // 1. specifies the target texture
    // 2. name of the texture parameter
    // 3. the value of the parameter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S here stands for x direction
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T here stands for y direction
    // For GL_TEXTURE_MIN_FILTER parameter, there are different options to do the down-sampling
    // GL_LINEAR and GL_NEAREST will be the faster operations as they sample from the image
    // GL_NEAREST_MIPMAP_NEAREST and others use mipmap to get better results but are slower in general
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Define the down-sampling operation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Define the up-sampling operation

    // Load the image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // flip the image vertically
    unsigned char *data = stbi_load("/Users/han/CLionProjects/OpenGl_Repo/img/container.jpg", &width, &height, &nrChannels, 0);

    if(data){
        // generate texture with the loaded image
        // For the second argument, which defines the level of mipmap, we choose 0 as we will auto generate later
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); // auto generate mipmap
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data); // manually free the resource

    // texture 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // S here stands for x direction
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); // T here stands for y direction
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Define the down-sampling operation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Define the up-sampling operation

    // Load the image
    data = stbi_load("/Users/han/CLionProjects/OpenGl_Repo/img/awesomeface.png", &width, &height, &nrChannels, 0);

    if(data){
        //std::cout << height << " " << width << std::endl;
        // generate texture with the loaded image
        // For the second argument, which defines the level of mipmap, we choose 0 as we will auto generate later
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); // auto generate mipmap
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data); // manually free the resource

    /*----------------------- Texture Unit -----------------------*/
    // In the GLSL code for the fragment shader, we used the texture sampler uniform to link the supplied texture to the
    // shader. On some graphics drivers you need to assign it some value with glUniform and on some drivers a default value
    // of 0 is provided if we only have 1. The assigned location of the texture sampler is known as the texture unit.
    // We can make use of the texture units to use multiple textures at the same time

    // OpenGL has at least 16 texture units that can be activated by using the macro GL_TEXTURE0 to GL_TEXTURE15
    // On GLES the limit is 8
    // These texture units can also be accessed like GL_TEXTURE0+1 == GL_TEXTURE1
    // Then before we bind the texture we can activate a certain texture by calling glActiveTexture(GL_TEXTURE0)
    // followed by glBindTexture(GL_TEXTURE_2D, texture) in the render loop
    glUseProgram(shaderManager.getShaderProgram()); // activate the shader program first
    glUniform1i(glGetUniformLocation(shaderManager.getShaderProgram(), "texture1"), 0); // set the uniforms
    glUniform1i(glGetUniformLocation(shaderManager.getShaderProgram(), "texture2"), 1);

    /*----------------------- Mix ratio -----------------------*/
    // Here we initialise the mix ratio inside the fragment shader before any callback is done
    glUniform1f(glGetUniformLocation(shaderManager.getShaderProgram(), "ratio"), ratio);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        windowManager.processInput();
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // this is a state setting function
        glClear(GL_COLOR_BUFFER_BIT); // clear the screen with the color defined on top

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glUseProgram(shaderManager.getShaderProgram());

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}