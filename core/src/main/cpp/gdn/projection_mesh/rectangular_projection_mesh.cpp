#include <gen/ArrayMesh.hpp>
#include <gen/Mesh.hpp>
#include <gen/QuadMesh.hpp>
#include <gen/Shader.hpp>
#include <gen/Shape.hpp>
#include <utils.h>

#include "projection_mesh_utils.h"
#include "rectangular_projection_mesh.h"

namespace gast {

namespace {
const float kCurvedScreenRadius = 6.0f;
const size_t kCurvedScreenResolution = 20;
}  // namespace

RectangularProjectionMesh::RectangularProjectionMesh(Vector2 mesh_size, bool is_curved) :
        ProjectionMesh(
                ProjectionMeshType::RECTANGULAR,
                MeshInstance::_new(),
                Ref<ShaderMaterial>(ShaderMaterial::_new())) {
    this->mesh_size = mesh_size;
    this->is_curved = is_curved;
    update_projection_mesh();
}

RectangularProjectionMesh::RectangularProjectionMesh():
        RectangularProjectionMesh(get_default_mesh_size(), /* is_curved */ false) {}

RectangularProjectionMesh::~RectangularProjectionMesh() = default;

void RectangularProjectionMesh::update_projection_mesh() {
    ArrayMesh *mesh = ArrayMesh::_new();
    if (is_curved) {
        mesh->add_surface_from_arrays(
                Mesh::PRIMITIVE_TRIANGLE_STRIP,
                create_curved_screen_surface_array(
                        mesh_size, kCurvedScreenRadius, kCurvedScreenResolution));
    } else {
        QuadMesh *quad_mesh = QuadMesh::_new();
        quad_mesh->set_size(mesh_size);
        mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, quad_mesh->get_mesh_arrays());
    }
    set_mesh(mesh);
    // TODO: Use `create_trimesh_shape()` for curved instead after resolving why the shape doesn't
    //  detect collisions.
    set_collision_shape(mesh->create_convex_shape());

    Shader *shader = Shader::_new();
    shader->set_custom_defines(kShaderCustomDefines);
    shader->set_code(generate_shader_code());
    set_shader(shader);

    get_mesh_instance()->set_surface_material(kDefaultSurfaceIndex, get_shader_material());

}

String RectangularProjectionMesh::generate_shader_code() {
    String shader_code = kMonoShaderCode;
    if (is_render_on_top()) {
        shader_code += kDisableDepthTestRenderMode;
    }
    return shader_code;
}

}  // namespace gast

