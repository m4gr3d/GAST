#include "gast_node_script.h"

#include <gast_manager.h>
#include <core/String.hpp>
#include <core/NodePath.hpp>

#include "utils.h"

namespace gast {

GastNodeScript::GastNodeScript() {}

GastNodeScript::~GastNodeScript() {}

void GastNodeScript::_register_methods() {
    register_method("_input_event", &GastNodeScript::_input_event);
}

void GastNodeScript::_init() {
    ALOGV("Initializing GastNodeScript class.");
}

void GastNodeScript::_input_event(const godot::Object *camera,
                                  const godot::Ref<godot::InputEvent> event,
                                  const godot::Vector3 click_position,
                                  const godot::Vector3 click_normal, const int64_t shape_idx) {
    ALOGV("Receiving input event for %s: %s", ((String) get_path()).utf8().get_data(),
          event->as_text().utf8().get_data());

    // TODO: Process the received input event.
}

}  // namespace gast
