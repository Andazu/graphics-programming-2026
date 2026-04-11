#include "shaderClass.h"

namespace
{
    std::filesystem::path resolve_shader_path(const char* filename)
    {
        const std::filesystem::path shaderFile(filename);
        std::filesystem::path current = std::filesystem::current_path();

        for (int i = 0; i < 6; ++i)
        {
            const auto directPath = current / shaderFile;
            if (std::filesystem::exists(directPath))
            {
                return directPath;
            }

            const auto resourcePath = current / "playground" / "Resource Files" / "Shaders" / shaderFile;
            if (std::filesystem::exists(resourcePath))
            {
                return resourcePath;
            }

            const auto localResourcePath = current / "Resource Files" / "Shaders" / shaderFile;
            if (std::filesystem::exists(localResourcePath))
            {
                return localResourcePath;
            }

            if (!current.has_parent_path())
            {
                break;
            }

            current = current.parent_path();
        }

        throw std::runtime_error(std::string("Could not find shader file: ") + filename);
    }
}

std::string get_file_contents(const char *filename) {
    const auto path = resolve_shader_path(filename);
    std::ifstream in(path, std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return (contents);
    }
    throw std::runtime_error("Failed to read shader file: " + path.string());
}

Shader::Shader(const char *vertexFile, const char *fragmentFile) {
    // Read files
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);

    // Convert into character arrays
    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

        // Create shader object
    // GL_VERTEX_SHADER is passed as that is the type of shader we want to create
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Attaching shader source code to the shader object and compiling the shader
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        glDeleteShader(vertexShader);
        throw std::runtime_error(std::string("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n") + infoLog);
    }

    // Shader 2: Fragment shader
    // Calculates color output of our pixels
    // For simplicity, now we just do one color, orange

    // Compiling fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        throw std::runtime_error(std::string("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n") + infoLog);
    }

    // Linking our 2 shaders to a shader program
    /* Shader program:
     * - Final linked version of multiple shaders combined
     * - After linking shaders to the shader program, we activate the shader program when rendering objects
     * - Activated shader program's shaders are used during render calls
    */
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    // Every shader and rendering call after glUseProgram will now use this program object
    // check for linking errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(ID);
        throw std::runtime_error(std::string("ERROR::SHADER::PROGRAM::LINKING_FAILED\n") + infoLog);
    }
    // Deleting the shader objects after linking to the program object since we dont need them anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Activate() {
    glUseProgram(ID);
}

void Shader::Deactivate() {
    glDeleteProgram(ID);
}
