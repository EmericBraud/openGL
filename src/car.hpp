#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

class Car {
public:
    // Constructeur : Créer le modèle de la voiture (VAO, VBO, EBO)
    Car() {
        loadModel("./models/Car.obj");
    }

    // Fonction de chargement du modèle
    void loadModel(const std::string &modelPath) {
        // Importer le modèle avec Assimp
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || !scene->mRootNode) {
            std::cerr << "Erreur lors du chargement du modèle: " << modelPath << std::endl;
            return;
        }

        // Extraire les données des vertices et indices
        processNode(scene->mRootNode, scene);

        // Créer les buffers pour les données des vertices et indices
        glGenVertexArrays(1, &carVAO);
        glGenBuffers(1, &carVBO);
        glGenBuffers(1, &carEBO);

        // Lier le VAO
        glBindVertexArray(carVAO);

        // Lier et charger les données du VBO
        glBindBuffer(GL_ARRAY_BUFFER, carVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Lier et charger les indices dans l'EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, carEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Définir les attributs des sommets (Position)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Définir les attributs des sommets (Couleur)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Dé-lier le VAO
        glBindVertexArray(0);
    }

    // Fonction de traitement des nodes et des meshes
    void processNode(aiNode* node, const aiScene* scene) {
        // Traiter chaque mesh du node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }

        // Traiter les enfants du node
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    // Fonction de traitement d'un mesh
    // Fonction de traitement d'un mesh
void processMesh(aiMesh* mesh, const aiScene* scene) {
    // Extraire les vertices et les indices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D position = mesh->mVertices[i];

        // Utiliser une couleur arbitraire (par exemple, rouge)
        float red = 1.0f;
        float green = 0.0f;
        float blue = 0.0f;

        // Ajouter la position et la couleur arbitraire dans les vectors
        vertices.push_back(position.x);
        vertices.push_back(position.y);
        vertices.push_back(position.z);
        vertices.push_back(red);    // Couleur rouge
        vertices.push_back(green);  // Pas de vert
        vertices.push_back(blue);   // Pas de bleu
    }

    // Extraire les indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
}

    // Fonction de mise à jour de la position et de la rotation du modèle
    void update() {
        // Vous pouvez aussi faire d'autres mises à jour ici
    }

    // Fonction de rendu du modèle de voiture
    void render(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection, glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
        // Créer la matrice modèle à partir de la position
        glm::mat4 model = glm::mat4(1.0f);  // Identité
        model = glm::translate(model, position); // Appliquer la translation
        
        // Appliquer une rotation de 90 degrés autour de l'axe X pour inverser l'orientation
    
        // Créer une matrice de rotation en fonction de la direction et de l'up
        glm::mat4 rotationMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), direction, -up); // Matrice de vue
        model *= rotationMatrix;
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotation sur l'axe X
    
        // Utiliser le shader
        glUseProgram(shaderProgram);
        
        // Envoyer les matrices au shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        // Lier le VAO et dessiner le modèle
        glBindVertexArray(carVAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        // Désactiver le programme shader après le dessin
        glUseProgram(0);
    }
    

    // Destructor : libérer les ressources
    ~Car() {
        glDeleteVertexArrays(1, &carVAO);
        glDeleteBuffers(1, &carVBO);
        glDeleteBuffers(1, &carEBO);
    }

private:
    unsigned int carVAO, carVBO, carEBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};
