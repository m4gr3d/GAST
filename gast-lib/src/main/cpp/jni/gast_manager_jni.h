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

JNIEXPORT jint JNICALL
JNI_METHOD(getExternalTextureId)(JNIEnv *env, jclass clazz, jstring node_path);

JNIEXPORT void JNICALL JNI_METHOD(registerCallback)(JNIEnv* env, jobject object);

JNIEXPORT void JNICALL JNI_METHOD(unregisterCallback)(JNIEnv* env, jobject object);
};

#endif // GAST_MANAGER_JNI_H
