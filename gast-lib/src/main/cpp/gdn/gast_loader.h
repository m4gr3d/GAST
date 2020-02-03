#ifndef GAST_LOADER_H
#define GAST_LOADER_H

#include <core/Godot.hpp>
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
    };
}  // namespace gast


#endif // GAST_LOADER_H
