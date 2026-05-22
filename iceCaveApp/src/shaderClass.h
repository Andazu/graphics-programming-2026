//
// Created by ander on 4/10/2026.
//

#ifndef ITU_GRAPHICS_PROGRAMMING_SHADERCLASS_H
#define ITU_GRAPHICS_PROGRAMMING_SHADERCLASS_H

#endif //ITU_GRAPHICS_PROGRAMMING_SHADERCLASS_H

#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<filesystem>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<stdexcept>

// Function that reads the shader text files
std::string get_file_contents(const char* filename);

class Shader
{
    public:
        // refernce ID
        GLuint ID;
        // Constructor
        Shader(const char* vertexFile, const char* fragmentFile);

        void Activate();
        void Deactivate();
};

#endif
