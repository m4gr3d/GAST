#ifndef GAST_NODE_SCRIPT_H
#define GAST_NODE_SCRIPT_H

#include <core/Godot.hpp>
#include <core/Vector3.hpp>
#include <gen/InputEvent.hpp>
#include <gen/Object.hpp>
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

};
}  // namespace gast

#endif // GAST_NODE_SCRIPT_H
