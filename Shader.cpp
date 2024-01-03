
#include <iostream>
using namespace std;
#define GLEW_STATIC

#include <glad/glad.h>
#include "Shader.h"
#include <string>
#include <fstream>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vShaderFile(vertexPath);
    std::ifstream fShaderFile(fragmentPath);

    while (!vShaderFile.eof())
    {
        std::string line;
        std::getline(vShaderFile, line);
        vertexCode += line + "\n";
    }

    while (!fShaderFile.eof())
    {
        std::string line;
        std::getline(fShaderFile, line);
        fragmentCode += line + "\n";
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    GLuint vertexShader_1 = glCreateShader(GL_VERTEX_SHADER); // 创建顶点着色器对象
    glShaderSource(vertexShader_1, 1, &vShaderCode, NULL);    // 将顶点着色器的内容传进来
    glCompileShader(vertexShader_1);                          // 编译顶点着色器
    GLint flag;                                               // 用于判断编译是否成功
    GLchar infoLog[512];                                      // 512个字符
    glGetShaderiv(vertexShader_1, GL_COMPILE_STATUS, &flag);  // 获取编译状态
    if (!flag)
    {
        glGetShaderInfoLog(vertexShader_1, 512, NULL, infoLog); // 缓冲池
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
            << infoLog << endl;
    }

    /* 第四步：编写片元着色器(也称片段着色器) */
    GLuint fragmentShader_1 = glCreateShader(GL_FRAGMENT_SHADER); // 创建片元着色器对象
    glShaderSource(fragmentShader_1, 1, &fShaderCode, NULL);      // 将顶点着色器的内容传进来
    glCompileShader(fragmentShader_1);                            // 编译顶点着色器
    glGetShaderiv(fragmentShader_1, GL_COMPILE_STATUS, &flag);    // 获取编译状态
    if (!flag)
    {
        glGetShaderInfoLog(fragmentShader_1, 512, NULL, infoLog); // 缓冲池
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
            << infoLog << endl;
    }

    /* 第五步：创建着色器程序 */
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader_1);
    glAttachShader(ID, fragmentShader_1);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &flag);
    if (!flag)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
            << infoLog << endl;
    }
    glDeleteShader(vertexShader_1);
    glDeleteShader(fragmentShader_1);
}

void Shader::use()
{
    glUseProgram(ID);
}