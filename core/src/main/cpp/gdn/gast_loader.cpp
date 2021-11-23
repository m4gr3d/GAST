#include "gast_loader.h"
#include <gast_manager.h>

namespace gast {

namespace {
const char *kHoverInputEvent = "hover_input_event";
const char *kPressInputEvent = "press_input_event";
const char *kReleaseInputEvent = "release_input_event";
const char *kScrollInputEvent = "scroll_input_event";
}

GastLoader::GastLoader() {}

GastLoader::~GastLoader() {}

void GastLoader::_init() {}

void GastLoader::_register_methods() {
    // Register methods
    register_method("initialize", &GastLoader::initialize);
    register_method("shutdown", &GastLoader::shutdown);
    register_method("on_physics_process", &GastLoader::on_physics_process);
    register_method("get_external_texture", &GastLoader::get_external_texture);
    register_method("get_shader_materials", &GastLoader::get_shader_materials);

    // Register signals
    Dictionary common_event_args;
    common_event_args[Variant("node_path")] = Variant(Variant::STRING);
    common_event_args[Variant("event_origin_id")] = Variant(Variant::STRING);
    common_event_args[Variant("x_percent")] = Variant(Variant::REAL);
    common_event_args[Variant("y_percent")] = Variant(Variant::REAL);

    register_signal<GastLoader>(kHoverInputEvent, common_event_args);
    register_signal<GastLoader>(kPressInputEvent, common_event_args);
    register_signal<GastLoader>(kReleaseInputEvent, common_event_args);

    Dictionary scroll_event_args = Dictionary(common_event_args);
    scroll_event_args[Variant("horizontal_delta")] = Variant(Variant::REAL);
    scroll_event_args[Variant("vertical_delta")] = Variant(Variant::REAL);

    register_signal<GastLoader>(kScrollInputEvent, scroll_event_args);
}

void GastLoader::initialize() {
    GastManager::gdn_initialize(this);
}

void GastLoader::shutdown() {
    GastManager::gdn_shutdown();
}

void GastLoader::on_physics_process() {
    GastManager::get_singleton_instance()->on_physics_process();
}

Ref<ExternalTexture> GastLoader::get_external_texture(const String gast_node_path) {
    GastNode* gast_node = GastManager::get_singleton_instance()->get_gast_node(gast_node_path);
    if (!gast_node) {
        return nullptr;
    }
    return gast_node->get_external_texture();
}

Array GastLoader::get_shader_materials(const String gast_node_path) {
    GastNode* gast_node = GastManager::get_singleton_instance()->get_gast_node(gast_node_path);
    if (!gast_node) {
        Array result;
        return result;
    }

    return gast_node->get_shader_materials();
}

void
GastLoader::emitHoverEvent(const String &node_path, const String &event_origin_id, float x_percent,
                           float y_percent) {
    emit_signal(kHoverInputEvent, node_path, event_origin_id, x_percent, y_percent);
}

void
GastLoader::emitPressEvent(const String &node_path, const String &event_origin_id, float x_percent,
                           float y_percent) {
    emit_signal(kPressInputEvent, node_path, event_origin_id, x_percent, y_percent);
}

void GastLoader::emitReleaseEvent(const String &node_path, const String &event_origin_id,
                                  float x_percent, float y_percent) {
    emit_signal(kReleaseInputEvent, node_path, event_origin_id, x_percent, y_percent);
}

void
GastLoader::emitScrollEvent(const String &node_path, const String &event_origin_id, float x_percent,
                            float y_percent, float horizontal_delta, float vertical_delta) {
    emit_signal(kScrollInputEvent, node_path, event_origin_id, x_percent, y_percent,
                horizontal_delta, vertical_delta);
}
}
