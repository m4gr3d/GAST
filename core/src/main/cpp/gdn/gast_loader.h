#ifndef GAST_LOADER_H
#define GAST_LOADER_H

#include <core/Array.hpp>
#include <core/Godot.hpp>
#include <core/String.hpp>
#include <core/Ref.hpp>
#include <gen/ExternalTexture.hpp>
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

    void on_physics_process();

    Ref<ExternalTexture> get_external_texture(const String gast_node_path);

    Array get_shader_materials(const String gast_node_path);

    void emitHoverEvent(const String &node_path, const String &event_origin_id, float x_percent,
                        float y_percent);

    void emitPressEvent(const String &node_path, const String &event_origin_id, float x_percent,
                        float y_percent);

    void
    emitReleaseEvent(const String &node_path, const String &event_origin_id, float x_percent,
                     float y_percent);

    void emitScrollEvent(const String &node_path, const String &event_origin_id, float x_percent,
                         float y_percent,
                         float horizontal_delta, float vertical_delta);
};
}  // namespace gast


#endif // GAST_LOADER_H
