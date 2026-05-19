#include <ituGL/core/DeviceGL.h>
#include <ituGL/application/Window.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/geometry/ElementBufferObject.h>
#include <iostream>
#include <exception>
#include <array>
#include <cmath>
#include <filesystem>
#include "shaderClass.h"
// #include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

// settings
const unsigned int SCR_WIDTH = 512;
const unsigned int SCR_HEIGHT = 512;

void processInput(GLFWwindow* window, float& cameraZ ,float& cameraVelocityZ);

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

        // Vertex data for a icicles with generated arrays
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        const int segments = 24;
        const float radius = 0.35f;
        const float topY = 0.5f;
        const float tipY = -0.8f;
        const float tipX = 0.0f;
        const float tipZ = 0.0f;
        glm::vec3 normal = glm::normalize(glm::vec3(tipX, tipY, tipZ));

        // Tip vertex
            // position             // normal
        vertices.insert(vertices.end(), {
            tipX, tipY, tipZ,       normal.x, normal.y, normal.z
        });

        const unsigned int tipIndex = 0;

        // Top ring vertices
        for (int i = 0; i < segments; i++)
        {
            float angle = (2.0f * 3.1415926f * i ) / segments;

            float x = std::cos(angle) * radius;
            float z = std::sin(angle) * radius;
            normal = glm::normalize(glm::vec3(x, 0.25f, z));

            vertices.insert(vertices.end(), {
               x, topY, z,          normal.x, normal.y, normal.z
            });
        }

        // Side triangles
        for (int i = 0; i < segments; i++) {
            unsigned int current = 1 + i;
            unsigned int next = 1 + ((i+1) % segments);

            indices.insert(indices.end(), {
                tipIndex, current, next
            });
        }

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
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), 2.0f, 2.0f, 2.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "objectColor"), 0.55f, 0.85f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightColor"), 1.0f, 1.0f, 1.0f);

        // Depth Buffer
        deviceGL.EnableFeature(GL_DEPTH_TEST);

        float cameraZ = -3.0f;
        float cameraX = 0.0f;
        float cameraY = 0.0f;
        float cameraVelocityZ = 0.0f;

        // Render loop
        while (!window.ShouldClose())
        {

            processInput(window.GetInternalWindow(), cameraZ, cameraVelocityZ);

            deviceGL.Clear(true, Color(0.2f, 0.3f, 0.3f, 1.0f), true, 1.0f);

            shaderProgram.Activate();

            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 proj = glm::mat4(1.0f);


            model = glm::rotate(
                model,
                glm::radians((float)glfwGetTime() * 50.0f),
                glm::vec3(1.0f, 1.0f, 0.0f)
            );


            view = glm::translate(view, glm::vec3(cameraX, cameraY, cameraZ + cameraVelocityZ));

            proj = glm::perspective(
                glm::radians(45.0f),
                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                0.1f,
                100.0f
            );

            int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
            int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
            int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");

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