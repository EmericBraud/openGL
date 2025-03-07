#version 330 core

layout (location = 0) in vec3 aPosition;  // Position du sommet
layout (location = 1) in vec3 aColor;     // Couleur du sommet

out vec3 fragColor;  // Passer la couleur au fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Passer la couleur du sommet au fragment shader
    fragColor = aColor;

    // Appliquer la matrice de transformation (modèle, vue, projection)
    gl_Position = projection * view * model * vec4(aPosition , 1.0);
}
