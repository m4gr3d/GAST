#ifndef GAST_LOADER_H
#define GAST_LOADER_H

#include <core/Godot.hpp>
#include <core/String.hpp>
#include <gen/InputEvent.hpp>
#include <gen/Reference.hpp>

namespace gast {

    namespace {
        using namespace godot;
    }  // namespace

    // Loader for GastManager. The 'initialize()' method must be invoked for
    // GastManager to be properly setup.
    class GastLoader : public Reference {
        GODOT_CLASS(GastLoader, Reference)

    public:
        GastLoader();
        ~GastLoader();

        static void _register_methods();

        void _init();

        // Initialize GastManager
        void initialize();

        // Shutdown GastManager
        void shutdown();

        // Used to forward _process callback to the Gast library
        void on_process(const String& node_path, float delta);

        // Used to forward _input callback to the Gast library
        void on_input(const String& node_path, const Ref<InputEvent> event);
    };
}  // namespace gast


#endif // GAST_LOADER_H
