#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
   // Convert vertex position from local object space to world space
   FragPos = vec3(model * vec4(aPos, 1.0));

   // Convert normal direction from local object space to world space
   Normal = mat3(transpose(inverse(model))) * aNormal;

   // Convert world-space position to clip space
   gl_Position = proj * view * vec4(FragPos, 1.0);
}