// GLSL version
#version 330 core

// We declare an input variable named aPos
// Vec3 means 3 floats: x,y,z
// Location = 0 means this shader expects the position data to come from vertex attribute slot 0 (vao.SetAttribute(0, position, 0))
// tldr, CPU sends vertex position to slot 0, this shader reads them as "aPos"
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 color;

uniform float scale;

// every shader needs a main function
// runs once per vertex
void main()
{
   // Tells OpenGL where this vertex should appear on screen
   // gl_Position needs to be vec4, so the 3D position is turned into 4 values with w = 1
   // We dont transform anything here, we just pass the position straight through
   gl_Position = vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);
   color = aColor;
}
