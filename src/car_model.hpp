#ifndef CAR_MODEL_H
#define CAR_MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <stdexcept>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class CarModel {
public:
    // Constructeur
    CarModel(const std::string& path);
    ~CarModel();

    // Fonction pour dessiner le modèle
    void Draw(unsigned int shaderProgram);

private:
    // Fonction pour charger un modèle à partir d'un fichier .obj
    void loadModel(const std::string& path);

    // Fonction pour traiter les maillages du modèle
    void processMesh(aiMesh* mesh, const aiScene* scene);

    // Fonction pour charger une texture à partir d'un fichier
    unsigned int loadTexture(const aiTexture* texture);

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO, textureID;
};

#endif
