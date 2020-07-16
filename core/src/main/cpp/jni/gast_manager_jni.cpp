#include <jni.h>
#include "gast_manager.h"
#include "utils.h"

// Current class and package names assumed for the Java side.
#undef JNI_PACKAGE_NAME
#define JNI_PACKAGE_NAME org_godotengine_plugin_gast

#undef JNI_CLASS_NAME
#define JNI_CLASS_NAME GastManager

using namespace gast;

extern "C" {

JNIEXPORT void JNICALL JNI_METHOD(initialize)(JNIEnv *env, jobject object) {
    GastManager::jni_initialize(env, object);
}

JNIEXPORT void JNICALL JNI_METHOD(shutdown)(JNIEnv *env, jobject) {
    GastManager::jni_shutdown(env);
}

JNIEXPORT void JNICALL
JNI_METHOD(setInputActionsToMonitor)(JNIEnv *env, jobject, jobjectArray input_actions_to_monitor) {
    GastManager::get_singleton_instance()->reset_monitored_input_actions();

    int count = env->GetArrayLength(input_actions_to_monitor);
    for (int i = 0; i < count; i++) {
        auto input_action = (jstring) (env->GetObjectArrayElement(input_actions_to_monitor, i));
        GastManager::get_singleton_instance()->add_input_actions_to_monitor(
                jstring_to_string(env, input_action));
    }
}

JNIEXPORT jint JNICALL
JNI_METHOD(getExternalTextureId)(JNIEnv *env, jobject, jstring node_path, jint surface_index) {
    return GastManager::get_singleton_instance()->get_external_texture_id(
        jstring_to_string(env, node_path), surface_index);
}

JNIEXPORT jstring JNICALL
JNI_METHOD(acquireAndBindGastNode)(JNIEnv *env, jobject, jstring parent_node_path,
                                   jboolean empty_parent) {
    return string_to_jstring(env, GastManager::get_singleton_instance()->acquire_and_bind_gast_node(
        jstring_to_string(env, parent_node_path), empty_parent));
}

JNIEXPORT void JNICALL
JNI_METHOD(unbindAndReleaseGastNode)(JNIEnv *env, jobject, jstring node_path) {
    GastManager::get_singleton_instance()->unbind_and_release_gast_node(
        jstring_to_string(env, node_path));
}

JNIEXPORT jstring JNICALL
JNI_METHOD(updateGastNodeParent)(JNIEnv *env, jobject, jstring node_path,
                                 jstring new_parent_node_path, jboolean empty_parent) {
    return string_to_jstring(env,
                             GastManager::get_singleton_instance()->update_gast_node_parent(
                                 jstring_to_string(env, node_path),
                                 jstring_to_string(env, new_parent_node_path), empty_parent));
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeVisibility)(JNIEnv *env, jobject, jstring node_path,
                                     jboolean should_duplicate_parent_visibility,
                                     jboolean visible) {
    GastManager::get_singleton_instance()->update_gast_node_visibility(
        jstring_to_string(env, node_path), should_duplicate_parent_visibility, visible);
}

JNIEXPORT void JNICALL
JNI_METHOD(setGastNodeCollidable)(JNIEnv *env, jobject, jstring node_path, jboolean collidable) {
    GastManager::get_singleton_instance()->set_gast_node_collidable(
        jstring_to_string(env, node_path), collidable);
}

JNIEXPORT jboolean JNICALL
JNI_METHOD(isGastNodeCollidable)(JNIEnv *env, jobject, jstring node_path) {
    return GastManager::get_singleton_instance()->is_gast_node_collidable(
        jstring_to_string(env, node_path));
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeSize)(JNIEnv *env, jobject, jstring node_path, jfloat width,
                               jfloat height) {
    GastManager::get_singleton_instance()->update_gast_node_size(jstring_to_string(env, node_path),
                                                                 width, height);
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeLocalTranslation)(JNIEnv *env, jobject, jstring node_path,
                                           jfloat x_translation, jfloat y_translation,
                                           jfloat z_translation) {
    GastManager::get_singleton_instance()->update_gast_node_local_translation(
        jstring_to_string(env, node_path), x_translation, y_translation, z_translation);
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeLocalScale)(JNIEnv *env, jobject, jstring node_path, jfloat x_scale,
                                     jfloat y_scale) {
    GastManager::get_singleton_instance()->update_gast_node_local_scale(
        jstring_to_string(env, node_path), x_scale, y_scale);
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeLocalRotation)(JNIEnv *env, jobject, jstring node_path,
                                        jfloat x_rotation, jfloat y_rotation, jfloat z_rotation) {
    GastManager::get_singleton_instance()->update_gast_node_local_rotation(
        jstring_to_string(env, node_path), x_rotation, y_rotation, z_rotation);
}
};
