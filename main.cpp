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

#include "src/light.hpp"
#include "src/map.hpp"
#include "src/player.hpp"
#include "src/skybox.hpp"


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

float deltaTime = 0.0f;               // Temps écoulé entre chaque frame
float lastFrame = 0.0f;               // Temps de la dernière frame
float lastX = 400.0f, lastY = 300.0f; // Dernière position de la souris
bool firstMouse = true; // Détecter le premier mouvement de la souris

// Fonction pour gérer les entrées clavier
void processInput(GLFWwindow *window, Player &player) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    player.lookBehind(false);

    float cameraSpeed = 2.5f * deltaTime; // Vitesse de mouvement de la caméra

    // Avancer avec 'Z' (ou 'W' si on préfère utiliser cette touche sur un
    // clavier AZERTY)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        player.move(player.getDirection() *
                    cameraSpeed); // Se déplacer dans la direction de la caméra
    }

    // Reculer avec 'S'
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        player.move(
            -player.getDirection() *
            cameraSpeed); // Se déplacer en arrière par rapport à la caméra
        player.lookBehind(true);
    }

    // Tourner à gauche avec 'Q'
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        player.rotate(-cameraSpeed); // Rotation vers la gauche (sens inverse
                                     // des aiguilles d'une montre)
    }

    // Tourner à droite avec 'D'
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        player.rotate(cameraSpeed); // Rotation vers la droite (sens des
                                    // aiguilles d'une montre)
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
    glEnable(GL_DEPTH_TEST); // Activer le test de profondeur pour afficher
                             // correctement les objets en 3D.

    // Initialiser le joueur
    PlayerCameraConfig cameraConfig = {45.0f, 800.0f / 600.0f, 0.1f, 100.0f};
    Player player(glm::vec3(10.0f, 0.0f, 10.0f), glm::vec3(0.0f, 2.0f, 0.0f),
                  cameraConfig);

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

            // Normale pointant vers le haut
            glm::vec3 normal(0.0f, 1.0f, 0.0f);

            // Ajouter les sommets pour un carré (deux triangles)
            auto addVertex = [&](float x, float y, float z) {
                terrainVertices.push_back(x);
                terrainVertices.push_back(y);
                terrainVertices.push_back(z);
                terrainVertices.push_back(color.r);
                terrainVertices.push_back(color.g);
                terrainVertices.push_back(color.b);
                terrainVertices.push_back(normal.x);
                terrainVertices.push_back(normal.y);
                terrainVertices.push_back(normal.z);
            };

            addVertex(xPos, yPos, zPos);
            addVertex(xPos + 1.0f, yPos, zPos);
            addVertex(xPos + 1.0f, yPos, zPos + 1.0f);
            addVertex(xPos, yPos, zPos + 1.0f);

            // Ajouter les indices pour les triangles
            int topLeft = x * MAP_SIZE + z;
            int topRight = (x + 1) * MAP_SIZE + z;
            int bottomRight = (x + 1) * MAP_SIZE + (z + 1);
            int bottomLeft = x * MAP_SIZE + (z + 1);

            terrainIndices.push_back(topLeft);
            terrainIndices.push_back(topRight);
            terrainIndices.push_back(bottomRight);
            terrainIndices.push_back(topLeft);
            terrainIndices.push_back(bottomRight);
            terrainIndices.push_back(bottomLeft);
        }
    }

    // =========================
    // Ajout d'un cube pour le test
    // =========================
    float cubeX = 12.0f;
    float cubeY = 1.0f;
    float cubeZ = 12.0f;
    float cubeSize = 1.0f;
    glm::vec3 cubeColor(1.0f, 0.0f, 0.0f); // Rouge
    std::vector<float> cubeVertices;

    // Normales de chaque face
    glm::vec3 normalUp(0.0f, 1.0f, 0.0f);
    glm::vec3 normalDown(0.0f, -1.0f, 0.0f);
    glm::vec3 normalFront(0.0f, 0.0f, 1.0f);
    glm::vec3 normalBack(0.0f, 0.0f, -1.0f);
    glm::vec3 normalLeft(-1.0f, 0.0f, 0.0f);
    glm::vec3 normalRight(1.0f, 0.0f, 0.0f);

    auto addCubeVertex = [&](float x, float y, float z, glm::vec3 normal) {
        cubeVertices.push_back(x);
        cubeVertices.push_back(y);
        cubeVertices.push_back(z);
        cubeVertices.push_back(cubeColor.r);
        cubeVertices.push_back(cubeColor.g);
        cubeVertices.push_back(cubeColor.b);
        cubeVertices.push_back(normal.x);
        cubeVertices.push_back(normal.y);
        cubeVertices.push_back(normal.z);
    };

    // Sommets du cube
    addCubeVertex(cubeX, cubeY, cubeZ, normalBack);
    addCubeVertex(cubeX + cubeSize, cubeY, cubeZ, normalBack);
    addCubeVertex(cubeX + cubeSize, cubeY + cubeSize, cubeZ, normalBack);
    addCubeVertex(cubeX, cubeY + cubeSize, cubeZ, normalBack);

    addCubeVertex(cubeX, cubeY, cubeZ + cubeSize, normalFront);
    addCubeVertex(cubeX + cubeSize, cubeY, cubeZ + cubeSize, normalFront);
    addCubeVertex(cubeX + cubeSize, cubeY + cubeSize, cubeZ + cubeSize,
                  normalFront);
    addCubeVertex(cubeX, cubeY + cubeSize, cubeZ + cubeSize, normalFront);

    addCubeVertex(cubeX, cubeY, cubeZ, normalLeft);
    addCubeVertex(cubeX, cubeY, cubeZ + cubeSize, normalLeft);
    addCubeVertex(cubeX, cubeY + cubeSize, cubeZ + cubeSize, normalLeft);
    addCubeVertex(cubeX, cubeY + cubeSize, cubeZ, normalLeft);

    addCubeVertex(cubeX + cubeSize, cubeY, cubeZ, normalRight);
    addCubeVertex(cubeX + cubeSize, cubeY, cubeZ + cubeSize, normalRight);
    addCubeVertex(cubeX + cubeSize, cubeY + cubeSize, cubeZ + cubeSize,
                  normalRight);
    addCubeVertex(cubeX + cubeSize, cubeY + cubeSize, cubeZ, normalRight);

    addCubeVertex(cubeX, cubeY + cubeSize, cubeZ, normalUp);
    addCubeVertex(cubeX + cubeSize, cubeY + cubeSize, cubeZ, normalUp);
    addCubeVertex(cubeX + cubeSize, cubeY + cubeSize, cubeZ + cubeSize,
                  normalUp);
    addCubeVertex(cubeX, cubeY + cubeSize, cubeZ + cubeSize, normalUp);

    addCubeVertex(cubeX, cubeY, cubeZ, normalDown);
    addCubeVertex(cubeX + cubeSize, cubeY, cubeZ, normalDown);
    addCubeVertex(cubeX + cubeSize, cubeY, cubeZ + cubeSize, normalDown);
    addCubeVertex(cubeX, cubeY, cubeZ + cubeSize, normalDown);

    // Indices du cube
    std::vector<unsigned int> cubeIndices = {
        0,  1,  2,  0,  2,  3,  // Face arrière
        4,  5,  6,  4,  6,  7,  // Face avant
        8,  9,  10, 8,  10, 11, // Face gauche
        12, 13, 14, 12, 14, 15, // Face droite
        16, 17, 18, 16, 18, 19, // Face du dessus
        20, 21, 22, 20, 22, 23  // Face du dessous
    };

    // Ajouter le cube aux tableaux du terrain
    int terrainVertexCount = terrainVertices.size() / 9;
    for (unsigned int i = 0; i < cubeIndices.size(); i++) {
        terrainIndices.push_back(terrainVertexCount + cubeIndices[i]);
    }
    terrainVertices.insert(terrainVertices.end(), cubeVertices.begin(),
                           cubeVertices.end());

    // Création du VAO, VBO et EBO
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Attributs du sommet (Couleur)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Attributs du sommet (Normale)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

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
    int lightDirLoc = glGetUniformLocation(terrainShaderProgram, "lightDir");
    int terrainShadowMapLoc =
        glGetUniformLocation(terrainShaderProgram, "shadowMap");
    int lightPosLoc = glGetUniformLocation(terrainShaderProgram, "lightPos");
    int lightSpaceMatrixTerrainLoc = glGetUniformLocation(terrainShaderProgram, "lightSpaceMatrix");
    // Définir la matrice de modèle (ici une matrice identité, mais tu peux y
    // appliquer des transformations)
    glm::mat4 model = glm::mat4(1.0f); // Matrice identité

    glUseProgram(terrainShaderProgram);
    // Envoi de la matrice 'model' au shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUseProgram(0);

    unsigned int carShaderProgram =
        createShaderProgram("shaders/car.vs", "shaders/car.fs");

    Light light(SPOT, glm::vec3(8.0f, 10.0f, 8.0f),
                glm::vec3(0.25f, -1.0f, 0.25f));
    // 1. Rendu dans la shadow map
    glViewport(0, 0, 2048, 2048);
    glBindFramebuffer(GL_FRAMEBUFFER, light.shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Activer un shader spécial pour le rendu des ombres
    unsigned int shadowShader =
        createShaderProgram("shaders/shadow.vs", "shaders/shadow.fs");
    glUseProgram(shadowShader);

    // Envoyer la transformation de la lumière
    int lightSpaceMatrixLoc =
        glGetUniformLocation(shadowShader, "lightSpaceMatrix");
    int shadowModelLoc = glGetUniformLocation(shadowShader, "model");

    std::vector<std::string> faces = {
        "assets/skybox/right.jpg",
        "assets/skybox/left.jpg",
        "assets/skybox/top.jpg",
        "assets/skybox/bottom.jpg",
        "assets/skybox/front.jpg",
        "assets/skybox/back.jpg"
    };
    Skybox skybox(faces);

    int skyboxShader = createShaderProgram("shaders/skybox.vs", "shaders/skybox.fs");


    // Boucle de rendu
    while (!glfwWindowShouldClose(window)) {
        processInput(window, player);

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Mettre à jour la caméra
        player.updateCamera();

        // 1. Rendu dans la shadow map
        glViewport(0, 0, 2048, 2048); // Vue de la shadow map
        glBindFramebuffer(GL_FRAMEBUFFER, light.shadowMapFBO);

        glUseProgram(shadowShader);
        glClear(GL_DEPTH_BUFFER_BIT);
        // Envoi de la transformation de la lumière dans le shader
        glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE,
                           glm::value_ptr(light.lightSpaceMatrix));

        // Dessiner les objets pour générer la shadow map
        player.renderForShadowMap(shadowShader, shadowModelLoc);

        // Dessiner le terrain pour la shadow map
        glUniformMatrix4fv(shadowModelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(terrainVAO);
        glDrawElements(GL_TRIANGLES, terrainIndices.size(), GL_UNSIGNED_INT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Dé-finir le framebuffer
        glViewport(0, 0, 800, 600); // Retourner à la taille de la fenêtre
        

        // Afficher le terrain
        glUseProgram(terrainShaderProgram);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE,
                           glm::value_ptr(player.getViewMatrix()));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
                           glm::value_ptr(player.getProjectionMatrix()));
        glUniformMatrix4fv(lightSpaceMatrixTerrainLoc, 1, GL_FALSE, glm::value_ptr(light.lightSpaceMatrix));
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(light.position));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(light.direction));
        glBindVertexArray(terrainVAO);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, light.shadowMapTexture);
        glUniform1i(terrainShadowMapLoc, 1);
        glActiveTexture(0);

        glDrawElements(GL_TRIANGLES, terrainIndices.size(), GL_UNSIGNED_INT, 0);
        glUseProgram(0);
        
        glUseProgram(carShaderProgram);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, light.shadowMapTexture);
        glUniformMatrix4fv(glGetUniformLocation(carShaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(light.lightSpaceMatrix));
        glUniform3fv(glGetUniformLocation(carShaderProgram, "lightPos"), 1, glm::value_ptr(light.position));
        glUniform3fv(glGetUniformLocation(carShaderProgram, "lightDir"), 1, glm::value_ptr(light.direction));
        glUniform1i(glGetUniformLocation(carShaderProgram, "shadowMap"), light.shadowMapTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getCumbeMapTexture());
        glUniform1i(glGetUniformLocation(carShaderProgram, "skybox"), 2);
        glUniform3fv(glGetUniformLocation(carShaderProgram, "viewPos"), 1, glm::value_ptr(player.getViewPos()));

        player.render(carShaderProgram);
        glActiveTexture(0);
        skybox.render(skyboxShader, glm::mat4(glm::mat3(player.getViewMatrix())), player.getProjectionMatrix());

        /*GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cout << "Erreur OpenGL: " << err << std::endl;
        }*/
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Nettoyage
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
