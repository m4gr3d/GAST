#include <gen/ArrayMesh.hpp>
#include <gen/Shader.hpp>
#include <gen/Shape.hpp>

#include "equirectangular_projection_mesh.h"
#include "projection_mesh_utils.h"

namespace gast {

namespace {
const float kEquirectSphereSize = 1.0f;
const size_t kEquirectSphereMeshBandCount = 80;
const size_t kEquirectSphereMeshSectorCount = 80;
const int kMeshIndex = 0;
const int kMeshCount = 1;
}  // namespace

EquirectangularProjectionMesh::EquirectangularProjectionMesh() :
        ProjectionMesh(ProjectionMeshType::EQUIRECTANGULAR) {}

EquirectangularProjectionMesh::~EquirectangularProjectionMesh() = default;

void EquirectangularProjectionMesh::update_projection_mesh() {
    ArrayMesh *mesh = ArrayMesh::_new();
    mesh->add_surface_from_arrays(
            Mesh::PRIMITIVE_TRIANGLES,
            create_spherical_surface_array(
                    kEquirectSphereSize,
                    kEquirectSphereMeshBandCount,
                    kEquirectSphereMeshSectorCount));
    set_mesh(kMeshIndex, mesh);
    set_collision_shape(kMeshIndex, mesh->create_trimesh_shape());

    Shader *shader = Shader::_new();
    shader->set_custom_defines(String(kShaderCustomDefines) + String(kShaderHighpFloatDefines));
    shader->set_code(generate_shader_code());
    set_shader(kMeshIndex, shader);
    update_sampling_transforms();
}

int EquirectangularProjectionMesh::get_mesh_count() const {
    return kMeshCount;
}

String EquirectangularProjectionMesh::generate_shader_code() {
    String shader_code = ProjectionMesh::generate_shader_code();
    // TODO: Allow culling to be configurable.
    shader_code += kCullFrontRenderMode;
    return shader_code;
}

void EquirectangularProjectionMesh::_init() {
    ProjectionMesh::_init();
    update_projection_mesh();
}

void EquirectangularProjectionMesh::_register_methods() {
    ProjectionMesh::_register_methods();
}

}  // namespace gast

