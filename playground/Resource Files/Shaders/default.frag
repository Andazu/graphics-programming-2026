#version 330 core
// We declare the shaders output color
// vec4 means 4 floats: RGB and Alpha
out vec4 FragColor;
in vec4 color;

void main()
{
   // Sets every fragment to the same color
   // "Color the whole shape orange"
   FragColor = color;
}
