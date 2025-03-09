#ifndef LIGHT_H
#define LIGHT_H

#include "include/glad/glad.h" // Inclure GLAD pour la gestion des extensions OpenGL
#include <GLFW/glfw3.h> // Inclure GLFW pour la gestion de la fenêtre et des entrées

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <vector>

enum LightType { DIRECTIONAL, POINT, SPOT };

class Light {
  public:
    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float constant, linear, quadratic;
    float cutOff, outerCutOff;

    GLuint shadowMapFBO;
    GLuint shadowMapTexture;
    glm::mat4 lightSpaceMatrix;

    Light(LightType type, glm::vec3 position = glm::vec3(0.0f),
          glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f),
          glm::vec3 color = glm::vec3(1.0f), float intensity = 1.0f)
        : type(type), position(position), direction(direction), color(color),
          intensity(intensity) {
        generateShadowMap();
    }

    void generateShadowMap() {
        glGenFramebuffers(1, &shadowMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

        glGenTextures(1, &shadowMapTexture);
        glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, shadowMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Erreur de framebuffer pour la shadow map!"
                      << std::endl;
        }

        if (type == DIRECTIONAL) {
            lightSpaceMatrix =
                glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 100.0f) *
                glm::lookAt(position, position + direction,
                            glm::vec3(0.0f, 1.0f, 0.0f));
        } else if (type == SPOT) {
            lightSpaceMatrix =
                glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f) *
                glm::lookAt(position, position + direction,
                            glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    void sendToShader(unsigned int shaderProgram) const {
        glUniform3fv(glGetUniformLocation(shaderProgram, "light.position"), 1,
                     &position[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "light.color"), 1,
                     &color[0]);
        glUniform1f(glGetUniformLocation(shaderProgram, "light.intensity"),
                    intensity);
    }
};

#endif
