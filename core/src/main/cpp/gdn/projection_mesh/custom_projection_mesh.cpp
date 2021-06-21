#include <gen/ArrayMesh.hpp>
#include <gen/QuadMesh.hpp>
#include <gen/Shader.hpp>
#include <gen/Shape.hpp>

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
    setup_default_projection_mesh();
}

CustomProjectionMesh::~CustomProjectionMesh() = default;

void CustomProjectionMesh::set_custom_mesh(int num_vertices_left, float *vertices_left,
                                           float *texture_coords_left, int draw_mode_int_left,
                                           int num_vertices_right, float *vertices_right,
                                           float *texture_coords_right, int draw_mode_int_right,
                                           int mesh_stereo_mode_int) {
    ArrayMesh *left_mesh = ArrayMesh::_new();
    create_array_mesh(left_mesh, num_vertices_left, vertices_left, texture_coords_left, draw_mode_int_left);
    ArrayMesh *right_mesh = ArrayMesh::_new();
    create_array_mesh(right_mesh, num_vertices_right, vertices_right, texture_coords_right, draw_mode_int_right);

    set_mesh(left_mesh);
    set_collision_shape(left_mesh->create_trimesh_shape());

    set_stereo_mode(static_cast<StereoMode>(mesh_stereo_mode_int));

    update_listener();
}

String CustomProjectionMesh::generate_shader_code() {
    String shader_code = ProjectionMesh::generate_shader_code();
    // TODO: Allow culling to be configurable.
    shader_code += kCullFrontRenderMode;
    return shader_code;
}

void CustomProjectionMesh::setup_default_projection_mesh() {
    ArrayMesh *mesh = ArrayMesh::_new();
    mesh->add_surface_from_arrays(
            Mesh::PRIMITIVE_TRIANGLES,
            create_spherical_surface_array(
                    kEquirectSphereSize,
                    kEquirectSphereMeshBandCount,
                    kEquirectSphereMeshSectorCount));
    set_mesh(mesh);
    set_collision_shape(mesh->create_trimesh_shape());

    Shader *shader = Shader::_new();
    shader->set_custom_defines(kShaderCustomDefines);
    shader->set_code(generate_shader_code());
    set_shader(shader);
    update_sampling_transforms();

    get_mesh_instance()->set_surface_material(kDefaultSurfaceIndex, get_shader_material());
}

void CustomProjectionMesh::_init() {}

void CustomProjectionMesh::_register_methods() {}

}  // namespace gast

