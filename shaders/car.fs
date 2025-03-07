#version 330 core

in vec3 vertexColor;  // Couleur du vertex (entrée du vertex shader)

out vec4 FragColor;   // Couleur du fragment (sortie du fragment shader)

void main()
{
    FragColor = vec4(vertexColor, 1.0f); // Appliquer la couleur du vertex
}
