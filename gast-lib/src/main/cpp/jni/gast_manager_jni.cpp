#include "gast_manager_jni.h"

#include "gast_node_manager.h"
#include "utils.h"

using namespace gast;

extern "C" {
JNIEXPORT jint JNICALL
JNI_METHOD(getExternalTextureId)(JNIEnv *env, jclass clazz, jstring node_path) {
    return GastNodeManager::get_singleton_instance()->get_external_texture_id(
            jstring_to_string(env, node_path));
}

JNIEXPORT void JNICALL JNI_METHOD(setupMeshInstance)(JNIEnv *env, jclass clazz, jstring node_path) {
    GastNodeManager::get_singleton_instance()->setup_mesh_instance(
            jstring_to_string(env, node_path));
}

JNIEXPORT jstring JNICALL
JNI_METHOD(createMeshInstance)(JNIEnv *env, jclass clazz, jstring parent_node_path) {
    return string_to_jstring(env, GastNodeManager::get_singleton_instance()->create_mesh_instance(
            jstring_to_string(env, parent_node_path)));
}

JNIEXPORT void JNICALL JNI_METHOD(registerCallback)(JNIEnv *env, jobject object) {
    GastNodeManager::get_singleton_instance()->register_callback(env, object);
}

JNIEXPORT void JNICALL JNI_METHOD(unregisterCallback)(JNIEnv *env, jobject object) {
    GastNodeManager::get_singleton_instance()->unregister_callback(env);
}
};
