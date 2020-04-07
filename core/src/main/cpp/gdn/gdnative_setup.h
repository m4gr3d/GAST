#ifndef GAST_GDN_SETUP_H
#define GAST_GDN_SETUP_H

#include <gdnative_api_struct.gen.h>

#ifdef __cplusplus
extern "C" {
#endif

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *options);
void GDN_EXPORT godot_gdnative_singleton();
void GDN_EXPORT godot_nativescript_init(void *handle);
void GDN_EXPORT godot_nativescript_terminate(void *handle);
void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *options);

#ifdef __cplusplus
}
#endif

#endif // GAST_GDN_SETUP_H
