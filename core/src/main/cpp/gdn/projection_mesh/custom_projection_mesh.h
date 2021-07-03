#ifndef CUSTOM_PROJECTION_MESH_H
#define CUSTOM_PROJECTION_MESH_H

#include "projection_mesh.h"
#include "projection_mesh_utils.h"

namespace gast {

namespace {
using namespace godot;
const char* kIsLeftEyeMeshUniform = "is_left_eye_mesh";
}

class CustomProjectionMesh : public ProjectionMesh {
GODOT_CLASS(CustomProjectionMesh, ProjectionMesh)

public:
    CustomProjectionMesh();
    ~CustomProjectionMesh();

    void _init();

    static void _register_methods();

    void set_custom_mesh(int num_vertices_left, float *vertices_left,
                         float *texture_coords_left, int draw_mode_int_left,
                         int num_vertices_right, float *vertices_right,
                         float *texture_coords_right, int draw_mode_int_right,
                         int mesh_stereo_mode_int, bool uv_origin_is_bottom_left);

    inline MeshInstance *get_right_mesh_instance() {
        return right_mesh_instance;
    }

private:
    MeshInstance *right_mesh_instance;
    Ref<ShaderMaterial> right_shader_material_ref;

    String generate_shader_code() override;
};

}  // namespace gast

#endif //CUSTOM_PROJECTION_MESH_H
