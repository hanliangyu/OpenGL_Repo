#ifndef OPENGL_DEMO_SHADER_H
#define OPENGL_DEMO_SHADER_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include "glad/glad.h"

// This ShaderManager class is created to read in shader source from local files or alternatively from input string
// It also abstracts away some of the boiler plate codes that are constantly repeating

class ShaderManager{
public:
    ShaderManager() = default;
    ShaderManager(std::string vertexPath, std::string fragmentPath, std::string tcsPath = "", std::string tesPatch = "");
    void addVertexSource(std::string vertexSource);
    void addFragmentSource(std::string fragmentSource);
    void configureVertexShader();
    void configureFragmentShader();
    void configureTCSShader();
    void configureTESShader();
    void configureShaderProgram();
    unsigned int getVertexShader() const;
    unsigned int getFragmentShader() const;
    unsigned int getShaderProgram() const;
private:
    void _readFile();
    std::string _vertexSource; // on heap as the source file might be big
    std::string _fragmentSource;
    std::string _tcsSource = "";
    std::string _tesSource = "";
    std::string _vertexPath; // File location
    std::string _fragmentPath;
    std::string _tcsPath = "";
    std::string _tesPath = "";
    unsigned int _vertexShader;
    unsigned int _fragmentShader;
    unsigned int _tcsShader;
    unsigned int _tesShader;
    unsigned int _shaderProgram;
};

#endif //OPENGL_DEMO_SHADER_H
