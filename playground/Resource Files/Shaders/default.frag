#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess = 64.0f;
uniform float rimStrength;
uniform float alpha;

out vec4 FragColor;

void main()
{
   vec3 norm = normalize(Normal);

   // Direction from fragment to light
   vec3 lightDir = normalize(lightPos - FragPos);

   // Direction from fragment to camera
   vec3 viewDir = normalize(viewPos - FragPos);

   // AMBIENT
   vec3 ambient = ambientStrength * lightColor;

   // DIFFUSE
   float diffuseAmount = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diffuseAmount * lightColor;

   // SPECULAR - Blinn-Phong
   vec3 halfwayDir = normalize(lightDir + viewDir);
   float specularAmount = pow(max(dot(norm, halfwayDir), 0.0), shininess);
   vec3 specular = specularStrength * specularAmount * lightColor;

   // RIM / FRESNEL-ISH EDGE GLOW
   float rimAmount = 1.0 - max(dot(norm, viewDir), 0.0);
   rimAmount = pow(rimAmount, 2.0);
   vec3 rim = rimAmount * rimStrength * vec3(0.5, 0.85, 1.0);

   vec3 result = (ambient + diffuse) * objectColor + specular + rim;

   FragColor = vec4(result, alpha);
}