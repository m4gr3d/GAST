#include "gast_loader.h"
#include <gast_manager.h>

namespace gast {

    GastLoader::GastLoader() {}

    GastLoader::~GastLoader() {}

    void GastLoader::_init() {}

    void GastLoader::_register_methods() {
        register_method("initialize", &GastLoader::initialize);
        register_method("shutdown", &GastLoader::shutdown);
        register_method("on_process", &GastLoader::on_process);
        register_method("on_input", &GastLoader::on_input);
    }

    void GastLoader::initialize() {
        GastManager::gdn_initialize();
    }

    void GastLoader::shutdown() {
        GastManager::gdn_shutdown();
    }

    void GastLoader::on_process(const String &node_path, float delta) {
        GastManager::get_singleton_instance()->on_gl_process(node_path, delta);
    }

    void GastLoader::on_input(const String& node_path, const Ref<InputEvent> event) {
        GastManager::get_singleton_instance()->on_gl_input(node_path, event);
    }
}