#include <jni.h>

#include "gdn/projection_mesh/custom_projection_mesh.h"
#include "gdn/projection_mesh/projection_mesh_utils.h"
#include "utils.h"

// Current class and package names assumed for the Java side.
#undef JNI_PACKAGE_NAME
#define JNI_PACKAGE_NAME org_godotengine_plugin_gast_projectionmesh

#undef JNI_CLASS_NAME
#define JNI_CLASS_NAME CustomProjectionMesh

namespace {
using namespace gast;
using namespace godot;

inline CustomProjectionMesh *from_pointer(jlong custom_projection_mesh_pointer) {
    return reinterpret_cast<CustomProjectionMesh *>(custom_projection_mesh_pointer);
}

}  // namespace

extern "C" {

JNIEXPORT void JNICALL JNI_METHOD(nativeSetCustomMesh)(
        JNIEnv *env, jobject, jlong mesh_pointer, jfloatArray vertices_left,
        jfloatArray texture_coords_left, jint draw_mode_int_left,
        jfloatArray vertices_right, jfloatArray texture_coords_right,
        jint draw_mode_int_right, jint mesh_stereo_mode_int, jboolean uv_origin_is_bottom_left) {
    CustomProjectionMesh *projection_mesh = from_pointer(mesh_pointer);
    ERR_FAIL_NULL(projection_mesh);

    jfloat *vertices_left_jfloat = env->GetFloatArrayElements(vertices_left, nullptr);
    jfloat *texture_coords_left_jfloat = env->GetFloatArrayElements(texture_coords_left, nullptr);
    jfloat *vertices_right_jfloat = env->GetFloatArrayElements(vertices_right, nullptr);
    jfloat *texture_coords_right_jfloat = env->GetFloatArrayElements(texture_coords_right, nullptr);

    projection_mesh->set_custom_mesh(env->GetArrayLength(vertices_left) / 3U, vertices_left_jfloat,
                                     texture_coords_left_jfloat, draw_mode_int_left,
                                     env->GetArrayLength(vertices_right) / 3U,
                                     vertices_right_jfloat, texture_coords_right_jfloat,
                                     draw_mode_int_right, mesh_stereo_mode_int,
                                     uv_origin_is_bottom_left);

    env->ReleaseFloatArrayElements(vertices_left, vertices_left_jfloat, 0);
    env->ReleaseFloatArrayElements(texture_coords_left, texture_coords_left_jfloat, 0);
    env->ReleaseFloatArrayElements(vertices_right, vertices_right_jfloat, 0);
    env->ReleaseFloatArrayElements(texture_coords_right, texture_coords_right_jfloat, 0);
}

}
