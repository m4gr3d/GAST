#include "gast_node_script.h"

#include <core/Array.hpp>
#include <core/String.hpp>
#include <core/NodePath.hpp>
#include <gen/Input.hpp>
#include <gen/InputEventScreenDrag.hpp>
#include <gen/InputEventScreenTouch.hpp>
#include <gen/Node.hpp>
#include <gen/SceneTree.hpp>

namespace gast {

GastNodeScript::GastNodeScript() {}

GastNodeScript::~GastNodeScript() {}

void GastNodeScript::_register_methods() {
    register_method("_input_event", &GastNodeScript::_input_event);
    register_method("_physics_process", &GastNodeScript::_physics_process);
}

void GastNodeScript::_init() {
    ALOGV("Initializing GastNodeScript class.");
}

void GastNodeScript::_input_event(const godot::Object *camera,
                                  const godot::Ref<godot::InputEvent> event,
                                  const godot::Vector3 click_position,
                                  const godot::Vector3 click_normal, const int64_t shape_idx) {
    if (event.is_null()) {
        return;
    }

    String node_path = get_path();

    // Calculate the 2D collision point of the raycast on the Gast node.
    Vector2 relative_collision_point = get_relative_collision_point(click_position);
    float x_percent = relative_collision_point.x;
    float y_percent = relative_collision_point.y;

    // This should only fire for touch screen input events, so we filter for those.
    if (event->is_class(InputEventScreenTouch::___get_class_name())) {
        auto *touch_event = Object::cast_to<InputEventScreenTouch>(*event);
        if (touch_event) {
            String touch_event_id = InputEventScreenTouch::___get_class_name() +
                                    String::num_int64(touch_event->get_index());
            if (touch_event->is_pressed()) {
                GastManager::get_singleton_instance()->on_render_input_press(node_path,
                                                                             touch_event_id,
                                                                             x_percent,
                                                                             y_percent);
            } else {
                GastManager::get_singleton_instance()->on_render_input_release(node_path,
                                                                               touch_event_id,
                                                                               x_percent,
                                                                               y_percent);
            }
        }
    } else if (event->is_class(InputEventScreenDrag::___get_class_name())) {
        auto *drag_event = Object::cast_to<InputEventScreenDrag>(*event);
        if (drag_event) {
            String drag_event_id = InputEventScreenDrag::___get_class_name() +
                                   String::num_int64(drag_event->get_index());
            GastManager::get_singleton_instance()->on_render_input_hover(node_path,
                                                                         drag_event_id, x_percent,
                                                                         y_percent);
        }
    }
}

void GastNodeScript::_physics_process(const real_t delta) {
    // Get the list of ray casts in the group
    Array gast_ray_casts = get_tree()->get_nodes_in_group(kGastRayCasterGroupName);
    if (gast_ray_casts.empty()) {
        return;
    }

    NodePath node_path = get_path();
    for (int i = 0; i < gast_ray_casts.size(); i++) {
        RayCast *ray_cast = get_ray_cast_from_variant(gast_ray_casts[i]);
        if (!ray_cast) {
            continue;
        }

        // Check if the ray cast collides with this node.
        if (ray_cast->is_colliding()) {
            Node *collider = Object::cast_to<Node>(ray_cast->get_collider());
            if (collider != nullptr && node_path == collider->get_path()) {
                handle_ray_cast_input(*ray_cast);
                continue;
            }
        }

        // Fall through and fire a hover exit event if this raycast was previously colliding with
        // this node.
        if (colliding_raycast_paths.erase(ray_cast->get_path()) > 0) {
            GastManager::get_singleton_instance()->on_render_input_hover(node_path,
                                                                         ray_cast->get_path(),
                                                                         kInvalidCoordinate.x,
                                                                         kInvalidCoordinate.y);
        }
    }
}

void GastNodeScript::handle_ray_cast_input(const RayCast &ray_cast) {
    Input *input = Input::get_singleton();
    String node_path = get_path();
    String ray_cast_path = ray_cast.get_path();

    // Add the raycast to the list of colliding raycasts
    colliding_raycast_paths.insert(ray_cast_path);

    // Calculate the 2D collision point of the raycast on the Gast node.
    Vector2 relative_collision_point = get_relative_collision_point(ray_cast.get_collision_point());
    float x_percent = relative_collision_point.x;
    float y_percent = relative_collision_point.y;

    // Check for click actions
    String ray_cast_click_action = get_click_action_from_node_path(ray_cast_path);
    if (input->is_action_just_pressed(ray_cast_click_action)) {
        GastManager::get_singleton_instance()->on_render_input_press(node_path, ray_cast_path,
                                                                     x_percent, y_percent);
    } else if (input->is_action_just_released(ray_cast_click_action)) {
        GastManager::get_singleton_instance()->on_render_input_release(node_path, ray_cast_path,
                                                                       x_percent, y_percent);
    } else {
        GastManager::get_singleton_instance()->on_render_input_hover(node_path, ray_cast_path,
                                                                     x_percent, y_percent);
    }

    // Check for scrolling actions
    bool did_scroll = false;
    float horizontal_scroll_delta = 0;
    float vertical_scroll_delta = 0;

    // Horizontal scrolls
    String ray_cast_horizontal_left_scroll_action = get_horizontal_left_scroll_action_from_node_path(
        ray_cast_path);
    String ray_cast_horizontal_right_scroll_action = get_horizontal_right_scroll_action_from_node_path(
        ray_cast_path);
    if (input->is_action_pressed(ray_cast_horizontal_left_scroll_action)) {
        did_scroll = true;
        horizontal_scroll_delta = -input->get_action_strength(
            ray_cast_horizontal_left_scroll_action);
    } else if (input->is_action_pressed(ray_cast_horizontal_right_scroll_action)) {
        did_scroll = true;
        horizontal_scroll_delta = input->get_action_strength(
            ray_cast_horizontal_right_scroll_action);
    }

    // Vertical scrolls
    String ray_cast_vertical_down_scroll_action = get_vertical_down_scroll_action_from_node_path(
        ray_cast_path);
    String ray_cast_vertical_up_scroll_action = get_vertical_up_scroll_action_from_node_path(
        ray_cast_path);
    if (input->is_action_pressed(ray_cast_vertical_down_scroll_action)) {
        did_scroll = true;
        vertical_scroll_delta = -input->get_action_strength(ray_cast_vertical_down_scroll_action);
    } else if (input->is_action_pressed(ray_cast_vertical_up_scroll_action)) {
        did_scroll = true;
        vertical_scroll_delta = input->get_action_strength(ray_cast_vertical_up_scroll_action);
    }

    if (did_scroll) {
        GastManager::get_singleton_instance()->on_render_input_scroll(node_path, ray_cast_path,
                                                                      x_percent, y_percent,
                                                                      horizontal_scroll_delta,
                                                                      vertical_scroll_delta);
    }
}

}  // namespace gast
