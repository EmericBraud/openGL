#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

class Car {
  public:
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 direction;
    float wheelsAngle;
    float acceleration;
    float velocity;
    float angle;

    // Constructeur : Créer le modèle de la voiture (VAO, VBO, EBO)
    Car() { 
        loadModel("./models/Car2.obj");
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        direction = glm::vec3(0.0f, 0.0f, 1.0f);
        acceleration = 0.0f;
        velocity = 0.0f;
        angle = 0.0f;
     }

    // Fonction de chargement du modèle
    void loadModel(const std::string &modelPath) {
        // Importer le modèle avec Assimp en forçant la triangulation des faces
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(
            modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);
    
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
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                     vertices.data(), GL_STATIC_DRAW);
    
        // Lier et charger les indices dans l'EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, carEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(unsigned int), indices.data(),
                     GL_STATIC_DRAW);
    
        // Définir les attributs des sommets (Position)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
    
        // Définir les attributs des sommets (Couleur)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    
        // Définir les attributs des sommets (Normale)
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                              (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    
        // Dé-lier le VAO
        glBindVertexArray(0);
    }
    

    void updateCar(float deltaTime, int pedal_acc, int steeringWheel) {
        const float acceleration_s = 5.0f;   // Accélération
        const float rotation_s = 2.0f;       // Rotation plus rapide pour plus de réactivité
        const float friction = 0.98f;        // Friction ajustée (plus naturelle)
        const float steeringReturnSpeed = 1.5f; // Vitesse de retour du volant
        const float maxVelocity = 20.0f;
        // Gestion de l'accélération (pédale)
        float targetAcceleration = acceleration_s * pedal_acc;
        
        // Appliquer l'accélération
        velocity += targetAcceleration * deltaTime;
    
        // Appliquer la friction seulement si aucune accélération n'est appliquée
        if (pedal_acc == 0 || pedal_acc * velocity < 0) {
            velocity *= pow(friction, deltaTime);
        }
    
        // Calculer la limite du volant en fonction de la vitesse
        float maxWheelAngle = 3.141592653589793f / (4 * (1 + abs(velocity) * 0.3));
    
        // Gestion du volant : tourner ou revenir progressivement à zéro
        if (steeringWheel != 0) {
            wheelsAngle += rotation_s * (-steeringWheel) * deltaTime;
        } else {
            // Retour progressif du volant vers 0
            wheelsAngle -= std::min(std::abs(wheelsAngle), steeringReturnSpeed * deltaTime) * glm::sign(wheelsAngle);
        }
    
        // Limiter l'angle du volant
        wheelsAngle = std::clamp(wheelsAngle, -maxWheelAngle, maxWheelAngle);
        velocity = std::clamp(velocity, -maxVelocity, maxVelocity);
        // Gérer la rotation de la voiture
        float turnDirection = (velocity >= 0) ? 1.0f : -1.0f; // Inverser le tournant si on recule
        angle += turnDirection * abs(velocity) * tan(wheelsAngle) * deltaTime * 0.5f;
    
        // Mise à jour de la position (utiliser l'angle de la voiture)
        position += glm::vec3(sin(angle) * velocity * deltaTime, 
                              0.0f, 
                              cos(angle) * velocity * deltaTime);
    
        // Mise à jour de la direction (évite les bugs visuels)
        direction = glm::vec3(sin(angle), 0.0f, cos(angle));
    }
    
    
    

    // Fonction de traitement des nodes et des meshes
    void processNode(aiNode *node, const aiScene *scene) {
        // Traiter chaque mesh du node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }

        // Traiter les enfants du node
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    // Fonction de traitement d'un mesh
    // Fonction de traitement d'un mesh
    void processMesh(aiMesh *mesh, const aiScene *scene) {
        // Vérifier s'il y a un matériau assigné à ce mesh
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    
        // Extraire la couleur diffuse (qui est souvent la couleur principale de l'objet)
        aiColor4D diffuseColor(1.0f, 0.0f, 0.0f, 1.0f); // Par défaut, rouge
        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor)) {
            // Si la couleur diffuse est définie, l'utiliser
            diffuseColor = diffuseColor;
        }
    
        // Calculer les normales pour ce mesh
        std::vector<glm::vec3> normals;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D position = mesh->mVertices[i];
            // Initialiser la normale à zéro
            glm::vec3 normal(0.0f);
    
            if (mesh->HasNormals()) {
                // Si les normales existent, les récupérer
                aiVector3D aiNormal = mesh->mNormals[i];
                normal = glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z);
            }
    
            // Utiliser la couleur diffuse extraite (par défaut rouge si non définie)
            float red = diffuseColor.r;
            float green = diffuseColor.g;
            float blue = diffuseColor.b;
    
            // Ajouter la position, la couleur et la normale dans les vectors
            vertices.push_back(position.x);
            vertices.push_back(position.y);
            vertices.push_back(position.z);
            vertices.push_back(red);   // Couleur extraite
            vertices.push_back(green); // Couleur extraite
            vertices.push_back(blue);  // Couleur extraite
    
            // Ajouter la normale aux données
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    
        // Extraire les indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    
        glm::vec3 sumPositions(0.0f);
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D position = mesh->mVertices[i];
            sumPositions += glm::vec3(position.x, position.y, position.z);
        }
        modelCenter = sumPositions / static_cast<float>(mesh->mNumVertices);
    }
    

    // Fonction de mise à jour de la position et de la rotation du modèle
    void update() {
        // Vous pouvez aussi faire d'autres mises à jour ici
    }
    void calculateNormals(std::vector<float> &vertices,
                          const std::vector<unsigned int> &indices) {
        for (unsigned int i = 0; i < indices.size(); i += 3) {
            unsigned int idx0 = indices[i];
            unsigned int idx1 = indices[i + 1];
            unsigned int idx2 = indices[i + 2];

            glm::vec3 v0(vertices[3 * idx0], vertices[3 * idx0 + 1],
                         vertices[3 * idx0 + 2]);
            glm::vec3 v1(vertices[3 * idx1], vertices[3 * idx1 + 1],
                         vertices[3 * idx1 + 2]);
            glm::vec3 v2(vertices[3 * idx2], vertices[3 * idx2 + 1],
                         vertices[3 * idx2 + 2]);

            glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

            // Ajouter la normale aux vertices (elle doit être à la même
            // position)
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }

    glm::mat4 getModelMatrix(){
        // Créer la matrice modèle à partir de la position
        glm::mat4 model = glm::mat4(1.0f); // Identité

        // 2. Translation inverse pour recentrer le modèle à l'origine
        model = glm::translate(model, position - modelCenter);

        // 3. Appliquer la rotation

        glm::mat4 rotationMatrix =
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), direction, -up);
        model *= rotationMatrix;
        model = glm::rotate(model, glm::radians(180.0f),
                            glm::vec3(1.0f, 0.0f, 0.0f));

        // 4. Remettre la voiture à sa position finale
        return glm::translate(model, modelCenter);
    }

    // Fonction de rendu du modèle de voiture
    void render(unsigned int shaderProgram, const glm::mat4 &view,
                const glm::mat4 &projection) {
        
        glm::mat4 model = getModelMatrix();
        // Envoyer les matrices au shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1,
                           GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1,
                           GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                           GL_FALSE, glm::value_ptr(projection));

        // Lier le VAO et dessiner le modèle
        glBindVertexArray(carVAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Désactiver le programme shader après le dessin
        glUseProgram(0);
    }

    void renderForShadowMap(unsigned int shaderProgram, GLuint shadowModelLoc) {
    
        glm::mat4 model = getModelMatrix();
    
        // Envoyer la matrice modèle au shader
        glUniformMatrix4fv(shadowModelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
        // Lier le VAO et dessiner pour la shadow map (seulement la profondeur)
        glBindVertexArray(carVAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

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
    glm::vec3 modelCenter;
    
};
