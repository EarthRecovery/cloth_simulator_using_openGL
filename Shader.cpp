
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
    GLuint vertexShader_1 = glCreateShader(GL_VERTEX_SHADER); // ����������ɫ������
    glShaderSource(vertexShader_1, 1, &vShaderCode, NULL);    // ��������ɫ�������ݴ�����
    glCompileShader(vertexShader_1);                          // ���붥����ɫ��
    GLint flag;                                               // �����жϱ����Ƿ�ɹ�
    GLchar infoLog[512];                                      // 512���ַ�
    glGetShaderiv(vertexShader_1, GL_COMPILE_STATUS, &flag);  // ��ȡ����״̬
    if (!flag)
    {
        glGetShaderInfoLog(vertexShader_1, 512, NULL, infoLog); // �����
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
            << infoLog << endl;
    }

    /* ���Ĳ�����дƬԪ��ɫ��(Ҳ��Ƭ����ɫ��) */
    GLuint fragmentShader_1 = glCreateShader(GL_FRAGMENT_SHADER); // ����ƬԪ��ɫ������
    glShaderSource(fragmentShader_1, 1, &fShaderCode, NULL);      // ��������ɫ�������ݴ�����
    glCompileShader(fragmentShader_1);                            // ���붥����ɫ��
    glGetShaderiv(fragmentShader_1, GL_COMPILE_STATUS, &flag);    // ��ȡ����״̬
    if (!flag)
    {
        glGetShaderInfoLog(fragmentShader_1, 512, NULL, infoLog); // �����
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
            << infoLog << endl;
    }

    /* ���岽��������ɫ������ */
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