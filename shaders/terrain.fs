#version 330 core

out vec4 FragColor;

in vec3 fragColor;
in vec4 FragPosLightSpace;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D shadowMap;
uniform vec3 lightDir;
uniform vec3 lightPos;

void main()
{
    // Normaliser la normale et la direction de la lumière
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);

    // Éclairage de base (Lambert)
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lighting = diff * lightColor;

    // Convertir FragPosLightSpace en coordonnées de texture
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Convertir de [-1,1] à [0,1]

    // Récupérer la profondeur du shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    // Comparer pour voir si l'on est dans l'ombre
    float shadow = (currentDepth > closestDepth +0.0001) ? 0.5 : 1.0; // Atténuation si ombre
    float resu = max(0.0 , dot(normalize(lightPos - FragPos), lightDirection));

    // Appliquer l'ombre
    lighting *= resu * shadow;

    FragColor = vec4(lighting, 1.0);
}
