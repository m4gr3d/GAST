#ifndef UTILS_H
#define UTILS_H

#include <android/log.h>
#include <core/Godot.hpp>
#include <core/String.hpp>
#include <core/Variant.hpp>
#include <gen/Node.hpp>
#include <gen/Object.hpp>

#define LOG_TAG "GAST"

#define ALOG_ASSERT(_cond, ...) \
    if (!(_cond)) __android_log_assert("conditional", LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

/** Auxiliary macros */
#define __JNI_METHOD_BUILD(package, class_name, method) \
    Java_ ## package ## _ ## class_name ## _ ## method
#define __JNI_METHOD_EVAL(package, class_name, method) \
    __JNI_METHOD_BUILD(package, class_name, method)

/**
 * Expands the JNI signature for a JNI method.
 *
 * Requires to redefine the macros JNI_PACKAGE_NAME and JNI_CLASS_NAME.
 * Not doing so will raise preprocessor errors during build.
 *
 * JNI_PACKAGE_NAME must be the JNI representation Java class package name.
 * JNI_CLASS_NAME must be the JNI representation of the Java class name.
 *
 * For example, for the class com.example.package.SomeClass:
 * JNI_PACKAGE_NAME: com_example_package
 * JNI_CLASS_NAME: SomeClass
 *
 * Note that underscores in Java package and class names are replaced with "_1"
 * in their JNI representations.
 */
#define JNI_METHOD(method) \
    __JNI_METHOD_EVAL(JNI_PACKAGE_NAME, JNI_CLASS_NAME, method)

/**
 * Expands a Java class name using slashes as package separators into its
 * JNI type string representation.
 *
 * For example, to get the JNI type representation of a Java String:
 * JAVA_TYPE("java/lang/String")
 */
#define JAVA_TYPE(class_name) "L" class_name ";"

/**
 * Default definitions for the macros required in JNI_METHOD.
 * Used to raise build errors if JNI_METHOD is used without redefining them.
 */
#define JNI_CLASS_NAME "Error: JNI_CLASS_NAME not redefined"
#define JNI_PACKAGE_NAME "Error: JNI_PACKAGE_NAME not redefined"

namespace gast {

using namespace godot;

/**
* Converts JNI jstring to Godot String.
* @param source Source JNI string. If null an empty string is returned.
* @param env JNI environment instance.
* @return Godot string instance.
*/
static inline String jstring_to_string(JNIEnv *env, jstring source) {
    if (env && source) {
        const char *const source_utf8 = env->GetStringUTFChars(source, NULL);
        if (source_utf8) {
            String result(source_utf8);
            env->ReleaseStringUTFChars(source, source_utf8);
            return result;
        }
    }
    return String();
}

/**
 * Converts Godot String to JNI jstring.
 * @param source Source Godot String.
 * @param env JNI environment instance.
 * @return JNI string instance.
 */
static inline jstring string_to_jstring(JNIEnv *env, const String &source) {
    if (env) {
        return env->NewStringUTF(source.utf8().get_data());
    }
    return nullptr;
}

static inline Node *get_node_from_variant(Variant variant) {
    if (variant.get_type() != Variant::OBJECT) {
        return nullptr;
    }

    Node *node = Object::cast_to<Node>(variant);
    return node;
}

}  // namespace gast

#endif // UTILS_H
