#version 330 core

layout (location = 0) in vec3 aPosition;  // Position du sommet
layout (location = 1) in vec3 aColor;     // Couleur du sommet
layout (location = 2) in vec3 aNormal;    // Normale du sommet

out vec3 fragColor;  
out vec4 FragPosLightSpace;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    fragColor = aColor;

    // Position du fragment dans l'espace monde
    FragPos = vec3(model * vec4(aPosition, 1.0));
    
    // Transformer la position vers l'espace lumière
    FragPosLightSpace = lightSpaceMatrix * model * vec4(FragPos, 1.0);

    // Transformer la normale correctement
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Calculer la position finale
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
