#include "gast_manager.h"

#include <core/Godot.hpp>
#include <core/NodePath.hpp>
#include <core/Vector2.hpp>
#include <core/Vector3.hpp>
#include <gen/Engine.hpp>
#include <gen/Input.hpp>
#include <gen/InputEventAction.hpp>
#include <gen/MainLoop.hpp>
#include <gen/Object.hpp>
#include <gen/Viewport.hpp>

namespace gast {

namespace {
const char *kGastNodeGroupName = "gast_node_group";
} // namespace

GastManager *GastManager::singleton_instance_ = nullptr;
GastLoader *GastManager::gast_loader_ = nullptr;
bool GastManager::gdn_initialized_ = false;
bool GastManager::jni_initialized_ = false;

jobject GastManager::callback_instance_ = nullptr;
jmethodID GastManager::on_render_input_action_ = nullptr;
jmethodID GastManager::on_render_input_hover_ = nullptr;
jmethodID GastManager::on_render_input_press_ = nullptr;
jmethodID GastManager::on_render_input_release_ = nullptr;
jmethodID GastManager::on_render_input_scroll_ = nullptr;

GastManager::GastManager() = default;

GastManager::~GastManager() {
    reusable_pool_.clear();
}

GastManager *GastManager::get_singleton_instance() {
    ALOG_ASSERT(gdn_initialized_, "Gast is not properly initialized.");
    if (singleton_instance_ == nullptr) {
        singleton_instance_ = new GastManager();
    }
    return singleton_instance_;
}

void GastManager::delete_singleton_instance() {
    if (!gdn_initialized_ && !jni_initialized_) {
        delete singleton_instance_;
        singleton_instance_ = nullptr;
    }
}

void GastManager::gdn_initialize(GastLoader *gast_loader) {
    ALOG_ASSERT(gast_loader_ == nullptr, "Gast is already initialized.");
    gdn_initialized_ = true;
    gast_loader_ = gast_loader;
}

void GastManager::gdn_shutdown() {
    gdn_initialized_ = false;
    gast_loader_ = nullptr;
    delete_singleton_instance();
}

void GastManager::jni_initialize(JNIEnv *env, jobject callback) {
    jni_initialized_ = true;
    register_callback(env, callback);
}

void GastManager::jni_shutdown(JNIEnv *env) {
    jni_initialized_ = false;
    unregister_callback(env);
    delete_singleton_instance();
}

void GastManager::register_callback(JNIEnv *env, jobject callback) {
    callback_instance_ = env->NewGlobalRef(callback);
    ALOG_ASSERT(callback_instance_ != nullptr, "Invalid value for callback.");

    jclass callback_class = env->GetObjectClass(callback_instance_);
    ALOG_ASSERT(callback_class != nullptr, "Invalid value for callback.");

    on_render_input_action_ = env->GetMethodID(callback_class, "onRenderInputAction",
                                               "(Ljava/lang/String;IF)V");
    ALOG_ASSERT(on_render_input_action_ != nullptr, "Unable to find onRenderInputAction");

    on_render_input_hover_ = env->GetMethodID(callback_class, "onRenderInputHover",
                                              "(Ljava/lang/String;Ljava/lang/String;FF)V");
    ALOG_ASSERT(on_render_input_hover_ != nullptr, "Unable to find onRenderInputHover");

    on_render_input_press_ = env->GetMethodID(callback_class, "onRenderInputPress",
                                              "(Ljava/lang/String;Ljava/lang/String;FF)V");
    ALOG_ASSERT(on_render_input_press_ != nullptr, "Unable to find onRenderInputPress");

    on_render_input_release_ = env->GetMethodID(callback_class, "onRenderInputRelease",
                                                "(Ljava/lang/String;Ljava/lang/String;FF)V");
    ALOG_ASSERT(on_render_input_release_ != nullptr, "Unable to find onRenderInputRelease");

    on_render_input_scroll_ = env->GetMethodID(callback_class, "onRenderInputScroll",
                                               "(Ljava/lang/String;Ljava/lang/String;FFFF)V");
    ALOG_ASSERT(on_render_input_scroll_ != nullptr, "Unable to find onRenderInputScroll");
}

void GastManager::unregister_callback(JNIEnv *env) {
    if (callback_instance_) {
        env->DeleteGlobalRef(callback_instance_);
        callback_instance_ = nullptr;
        on_render_input_action_ = nullptr;
        on_render_input_hover_ = nullptr;
        on_render_input_press_ = nullptr;
        on_render_input_release_ = nullptr;
        on_render_input_scroll_ = nullptr;
    }
}

void GastManager::update_node_visibility(const String &node_path, bool visible) {
    auto *node = Object::cast_to<Spatial>(get_node(node_path));
    if (!node) {
        ALOGE("Unable to find target node with path %s", get_node_tag(node_path));
        return;
    }

    node->set_visible(visible);
}

GastNode *GastManager::get_gast_node(const godot::String &node_path) {
    auto *gast_node = Object::cast_to<GastNode>(get_node(node_path));
    if (!gast_node || !gast_node->is_in_group(kGastNodeGroupName)) {
        ALOGW("Unable to find a GastNode node with path %s", get_node_tag(node_path));
        return nullptr;
    }

    return gast_node;
}

SceneTree *GastManager::get_scene_tree() {
    MainLoop *main_loop = Engine::get_singleton()->get_main_loop();
    auto *scene_tree = Object::cast_to<SceneTree>(main_loop);
    return scene_tree;
}

Node *GastManager::get_node(const godot::String &node_path) {
    // First search by treating the given argument as a node path since it's more efficient.
    if (node_path.empty()) {
        ALOGE("Invalid node path argument: %s", get_node_tag(node_path));
        return nullptr;
    }

    auto *scene_tree = get_scene_tree();
    if (!scene_tree) {
        ALOGW("Unable to retrieve scene tree.");
        return nullptr;
    }

    const Viewport *viewport = scene_tree->get_root();
    NodePath node_path_obj(node_path);
    Node *node = viewport->get_node_or_null(node_path_obj);
    if (!node) {
        // Treat the parameter as the node's name and give it another try.
        node = viewport->find_node(node_path, true, false);
    }

    return node;
}

GastNode
*GastManager::acquire_and_bind_gast_node(const godot::String &parent_node_path, bool empty_parent) {
    ALOGV("Retrieving node's parent with path %s", get_node_tag(parent_node_path));
    Node *parent_node = get_node(parent_node_path);
    if (!parent_node) {
        ALOGE("Unable to retrieve parent node with path %s",
              get_node_tag(parent_node_path));
        return nullptr;
    }

    GastNode *gast_node;

    // Check if we have one already setup in the reusable pool, otherwise create a new one.
    if (reusable_pool_.empty()) {
        ALOGV("Creating a new Gast node.");
        // Creating a new static body node
        gast_node = GastNode::_new();
    } else {
        gast_node = reusable_pool_.back();
        reusable_pool_.pop_back();
    }

    // Add the new node to the GastNode group. This is how we keep track of the nodes
    // that are created and managed by this plugin.
    gast_node->add_to_group(kGastNodeGroupName);

    if (gast_node->get_parent() != nullptr) {
        ALOGV("Removing Gast node parent.");
        gast_node->get_parent()->remove_child(gast_node);
    }

    ALOGV("Adding the Gast node to the parent node.");
    if (empty_parent) {
        remove_all_children_from_node(parent_node);
    }
    parent_node->add_child(gast_node);
    gast_node->set_owner(parent_node);

    return gast_node;
}

void GastManager::unbind_and_release_gast_node(GastNode *gast_node) {
    // Remove the Gast node from its parent and move it to the reusable pool.
    if (!gast_node) {
        return;
    }

    // Remove the Gast node from its parent.
    if (gast_node->get_parent() != nullptr) {
        gast_node->get_parent()->remove_child(gast_node);
        gast_node->set_owner(nullptr);
    }

    // Remove the node from the GastNode group
    gast_node->remove_from_group(kGastNodeGroupName);

    // Move the Gast node to the reusable pool.
    reusable_pool_.push_back(gast_node);
}

void GastManager::check_for_monitored_input_actions() {
    // Check if one of the monitored input actions was dispatched.
    if (input_actions_to_monitor_.empty()) {
        return;
    }

    Input *input = Input::get_singleton();
    for (auto const& action : input_actions_to_monitor_) {
        InputPressState press_state = kInvalid;

        if (input->is_action_just_pressed(action)) {
            press_state = kJustPressed;
        } else if (input->is_action_pressed(action)) {
            press_state = kPressed;
        } else if (input->is_action_just_released(action)) {
            press_state = kJustReleased;
        }

        if (press_state != kInvalid) {
            on_render_input_action(action, press_state, input->get_action_strength(action));
        }
    }
}

bool GastManager::get_raycast_collision_info(const RayCast &ray_cast, CollisionInfo *collision_info) {
    auto *collider = Object::cast_to<GastNode>(ray_cast.get_collider());
    if (collision_info != nullptr) {
        collision_info->collider = collider;
        collision_info->collision_point = ray_cast.get_collision_point();
        collision_info->collision_normal = ray_cast.get_collision_normal();
    }
    return collider != nullptr;
}

void GastManager::process_raycast_input() {
    auto *scene_tree = get_scene_tree();
    if (!scene_tree) {
        ALOGW("Unable to retrieve scene tree.");
        return;
    }

    // Get the list of ray casts in the group
    Array gast_ray_casts = scene_tree->get_nodes_in_group(kGastRayCasterGroupName);
    if (gast_ray_casts.empty()) {
        return;
    }

    for (int i = 0; i < gast_ray_casts.size(); i++) {
        RayCast *ray_cast = GastNode::get_ray_cast_from_variant(gast_ray_casts[i]);
        if (!ray_cast || !ray_cast->is_enabled()) {
            continue;
        }

        // Check if the ray cast is colliding.
        String ray_cast_path = ray_cast->get_path();
        std::shared_ptr<CollisionInfo> collision_info =
                colliding_raycast_paths.count(ray_cast_path) != 0
                ? colliding_raycast_paths[ray_cast_path] : std::make_shared<CollisionInfo>();

        const CollisionInfo previous_collision_info = *collision_info;
        bool collides_with_gast_node = get_raycast_collision_info(*ray_cast, collision_info.get());

        // Check if the previous collider is different from the current one. If that's the case,
        // we need to send a exit event to the previous one.
        if (previous_collision_info.collider != nullptr &&
            previous_collision_info.collider != collision_info->collider) {
            cleanup_collision_info(previous_collision_info, ray_cast->get_name());
            colliding_raycast_paths.erase(ray_cast_path);
        }

        if (collides_with_gast_node) {
            // Calculate the 2D collision point of the raycast on the Gast node.
            Vector2 relative_collision_point = collision_info->collider->get_relative_collision_point(
                    collision_info->collision_point);
            collision_info->press_in_progress = collision_info->collider->handle_ray_cast_input(
                    ray_cast->get_name(),
                    relative_collision_point);

            // Add the raycast to the list of colliding raycasts and update its collision info.
            colliding_raycast_paths[ray_cast_path] = collision_info;
        }
    }
}

void GastManager::on_physics_process() {
    check_for_monitored_input_actions();
    process_raycast_input();
}

void GastManager::cleanup_collision_info(const CollisionInfo &collision_info,
                                         const String &ray_cast_name) {
    if (collision_info.press_in_progress) {
        Vector2 last_coordinate = collision_info.collider->get_relative_collision_point(
                collision_info.collision_point);
        // Fire a release event.
        on_render_input_release(collision_info.collider->get_path(),
                                ray_cast_name, last_coordinate.x,
                                last_coordinate.y);
    } else {
        // Fire a hover exit event.
        on_render_input_hover(collision_info.collider->get_path(), ray_cast_name,
                              kInvalidCoordinate.x, kInvalidCoordinate.y);
    }
}

void GastManager::on_render_input_action(const String &action, InputPressState press_state,
                                         float strength) {
    if (callback_instance_ && on_render_input_action_) {
        JNIEnv *env = godot::android_api->godot_android_get_env();
        env->CallVoidMethod(callback_instance_, on_render_input_action_,
                            string_to_jstring(env, action), press_state, strength);
    }
}

void GastManager::on_render_input_hover(const String &node_path, const String &pointer_id,
                                        float x_percent,
                                        float y_percent) {
    if (gast_loader_) {
        gast_loader_->emitHoverEvent(node_path, pointer_id, x_percent, y_percent);
    }

    if (callback_instance_ && on_render_input_hover_) {
        JNIEnv *env = godot::android_api->godot_android_get_env();
        env->CallVoidMethod(callback_instance_, on_render_input_hover_,
                            string_to_jstring(env, node_path), string_to_jstring(env, pointer_id),
                            x_percent, y_percent);
    }
}

void GastManager::on_render_input_press(const String &node_path, const String &pointer_id,
                                        float x_percent,
                                        float y_percent) {
    if (gast_loader_) {
        gast_loader_->emitPressEvent(node_path, pointer_id, x_percent, y_percent);
    }

    if (callback_instance_ && on_render_input_press_) {
        JNIEnv *env = godot::android_api->godot_android_get_env();
        env->CallVoidMethod(callback_instance_, on_render_input_press_,
                            string_to_jstring(env, node_path), string_to_jstring(env, pointer_id),
                            x_percent, y_percent);
    }
}

void GastManager::on_render_input_release(const String &node_path, const String &pointer_id,
                                          float x_percent,
                                          float y_percent) {
    if (gast_loader_) {
        gast_loader_->emitReleaseEvent(node_path, pointer_id, x_percent, y_percent);
    }

    if (callback_instance_ && on_render_input_release_) {
        JNIEnv *env = godot::android_api->godot_android_get_env();
        env->CallVoidMethod(callback_instance_, on_render_input_release_,
                            string_to_jstring(env, node_path), string_to_jstring(env, pointer_id),
                            x_percent, y_percent);
    }
}

void GastManager::on_render_input_scroll(const godot::String &node_path,
                                         const godot::String &pointer_id, float x_percent,
                                         float y_percent, float horizontal_delta,
                                         float vertical_delta) {
    if (gast_loader_) {
        gast_loader_->emitScrollEvent(node_path, pointer_id, x_percent, y_percent, horizontal_delta,
                                      vertical_delta);
    }

    if (callback_instance_ && on_render_input_scroll_) {
        JNIEnv *env = godot::android_api->godot_android_get_env();
        env->CallVoidMethod(callback_instance_, on_render_input_scroll_,
                            string_to_jstring(env, node_path), string_to_jstring(env, pointer_id),
                            x_percent, y_percent, horizontal_delta, vertical_delta);
    }
}

bool GastManager::update_gast_node_parent(GastNode *node,
                                          const String &new_parent_node_path, bool empty_parent) {
    if (!node) {
        ALOGW("Invalid GastNode instance!");
        return false;
    }

    // Check if current parent differs from new one.
    if (node->get_parent() != nullptr) {
        String parent_node_path = node->get_parent()->get_path();
        if (parent_node_path == new_parent_node_path) {
            ALOGV("Current parent is same as newly proposed one.");
            return false;
        }
    }

    // Check if the new parent exists.
    Node *new_parent = get_node(new_parent_node_path);
    if (!new_parent) {
        ALOGW("Unable to retrieve new parent node with path %s",
              get_node_tag(new_parent_node_path));
        return false;
    }

    // Perform the update
    if (node->get_parent() != nullptr) {
        node->get_parent()->remove_child(node);
    }

    if (empty_parent) {
        remove_all_children_from_node(new_parent);
    }
    new_parent->add_child(node);
    node->set_owner(new_parent);

    return true;
}

}  // namespace gast
