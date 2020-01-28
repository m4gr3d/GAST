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

        int get_external_texture_id(const String& mesh_name, const String& texture_param_name);

        void register_callback(JNIEnv* env, jobject callback);

        void unregister_callback();

        void on_gl_process(float delta);

        void on_gl_input_hover(float x_percent, float y_percent);

        void on_gl_input_press(float x_percent, float y_percent);

        void on_gl_input_release(float x_percent, float y_percent);

    private:
        ExternalTexture *
        get_external_texture(const String& mesh_name, const String& texture_param_name);

        MeshInstance *get_mesh_instance(const String& mesh_name);

        GastNodeManager();

        ~GastNodeManager();

        static GastNodeManager *singleton_instance;

        jobject callback_instance;
        jclass callback_class;

        jmethodID on_gl_process_;
        jmethodID on_gl_input_hover_;
        jmethodID on_gl_input_press_;
        jmethodID on_gl_input_release_;
    };
}  // namespace gast

#endif // GAST_MANAGER_H
