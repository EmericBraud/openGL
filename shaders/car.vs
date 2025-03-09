#version 330 core

layout (location = 0) in vec3 aPos;  // Position du vertex
layout (location = 1) in vec3 aColor; // Couleur du vertex
layout (location = 2) in vec3 aNorm;

out vec3 fragColor; // Variable de sortie pour la couleur
out vec3 fragNorm;
out vec4 FragPosLightSpace;
out vec3 fragPos;

uniform mat4 model;     // Matrice modèle
uniform mat4 view;      // Matrice de vue
uniform mat4 projection; // Matrice de projection
uniform mat4 lightSpaceMatrix;

void main()
{
    // Appliquer les transformations
    fragNorm = normalize(mat3(transpose(inverse(model))) * aNorm);
    fragPos = vec3(model * vec4(aPos, 1.0));
    FragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0f);
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    fragColor = aColor; // Passer la couleur au fragment shader
}
