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
#include "shaderClass.h"

/*
    LEARNING OBJECTIVES
    Setup an OpenGL application from scratch. - Done
    Create a GLFW window and an OpenGL context. - Done
    Setup the buffers necessary for rendering with OpenGL (Vertex Buffer Object (VBO), Vertex Array Object (VAO) and Element Buffer Object (EBO)). - Done
    Draw simple shapes. - Done
    Develop a basic understanding of the Normalized Device Coordinates (NDC) and of the window coordinates. - Done!
*/

// settings
const unsigned int SCR_WIDTH = 512;
const unsigned int SCR_HEIGHT = 512;

void processInput(GLFWwindow* window);

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

        // Vertex data for a triangle in a float array
        float vertices[] = {
        // Coordinates          // Colors
        0.5f,  0.5f, 0.0f,      0.0f, 0.8f, 0.3f, 0.02f,  // top right
        0.5f, -0.5f, 0.0f,      0.9f, 0.2f, 0.6f, 1.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,      0.1f, 0.7f, 0.9f, 1.0f,   // bottom left
       -0.5f,  0.5f, 0.0f,      0.8f, 0.4f, 0.1f, 1.0f,   // top left
        0.0f,  1.0f, 0.0f,      0.5f, 0.1f, 0.8f, 1.0f,   // top middle
        1.0f,  0.0f, 0.0f,      0.2f, 1.0f, 0.4f, 1.0f,   // right middle
        0.0f, -1.0f, 0.0f,      1.0f, 0.9f, 0.2f, 1.0f,   // bottom middle
       -1.0f,  0.0f, 0.0f,      0.3f, 0.5f, 1.0f, 1.0f,   // left middle
   };
        unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
        0, 3, 4, // third triangle
        0, 1, 5, // fourth triangle
        1, 6, 2, // fifth triangle
        2, 7, 3, // sixth triangle
    };

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
        // Use shader location 0, read 3 floats (x,y,z) start at byte offset 0, jump 7 floats to get to the next vertex
        vao.SetAttribute(0, position, 0, 7 * sizeof(float));

        // The color attribute is made of 4 floats: RGB and Alpha
        VertexAttribute color(Data::Type::Float, 4);
        // Use shader location 1, read 4 floats (r,g,b,a) start after the first 3 floats, jump 7 floats to get to the next vertex
        vao.SetAttribute(1, color, 3 * sizeof(float), 7 * sizeof(float));

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        VertexBufferObject::Unbind();

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        VertexArrayObject::Unbind();

        // Now we can unbind the EBO as well
        ElementBufferObject::Unbind();

        // Uniform reference value
        GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

        // uncomment this call to draw in wireframe polygons.
        //glPolygonMode(NULL, NULL);
        bool PolyGonMode = false;

        // Render loop
        while (!window.ShouldClose())
        {
            // input
            // -----
            processInput(window.GetInternalWindow());

            // render
            // ------
            deviceGL.Clear(Color(0.2f, 0.3f, 0.3f, 1.0f));

            // drawing an object
            shaderProgram.Activate();
            // Give value to uniform
            glUniform1f(uniID, 0.5f);

            vao.Bind(); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            //glDrawArrays(GL_TRIANGLES, 0, 3);
            glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
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

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}
