#ifndef GAST_MANAGER_H
#define GAST_MANAGER_H

#include <core/String.hpp>
#include <gen/ExternalTexture.hpp>
#include <gen/InputEvent.hpp>
#include <gen/MeshInstance.hpp>
#include <jni.h>
#include <list>

namespace gast {

    namespace {
        using namespace godot;
    }  // namespace

    class GastManager {
    public:
        static GastManager *get_singleton_instance();

        static void gdn_initialize();

        static void gdn_shutdown();

        static void jni_initialize(JNIEnv *env, jobject callback);

        static void jni_shutdown(JNIEnv *env);

        int get_external_texture_id(const String &node_path);

        void on_gl_process(const String &node_path, float delta);

        void on_gl_input(const String &node_path, const Ref<InputEvent> event);

        void on_gl_input_hover(const String &node_path, float x_percent, float y_percent);

        void on_gl_input_press(const String &node_path, float x_percent, float y_percent);

        void on_gl_input_release(const String &node_path, float x_percent, float y_percent);

        /// Create a mesh instance with the given parent node and set it up.
        /// @return The node path to the newly created mesh instance
        String acquire_and_bind_mesh_instance(const String &parent_node_path);

        /// Setup the mesh instance with the given node path for GAST view support.
        bool bind_mesh_instance(const String &node_path);

        /// Unbind the mesh instance with the given node path. This is the counterpart to
        /// bind_mesh_instance.
        void unbind_mesh_instance(const String &node_path);

        /// Unbind and release the mesh instance with the given node path. This is the counterpart
        /// to acquire_and_bind_mesh_instance.
        void unbind_and_release_mesh_instance(const String &node_path);

        String
        update_mesh_instance_parent(const String &node_path, const String &new_parent_node_path);

        void update_mesh_instance_visibility(const String &node_path,
                                             bool should_duplicate_parent_visibility, bool visible);

        void update_mesh_instance_size(const String &node_path, float width, float height);

        void update_mesh_instance_translation(const String &node_path, float x_translation,
                                              float y_translation,
                                              float z_translation);

        void update_mesh_instance_scale(const String &node_path, float x_scale, float y_scale);

        void
        update_mesh_instance_rotation(const String &node_path, float x_rotation, float y_rotation,
                                      float z_rotation);

    private:
        static void delete_singleton_instance();

        static void register_callback(JNIEnv *env, jobject callback);

        static void unregister_callback(JNIEnv *env);

        ExternalTexture *
        get_external_texture(const String &node_path);

        MeshInstance *get_mesh_instance(const String &node_path);

        Node *get_node(const String &node_path);

        bool bind_mesh_instance(MeshInstance& mesh_instance);

        void unbind_mesh_instance(MeshInstance& mesh_instance);

        GastManager();

        ~GastManager();

        std::list<MeshInstance *> reusable_pool_;

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
