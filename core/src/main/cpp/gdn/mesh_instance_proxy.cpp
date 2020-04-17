#include <gast_manager.h>
#include "mesh_instance_proxy.h"
#include "utils.h"
#include <core/String.hpp>
#include <core/NodePath.hpp>

namespace gast {

    MeshInstanceProxy::MeshInstanceProxy() {}

    MeshInstanceProxy::~MeshInstanceProxy() {}

    void MeshInstanceProxy::_register_methods() {
        register_method("_process", &MeshInstanceProxy::_process);
        register_method("_input", &MeshInstanceProxy::_input);
    }

    void MeshInstanceProxy::_init() {
        ALOGV("Initializing MeshInstanceProxy class for %s.", ((String)get_path()).utf8().get_data());
    }

    void MeshInstanceProxy::_input(const godot::Ref<godot::InputEvent> event) {
        GastManager::get_singleton_instance()->on_render_input(get_path(), event);
    }

    void MeshInstanceProxy::_process(float delta) {
        GastManager::get_singleton_instance()->on_render_process(get_path(), delta);
    }
}