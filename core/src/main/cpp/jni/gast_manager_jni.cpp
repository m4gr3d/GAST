#include <jni.h>
#include "gast_manager.h"
#include "utils.h"

// Current class and package names assumed for the Java side.
#undef JNI_PACKAGE_NAME
#define JNI_PACKAGE_NAME com_google_vr_youtube_gast

#undef JNI_CLASS_NAME
#define JNI_CLASS_NAME GastManager

using namespace gast;

extern "C" {

JNIEXPORT void JNICALL JNI_METHOD(initialize)(JNIEnv *env, jobject object) {
    GastManager::jni_initialize(env, object);
}

JNIEXPORT void JNICALL JNI_METHOD(shutdown)(JNIEnv *env, jobject object) {
    GastManager::jni_shutdown(env);
}

JNIEXPORT jint JNICALL
JNI_METHOD(getExternalTextureId)(JNIEnv *env, jobject, jstring node_path) {
    return GastManager::get_singleton_instance()->get_external_texture_id(
            jstring_to_string(env, node_path));
}

JNIEXPORT jstring JNICALL
JNI_METHOD(acquireAndBindMeshInstance)(JNIEnv *env, jobject, jstring parent_node_path) {
    return string_to_jstring(env,
                             GastManager::get_singleton_instance()->acquire_and_bind_mesh_instance(
                                     jstring_to_string(env, parent_node_path)));
}

JNIEXPORT jboolean JNICALL
JNI_METHOD(bindMeshInstance)(JNIEnv *env, jobject, jstring node_path) {
    return static_cast<jboolean>(GastManager::get_singleton_instance()->bind_mesh_instance(
            jstring_to_string(env, node_path)));
}

JNIEXPORT void JNICALL JNI_METHOD(unbindMeshInstance)(JNIEnv *env, jobject, jstring node_path) {
    GastManager::get_singleton_instance()->unbind_mesh_instance(
            jstring_to_string(env, node_path));
}

JNIEXPORT void JNICALL
JNI_METHOD(unbindAndReleaseMeshInstance)(JNIEnv *env, jobject, jstring node_path) {
    GastManager::get_singleton_instance()->unbind_and_release_mesh_instance(
            jstring_to_string(env, node_path));
}

JNIEXPORT jstring JNICALL
JNI_METHOD(updateMeshInstanceParent)(JNIEnv *env, jobject object, jstring node_path,
                                     jstring new_parent_node_path) {
    return string_to_jstring(env,
                             GastManager::get_singleton_instance()->update_mesh_instance_parent(
                                     jstring_to_string(env, node_path),
                                     jstring_to_string(env, new_parent_node_path)));
}

JNIEXPORT void JNICALL
JNI_METHOD(updateMeshInstanceVisibility)(JNIEnv *env, jobject object, jstring node_path,
                                         jboolean should_duplicate_parent_visibility,
                                         jboolean visible) {
    GastManager::get_singleton_instance()->update_mesh_instance_visibility(
            jstring_to_string(env, node_path), should_duplicate_parent_visibility, visible);
}

JNIEXPORT void JNICALL
JNI_METHOD(updateMeshInstanceSize)(JNIEnv *env, jobject object, jstring node_path, jfloat width,
                                   jfloat height) {
    GastManager::get_singleton_instance()->update_mesh_instance_size(
            jstring_to_string(env, node_path), width, height);
}

JNIEXPORT void JNICALL
JNI_METHOD(updateMeshInstanceTranslation)(JNIEnv *env, jobject object, jstring node_path,
                                          jfloat x_translation, jfloat y_translation, jfloat z_translation) {
    GastManager::get_singleton_instance()->update_mesh_instance_translation(
            jstring_to_string(env, node_path), x_translation, y_translation, z_translation);
}

JNIEXPORT void JNICALL
JNI_METHOD(updateMeshInstanceScale)(JNIEnv *env, jobject object, jstring node_path, jfloat x_scale,
                                    jfloat y_scale) {
    GastManager::get_singleton_instance()->update_mesh_instance_scale(
            jstring_to_string(env, node_path), x_scale, y_scale);
}

JNIEXPORT void JNICALL
JNI_METHOD(updateMeshInstanceRotation)(JNIEnv *env, jobject object, jstring node_path,
                                       jfloat x_rotation, jfloat y_rotation, jfloat z_rotation) {
    GastManager::get_singleton_instance()->update_mesh_instance_rotation(
            jstring_to_string(env, node_path), x_rotation, y_rotation, z_rotation);
}
};
