#include "custom_projection_mesh.h"
#include "projection_mesh_utils.h"

namespace gast {

namespace {
const float kEquirectSphereSize = 1.0f;
const size_t kEquirectSphereMeshBandCount = 80;
const size_t kEquirectSphereMeshSectorCount = 80;
const int kLeftMeshIndex = 0;
const int kRightMeshIndex = 1;
const int kMeshCount = 2;

const char *kShaderViewIndexUniform = "shader_view_index";
}  // namespace

CustomProjectionMesh::CustomProjectionMesh() :
        ProjectionMesh(ProjectionMeshType::MESH) {}

CustomProjectionMesh::~CustomProjectionMesh() = default;

void CustomProjectionMesh::_init() {
    ProjectionMesh::_init();
}

int CustomProjectionMesh::get_mesh_count() const {
    return kMeshCount;
}

void CustomProjectionMesh::set_custom_mesh(int num_vertices_left, float *vertices_left,
                                           float *texture_coords_left, int draw_mode_int_left,
                                           int num_vertices_right, float *vertices_right,
                                           float *texture_coords_right, int draw_mode_int_right,
                                           int mesh_stereo_mode_int, bool uv_origin_is_bottom_left) {
    // Common to all meshes
    Shader *shader = Shader::_new();
    shader->set_custom_defines(kShaderCustomDefines);
    shader->set_code(generate_shader_code());

    set_uv_origin_is_bottom_left(uv_origin_is_bottom_left);
    set_stereo_mode(static_cast<StereoMode>(mesh_stereo_mode_int));

    // Left mesh specific setup
    ArrayMesh *left_mesh = ArrayMesh::_new();
    create_array_mesh(left_mesh, num_vertices_left, vertices_left, texture_coords_left, draw_mode_int_left);
    set_mesh(kLeftMeshIndex, left_mesh);
    set_collision_shape(kLeftMeshIndex, left_mesh->create_trimesh_shape());
    set_shader(kLeftMeshIndex, shader);
    update_shader_param(kLeftMeshIndex, kShaderViewIndexUniform, /* left view index */ 0);

    // Right mesh specific setup
    ArrayMesh *right_mesh = ArrayMesh::_new();
    create_array_mesh(right_mesh, num_vertices_right, vertices_right, texture_coords_right, draw_mode_int_right);
    set_mesh(kRightMeshIndex, right_mesh);
    set_collision_shape(kRightMeshIndex, right_mesh->create_trimesh_shape());
    set_shader(kRightMeshIndex, shader);
//    right_shader_material_ref = get_shader_material()->duplicate(/* subresources= */ true);
    update_shader_param(kRightMeshIndex, kShaderViewIndexUniform, /* right view index */ 1);
}

String CustomProjectionMesh::generate_shader_code() {
    String shader_code = ProjectionMesh::generate_shader_code();
    // TODO: Allow culling to be configurable.
    shader_code += kCullFrontRenderMode;
    return shader_code;
}

void CustomProjectionMesh::_register_methods() {
    ProjectionMesh::_register_methods();
}

}  // namespace gast
