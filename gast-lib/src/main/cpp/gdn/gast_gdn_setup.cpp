#include <core/Godot.hpp>
#include <utils.h>
#include "gast_gdn_setup.h"

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* options) {
    godot::Godot::gdnative_init(options);
    // TODO: Remove following implementation and setup of GAST native scripts.
    godot_nativescript_init((void *) options->active_library_path);
}

void GDN_EXPORT godot_gdnative_singleton() {}

void GDN_EXPORT godot_nativescript_init(void *handle) {
    ALOGV("FHK - nativescript init.");
    godot::Godot::nativescript_init(handle);
}

void GDN_EXPORT godot_nativescript_terminate(void *handle) {
    godot::Godot::nativescript_terminate(handle);
}

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* options) {
    godot::Godot::gdnative_terminate(options);
}