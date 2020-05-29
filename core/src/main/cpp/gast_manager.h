#ifndef GAST_MANAGER_H
#define GAST_MANAGER_H

#include <core/String.hpp>
#include <core/Vector2.hpp>
#include <core/Vector3.hpp>
#include <gen/CollisionShape.hpp>
#include <gen/ExternalTexture.hpp>
#include <gen/InputEvent.hpp>
#include <gen/Mesh.hpp>
#include <gen/MeshInstance.hpp>
#include <gen/Node.hpp>
#include <gen/Shape.hpp>
#include <gen/Spatial.hpp>
#include <gen/StaticBody.hpp>
#include <jni.h>
#include <list>

#include "utils.h"

namespace gast {

namespace {
using namespace godot;
constexpr int kInvalidSurfaceIndex = -1;

// Name of the group containing the RayCast nodes that interact with the Gast nodes.
const char *kGastRayCasterGroupName = "gast_ray_caster";
}  // namespace

class GastManager {
public:
    static GastManager *get_singleton_instance();

    static void gdn_initialize();

    static void gdn_shutdown();

    static void jni_initialize(JNIEnv *env, jobject callback);

    static void jni_shutdown(JNIEnv *env);

    int get_external_texture_id(const String &node_path, int surface_index = kInvalidSurfaceIndex);

    void process_input(const Ref<InputEvent> event);

    void on_render_input_hover(const String &node_path, const String &pointer_id, float x_percent,
                               float y_percent);

    void on_render_input_press(const String &node_path, const String &pointer_id, float x_percent,
                               float y_percent);

    void on_render_input_release(const String &node_path, const String &pointer_id, float x_percent,
                                 float y_percent);

    void on_render_input_scroll(const String &node_path, const String &pointer_id, float x_percent,
                                float y_percent, float horizontal_delta, float vertical_delta);

    /// Create a Gast node with the given parent node and set it up.
    /// @return The node path to the newly created Gast node
    String acquire_and_bind_gast_node(const String &parent_node_path, bool empty_parent);

    /// Unbind and release the Gast node with the given node path. This is the counterpart
    /// to acquire_and_bind_gast_node.
    void unbind_and_release_gast_node(const String &node_path);

    String update_gast_node_parent(const String &node_path, const String &new_parent_node_path,
                                   bool empty_parent);

    void update_gast_node_visibility(const String &node_path,
                                     bool should_duplicate_parent_visibility, bool visible);

    void set_gast_node_collidable(const String &node_path, bool collidable);

    bool is_gast_node_collidable(const String &node_path);

    Vector2 get_gast_node_size(const String &node_path);

    void update_gast_node_size(const String &node_path, float width, float height);

    void update_gast_node_local_translation(const String &node_path, float x_translation,
                                            float y_translation,
                                            float z_translation);

    void update_gast_node_local_scale(const String &node_path, float x_scale, float y_scale);

    void update_gast_node_local_rotation(const String &node_path, float x_rotation,
                                         float y_rotation,
                                         float z_rotation);

private:
    static void delete_singleton_instance();

    static void register_callback(JNIEnv *env, jobject callback);

    static void unregister_callback(JNIEnv *env);

    inline void remove_all_children_from_node(Node *node) {
        if (!node) {
            return;
        }

        Array children = node->get_children();
        for (int i = 0; i < children.size(); i++) {
            Node *child = get_node_from_variant(children[i]);
            if (child) {
                node->remove_child(child);
            }
        }
    }

    inline MeshInstance *get_mesh_instance_from_gast_node(StaticBody &static_body) {
        CollisionShape *collision_shape = get_collision_shape_from_gast_node(static_body);
        if (!collision_shape) {
            return nullptr;
        }

        Node *node = collision_shape->get_child(0);
        if (!node || !node->is_class("MeshInstance")) {
            return nullptr;
        }
        MeshInstance *mesh_instance = Object::cast_to<MeshInstance>(node);
        return mesh_instance;
    }

    inline CollisionShape *get_collision_shape_from_gast_node(StaticBody &static_body) {
        Node *node = static_body.get_child(0);
        if (!node || !node->is_class("CollisionShape")) {
            return nullptr;
        }

        CollisionShape *collision_shape = Object::cast_to<CollisionShape>(node);
        return collision_shape;
    }

    ExternalTexture *get_external_texture(const String &node_path, int surface_index);

    ExternalTexture *get_external_texture(Ref<Mesh> mesh, int surface_index);

    StaticBody *get_gast_node(const String &node_path);

    Node *get_node(const String &node_path);

    bool bind_gast_node(StaticBody &static_body);

    void unbind_gast_node(StaticBody &static_body);

    GastManager();

    ~GastManager();

    std::list<StaticBody *> reusable_pool_;

    static GastManager *singleton_instance_;
    static bool gdn_initialized_;
    static bool jni_initialized_;

    static jobject callback_instance_;
    static jmethodID on_render_input_hover_;
    static jmethodID on_render_input_press_;
    static jmethodID on_render_input_release_;
    static jmethodID on_render_input_scroll_;
};
}  // namespace gast

#endif // GAST_MANAGER_H
