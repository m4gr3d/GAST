#include "gast_manager_jni.h"

#include "gast_manager.h"
#include "utils.h"

using namespace gast;

extern "C" {
JNIEXPORT jint JNICALL
JNI_METHOD(getExternalTextureId)(JNIEnv *env, jclass clazz, jstring node_group_name,
                                 jstring external_texture_param_name) {
    return GastManager::get_singleton_instance()->get_external_texture_id(
            jstring_to_string(env, node_group_name),
            jstring_to_string(env, external_texture_param_name));
}
};
