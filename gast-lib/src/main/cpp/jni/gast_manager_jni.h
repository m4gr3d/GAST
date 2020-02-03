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

JNIEXPORT void JNICALL JNI_METHOD(setupMeshInstance)(JNIEnv *env, jobject object, jstring node_path);

JNIEXPORT jstring JNICALL
JNI_METHOD(createMeshInstance)(JNIEnv *env, jobject object, jstring parent_node_path);

};

#endif // GAST_MANAGER_JNI_H
