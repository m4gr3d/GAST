#ifndef GAST_MANAGER_H
#define GAST_MANAGER_H

#include <core/String.hpp>
#include <core/Vector2.hpp>
#include <core/Vector3.hpp>
#include <gen/Node.hpp>
#include <gen/Spatial.hpp>
#include <jni.h>
#include <list>

#include "gdn/gast_loader.h"
#include "gdn/gast_node.h"
#include "utils.h"

namespace gast {

namespace {
using namespace godot;

// Name of the group containing the RayCast nodes that interact with the Gast nodes.
const char *kGastRayCasterGroupName = "gast_ray_caster";

/// Mirrors src/main/java/org/godotengine/plugin/gast/input/GastInputListener#InputPressState
enum InputPressState {
    kInvalid = -1,
    kJustPressed = 0,
    kPressed = 1,
    kJustReleased = 2
};
}  // namespace

class GastManager {
public:
    static GastManager *get_singleton_instance();

    static void gdn_initialize(GastLoader *gast_loader);

    static void gdn_shutdown();

    static void jni_initialize(JNIEnv *env, jobject callback);

    static void jni_shutdown(JNIEnv *env);

    void on_process();

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

    void reset_monitored_input_actions() {
        input_actions_to_monitor_.clear();
    }

    void add_input_actions_to_monitor(String input_action) {
        input_actions_to_monitor_.push_back(input_action);
    }

    GastNode *get_gast_node(const String &node_path);

private:
    static void delete_singleton_instance();

    static void register_callback(JNIEnv *env, jobject callback);

    static void unregister_callback(JNIEnv *env);

    static inline void remove_all_children_from_node(Node *node) {
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

    void on_render_input_action(const String &action, InputPressState press_state, float strength);

    Node *get_node(const String &node_path);

    GastManager();

    ~GastManager();

    std::list<GastNode *> reusable_pool_;
    std::list<String> input_actions_to_monitor_;

    static GastManager *singleton_instance_;
    static GastLoader *gast_loader_;
    static bool gdn_initialized_;
    static bool jni_initialized_;

    static jobject callback_instance_;
    static jmethodID on_render_input_action_;
    static jmethodID on_render_input_hover_;
    static jmethodID on_render_input_press_;
    static jmethodID on_render_input_release_;
    static jmethodID on_render_input_scroll_;
};
}  // namespace gast

#endif // GAST_MANAGER_H
