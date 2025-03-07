#version 330 core

layout(location = 0) in vec3 aPos;  // Position du sommet

uniform mat4 lightSpaceMatrix;  // Matrice de projection et de vue de la lumière

void main() {
    // Projeter la position dans l'espace de la lumière
    gl_Position = lightSpaceMatrix * vec4(aPos, 1.0f);
}
