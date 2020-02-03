#ifndef GAST_MANAGER_H
#define GAST_MANAGER_H

#include <core/String.hpp>
#include <gen/ExternalTexture.hpp>
#include <gen/InputEvent.hpp>
#include <gen/MeshInstance.hpp>
#include <jni.h>

namespace gast {

    namespace {
        using namespace godot;
    }  // namespace

    class GastManager {
    public:
        static GastManager *get_singleton_instance();

        static void gdn_initialize();

        static void gdn_shutdown();

        static void jni_initialize(JNIEnv* env, jobject callback);

        static void jni_shutdown(JNIEnv* env);

        int get_external_texture_id(const String& node_path);

        void on_gl_process(const String& node_path, float delta);

        void on_gl_input(const String& node_path, const Ref<InputEvent> event);

        void on_gl_input_hover(const String& node_path, float x_percent, float y_percent);

        void on_gl_input_press(const String& node_path, float x_percent, float y_percent);

        void on_gl_input_release(const String& node_path, float x_percent, float y_percent);

        void setup_mesh_instance(const String& node_path);

        // Creates a mesh instance and returns its node path.
        String create_mesh_instance(const String& parent_node_path);

    private:
        static void delete_singleton_instance();

        static void register_callback(JNIEnv* env, jobject callback);

        static void unregister_callback(JNIEnv* env);

        ExternalTexture *
        get_external_texture(const String& node_path);

        MeshInstance *get_mesh_instance(const String& node_path);

        Node *get_node(const String& node_path);

        GastManager();

        ~GastManager();

        static GastManager *singleton_instance_;
        static bool gdn_initialized_;
        static bool jni_initialized_;

        static jobject callback_instance_;
        static jmethodID on_gl_process_;
        static jmethodID on_gl_input_hover_;
        static jmethodID on_gl_input_press_;
        static jmethodID on_gl_input_release_;
    };
}  // namespace gast

#endif // GAST_MANAGER_H
