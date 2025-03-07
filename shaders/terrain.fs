#version 330 core

out vec4 FragColor;

in vec3 fragColor;

void main()
{
    // Définir la couleur finale du fragment
    FragColor = vec4(fragColor, 1.0);
}
