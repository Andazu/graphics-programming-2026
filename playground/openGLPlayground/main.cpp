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
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// settings
const unsigned int SCR_WIDTH = 512;
const unsigned int SCR_HEIGHT = 512;

void processInput(GLFWwindow* window);

namespace
{
    std::filesystem::path get_texture_path()
    {
        const auto sourceDir = std::filesystem::path(__FILE__).parent_path();
        return (sourceDir / ".." / "Resource Files" / "Textures" / "pop_cat.png").lexically_normal();
    }
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

        // Vertex data for a cube. Each face has its own vertices so every face can use
        // the full 0..1 texture coordinate range.
        float vertices[] = {
            // Coordinates          // Colors           // Texture coordinates
            // Front
            -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

            // Back
             0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

            // Left
            -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

            // Right
             0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

            // Top
            -0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

            // Bottom
            -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        };
        unsigned int indices[] = {  // note that we start from 0!
             0,  1,  2,  2,  3,  0, // Front
             4,  5,  6,  6,  7,  4, // Back
             8,  9, 10, 10, 11,  8, // Left
            12, 13, 14, 14, 15, 12, // Right
            16, 17, 18, 18, 19, 16, // Top
            20, 21, 22, 22, 23, 20, // Bottom
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
        // Use shader location 0, read 3 floats (x,y,z) start at byte offset 0, jump 8 floats to get to the next vertex
        vao.SetAttribute(0, position, 0, 8 * sizeof(float));

        // The color attribute is made of 3 floats: RGB
        VertexAttribute color(Data::Type::Float, 3);
        // Use shader location 1, read 3 floats (r,g,b) start after the first 3 floats, jump 8 floats to get to the next vertex
        vao.SetAttribute(1, color, 3 * sizeof(float), 8 * sizeof(float));

        // The texture attribute is made of two floats
        VertexAttribute texCoord(Data::Type::Float, 2);
        // Use shader location 2, read 2 floats (s,t) start after the first 6 floats, jump 8 floats to get to the next vertex
        vao.SetAttribute(2, texCoord, 6 * sizeof(float), 8 * sizeof(float));

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        VertexBufferObject::Unbind();

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        VertexArrayObject::Unbind();

        // Now we can unbind the EBO as well
        ElementBufferObject::Unbind();

        // Uniform reference value
        GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

        // Transformations - translating a vector of (1,0,0) by (1,1,0)
        // Creating vec
        glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
        glm::mat4 trans = glm::mat4(1.0f);
        // Creating transformation matrix
        trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
        // Multiply vec by transformation matrix ( (1+1,0+1,0+0) = (2,1,0) ) = Works!
        vec = trans * vec;
        std::cout << vec.x << vec.y << vec.z << std::endl;

        // Rotation and Scale
        trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
        // Pass transformation matrix to shader
        // Get id for uniform, we pass it later after shaderProgram is activated :)
        const GLuint transformLoc = glGetUniformLocation(shaderProgram.ID, "transform");

        // Texture
        int widthImg = 0, heightImg = 0, numColCh = 0;
        stbi_set_flip_vertically_on_load(true);
        // Loading texture
        const auto texturePath = get_texture_path();
        unsigned char* bytes = stbi_load(texturePath.string().c_str(), &widthImg, &heightImg, &numColCh, 4);
        if (!bytes)
        {
            throw std::runtime_error(std::string("Failed to load texture: ") + stbi_failure_reason());
        }

        // Reference variable
        GLuint texture;
        // Generate texture object, one texture and a reference variable
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Apply settings
        // - Image processing GL_LINEAR
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // - Texture repeating
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Generate image
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        // Generate mipmaps, which are just the same image but at smaller sizes for responsiveness
        glGenerateMipmap(GL_TEXTURE_2D);

        // Texture cleanup
        stbi_image_free(bytes);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Uniform for texture coordinates
        GLuint tex0Uni = glGetUniformLocation(shaderProgram.ID, "tex0");
        shaderProgram.Activate();
        glUniform1i(tex0Uni, 0);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        // uncomment this call to draw in wireframe polygons.
        //glPolygonMode(NULL, NULL);
        bool PolyGonMode = false;

        // Depth Buffer
        deviceGL.EnableFeature(GL_DEPTH_TEST);

        // Render loop
        while (!window.ShouldClose())
        {
            // input
            // -----
            processInput(window.GetInternalWindow());

            // render
            // ------
            deviceGL.Clear(true, Color(0.2f, 0.3f, 0.3f, 1.0f), true, 1.0f);

            // Update transformation matrix
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
            trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

            // Drawing an object - Telling OpenGL which shader program we want to use
            shaderProgram.Activate();

            // GOING 3D
            // Initialize matrices
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 proj = glm::mat4(1.0f);
            // Rotate our cube
            model = glm::rotate(model, glm::radians((float)glfwGetTime() * 50), glm::vec3(1.0f, 1.0f, 0.0f));
            // Move the world away from our camera
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            // Set our view in radians, aspect ration to our screen dimension, and the culling distance as the last two parameters
            proj = glm::perspective(glm::radians(45.0f), (float)(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f );

            // Get location of Uniform
            int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
            // Assign value to uniform
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // Repeat for view and proj
            int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));


            // Assigning value to uniform
            glUniform1f(uniID, 0.5f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
            glBindTexture(GL_TEXTURE_2D, texture);

            vao.Bind(); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            //glDrawArrays(GL_TRIANGLES, 0, 3);
            glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

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
