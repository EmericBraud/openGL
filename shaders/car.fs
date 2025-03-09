#version 330 core

in vec3 fragColor;  // Couleur du vertex (entrée du vertex shader)
in vec3 fragNorm;
in vec4 FragPosLightSpace;
in vec3 fragPos;

uniform sampler2D shadowMap;
uniform vec3 lightDir;
uniform vec3 lightPos;
uniform samplerCube skybox;

out vec4 FragColor;   // Couleur du fragment (sortie du fragment shader)

void main()
{
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(fragNorm, lightDirection), 0.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lighting = diff * lightColor;

    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Convertir de [-1,1] à [0,1]

 // Récupérer la profondeur du shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    // Comparer pour voir si l'on est dans l'ombre
    float shadow = (currentDepth > closestDepth +0.0001) ? 0.5 : 1.0; // Atténuation si ombre
    float resu = max(0.0 , dot(normalize(lightPos - fragPos), lightDirection));

    // Appliquer l'ombre
    lighting *= resu * shadow * fragColor;

    FragColor = vec4(lighting, 1.0);
}
