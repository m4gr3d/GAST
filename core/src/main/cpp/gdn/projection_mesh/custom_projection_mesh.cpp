#include "custom_projection_mesh.h"
#include "projection_mesh_utils.h"

namespace gast {

namespace {
const float kEquirectSphereSize = 1.0f;
const size_t kEquirectSphereMeshBandCount = 80;
const size_t kEquirectSphereMeshSectorCount = 80;
}  // namespace

CustomProjectionMesh::CustomProjectionMesh() :
        ProjectionMesh(ProjectionMeshType::MESH) {
    right_mesh_instance = MeshInstance::_new();
    right_shader_material_ref = Ref<ShaderMaterial>(ShaderMaterial::_new());
}

CustomProjectionMesh::~CustomProjectionMesh() = default;

void CustomProjectionMesh::set_custom_mesh(int num_vertices_left, float *vertices_left,
                                           float *texture_coords_left, int draw_mode_int_left,
                                           int num_vertices_right, float *vertices_right,
                                           float *texture_coords_right, int draw_mode_int_right,
                                           int mesh_stereo_mode_int, bool uv_origin_is_bottom_left) {
    ArrayMesh *left_mesh = ArrayMesh::_new();
    create_array_mesh(left_mesh, num_vertices_left, vertices_left, texture_coords_left, draw_mode_int_left);
    ArrayMesh *right_mesh = ArrayMesh::_new();
    create_array_mesh(right_mesh, num_vertices_right, vertices_right, texture_coords_right, draw_mode_int_right);

    set_mesh(left_mesh);
    set_collision_shape(left_mesh->create_trimesh_shape());

    Shader *shader = Shader::_new();
    shader->set_custom_defines(kShaderCustomDefines);
    shader->set_code(generate_shader_code());
    set_shader(shader);

    set_uv_origin_is_bottom_left(uv_origin_is_bottom_left);
    set_stereo_mode(static_cast<StereoMode>(mesh_stereo_mode_int));
    get_shader_material()->set_shader_param(kIsLeftEyeMeshUniform, true);

    get_mesh_instance()->set_surface_material(kDefaultSurfaceIndex, get_shader_material());

    right_mesh_instance->set_mesh(right_mesh);
    right_shader_material_ref = get_shader_material()->duplicate(/* subresources= */ true);
    right_shader_material_ref->set_shader_param(kIsLeftEyeMeshUniform, false);
    right_mesh_instance->set_surface_material(
            kDefaultSurfaceIndex, right_shader_material_ref);

    update_listener();
}

String CustomProjectionMesh::generate_shader_code() {
    String shader_code = ProjectionMesh::generate_shader_code();
    // TODO: Allow culling to be configurable.
    shader_code += kCullFrontRenderMode;
    return shader_code;
}

void CustomProjectionMesh::_init() {}

void CustomProjectionMesh::_register_methods() {}

}  // namespace gast

