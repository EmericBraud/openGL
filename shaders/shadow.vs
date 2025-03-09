#version 330 core

layout (location = 0) in vec3 aPos; // Position du sommet

uniform mat4 model;     // Matrice modèle
uniform mat4 lightSpaceMatrix; // Matrice de transformation de la lumière

void main()
{
    // Transformer la position du sommet dans l'espace de la lumière
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
