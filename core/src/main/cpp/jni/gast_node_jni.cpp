#include <jni.h>
#include <core/Defs.hpp>
#include <core/String.hpp>
#include <core/Vector2.hpp>
#include <core/Vector3.hpp>
#include "gdn/gast_node.h"
#include "gdn/projection_mesh/projection_mesh.h"
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

inline GastNode *from_pointer(jlong gast_node_pointer) {
    return reinterpret_cast<GastNode *>(gast_node_pointer);
}

inline jlong to_pointer(GastNode *gast_node) {
    return reinterpret_cast<intptr_t>(gast_node);
}

inline jlong to_pointer(ProjectionMesh *projection_mesh) {
    return reinterpret_cast<intptr_t>(projection_mesh);
}

}  // namespace

extern "C" {

JNIEXPORT jlong JNICALL
JNI_METHOD(acquireAndBindGastNode)(JNIEnv *env, jobject, jstring parent_node_path,
                                   jboolean empty_parent) {
    return to_pointer(GastManager::get_singleton_instance()->acquire_and_bind_gast_node(
            jstring_to_string(env, parent_node_path), empty_parent));
}

JNIEXPORT void JNICALL
JNI_METHOD(unbindAndReleaseGastNode)(JNIEnv *, jobject, jlong node_pointer) {
    GastManager::get_singleton_instance()->unbind_and_release_gast_node(from_pointer(node_pointer));
}

JNIEXPORT jstring JNICALL JNI_METHOD(nativeGetNodePath)(JNIEnv *env, jobject, jlong node_pointer) {
    String node_path = String("");
    GastNode *gast_node = from_pointer(node_pointer);
    if (gast_node) {
        node_path = gast_node->get_path();
    }

    return string_to_jstring(env, node_path);
}

JNIEXPORT void JNICALL
JNI_METHOD(nativeSetName)(JNIEnv *env, jobject, jlong node_pointer, jstring new_name) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_name(jstring_to_string(env, new_name));
}

JNIEXPORT jboolean JNICALL
JNI_METHOD(updateGastNodeParent)(JNIEnv *env, jobject, jlong node_pointer,
                                 jstring new_parent_node_path, jboolean empty_parent) {
    return GastManager::get_singleton_instance()->update_gast_node_parent(
            from_pointer(node_pointer),
            jstring_to_string(env, new_parent_node_path), empty_parent);
}

JNIEXPORT jint JNICALL
JNI_METHOD(getTextureId)(JNIEnv *, jobject, jlong node_pointer) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL_V(gast_node, kInvalidTexId);
    return gast_node->get_external_texture_id();
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeVisibility)(JNIEnv *, jobject, jlong node_pointer,
                                     jboolean should_duplicate_parent_visibility,
                                     jboolean visible) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL(gast_node);
    bool is_visible = should_duplicate_parent_visibility ? gast_node->is_visible_in_tree()
                                                         : gast_node->is_visible();
    if (is_visible != visible) {
        gast_node->set_visible(visible);
    }
}

JNIEXPORT void JNICALL
JNI_METHOD(setGastNodeCollidable)(JNIEnv *, jobject, jlong node_pointer, jboolean collidable) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_collidable(collidable);
}

JNIEXPORT jboolean JNICALL
JNI_METHOD(isGastNodeCollidable)(JNIEnv *, jobject, jlong node_pointer) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL_V(gast_node, kDefaultCollidable);
    return gast_node->is_collidable();
}

JNIEXPORT jboolean JNICALL JNI_METHOD(isGazeTracking)(JNIEnv *, jobject, jlong node_pointer) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL_V(gast_node, kDefaultGazeTracking);
    return gast_node->is_gaze_tracking();
}

JNIEXPORT void JNICALL
JNI_METHOD(setGazeTracking)(JNIEnv *, jobject, jlong node_pointer, jboolean gaze_tracking) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_gaze_tracking(gaze_tracking);
}

JNIEXPORT jboolean JNICALL JNI_METHOD(isRenderOnTop)(JNIEnv *, jobject, jlong node_pointer) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL_V(gast_node, kDefaultRenderOnTop);
    return gast_node->is_render_on_top();
}

JNIEXPORT void JNICALL
JNI_METHOD(setRenderOnTop)(JNIEnv *, jobject, jlong node_pointer, jboolean render_on_top) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_render_on_top(render_on_top);
}

JNIEXPORT jlong JNICALL JNI_METHOD(getCollisionLayers)(JNIEnv*, jobject, jlong node_pointer) {
  GastNode* gast_node = from_pointer(node_pointer);
  ERR_FAIL_NULL_V(gast_node, false);
  return gast_node->get_collision_layer();
}

JNIEXPORT void JNICALL
JNI_METHOD(setCollisionLayers)(JNIEnv*, jobject, jlong node_pointer, jlong layers) {
  GastNode* gast_node = from_pointer(node_pointer);
  ERR_FAIL_NULL(gast_node);
  gast_node->set_collision_layer(layers);
}

JNIEXPORT jlong JNICALL JNI_METHOD(getCollisionMasks)(JNIEnv*, jobject, jlong node_pointer) {
  GastNode* gast_node = from_pointer(node_pointer);
  ERR_FAIL_NULL_V(gast_node, false);
  return gast_node->get_collision_mask();
}

JNIEXPORT void JNICALL
JNI_METHOD(setCollisionMasks)(JNIEnv*, jobject, jlong node_pointer, jlong masks) {
  GastNode* gast_node = from_pointer(node_pointer);
  ERR_FAIL_NULL(gast_node);
  gast_node->set_collision_mask(masks);
}

JNIEXPORT void JNICALL JNI_METHOD(updateAlpha)(JNIEnv *, jobject, jlong node_pointer, jfloat alpha) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_alpha(alpha);
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeLocalTranslation)(JNIEnv *, jobject, jlong node_pointer,
                                           jfloat x_translation, jfloat y_translation,
                                           jfloat z_translation) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_translation(Vector3(x_translation, y_translation, z_translation));
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeLocalScale)(JNIEnv *, jobject, jlong node_pointer, jfloat x_scale,
                                     jfloat y_scale, jfloat z_scale) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_scale(Vector3(x_scale, y_scale, z_scale));
}

JNIEXPORT void JNICALL
JNI_METHOD(updateGastNodeLocalRotation)(JNIEnv *, jobject, jlong node_pointer,
                                        jfloat x_rotation, jfloat y_rotation, jfloat z_rotation) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_rotation_degrees(Vector3(x_rotation, y_rotation, z_rotation));
}

JNIEXPORT jlong JNICALL
JNI_METHOD(nativeGetProjectionMesh)(JNIEnv *, jobject, jlong node_pointer) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL_V(gast_node, 0);
    return to_pointer(gast_node->get_projection_mesh());
}

JNIEXPORT jint JNICALL
JNI_METHOD(nativeGetProjectionMeshType)(JNIEnv *, jobject, jlong node_pointer) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL_V(gast_node, ProjectionMesh::ProjectionMeshType::RECTANGULAR);
    return gast_node->get_projection_mesh_type();
}

JNIEXPORT void JNICALL
JNI_METHOD(nativeSetProjectionMesh)(JNIEnv *, jobject, jlong node_pointer,
                                    jint projection_mesh_type) {
    GastNode *gast_node = from_pointer(node_pointer);
    ERR_FAIL_NULL(gast_node);
    gast_node->set_projection_mesh(projection_mesh_type);
}

}
