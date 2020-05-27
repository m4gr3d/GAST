#include "gast_loader.h"
#include <gast_manager.h>

namespace gast {

GastLoader::GastLoader() {}

GastLoader::~GastLoader() {}

void GastLoader::_init() {}

void GastLoader::_register_methods() {
    register_method("initialize", &GastLoader::initialize);
    register_method("shutdown", &GastLoader::shutdown);
    register_method("on_unhandled_input", &GastLoader::on_unhandled_input);
}

void GastLoader::initialize() {
    GastManager::gdn_initialize();
}

void GastLoader::shutdown() {
    GastManager::gdn_shutdown();
}

void GastLoader::on_unhandled_input(const Ref<InputEvent> event) {
    GastManager::get_singleton_instance()->process_input(event);
}
}
