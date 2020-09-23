#include <jni.h>
#include <core/Defs.hpp>
#include <core/String.hpp>
#include <core/Vector2.hpp>
#include <core/Vector3.hpp>
#include "gdn/gast_node.h"
#include "gast_manager.h"
#include "utils.h"

// Current class and package names assumed for the Java side.
#undef JNI_PACKAGE_NAME
#define JNI_PACKAGE_NAME org_godotengine_plugin_gast

#undef JNI_CLASS_NAME
#define JNI_CLASS_NAME GastNode

namespace {
using namespace gast;
using namespace godot;

inline GastNode *get_gast_node(JNIEnv *env, jstring node_path) {
    return GastManager::get_singleton_instance()->get_gast_node(jstring_to_string(env, node_path));
}

}  // namespace

extern "C" {

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

JNIEXPORT jint JNICALL
JNI_METHOD(getTextureId)(JNIEnv *env, jobject, jstring node_path, jint surface_index) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL_V(gast_node, kInvalidTexId);
    return gast_node->get_external_texture_id(surface_index);
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeVisibility)(JNIEnv *env, jobject, jstring node_path,
                                     jboolean should_duplicate_parent_visibility,
                                     jboolean visible) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL(gast_node);
    bool is_visible = should_duplicate_parent_visibility ? gast_node->is_visible_in_tree()
                                                         : gast_node->is_visible();
    if (is_visible != visible) {
        gast_node->set_visible(visible);
    }
}

JNIEXPORT void JNICALL
JNI_METHOD(setGastNodeCollidable)(JNIEnv *env, jobject, jstring node_path, jboolean collidable) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_collidable(collidable);
}

JNIEXPORT jboolean JNICALL
JNI_METHOD(isGastNodeCollidable)(JNIEnv *env, jobject, jstring node_path) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL_V(gast_node, kDefaultCollidable);
    return gast_node->is_collidable();
}

JNIEXPORT void JNICALL
JNI_METHOD(setGastNodeCurved)(JNIEnv *env, jobject, jstring node_path, jboolean curved) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_curved(curved);
}

JNIEXPORT jboolean JNICALL
JNI_METHOD(isGastNodeCurved)(JNIEnv *env, jobject, jstring node_path) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL_V(gast_node, kDefaultCurveValue);
    return gast_node->is_curved();
}

JNIEXPORT jfloat JNICALL
JNI_METHOD(getGastNodeGradientHeightRatio)(JNIEnv *env, jobject, jstring node_path) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL_V(gast_node, kDefaultGradientHeightRatio);
    return gast_node->get_gradient_height_ratio();
}

JNIEXPORT void JNICALL
JNI_METHOD(setGastNodeGradientHeightRatio)(JNIEnv *env, jobject, jstring node_path, jfloat ratio) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_gradient_height_ratio(ratio);
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeSize)(JNIEnv *env, jobject, jstring node_path, jfloat width,
                               jfloat height) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_size(Vector2(width, height));
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeLocalTranslation)(JNIEnv *env, jobject, jstring node_path,
                                           jfloat x_translation, jfloat y_translation,
                                           jfloat z_translation) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_translation(Vector3(x_translation, y_translation, z_translation));
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeLocalScale)(JNIEnv *env, jobject, jstring node_path, jfloat x_scale,
                                     jfloat y_scale) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_scale(Vector3(x_scale, y_scale, 1));
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeLocalRotation)(JNIEnv *env, jobject, jstring node_path,
                                        jfloat x_rotation, jfloat y_rotation, jfloat z_rotation) {
    GastNode *gast_node = get_gast_node(env, node_path);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_rotation_degrees(Vector3(x_rotation, y_rotation, z_rotation));
}

}
