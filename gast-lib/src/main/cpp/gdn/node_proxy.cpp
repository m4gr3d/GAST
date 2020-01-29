#include <gast_node_manager.h>
#include "node_proxy.h"

namespace gast {

    NodeProxy::NodeProxy() {}

    NodeProxy::~NodeProxy() {}

    void NodeProxy::_register_methods() {
        register_method("_process", &NodeProxy::_process);
        register_method("_input", &NodeProxy::_input);
    }

    void NodeProxy::_init() {}

    void NodeProxy::_input(const godot::Ref<godot::InputEvent> event) {

    }

    void NodeProxy::_process(float delta) {
        GastNodeManager::get_singleton_instance()->on_gl_process(get_path(), delta);
    }
}