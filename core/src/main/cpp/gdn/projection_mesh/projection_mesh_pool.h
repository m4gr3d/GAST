#ifndef CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_PROJECTION_MESH_POOL_H_
#define CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_PROJECTION_MESH_POOL_H_

#include "equirectangular_projection_mesh.h"
#include "rectangular_projection_mesh.h"


namespace gast {

class ProjectionMeshPool {
public:
    ProjectionMeshPool();

    ~ProjectionMeshPool();

    RectangularProjectionMesh *get_or_create_rectangular_projection_mesh();
    EquirectangularProjectionMesh *get_or_create_equirectangular_projection_mesh();

private:
    RectangularProjectionMesh *rectangular_projection_mesh = nullptr;
    EquirectangularProjectionMesh *equirectangular_projection_mesh = nullptr;
};

}  // namespace gast
#endif //CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_PROJECTION_MESH_POOL_H_
