#ifndef MESH_INSTANCE_PROXY_H
#define MESH_INSTANCE_PROXY_H

#include <core/Godot.hpp>
#include <gen/InputEvent.hpp>
#include <gen/MeshInstance.hpp>

namespace gast {

    namespace {
        using namespace godot;
    } //  namespace

    class MeshInstanceProxy : public MeshInstance {
        GODOT_CLASS(MeshInstanceProxy, MeshInstance)

    public:
        MeshInstanceProxy();
        ~MeshInstanceProxy();

        static void _register_methods();

        void _init();

        void _process(float delta);

        void _input(const Ref<InputEvent> event);

    };
}  // namespace gast

#endif // MESH_INSTANCE_PROXY_H
