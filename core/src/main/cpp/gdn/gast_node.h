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
#include <gen/RayCast.hpp>
#include <gen/ShaderMaterial.hpp>
#include <gen/StaticBody.hpp>
#include <map>

#include "utils.h"

namespace gast {

namespace {
using namespace godot;
constexpr int kInvalidTexId = -1;
constexpr int kInvalidSurfaceIndex = -1;
const bool kDefaultCollidable = true;
const bool kDefaultCurveValue = false;
const float kDefaultGradientHeightRatio = 0.0f;
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

    void _physics_process(const real_t delta);

    int get_external_texture_id(int surface_index = kInvalidSurfaceIndex);

    inline void set_collidable(bool collidable) {
        if (this->collidable == collidable) {
            return;
        }
        this->collidable = collidable;
        update_collision_shape();
    }

    inline bool is_collidable() {
        return collidable;
    }

    inline void set_curved(bool curved) {
        if (this->curved == curved) {
            return;
        }
        this->curved = curved;
        update_shader_params();
        update_collision_shape();
    }

    inline bool is_curved() {
        return curved;
    }

    Vector2 get_size();

    void set_size(Vector2 size);

    inline float get_gradient_height_ratio() {
        return gradient_height_ratio;
    }

    inline void set_gradient_height_ratio(float ratio) {
        if (this->gradient_height_ratio == ratio) {
            return;
        }
        this->gradient_height_ratio = std::min(1.0f, std::max(0.0f, ratio));
        update_shader_params();
    }

private:

    // Tracks raycast collision info.
    struct CollisionInfo {
        // Tracks whether a press is in progress. If so, collision is faked via simulation
        // when the raycast no longer collides with the node.
        bool press_in_progress;
        Vector3 collision_point;
        Vector3 collision_normal;
    };

    inline CollisionShape *get_collision_shape() {
        Node *node = get_child(0);
        CollisionShape *collision_shape = Object::cast_to<CollisionShape>(node);
        return collision_shape;
    }

    inline MeshInstance *get_mesh_instance() {
        CollisionShape *collision_shape = get_collision_shape();
        if (!collision_shape) {
            return nullptr;
        }

        Node *node = collision_shape->get_child(0);
        MeshInstance *mesh_instance = Object::cast_to<MeshInstance>(node);
        return mesh_instance;
    }

    inline PlaneMesh *get_plane_mesh() {
        PlaneMesh *plane_mesh = nullptr;

        MeshInstance *mesh_instance = get_mesh_instance();
        if (mesh_instance) {
            Ref<Mesh> mesh_ref = mesh_instance->get_mesh();
            if (mesh_ref.is_valid()) {
                plane_mesh = Object::cast_to<PlaneMesh>(*mesh_ref);
            }
        }
        return plane_mesh;
    }

    static inline RayCast *get_ray_cast_from_variant(Variant variant) {
        RayCast *ray_cast = Object::cast_to<RayCast>(variant);
        return ray_cast;
    }

    Vector2 get_relative_collision_point(Vector3 absolute_collision_point);

    static inline String get_click_action_from_node_path(const String& node_path) {
        // Replace the '/' character with a '_' character
        return node_path.replace("/", "_") + "_click";
    }

    static inline String get_horizontal_left_scroll_action_from_node_path(const String& node_path) {
        // Replace the '/' character with a '_' character
        return node_path.replace("/", "_") + "_left_scroll";
    }

    static inline String get_horizontal_right_scroll_action_from_node_path(const String& node_path) {
        // Replace the '/' character with a '_' character
        return node_path.replace("/", "_") + "_right_scroll";
    }

    static inline String get_vertical_up_scroll_action_from_node_path(const String& node_path) {
        // Replace the '/' character with a '_' character
        return node_path.replace("/", "_") + "_up_scroll";
    }

    static inline String get_vertical_down_scroll_action_from_node_path(const String& node_path) {
        // Replace the '/' character with a '_' character
        return node_path.replace("/", "_") + "_down_scroll";
    }

    ExternalTexture *get_external_texture(int surface_index);

    ShaderMaterial *get_shader_material(int surface_index);

    // Calculate whether a collision occurs between the given `RayCast` and `Plane`.
    // Return True if they collide, with `collision_point` filled appropriately.
    bool calculate_raycast_plane_collision(const RayCast &raycast, const Plane &plane,
                                           Vector3 *collision_point);

    // Handle the raycast input. Returns true if a press is in progress.
    bool handle_ray_cast_input(const String &ray_cast_path, Vector2 relative_collision_point);

    void update_collision_shape();

    void update_shader_params();

    bool has_captured_raycast(const RayCast &ray_cast) {
        return colliding_raycast_paths.count(ray_cast.get_path()) != 0;
    }

    bool collidable;
    bool curved;
    float gradient_height_ratio;

    // Map used to keep track of the raycasts colliding with this node.
    // The boolean specifies whether a `press` is currently in progress.
    std::map<String, std::shared_ptr<CollisionInfo>> colliding_raycast_paths;
};
}  // namespace gast

#endif // GAST_NODE_H
