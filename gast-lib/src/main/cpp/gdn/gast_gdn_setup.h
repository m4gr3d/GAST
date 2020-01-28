#ifndef GAST_GDN_SETUP_H
#define GAST_GDN_SETUP_H

#include <gdnative_api_struct.gen.h>

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* options);
extern "C" void GDN_EXPORT godot_gdnative_singleton();
extern "C" void GDN_EXPORT godot_nativescript_init(void *handle);
extern "C" void GDN_EXPORT godot_nativescript_terminate(void *handle);
extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* options);

#endif // GAST_GDN_SETUP_H
