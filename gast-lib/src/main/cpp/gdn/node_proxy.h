#ifndef NODE_PROXY_H
#define NODE_PROXY_H

#include <core/Godot.hpp>
#include <gen/InputEvent.hpp>
#include <gen/Node.hpp>

namespace gast {

    namespace {
        using namespace godot;
    } //  namespace

    class NodeProxy : public Node {
        GODOT_CLASS(NodeProxy, Node)

    public:
        NodeProxy();
        ~NodeProxy();

        static void _register_methods();

        void _init();

        void _process(float delta);

        void _input(const Ref<InputEvent> event);

    };
}  // namespace gast

#endif // NODE_PROXY_H
