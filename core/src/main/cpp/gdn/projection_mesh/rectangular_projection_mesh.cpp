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
const int kMeshIndex = 0;
const int kMeshCount = 1;
}  // namespace

RectangularProjectionMesh::RectangularProjectionMesh(Vector2 mesh_size, bool is_curved) :
        ProjectionMesh(ProjectionMesh::ProjectionMeshType::RECTANGULAR) {
    this->mesh_size = mesh_size;
    this->is_curved = is_curved;
    this->gradient_height_ratio = kDefaultGradientHeightRatio;
}

RectangularProjectionMesh::RectangularProjectionMesh():
        RectangularProjectionMesh(kDefaultMeshSize, /* is_curved */ false) {}

RectangularProjectionMesh::~RectangularProjectionMesh() = default;

void RectangularProjectionMesh::_init() {
    ProjectionMesh::_init();
    update_projection_mesh();
}

void RectangularProjectionMesh::_register_methods() {
    ProjectionMesh::_register_methods();
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
            kDefaultMeshSize);
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
                Mesh::PRIMITIVE_TRIANGLES,
                create_curved_screen_surface_array(
                        mesh_size, kCurvedScreenRadius, kCurvedScreenResolution));
        set_collision_shape(kMeshIndex, mesh->create_trimesh_shape());
    } else {
        QuadMesh *quad_mesh = QuadMesh::_new();
        quad_mesh->set_size(mesh_size);
        mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, quad_mesh->get_mesh_arrays());
        set_collision_shape(kMeshIndex, mesh->create_convex_shape());
    }
    set_mesh(kMeshIndex, mesh);

    Shader *shader = Shader::_new();
    shader->set_custom_defines(kShaderCustomDefines);
    shader->set_code(generate_shader_code());
    set_shader(kMeshIndex, shader);
    update_sampling_transforms();
}

void RectangularProjectionMesh::set_mesh_size(Vector2 size) {
    if (this->mesh_size == size) {
        return;
    }
    this->mesh_size = size;
    update_projection_mesh();
}

inline bool RectangularProjectionMesh::should_use_alpha_shader_code() {
    return ProjectionMesh::should_use_alpha_shader_code()
           || (gradient_height_ratio >= kGradientHeightRatioThreshold);
}

Vector2 RectangularProjectionMesh::get_relative_collision_point(Vector3 local_collision_point) {
    Vector2 relative_collision_point = kInvalidCoordinate;

    // Normalize the collision point.
    Vector2 node_size = get_mesh_size();
    if (node_size.width > 0 && node_size.height > 0) {
        float max_x = node_size.width / 2;
        float min_x = -max_x;
        float max_y = node_size.height / 2;
        float min_y = -max_y;
        relative_collision_point = Vector2((local_collision_point.x - min_x) / node_size.width,
                                           (local_collision_point.y - min_y) / node_size.height);

        // Adjust the y coordinate to match the Android view coordinates system.
        relative_collision_point.y = 1 - relative_collision_point.y;
    }

    return relative_collision_point;
}

void RectangularProjectionMesh::set_curved(bool is_curved) {
    if (this->is_curved == is_curved) {
        return;
    }
    this->is_curved = is_curved;
    update_projection_mesh();
}

void RectangularProjectionMesh::set_gradient_height_ratio(float ratio) {
    if (this->gradient_height_ratio == ratio) {
        return;
    }
    this->gradient_height_ratio = std::min(1.0f, std::max(0.0f, ratio));
    update_shader_param(kGastGradientHeightRatioParamName, gradient_height_ratio);
}

int RectangularProjectionMesh::get_mesh_count() const {
    return kMeshCount;
}

void RectangularProjectionMesh::update_properties(ProjectionMesh *projection_mesh) {
    ProjectionMesh::update_properties(projection_mesh);

    // Try to cast it to a RectangularProjectionMesh
    if (projection_mesh->is_rectangular_projection_mesh()) {
        auto *rectangular_projection_mesh = Object::cast_to<RectangularProjectionMesh>(
                projection_mesh);

        set_gradient_height_ratio(rectangular_projection_mesh->get_gradient_height_ratio());
        set_curved(rectangular_projection_mesh->is_curved);
        set_mesh_size(rectangular_projection_mesh->get_mesh_size());
    }
}

}  // namespace gast

