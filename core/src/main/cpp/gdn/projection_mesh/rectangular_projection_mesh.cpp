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
const bool kDefaultCurveValue = false;
const float kDefaultGradientHeightRatio = 0.0f;
}  // namespace

RectangularProjectionMesh::RectangularProjectionMesh(Vector2 mesh_size, bool is_curved) :
        ProjectionMesh() {
    this->mesh_size = mesh_size;
    this->is_curved = is_curved;
    this->gradient_height_ratio = kDefaultGradientHeightRatio;
    update_projection_mesh();
}

RectangularProjectionMesh::RectangularProjectionMesh():
        RectangularProjectionMesh(get_default_mesh_size(), /* is_curved */ false) {}

RectangularProjectionMesh::~RectangularProjectionMesh() = default;

void RectangularProjectionMesh::_register_methods() {
    register_method("_process", &RectangularProjectionMesh::_process);
    register_method("set_size", &RectangularProjectionMesh::set_mesh_size);
    register_method("get_size", &RectangularProjectionMesh::get_mesh_size);
    register_method("set_curved", &RectangularProjectionMesh::set_curved);
    register_method("is_curved", &RectangularProjectionMesh::get_curved);
    register_method("set_gradient_height_ratio",
            &RectangularProjectionMesh::set_gradient_height_ratio);
    register_method("get_gradient_height_ratio",
            &RectangularProjectionMesh::get_gradient_height_ratio);

    register_property<RectangularProjectionMesh, Vector2>("size",
            &RectangularProjectionMesh::set_mesh_size, &RectangularProjectionMesh::get_mesh_size,
            get_default_mesh_size());
    register_property<RectangularProjectionMesh, bool>("curved",
            &RectangularProjectionMesh::set_curved, &RectangularProjectionMesh::get_curved,
            kDefaultCurveValue);
    register_property<RectangularProjectionMesh, float>("gradient_height_ratio",
                                       &RectangularProjectionMesh::set_gradient_height_ratio,
                                       &RectangularProjectionMesh::get_gradient_height_ratio,
                                       kDefaultGradientHeightRatio);
}

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

void RectangularProjectionMesh::_init() {}

void RectangularProjectionMesh::_process(const real_t delta) {}

}  // namespace gast

