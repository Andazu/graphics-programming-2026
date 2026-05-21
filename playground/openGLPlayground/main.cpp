#include <ituGL/core/DeviceGL.h>
#include <ituGL/application/Window.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/geometry/ElementBufferObject.h>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <array>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <string>
#include "shaderClass.h"
// #include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <random>
#include <stb_image.h>

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
bool OVERLAPS = false;

void processInput(GLFWwindow* window, float& cameraZ ,float& cameraVelocityZ);

std::filesystem::path resolveTexturePath(const char* filename)
{
    const std::filesystem::path textureFile(filename);
    std::filesystem::path current = std::filesystem::current_path();

    for (int i = 0; i < 6; ++i)
    {
        const auto resourcePath = current / "playground" / "Resource Files" / "Textures" / textureFile;
        if (std::filesystem::exists(resourcePath))
        {
            return resourcePath;
        }

        const auto localResourcePath = current / "Resource Files" / "Textures" / textureFile;
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

    throw std::runtime_error(std::string("Could not find texture file: ") + filename);
}

GLuint loadTexture(const char* filename)
{
    const std::filesystem::path texturePath = resolveTexturePath(filename);

    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(texturePath.string().c_str(), &width, &height, &channels, 0);

    if (!data)
    {
        throw std::runtime_error("Failed to load texture file: " + texturePath.string());
    }

    GLenum format = GL_RGB;
    if (channels == 1)
    {
        format = GL_RED;
    }
    else if (channels == 4)
    {
        format = GL_RGBA;
    }

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return textureID;
}

int main()
{
    try {
        // glfw: initialize and configure
        // ------------------------------
        DeviceGL deviceGL;

        // glfw window creation
        // --------------------
        Window window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL");
        if (!window.IsValid())
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            return -1;
        }

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        deviceGL.SetCurrentWindow(window);
        if (!deviceGL.IsReady())
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        // build and compile our shader program
        // ------------------------------------
        Shader shaderProgram("default.vert", "default.frag");
        Shader backgroundShader("background.vert", "background.frag");

        float backgroundVertices[] = {
            // position    // tex coords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        GLuint backgroundVAO = 0;
        GLuint backgroundVBO = 0;
        glGenVertexArrays(1, &backgroundVAO);
        glGenBuffers(1, &backgroundVBO);

        glBindVertexArray(backgroundVAO);
        glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        GLuint backgroundTexture = loadTexture("iceCave.jpg");

        backgroundShader.Activate();
        glUniform1i(glGetUniformLocation(backgroundShader.ID, "backgroundTexture"), 0);

        // Vertex data for a icicles with generated arrays
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        const int segments = 24;
        const float radius = 0.18f;
        const float maxRadius = 0.20f;
        const float minRadius = 0.05f;
        const float topY = 0.5f;
        float tipY = -0.5f;
        float tipYMax = -0.8f;
        float tipYMin = -0.1f;
        const float roofWidth = 7.0f;
        const float roofDepth = 7.0f;
        const int icicleCount = 6000;
        const float minIcicleDistance = maxRadius * 2.2f;

        std::mt19937 rng(123);
        std::uniform_real_distribution<float> randomX(-roofWidth + radius, roofWidth - radius);
        std::uniform_real_distribution<float> randomZ(-roofDepth + radius, roofDepth - radius);
        std::uniform_real_distribution<float> randomTipY(tipYMax, tipYMin);
        std::uniform_real_distribution<float> randomRadius(minRadius, maxRadius);

        std::vector<glm::vec2> iciclePositions;

        int attempts = 0;
        const int maxAttempts = 6000;

        while (iciclePositions.size() < icicleCount && attempts < maxAttempts)
        {
            attempts++;

            glm::vec2 candidate(randomX(rng), randomZ(rng));
            bool overlaps = false;

            for (const glm::vec2& existing : iciclePositions)
            {
                float distance = glm::length(candidate - existing);

                if (distance < minIcicleDistance && !OVERLAPS)
                {
                    overlaps = true;
                    break;
                }
            }

            if (!overlaps)
            {
                iciclePositions.push_back(candidate);
            }
        }

        for (const glm::vec2& center : iciclePositions)
        {
            const float centerX = center.x;
            const float centerZ = center.y;

            std::uniform_real_distribution<float> randomRadius(minRadius, maxRadius);
            float icicleRadius = randomRadius(rng);
            float tipY = randomTipY(rng);

            std::uniform_real_distribution<float> randomTipOffset(-0.06f, 0.06f);
            float tipOffsetX = randomTipOffset(rng);
            float tipOffsetZ = randomTipOffset(rng);

            std::uniform_real_distribution<float> randomShape(0.75f, 1.25f);

            unsigned int tipIndex = static_cast<unsigned int>(vertices.size() / 6);

            // Source - https://stackoverflow.com/a/686373
            // Posted by John Dibling, modified by community. See post 'Timeline' for change history
            // Retrieved 2026-05-19, License - CC BY-SA 3.0


            // Tip vertex
            // position                         // normal
            vertices.insert(vertices.end(), {
                centerX + tipOffsetX, tipY, centerZ + tipOffsetZ,
                0.0f, -1.0f, 0.0f
            });

            unsigned int ringStartIndex = static_cast<unsigned int>(vertices.size() / 6);

            // Top ring vertices
            for (int i = 0; i < segments; i++)
            {
                float angle = (2.0f * 3.1415926f * i ) / segments;

                float shapeNoise = randomShape(rng);

                float localX = std::cos(angle) * icicleRadius * shapeNoise;
                float localZ = std::sin(angle) * icicleRadius * shapeNoise;

                float x = centerX + localX;
                float z = centerZ + localZ;

                float height = topY - tipY;

                glm::vec3 normal = glm::normalize(glm::vec3(
                    localX,
                    icicleRadius / height,
                    localZ
                ));

                vertices.insert(vertices.end(), {
                   x, topY, z,                  normal.x, normal.y, normal.z
                });
            }

            // Side triangles
            for (int i = 0; i < segments; i++) {
                unsigned int current = ringStartIndex + i;
                unsigned int next = ringStartIndex + ((i+1) % segments);

                indices.insert(indices.end(), {
                    tipIndex, current, next
                });
            }
        }

        // Ceiling
        unsigned int roofStartIndex = static_cast<unsigned int>(vertices.size() / 6);

            // position                         // normal
        vertices.insert(vertices.end(), {
            -roofWidth, topY, -roofDepth,       0.0f, -1.0f, 0.0f,
            roofWidth, topY, -roofDepth,        0.0f, -1.0f, 0.0f,
            roofWidth, topY,  roofDepth,        0.0f, -1.0f, 0.0f,
            -roofWidth, topY,  roofDepth,       0.0f, -1.0f, 0.0f
        });

        indices.insert(indices.end(), {
            roofStartIndex + 0, roofStartIndex + 2, roofStartIndex + 1,
            roofStartIndex + 0, roofStartIndex + 3, roofStartIndex + 2
        });

        // Vertex Array Object (VAO) - Part 2
        // Stores pointers to VAOs and tells opengl where to find them
        VertexArrayObject vao;
        vao.Bind();

        // Vertex buffer object (VBO) - Part 1 of graphics pipeline
        // An array of references to the vertex data
        VertexBufferObject vbo;
        vbo.Bind();
        vbo.AllocateData<float>(std::span(vertices));

        // Element Buffer Object (EBO) - Part 3
        // Stores indicies that describe which vertices to draw - Indexed drawing!
        ElementBufferObject ebo;
        ebo.Bind();
        ebo.AllocateData<unsigned int>(std::span(indices));

        // The position attribute is made of 3 floats: x, y, z
        VertexAttribute position(Data::Type::Float, 3);
        // Use shader location 0, read 3 floats (x,y,z) start at byte offset 0, jump 6 floats to get to the next vertex
        vao.SetAttribute(0, position, 0, 6 * sizeof(float));

        // The normal attribute is made of 3 floats: RGB
        VertexAttribute normalV(Data::Type::Float, 3);
        // Use shader location 1, read 3 floats (r,g,b) start after the first 3 floats, jump 6 floats to get to the next vertex
        vao.SetAttribute(1, normalV, 3 * sizeof(float), 6 * sizeof(float));

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        VertexBufferObject::Unbind();

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        VertexArrayObject::Unbind();

        // Now we can unbind the EBO as well
        ElementBufferObject::Unbind();

        // Uniform reference value
        GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

        // Uniform for texture coordinates
        // GLuint tex0Uni = glGetUniformLocation(shaderProgram.ID, "tex0");
        shaderProgram.Activate();
        // glUniform1i(tex0Uni, 0);

        // uncomment this call to draw in wireframe polygons.
        //glPolygonMode(NULL, NULL);
        bool PolyGonMode = false;

        // Send light values to shaderProgram
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), 0.0f, 2.0f, 10.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "objectColor"), 0.45f, 0.85f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightColor"), 1.0f, 1.0f, 1.0f);

        glUniform1f(glGetUniformLocation(shaderProgram.ID, "ambientStrength"), 0.18f);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "specularStrength"), 0.9f);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "shininess"), 64.0f);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "rimStrength"), 0.45f);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "alpha"), 0.96f);

        // Depth Buffer
        deviceGL.EnableFeature(GL_DEPTH_TEST);

        // If alpha is 0.45, icicle contributes 45% and the background contributes 55%
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float cameraZ = -6.0f;
        float cameraX = 0.0f;
        float cameraY = 0.35f;
        float cameraVelocityZ = 0.0f;

        // Render loop
        while (!window.ShouldClose())
        {

            processInput(window.GetInternalWindow(), cameraZ, cameraVelocityZ);

            deviceGL.Clear(true, Color(0.2f, 0.3f, 0.3f, 1.0f), true, 1.0f);

            glDisable(GL_DEPTH_TEST);
            backgroundShader.Activate();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, backgroundTexture);
            glBindVertexArray(backgroundVAO);
            glDepthMask(GL_FALSE);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glDepthMask(GL_TRUE);
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);

            shaderProgram.Activate();

            glm::vec3 cameraWorldPos(-cameraX, -cameraY, -cameraZ);
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 proj = glm::mat4(1.0f);

            model = glm::rotate(
                model,
                glm::radians((float)glfwGetTime() * -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );


            view = glm::translate(view, glm::vec3(cameraX, cameraY, cameraZ));

            proj = glm::perspective(
                glm::radians(45.0f),
                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                0.1f,
                100.0f
            );

            int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
            int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
            int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");

            glUniform3f(glGetUniformLocation(shaderProgram.ID, "viewPos"), cameraWorldPos.x, cameraWorldPos.y, cameraWorldPos.z);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

            vao.Bind();

            glDrawElements(
                GL_TRIANGLES,
                static_cast<GLsizei>(indices.size()),
                GL_UNSIGNED_INT,
                0
            );

            window.SwapBuffers();
            deviceGL.PollEvents();
        }

        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
        // This is now done in the destructor of DeviceGL
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
        return -1;
    }
}

void processInput(GLFWwindow* window, float& cameraZ ,float& cameraVelocityZ)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Move
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraVelocityZ += 0.0000001f;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraVelocityZ -= 0.0000001f;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraVelocityZ = 0.0f;
    }

    cameraZ += cameraVelocityZ;
}
