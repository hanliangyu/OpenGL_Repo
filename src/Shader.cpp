#include "Shader.h"

// Implementation of the ShaderManager class

ShaderManager::ShaderManager(std::string vertexPath, std::string fragmentPath,
                             std::string tcsPath, std::string tesPatch)
:_vertexPath(vertexPath), _fragmentPath(fragmentPath), _tcsPath(tcsPath), _tesPath(tesPatch)
{
    _readFile();
}

void ShaderManager::_readFile() {
    // Use ifstream objects
    std::ifstream vertexFile;
    std::ifstream fragmentFile;
    // ensure ifstream objects can throw exceptions: (logic + read error)
    vertexFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fragmentFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        vertexFile.open(_vertexPath);
        fragmentFile.open(_fragmentPath);
        std::stringstream vertexStream;
        std::stringstream fragmentStream;
        vertexStream << vertexFile.rdbuf() << "\0";
        fragmentStream << fragmentFile.rdbuf() << "\0";
        vertexFile.close();
        fragmentFile.close();
        // Save the files onto the internal pointers
        _vertexSource = vertexStream.str();
        _fragmentSource = fragmentStream.str();
    }
    catch(std::ifstream::failure &e){
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
    // tessellation shaders
    if(_tcsPath != "" && _tesPath != ""){
        std::cout << "LOADING TESS SHADER..\n";
        std::ifstream tcsFile;
        std::ifstream tesFile;
        tcsFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        tesFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try {
            tcsFile.open(_tcsPath);
            tesFile.open(_tesPath);
            std::stringstream tcsStream;
            std::stringstream tesStream;
            tcsStream << tcsFile.rdbuf() << "\0";
            tesStream << tesFile.rdbuf() << "\0";
            tcsFile.close();
            tesFile.close();
            // Save the files onto the internal pointers
            _tcsSource = tcsStream.str();
            _tesSource = tesStream.str();
        }
        catch(std::ifstream::failure &e){
            std::cout << "ERROR::SHADER::TESS_FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }
    }
}

void ShaderManager::addVertexSource(std::string vertexSource) {
    _vertexSource = vertexSource;
}

void ShaderManager::addFragmentSource(std::string fragmentSource) {
    _fragmentSource = fragmentSource;
}

void ShaderManager::configureVertexShader() {
    _vertexShader = glCreateShader(GL_VERTEX_SHADER);
    auto vertexCode = _vertexSource.c_str();
    glShaderSource(_vertexShader, 1, &vertexCode, nullptr);
    glCompileShader(_vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(_vertexShader, GL_COMPILE_STATUS, &success); // retrieve the status of the shader compilation

    if(!success){
        glGetShaderInfoLog(_vertexShader, 512, nullptr, infoLog);
        std::cout << "VERTEX SHADER COMPILATION ERROR: " << infoLog << "\n";
    }
}

void ShaderManager::configureFragmentShader() {
    _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    auto fragmentCode = _fragmentSource.c_str();
    glShaderSource(_fragmentShader, 1, &fragmentCode, nullptr);
    glCompileShader(_fragmentShader);

    int success;
    char infoLog[512];
    glGetShaderiv(_fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        //memset(infoLog, 0, 512);
        glGetShaderInfoLog(_fragmentShader, 512, nullptr, infoLog);
        std::cout << "FRAGMENT SHADER COMPILATION ERROR: " << infoLog << "\n";
    }
}

void ShaderManager::configureTCSShader() {
    _tcsShader = glCreateShader(GL_TESS_CONTROL_SHADER);
    auto tcsCode = _tcsSource.c_str();
    glShaderSource(_tcsShader, 1, &tcsCode, nullptr);
    glCompileShader(_tcsShader);

    int success;
    char infoLog[512];
    glGetShaderiv(_tcsShader, GL_COMPILE_STATUS, &success);
    if(!success){
        //memset(infoLog, 0, 512);
        glGetShaderInfoLog(_tcsShader, 512, nullptr, infoLog);
        std::cout << "TCS SHADER COMPILATION ERROR: " << infoLog << "\n";
    }
}

void ShaderManager::configureTESShader() {
    _tesShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    auto tesCode = _tesSource.c_str();
    glShaderSource(_tesShader, 1, &tesCode, nullptr);
    glCompileShader(_tesShader);

    int success;
    char infoLog[512];
    glGetShaderiv(_tesShader, GL_COMPILE_STATUS, &success);
    if(!success){
        //memset(infoLog, 0, 512);
        glGetShaderInfoLog(_tesShader, 512, nullptr, infoLog);
        std::cout << "TES SHADER COMPILATION ERROR: " << infoLog << "\n";
    }
}

void ShaderManager::configureShaderProgram() {
    _shaderProgram = glCreateProgram();

    // Then attach the two shaders
    glAttachShader(_shaderProgram, _vertexShader);
    glAttachShader(_shaderProgram, _fragmentShader);
    if(_tcsSource != "" && _tesSource != ""){
        glAttachShader(_shaderProgram, _tcsShader);
        glAttachShader(_shaderProgram, _tesShader);
    }
    glLinkProgram(_shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(_shaderProgram, 512, nullptr, infoLog);
        std::cout << "SHADER LINKING ERROR: " << infoLog << "\n";
    }
    glDeleteShader(_vertexShader);
    glDeleteShader(_fragmentShader);
}

unsigned int ShaderManager::getVertexShader() const {
    return _vertexShader;
}

unsigned int ShaderManager::getFragmentShader() const {
    return _fragmentShader;
}

unsigned int ShaderManager::getShaderProgram() const {
    return _shaderProgram;
}