#ifndef GAST_NODE_H
#define GAST_NODE_H

#include <core/Godot.hpp>
#include <core/Ref.hpp>
#include <core/Vector2.hpp>
#include <core/Vector3.hpp>
#include <gen/CollisionShape.hpp>
#include <gen/ConcavePolygonShape.hpp>
#include <gen/ExternalTexture.hpp>
#include <gen/InputEvent.hpp>
#include <gen/MeshInstance.hpp>
#include <gen/Mesh.hpp>
#include <gen/Object.hpp>
#include <gen/PlaneMesh.hpp>
#include <gen/QuadMesh.hpp>
#include <gen/RayCast.hpp>
#include <gen/Shader.hpp>
#include <gen/ShaderMaterial.hpp>
#include <gen/StaticBody.hpp>

#include "gdn/projection_mesh/custom_projection_mesh.h"
#include "gdn/projection_mesh/projection_mesh.h"
#include "gdn/projection_mesh/projection_mesh_pool.h"
#include "utils.h"

namespace gast {

namespace {
using namespace godot;
constexpr int kInvalidTexId = -1;
}  // namespace

/// Script for a GAST node. Enables GAST specific logic and processing.
class GastNode : public StaticBody {
GODOT_CLASS(GastNode, StaticBody)

public:
    GastNode();

    ~GastNode();

    static void _register_methods();

    void _init();

    void _enter_tree();

    void _exit_tree();

    void
    _input_event(const Object *camera, const Ref<InputEvent> event, const Vector3 click_position,
                 const Vector3 click_normal, const int64_t shape_idx);

    void _process(const real_t delta);

    void _notification(const int64_t what);

    Ref<ExternalTexture> get_external_texture();

    int get_external_texture_id();

    inline void set_collidable(bool collidable) {
        projection_mesh->set_collidable(collidable);
    }

    inline bool is_collidable() {
        return projection_mesh->is_collidable();
    }

    void set_projection_mesh(ProjectionMesh::ProjectionMeshType projection_mesh_type);

    inline ProjectionMesh* get_projection_mesh() {
        return this->projection_mesh;
    }

    inline ProjectionMesh::ProjectionMeshType get_projection_mesh_type() {
        return projection_mesh->get_projection_mesh_type();
    }

    inline void set_render_on_top(bool enable) {
        projection_mesh->set_render_on_top(enable);
    }

    inline bool is_render_on_top() {
        return projection_mesh->is_render_on_top();
    }

    inline void set_gaze_tracking(bool gaze_tracking) {
        projection_mesh->set_gaze_tracking(gaze_tracking);
    }

    inline bool is_gaze_tracking() {
        return projection_mesh->is_gaze_tracking();
    }

    inline void set_alpha(float alpha) {
        projection_mesh->set_alpha(alpha);
    }

    inline void set_has_transparency(bool has_transparency) {
        projection_mesh->set_has_transparency(has_transparency);
    }

    static inline RayCast *get_ray_cast_from_variant(Variant variant) {
        RayCast *ray_cast = Object::cast_to<RayCast>(variant);
        return ray_cast;
    }

    Vector2 get_relative_collision_point(Vector3 absolute_collision_point);

    // Handle the raycast input. Returns true if a press is in progress.
    bool handle_ray_cast_input(const String &ray_cast_name, Vector2 relative_collision_point);

    // Returns true if the plane defined by this node intersects the given ray.
    bool intersects_ray(Vector3 ray_origin, Vector3 ray_direction, Vector3 *intersection);

private:

    static inline String get_click_action_from_node_name(const String &node_name) {
        // Replace the '/' character with a '_' character
        return node_name.replace("/", "_") + "_click";
    }

    static inline String get_horizontal_left_scroll_action_from_node_name(const String &node_name) {
        // Replace the '/' character with a '_' character
        return node_name.replace("/", "_") + "_left_scroll";
    }

    static inline String
    get_horizontal_right_scroll_action_from_node_name(const String &node_name) {
        // Replace the '/' character with a '_' character
        return node_name.replace("/", "_") + "_right_scroll";
    }

    static inline String get_vertical_up_scroll_action_from_node_name(const String &node_name) {
        // Replace the '/' character with a '_' character
        return node_name.replace("/", "_") + "_up_scroll";
    }

    static inline String get_vertical_down_scroll_action_from_node_name(const String &node_name) {
        // Replace the '/' character with a '_' character
        return node_name.replace("/", "_") + "_down_scroll";
    }

    void update_collision_shape();

    void reset_mesh_and_collision_shape();

    void update_projection_mesh_camera_uniforms();

    ProjectionMeshPool projection_mesh_pool;
    ProjectionMesh *projection_mesh = nullptr;
    Ref<ExternalTexture> external_texture;
};

}  // namespace gast

#endif // GAST_NODE_H
