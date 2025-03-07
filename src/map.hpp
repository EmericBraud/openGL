#ifndef MAP_H
#define MAP_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

#define MAP_SIZE 256

enum class TerrainType {
    DIRT,
    ROAD,
    GRAVEL
};

class Map{
    private:
    std::array<std::array<TerrainType, MAP_SIZE>, MAP_SIZE> terrain;
    public:
    Map();
    TerrainType getTerrainAt(int x, int z) const { return terrain[x][z]; };
    glm::vec3 getTerrainColor(TerrainType type) {
        switch (type) {
            case TerrainType::DIRT:
                return glm::vec3(0.6f, 0.3f, 0.0f); // Terre (marron)
            case TerrainType::ROAD:
                return glm::vec3(0.5f, 0.5f, 0.5f); // Route (gris)
            case TerrainType::GRAVEL:
                return glm::vec3(0.7f, 0.7f, 0.7f); // Gravier (gris clair)
            default:
                return glm::vec3(0.0f, 0.0f, 0.0f); // Couleur par défaut
        }
    }
    
};

#endif