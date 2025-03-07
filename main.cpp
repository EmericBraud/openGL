#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "src/map.hpp"
#include "src/player.hpp"
#include "src/light.hpp"


// Définir la taille de la carte (à ajuster en fonction de vos besoins)
#define MAP_SIZE 256 // Exemple de taille de la carte

std::string loadShaderSource(const char *filePath) {
    std::ifstream shaderFile;
    std::stringstream shaderStream;

    // Ouvrir le fichier
    shaderFile.open(filePath);
    if (!shaderFile) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier shader : "
                  << filePath << std::endl;
        return "";
    }

    // Lire le fichier dans un flux
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return shaderStream.str();
}

unsigned int compileShader(const char *filePath, GLenum shaderType) {
    std::string shaderSource = loadShaderSource(filePath);
    if (shaderSource.empty()) {
        std::cerr << "Erreur : Shader vide" << std::endl;
        return 0;
    }

    const char *shaderCode = shaderSource.c_str();
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);

    // Vérification des erreurs
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Erreur de compilation du shader :\n"
                  << infoLog << std::endl;
    }

    return shader;
}

unsigned int createShaderProgram(const char *vertexPath,
                                 const char *fragmentPath) {
    unsigned int vertexShader = compileShader(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragmentShader =
        compileShader(fragmentPath, GL_FRAGMENT_SHADER);

    if (vertexShader == 0 || fragmentShader == 0) {
        return 0;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Vérification des erreurs de linkage
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Erreur de linkage du shader program :\n"
                  << infoLog << std::endl;
    }

    // Nettoyage
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

float deltaTime = 0.0f; // Temps écoulé entre chaque frame
float lastFrame = 0.0f; // Temps de la dernière frame
float lastX = 400.0f, lastY = 300.0f; // Dernière position de la souris
bool firstMouse = true; // Détecter le premier mouvement de la souris

// Fonction pour gérer les entrées clavier
void processInput(GLFWwindow *window, Player &player) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    float cameraSpeed = 2.5f * deltaTime; // Vitesse de mouvement de la caméra

    // Avancer avec 'Z' (ou 'W' si on préfère utiliser cette touche sur un clavier AZERTY)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        player.move(player.getDirection() * cameraSpeed); // Se déplacer dans la direction de la caméra
    }

    // Reculer avec 'S'
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        player.move(-player.getDirection() * cameraSpeed); // Se déplacer en arrière par rapport à la caméra
    }

    // Tourner à gauche avec 'Q'
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        player.rotate(-cameraSpeed); // Rotation vers la gauche (sens inverse des aiguilles d'une montre)
    }

    // Tourner à droite avec 'D'
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        player.rotate(cameraSpeed); // Rotation vers la droite (sens des aiguilles d'une montre)
    }

    // Quitter avec 'Echap'
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}



int main() {
    // Initialisation de GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configuration de GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Création de la fenêtre
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Associer le contexte OpenGL à la fenêtre
    glfwMakeContextCurrent(window);

    // Initialisation de GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);

    
    std::cout << "GLAD initialized successfully!" << std::endl;
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glEnable(GL_DEPTH_TEST);  // Activer le test de profondeur pour afficher correctement les objets en 3D.
    

    // Initialiser le joueur
    PlayerCameraConfig cameraConfig = {45.0f, 800.0f / 600.0f, 0.1f, 100.0f};
    Player player(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f), cameraConfig);

    // Initialisation de la carte (terrain)
    Map map;

    // Nouveau tableau de sommets pour le terrain
    // Nouveau tableau de sommets avec la couleur associée
    std::vector<float> terrainVertices;
    std::vector<unsigned int> terrainIndices;

    for (int x = 0; x < MAP_SIZE - 1; x++) {
        for (int z = 0; z < MAP_SIZE - 1; z++) {
            // Position du terrain
            float xPos = static_cast<float>(x);
            float zPos = static_cast<float>(z);
            const float yPos = 0.0f;

            // Déterminer le type de terrain à cette position
            TerrainType terrainType = map.getTerrainAt(x, z);
            glm::vec3 color = map.getTerrainColor(terrainType);

            // Ajouter les sommets pour un carré (deux triangles)
            terrainVertices.push_back(xPos);
            terrainVertices.push_back(yPos);
            terrainVertices.push_back(zPos);
            terrainVertices.push_back(color.r);
            terrainVertices.push_back(color.g);
            terrainVertices.push_back(color.b);

            terrainVertices.push_back(xPos + 1.0f);
            terrainVertices.push_back(yPos);
            terrainVertices.push_back(zPos);
            terrainVertices.push_back(color.r);
            terrainVertices.push_back(color.g);
            terrainVertices.push_back(color.b);

            terrainVertices.push_back(xPos + 1.0f);
            terrainVertices.push_back(yPos);
            terrainVertices.push_back(zPos + 1.0f);
            terrainVertices.push_back(color.r);
            terrainVertices.push_back(color.g);
            terrainVertices.push_back(color.b);

            terrainVertices.push_back(xPos);
            terrainVertices.push_back(yPos);
            terrainVertices.push_back(zPos + 1.0f);
            terrainVertices.push_back(color.r);
            terrainVertices.push_back(color.g);
            terrainVertices.push_back(color.b);

            // Ajouter les indices pour les triangles
            terrainIndices.push_back(x * MAP_SIZE + z);
            terrainIndices.push_back((x + 1) * MAP_SIZE + z);
            terrainIndices.push_back((x + 1) * MAP_SIZE + (z + 1));
            terrainIndices.push_back(x * MAP_SIZE + z);
            terrainIndices.push_back((x + 1) * MAP_SIZE + (z + 1));
            terrainIndices.push_back(x * MAP_SIZE + (z + 1));
        }
    }

    // Créer le VAO pour le terrain
    unsigned int terrainVAO, terrainVBO, terrainEBO;
    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glGenBuffers(1, &terrainEBO);

    glBindVertexArray(terrainVAO);

    // VBO : Envoi des sommets
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(float),
                 terrainVertices.data(), GL_STATIC_DRAW);

    // EBO : Envoi des indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 terrainIndices.size() * sizeof(unsigned int),
                 terrainIndices.data(), GL_STATIC_DRAW);

    // Attributs du sommet (Position)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Attributs du sommet (Couleur)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Créer le programme shader
    unsigned int terrainShaderProgram =
        createShaderProgram("shaders/terrain.vs", "shaders/terrain.fs");

    // Obtenir les locations des uniformes
    int viewLoc = glGetUniformLocation(terrainShaderProgram, "view");
    int projectionLoc =
        glGetUniformLocation(terrainShaderProgram, "projection");
    // Obtenir la location de l'uniforme 'model'
    int modelLoc = glGetUniformLocation(terrainShaderProgram, "model");
    // Définir la matrice de modèle (ici une matrice identité, mais tu peux y appliquer des transformations)
    glm::mat4 model = glm::mat4(1.0f); // Matrice identité

    glUseProgram(terrainShaderProgram);
    // Envoi de la matrice 'model' au shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUseProgram(0);

    unsigned int carShaderProgram =
        createShaderProgram("shaders/car.vs", "shaders/car.fs");

    // Boucle de rendu
    while (!glfwWindowShouldClose(window)) {
        processInput(window, player);

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Mettre à jour la caméra
        player.updateCamera();

        // Afficher le terrain
        glUseProgram(terrainShaderProgram);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE,
                           glm::value_ptr(player.getViewMatrix()));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
                           glm::value_ptr(player.getProjectionMatrix()));

        glBindVertexArray(terrainVAO);
        glDrawElements(GL_TRIANGLES, terrainIndices.size(), GL_UNSIGNED_INT, 0);
        glUseProgram(0);

        player.render(carShaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Nettoyage
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
