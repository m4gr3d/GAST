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

}
