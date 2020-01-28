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

    }
}