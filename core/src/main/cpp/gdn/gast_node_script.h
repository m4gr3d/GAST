#ifndef GAST_NODE_SCRIPT_H
#define GAST_NODE_SCRIPT_H

#include <gast_manager.h>
#include <core/Godot.hpp>
#include <core/Vector2.hpp>
#include <core/Vector3.hpp>
#include <gen/InputEvent.hpp>
#include <gen/Object.hpp>
#include <gen/RayCast.hpp>
#include <gen/StaticBody.hpp>

#include "utils.h"

namespace gast {

namespace {
using namespace godot;

static const Vector2 kInvalidCoordinate = Vector2(-1, -1);
}  // namespace

/// Script for a GAST node. Enables GAST specific logic and processing.
class GastNodeScript : public StaticBody {
GODOT_CLASS(GastNodeScript, StaticBody)

public:
    GastNodeScript();

    ~GastNodeScript();

    static void _register_methods();

    void _init();

    void
    _input_event(const Object *camera, const Ref<InputEvent> event, const Vector3 click_position,
                 const Vector3 click_normal, const int64_t shape_idx);

    void _physics_process(const real_t delta);

private:
    inline RayCast *get_ray_cast_from_variant(Variant variant) {
        if (variant.get_type() != Variant::OBJECT) {
            return nullptr;
        }

        Object *object = get_wrapper<Object>(variant.operator godot_object *());
        if (!object || !object->is_class("RayCast")) {
            return nullptr;
        }

        RayCast *ray_cast = Object::cast_to<RayCast>(variant);
        return ray_cast;
    }

    inline Vector2 get_relative_collision_point(Vector3 absolute_collision_point) {
        Vector2 relative_collision_point = kInvalidCoordinate;

        // Turn the collision point into local space
        Vector3 local_point = to_local(absolute_collision_point);

        // Normalize the collision point. A Gast node is a flat quad so we only worry about
        // the x,y coordinates
        Vector2 node_size = GastManager::get_singleton_instance()->get_gast_node_size(get_path());
        if (node_size.width > 0 && node_size.height > 0) {
            float max_x = node_size.width / 2;
            float min_x = -max_x;
            float max_y = node_size.height / 2;
            float min_y = -max_y;
            if (local_point.x >= min_x && local_point.x <= max_x && local_point.y >= min_y &&
                local_point.y <= max_y) {
                relative_collision_point = Vector2((local_point.x - min_x) / node_size.width,
                                                   (local_point.y - min_y) / node_size.height);

                // Adjust the y coordinate to match the Android view coordinates system.
                relative_collision_point.y = 1 - relative_collision_point.y;
            }
        }

        return relative_collision_point;
    }

    inline String get_action_from_node_path(const String& node_path) {
        // Replace the '/' character with a '_' character
        return node_path.replace("/", "_");
    }

    void handle_ray_cast_input(const RayCast &ray_cast);
};
}  // namespace gast

#endif // GAST_NODE_SCRIPT_H
