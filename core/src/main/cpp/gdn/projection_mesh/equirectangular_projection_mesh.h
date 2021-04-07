#ifndef CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_EQUIRECTANGULAR_PROJECTION_MESH_H_
#define CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_EQUIRECTANGULAR_PROJECTION_MESH_H_

#include "projection_mesh.h"
#include "projection_mesh_utils.h"

namespace gast {

namespace {
using namespace godot;
}

class EquirectangularProjectionMesh : public ProjectionMesh {
public:
    EquirectangularProjectionMesh();
    ~EquirectangularProjectionMesh();

    void update_projection_mesh();

private:
    String generate_shader_code();
};

}  // namespace gast

#endif //CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_EQUIRECTANGULAR_PROJECTION_MESH_H_
