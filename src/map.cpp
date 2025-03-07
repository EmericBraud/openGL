#include "map.hpp"

Map::Map() {
    for (auto& row : terrain) {
        row.fill(TerrainType::DIRT);
    }
}
