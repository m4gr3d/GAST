#ifndef GAST_MANAGER_H
#define GAST_MANAGER_H

#include <core/String.hpp>
#include <gen/ExternalTexture.hpp>
#include <gen/MeshInstance.hpp>
#include <jni.h>

namespace gast {

    namespace {
        using namespace godot;
    }  // namespace

    class GastNodeManager {
    public:
        static GastNodeManager *get_singleton_instance();

        static void delete_singleton_instance();

        int get_external_texture_id(const String& node_path);

        void register_callback(JNIEnv* env, jobject callback);

        void unregister_callback(JNIEnv* env);

        void on_gl_process(const String& node_path, float delta);

        void on_gl_input_hover(const String& node_path, float x_percent, float y_percent);

        void on_gl_input_press(const String& node_path, float x_percent, float y_percent);

        void on_gl_input_release(const String& node_path, float x_percent, float y_percent);

        void setup_mesh_instance(const String& node_path);

        // Creates a mesh instance and returns its node path.
        String create_mesh_instance(const String& parent_node_path);

    private:
        ExternalTexture *
        get_external_texture(const String& node_path);

        MeshInstance *get_mesh_instance(const String& node_path);

        Node *get_node(const String& node_path);

        GastNodeManager();

        ~GastNodeManager();

        static GastNodeManager *singleton_instance;

        jobject callback_instance = nullptr;
        jmethodID on_gl_process_ = nullptr;
        jmethodID on_gl_input_hover_ = nullptr;
        jmethodID on_gl_input_press_ = nullptr;
        jmethodID on_gl_input_release_ = nullptr;
    };
}  // namespace gast

#endif // GAST_MANAGER_H
