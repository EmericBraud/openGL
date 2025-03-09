#version 330 core

in vec3 fragColor;  // Couleur du vertex (entrée du vertex shader)
in vec3 fragNorm;
in vec4 FragPosLightSpace;
in vec3 fragPos;

uniform sampler2D shadowMap;
uniform vec3 lightDir;
uniform vec3 lightPos;
uniform samplerCube skybox;
uniform vec3 viewPos;

out vec4 FragColor;   // Couleur du fragment (sortie du fragment shader)

void main() {
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
    float shadow = (currentDepth > closestDepth + 0.0001) ? 0.5 : 1.0; // Atténuation si ombre
    float spotLight = max(0.0, dot(normalize(lightPos - fragPos), lightDirection));

    // Appliquer l'ombre
    lighting *= spotLight * shadow * fragColor;

    //Reflection of skybox
    vec3 viewDir = normalize(fragPos - viewPos);
    vec3 randomOffset = vec3(fract(sin(dot(fragPos.xy, vec2(12.9898, 78.233))) * 43758.5453), fract(sin(dot(fragPos.yz, vec2(12.9898, 78.233))) * 43758.5453), fract(sin(dot(fragPos.zx, vec2(12.9898, 78.233))) * 43758.5453));
    randomOffset = (randomOffset - 0.5) * 0.1; // Ajuster l'intensité du flou

// Modifier la normale avec un petit bruit aléatoire

// Calcul de la réflexion avec la normale perturbée
    vec4 reflectColor = vec4(0.0);
    int numSamples = 5; // Plus de samples = réflexion plus floue

    for(int i = 0; i < numSamples; i++) {
    // Générer une petite perturbation aléatoire
        vec3 randomOffset = vec3(fract(sin(dot(fragPos.xy + i, vec2(12.9898, 78.233))) * 43758.5453), fract(sin(dot(fragPos.yz + i, vec2(12.9898, 78.233))) * 43758.5453), fract(sin(dot(fragPos.zx + i, vec2(12.9898, 78.233))) * 43758.5453));
        randomOffset = (randomOffset - 0.5) * 0.05;

        vec3 perturbedNorm = normalize(fragNorm + randomOffset);
        reflectColor += texture(skybox, reflect(viewDir, perturbedNorm));
    }

// Faire la moyenne des échantillons
    reflectColor /= float(numSamples);


    //Reflection of light
    vec3 reflectedLight = normalize(reflect(lightDirection, fragNorm));
    float reflectionStrength = pow(max(dot(viewDir, reflectedLight), 0.0), 40.0);

    FragColor = vec4(lighting*0.8 + reflectionStrength * lightColor /(1+distance(lightPos, fragPos)*0.01), 1.0) + 0.3 * reflectColor;
}
