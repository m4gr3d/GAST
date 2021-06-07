#ifndef EQUIRECTANGULAR_PROJECTION_MESH_H
#define EQUIRECTANGULAR_PROJECTION_MESH_H

#include "projection_mesh.h"
#include "projection_mesh_utils.h"

namespace gast {

namespace {
using namespace godot;
}

class EquirectangularProjectionMesh : public ProjectionMesh {
GODOT_CLASS(EquirectangularProjectionMesh, ProjectionMesh)

public:
    EquirectangularProjectionMesh();
    ~EquirectangularProjectionMesh();

    void _init();

    static void _register_methods();

private:
    String generate_shader_code() override;
    void update_projection_mesh();
};

}  // namespace gast

#endif //EQUIRECTANGULAR_PROJECTION_MESH_H
