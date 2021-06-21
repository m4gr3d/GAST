#include <core/Godot.hpp>
#include <utils.h>
#include "gdnative_setup.h"
#include "gast_loader.h"
#include "gast_node.h"

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *options) {
    godot::Godot::gdnative_init(options);
}

void GDN_EXPORT godot_gdnative_singleton() {}

void GDN_EXPORT godot_nativescript_init(void *handle) {
    godot::Godot::nativescript_init(handle);

    godot::register_class<gast::GastLoader>();
    godot::register_class<gast::GastNode>();
    godot::register_class<gast::ProjectionMesh>();
    godot::register_class<gast::RectangularProjectionMesh>();
    godot::register_class<gast::EquirectangularProjectionMesh>();
    godot::register_class<gast::CustomProjectionMesh>();
}

void GDN_EXPORT godot_nativescript_terminate(void *handle) {
    godot::Godot::nativescript_terminate(handle);
}

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *options) {
    godot::Godot::gdnative_terminate(options);
}
