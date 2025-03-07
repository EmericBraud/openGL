#version 330 core

layout (location = 0) in vec3 aPos;  // Position du vertex
layout (location = 1) in vec3 aColor; // Couleur du vertex

out vec3 vertexColor; // Variable de sortie pour la couleur

uniform mat4 model;     // Matrice modèle
uniform mat4 view;      // Matrice de vue
uniform mat4 projection; // Matrice de projection

void main()
{
    // Appliquer les transformations
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    vertexColor = aColor; // Passer la couleur au fragment shader
}
