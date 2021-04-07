#include "projection_mesh_pool.h"

namespace gast {

ProjectionMeshPool::ProjectionMeshPool() = default;

ProjectionMeshPool::~ProjectionMeshPool() = default;

RectangularProjectionMesh *ProjectionMeshPool::get_or_create_rectangular_projection_mesh() {
    if (!rectangular_projection_mesh) {
        rectangular_projection_mesh = new RectangularProjectionMesh();
    }
    return rectangular_projection_mesh;
}

EquirectangularProjectionMesh *ProjectionMeshPool::get_or_create_equirectangular_projection_mesh() {
    if (!equirectangular_projection_mesh) {
        equirectangular_projection_mesh = new EquirectangularProjectionMesh();
    }
    return equirectangular_projection_mesh;
}

}  // namespace gast
