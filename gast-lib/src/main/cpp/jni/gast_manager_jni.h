#ifndef GAST_MANAGER_JNI_H
#define GAST_MANAGER_JNI_H

#include <jni.h>
#include "utils.h"

// Current class and package names assumed for the Java side.
#undef JNI_PACKAGE_NAME
#define JNI_PACKAGE_NAME com_google_vr_youtube_gast

#undef JNI_CLASS_NAME
#define JNI_CLASS_NAME GastManager

extern "C" {

JNIEXPORT void JNICALL JNI_METHOD(initialize)(JNIEnv *env, jobject object);

JNIEXPORT void JNICALL JNI_METHOD(shutdown)(JNIEnv *env, jobject object);

JNIEXPORT jint JNICALL
JNI_METHOD(getExternalTextureId)(JNIEnv *env, jobject object, jstring node_path);

JNIEXPORT void JNICALL
JNI_METHOD(unbindMeshInstance)(JNIEnv *env, jobject object, jstring node_path);

JNIEXPORT jstring JNICALL
JNI_METHOD(bindMeshInstance)(JNIEnv *env, jobject object, jstring parent_node_path);

JNIEXPORT jstring JNICALL
JNI_METHOD(updateMeshInstanceParent)(JNIEnv *env, jobject object, jstring node_path,
                                     jstring new_parent_node_path);

JNIEXPORT void JNICALL
JNI_METHOD(updateMeshInstanceVisibility)(JNIEnv *env, jobject object, jstring node_path,
                                         jboolean should_duplicate_parent_visibility,
                                         jboolean visible);

JNIEXPORT void JNICALL
JNI_METHOD(updateMeshInstanceSize)(JNIEnv *env, jobject object, jstring node_path, jfloat width,
                                   jfloat height);

JNIEXPORT void JNICALL
JNI_METHOD(updateMeshInstanceTranslation)(JNIEnv *env, jobject object, jstring node_path,
                                          jfloat x_translation, jfloat y_translation,
                                          jfloat z_translation);

JNIEXPORT void JNICALL
JNI_METHOD(updateMeshInstanceScale)(JNIEnv *env, jobject object, jstring node_path, jfloat x_scale,
                                    jfloat y_scale);

JNIEXPORT void JNICALL
JNI_METHOD(updateMeshInstanceRotation)(JNIEnv *env, jobject object, jstring node_path,
                                       jfloat x_rotation, jfloat y_rotation, jfloat z_rotation);

};

#endif // GAST_MANAGER_JNI_H
