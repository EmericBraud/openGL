#ifndef SKYBOX_H
#define SKYBOX_H

#define STB_IMAGE_IMPLEMENTATION
#include "../include/glad/glad.h"
#include "../include/stb/stb_image.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>

class Skybox {
  public:
    // Constructeur
    Skybox(const std::vector<std::string> &faces) { loadCubemap(faces); }

    // Fonction pour dessiner la skybox
    void render(unsigned int shaderProgram, const glm::mat4 &view,
                const glm::mat4 &projection) {
        glUseProgram(shaderProgram);
        
        if (cubemapTexture == 0) {
            std::cout << "Erreur : La texture cubemap n'est pas valide !" << std::endl;
            return;
        }

        // Passer les matrices de vue et de projection
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1,
                           GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                           GL_FALSE, glm::value_ptr(projection));
        // Désactiver l'écriture dans le depth buffer (nous ne voulons pas que
        // la skybox écrase d'autres objets)
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(skyboxVAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0);


        // Lier le VAO de la skybox et dessiner
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Réactiver l'écriture dans le depth buffer
        glDepthFunc(GL_LESS);
        glUseProgram(0);
    }

    // Destructor : libérer les ressources
    ~Skybox() {
        glDeleteTextures(1, &cubemapTexture);
        glDeleteVertexArrays(1, &skyboxVAO);
        glDeleteBuffers(1, &skyboxVBO);
        glDeleteBuffers(1, &skyboxEBO);
    }

    unsigned int getCumbeMapTexture(){
        return cubemapTexture;
    }

  private:
    unsigned int cubemapTexture;
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;

    // Fonction pour charger une cubemap
    void loadCubemap(const std::vector<std::string> &faces) {
        glGenTextures(1, &cubemapTexture);
        if (cubemapTexture == 0) {
            std::cout << "Erreur lors de la création de la texture cubemap !"
                      << std::endl;
            return;
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                        GL_CLAMP_TO_EDGE);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char *data =
                stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
                             width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            } else {
                std::cout << "Échec du chargement de la texture cubemap à "
                             "l'emplacement : "
                          << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        
        // Vérifier que la texture a bien été créée et configurée
        GLint textureStatus;
        glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP, 0, GL_TEXTURE_WIDTH,
            &textureStatus);
            if (cubemapTexture == 0) {
            std::cout << "Erreur : La texture cubemap n'a pas été générée !" << std::endl;
        }
        
        for (unsigned int i = 0; i < faces.size(); i++) {
            if (!stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0)) {
                std::cout << "⚠ Erreur de chargement de la texture : " << faces[i] << std::endl;
            }
        }
                                
        // Initialiser les données du cube (6 faces)
        float skyboxVertices[] = {
            // Positions X, Y, Z
            -1.0f,  1.0f, -1.0f, // 0 - Haut gauche avant
            -1.0f, -1.0f, -1.0f, // 1 - Bas gauche avant
             1.0f, -1.0f, -1.0f, // 2 - Bas droit avant
             1.0f,  1.0f, -1.0f, // 3 - Haut droit avant
            -1.0f,  1.0f,  1.0f, // 4 - Haut gauche arrière
            -1.0f, -1.0f,  1.0f, // 5 - Bas gauche arrière
             1.0f, -1.0f,  1.0f, // 6 - Bas droit arrière
             1.0f,  1.0f,  1.0f  // 7 - Haut droit arrière
        };
        

        // Indices pour le cube
        unsigned int skyboxIndices[] = {
            // Face avant
            0, 1, 2,  2, 3, 0,
            // Face arrière
            4, 5, 6,  6, 7, 4,
            // Face gauche
            4, 5, 1,  1, 0, 4,
            // Face droite
            3, 2, 6,  6, 7, 3,
            // Face haut
            4, 0, 3,  3, 7, 4,
            // Face bas
            1, 5, 6,  6, 2, 1
        };
        

        // Créer et configurer le VAO, VBO et EBO
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glGenBuffers(1, &skyboxEBO);

        glBindVertexArray(skyboxVAO);

        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices,
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices),
                     &skyboxIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

#endif
