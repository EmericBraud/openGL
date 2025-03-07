#include "player.hpp"
Player::Player(const glm::vec3 &startPosition, const glm::vec3 &cameraOffset,
               const PlayerCameraConfig &config)
    : position(startPosition), cameraOffset(cameraOffset), cameraConfig(config),
      direction(0.0f, 0.0f, 1.0f) {
        
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    // Initialisation de la matrice de projection
    projectionMatrix = glm::perspective(
        glm::radians(cameraConfig.fov), cameraConfig.aspectRatio,
        cameraConfig.nearPlane, cameraConfig.farPlane);

    // Mise à jour de la caméra pour la première fois
    updateCamera();
}

void Player::updateCamera() {
    glm::vec3 cameraPosition = position + cameraOffset - direction * 2.0f;
    glm::vec3 target = position; 
    viewMatrix = glm::lookAt(cameraPosition, target + glm::vec3(0.0f, cameraOffset.y/2, 0.0f), up);
}


void Player::move(glm::vec3 delta) {
    position += delta;
    updateCamera(); // Mettre à jour la caméra après déplacement
}

void Player::rotate(float angle) {
    direction = glm::normalize(glm::vec3(
        cos(angle) * direction.x - sin(angle) * direction.z, direction.y,
        sin(angle) * direction.x + cos(angle) * direction.z));

    updateCamera(); // Mettre à jour la caméra après rotation
}
