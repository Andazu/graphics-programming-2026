#version 330 core
// We declare the shaders output color
// vec4 means 4 floats: RGB and Alpha
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

out vec4 FragColor;

void main()
{
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPos - FragPos);

   // How much the surface faces the light
   float diffuseAmount = max(dot(norm, lightDir), 0.0);

   vec3 ambient = 0.2 * lightColor;
   vec3 diffuse = diffuseAmount * lightColor;

   vec3 result = (ambient + diffuse) * objectColor;
   FragColor = vec4(result, 1.0);
}
