#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "config.hpp"
#include "car.hpp"

typedef struct PlayerCameraConfig{
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
}PlayerCameraConfig;

class Player{
    private:
    glm::vec3 cameraOffset; // Décalage de la caméra
    PlayerCameraConfig cameraConfig;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    bool lookingBehind;


    public:
    Car car;
    Player(const glm::vec3& startPosition, const glm::vec3& cameraOffset, const PlayerCameraConfig& config);
    void updateCamera(); // Mettre à jour la caméra
    glm::mat4 getViewMatrix() const { return viewMatrix; }
    glm::mat4 getProjectionMatrix() const { return projectionMatrix; }
    void move(glm::vec3 delta);
    void rotate(float angle);
    void render(const unsigned int shaderProgram){
        car.render(shaderProgram, viewMatrix, projectionMatrix);
    }
    void renderForShadowMap(unsigned int shaderProgram, GLuint shadowModelLoc){
        car.renderForShadowMap(shaderProgram, shadowModelLoc);
    }
    glm::vec3 getViewPos(){
        if(lookingBehind){
            return car.position + cameraOffset + car.direction * 5.0f;
        }
        return car.position + cameraOffset - car.direction * 5.0f;
    }
    // Retourner la direction actuelle du joueur
    glm::vec3 getDirection() const {
        return car.direction;
    }
    void lookBehind(bool activator){
        lookingBehind = false;
        if(activator){
            lookingBehind = true;
        }
    }
};

#endif