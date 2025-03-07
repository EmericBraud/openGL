#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "car_model.hpp"

class Car {
  public:
    // Constructeur : Créer le cube (VAO, VBO, EBO)
    Car() {
        // Sommets du cube (positions et couleurs)
        cubeVertices = {
            // Position x, y, z,   Couleur r, g, b
            -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // Face avant gauche bas
            0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Face avant droite bas
            0.5f,  0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, // Face avant droite haut
            -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, 0.0f, // Face avant gauche haut
            -0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 1.0f, // Face arrière gauche bas
            0.5f,  -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, // Face arrière droite bas
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // Face arrière droite haut
            -0.5f, 0.5f,  0.5f,  0.5f, 0.5f, 0.5f  // Face arrière gauche haut
        };

        // Indices pour former les triangles du cube
        cubeIndices = {// Face avant
                       0, 1, 2, 0, 2, 3,
                       // Face arrière
                       4, 5, 6, 4, 6, 7,
                       // Face gauche
                       0, 3, 7, 0, 7, 4,
                       // Face droite
                       1, 2, 6, 1, 6, 5,
                       // Face du haut
                       3, 2, 6, 3, 6, 7,
                       // Face du bas
                       0, 1, 5, 0, 5, 4};

        // Créer les buffers
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glGenBuffers(1, &cubeEBO);

        // Lier le VAO
        glBindVertexArray(cubeVAO);

        // Lier le VBO et charger les données des sommets
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float),
                     cubeVertices.data(), GL_STATIC_DRAW);

        // Lier le EBO et charger les indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     cubeIndices.size() * sizeof(unsigned int),
                     cubeIndices.data(), GL_STATIC_DRAW);

        // Définir les attributs des sommets (Position)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(0);

        // Définir les attributs des sommets (Couleur)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Dé-lier le VAO
        glBindVertexArray(0);

        // Initialisation de la position et de la rotation
    }

    // Fonction de mise à jour de la position et de la rotation du cube
    void update() {
        // Vous pouvez aussi faire d'autres mises à jour ici
    }

    // Fonction de rendu du cube
    void render(unsigned int shaderProgram, const glm::mat4 &view, const glm::mat4 &projection, glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
        // Créer la matrice modèle à partir de la position
        glm::mat4 model = glm::mat4(1.0f);  // Identité
        model = glm::translate(model, position + glm::vec3(0, 0.5f, 0.0f)); // Appliquer la translation
    
        // Créer une matrice de rotation en fonction de la direction et de l'up
        // Utiliser glm::lookAt pour obtenir une matrice de vue à partir de la position, direction et up
        glm::mat4 rotationMatrix = glm::mat4(1.0f); // Identité
        rotationMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), direction, -up); // Matrice de vue
    
        // Appliquer la rotation au modèle
        model *= rotationMatrix; 
    
        // Utiliser le shader
        glUseProgram(shaderProgram);
    
        // Envoyer les matrices au shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
        // Lier le VAO et dessiner le cube
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    
        // Désactiver le programme shader après le dessin
        glUseProgram(0);
    }
    

    // Destructor : libérer les ressources
    ~Car() {
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteBuffers(1, &cubeEBO);
    }

  private:
    unsigned int cubeVAO, cubeVBO, cubeEBO;
    std::vector<float> cubeVertices;
    std::vector<unsigned int> cubeIndices;
};
