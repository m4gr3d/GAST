#include <core/Godot.hpp>
#include <utils.h>
#include "gdnative_setup.h"
#include "gast_loader.h"
#include "gast_node_script.h"

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *options) {
    godot::Godot::gdnative_init(options);
}

void GDN_EXPORT godot_gdnative_singleton() {}

void GDN_EXPORT godot_nativescript_init(void *handle) {
    godot::Godot::nativescript_init(handle);

    godot::register_class<gast::GastLoader>();
    godot::register_class<gast::GastNodeScript>();
}

void GDN_EXPORT godot_nativescript_terminate(void *handle) {
    godot::Godot::nativescript_terminate(handle);
}

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *options) {
    godot::Godot::gdnative_terminate(options);
}
