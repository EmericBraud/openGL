#include "player.hpp"
Player::Player(const glm::vec3 &startPosition, const glm::vec3 &cameraOffset,
               const PlayerCameraConfig &config)
    : cameraOffset(cameraOffset), cameraConfig(config)
      {
        
    lookingBehind = false;
    // Initialisation de la matrice de projection
    projectionMatrix = glm::perspective(
        glm::radians(cameraConfig.fov), cameraConfig.aspectRatio,
        cameraConfig.nearPlane, cameraConfig.farPlane);

    // Mise à jour de la caméra pour la première fois
    updateCamera();
    car.position = startPosition;
}

void Player::updateCamera() {
    glm::vec3 cameraPosition = getViewPos();
    glm::vec3 target = car.position; 
    viewMatrix = glm::lookAt(cameraPosition, target + glm::vec3(0.0f, cameraOffset.y/2, 0.0f), car.up);
}


void Player::move(glm::vec3 delta) {
    car.position += delta * 10.0f;
    updateCamera(); // Mettre à jour la caméra après déplacement
}

void Player::rotate(float angle) {
    car.direction = glm::normalize(glm::vec3(
        cos(angle) * car.direction.x - sin(angle) * car.direction.z, car.direction.y,
        sin(angle) * car.direction.x + cos(angle) * car.direction.z));

    updateCamera(); // Mettre à jour la caméra après rotation
}
