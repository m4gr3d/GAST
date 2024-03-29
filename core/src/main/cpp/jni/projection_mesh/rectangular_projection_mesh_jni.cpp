#include <jni.h>

#include "gdn/projection_mesh/rectangular_projection_mesh.h"
#include "utils.h"

// Current class and package names assumed for the Java side.
#undef JNI_PACKAGE_NAME
#define JNI_PACKAGE_NAME org_godotengine_plugin_gast_projectionmesh

#undef JNI_CLASS_NAME
#define JNI_CLASS_NAME RectangularProjectionMesh

namespace {
using namespace gast;
using namespace godot;

inline RectangularProjectionMesh *from_pointer(jlong rectangular_projection_mesh_pointer) {
    return reinterpret_cast<RectangularProjectionMesh *>(rectangular_projection_mesh_pointer);
}

}  // namespace

extern "C" {

JNIEXPORT void JNICALL
JNI_METHOD(nativeSetMeshSize)(JNIEnv *, jobject, jlong mesh_pointer, jfloat width, jfloat height) {
    RectangularProjectionMesh *mesh = from_pointer(mesh_pointer);
    ERR_FAIL_NULL(mesh);
    mesh->set_mesh_size(Vector2(width, height));
}

JNIEXPORT void JNICALL
JNI_METHOD(nativeSetCurved)(JNIEnv *, jobject, jlong mesh_pointer, jboolean curved) {
    RectangularProjectionMesh *mesh = from_pointer(mesh_pointer);
    ERR_FAIL_NULL(mesh);
    mesh->set_curved(curved);
}

JNIEXPORT jfloat JNICALL JNI_METHOD(getGradientHeightRatio)(JNIEnv *, jobject, jlong mesh_pointer) {
    RectangularProjectionMesh *mesh = from_pointer(mesh_pointer);
    ERR_FAIL_NULL_V(mesh, kDefaultGradientHeightRatio);
    return mesh->get_gradient_height_ratio();
}

JNIEXPORT void JNICALL
JNI_METHOD(setGradientHeightRatio)(JNIEnv *, jobject, jlong mesh_pointer, jfloat ratio) {
    RectangularProjectionMesh *mesh = from_pointer(mesh_pointer);
    ERR_FAIL_NULL(mesh);
    mesh->set_gradient_height_ratio(ratio);
}

}
