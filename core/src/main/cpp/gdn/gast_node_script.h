#ifndef GAST_NODE_SCRIPT_H
#define GAST_NODE_SCRIPT_H

#include <core/Godot.hpp>
#include <core/Vector2.hpp>
#include <core/Vector3.hpp>
#include <gen/InputEvent.hpp>
#include <gen/Object.hpp>
#include <gen/RayCast.hpp>
#include <gen/StaticBody.hpp>

namespace gast {

namespace {
using namespace godot;
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
        // Get the location of this node
        return Vector2(0, 0);
    }

    void handle_ray_cast_input(const RayCast &ray_cast);
};
}  // namespace gast

#endif // GAST_NODE_SCRIPT_H
