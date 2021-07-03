#include "gast_node.h"
#include "gast_manager.h"
#include "gdn/projection_mesh/rectangular_projection_mesh.h"
#include <utils.h>

#include <core/Array.hpp>
#include <core/String.hpp>
#include <core/NodePath.hpp>
#include <core/Rect2.hpp>
#include <core/Transform.hpp>
#include <gen/Camera.hpp>
#include <gen/Input.hpp>
#include <gen/InputEventScreenDrag.hpp>
#include <gen/InputEventScreenTouch.hpp>
#include <gen/InputMap.hpp>
#include <gen/Material.hpp>
#include <gen/Mesh.hpp>
#include <gen/Node.hpp>
#include <gen/QuadMesh.hpp>
#include <gen/ArrayMesh.hpp>
#include <gen/Resource.hpp>
#include <gen/ResourceLoader.hpp>
#include <gen/SceneTree.hpp>
#include <gen/Shape.hpp>
#include <gen/Texture.hpp>
#include <gen/Viewport.hpp>

namespace gast {

GastNode::GastNode() : collidable(kDefaultCollidable),
                       projection_mesh_pool(ProjectionMeshPool()),
                       mesh_listener(GastNodeMeshUpdateListener(this)) {}

GastNode::~GastNode() = default;

void GastNode::_register_methods() {
    register_method("_enter_tree", &GastNode::_enter_tree);
    register_method("_exit_tree", &GastNode::_exit_tree);
    register_method("_input_event", &GastNode::_input_event);
    register_method("_process", &GastNode::_process);
    register_method("_notification", &GastNode::_notification);

    register_method("set_collidable", &GastNode::set_collidable);
    register_method("is_collidable", &GastNode::is_collidable);

    register_method("get_external_texture_id", &GastNode::get_external_texture_id);

    register_property<GastNode, bool>("collidable", &GastNode::set_collidable,
                                      &GastNode::is_collidable, kDefaultCollidable);
}

void GastNode::_init() {
    // Add a CollisionShape to the static body node
    CollisionShape *collision_shape = CollisionShape::_new();
    add_child(collision_shape);
}

void GastNode::_enter_tree() {
    // Create the external texture
    external_texture = Ref<ExternalTexture>(ExternalTexture::_new());

    // Initialize the projection mesh. We default to a rectangular one.
    set_projection_mesh(ProjectionMesh::ProjectionMeshType::RECTANGULAR);
}

void GastNode::_exit_tree() {
    reset_mesh_and_collision_shape();
}

void GastNode::reset_mesh_and_collision_shape() {
    // Unset the GAST mesh resource
    projection_mesh->reset_mesh();
    projection_mesh->reset_external_texture();
    // Unset the box shape resource
    update_collision_shape();
}

void GastNode::setup_projection_mesh() {
    CollisionShape *collisionShape = get_collision_shape();
    for (int i = 0; i < collisionShape->get_child_count(); i++) {
        collisionShape->remove_child(collisionShape->get_child(i));
    }
    projection_mesh->set_external_texture(external_texture);
    collisionShape->add_child(projection_mesh->get_mesh_instance());
    if (projection_mesh->get_projection_mesh_type() == ProjectionMesh::ProjectionMeshType::MESH) {
        auto *custom_projection_mesh = Object::cast_to<CustomProjectionMesh>(projection_mesh);
        collisionShape->add_child(custom_projection_mesh->get_right_mesh_instance());
    }
    update_collision_shape();
    projection_mesh->update_render_priority();
}

void GastNode::update_collision_shape() {
    CollisionShape *collision_shape = get_collision_shape();
    if (!collision_shape) {
        ALOGW("Unable to retrieve collision shape for %s. Aborting...", get_node_tag(*this));
        return;
    }

    if (!is_visible_in_tree() || !collidable || projection_mesh->get_mesh_instance() == nullptr) {
        collision_shape->set_shape(Ref<Resource>());
    } else {
        collision_shape->set_shape(projection_mesh->get_collision_shape());
    }
}

int GastNode::get_external_texture_id() {
    int tex_id = external_texture.is_null() ? kInvalidTexId
                                             : external_texture->get_external_texture_id();
    return tex_id;
}

Ref<ExternalTexture> GastNode::get_external_texture() {
    return external_texture;
}

void GastNode::_input_event(const godot::Object *camera,
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

void GastNode::_notification(const int64_t what) {
    switch(what) {
        case NOTIFICATION_VISIBILITY_CHANGED:
            update_collision_shape();
            break;
    }
}

void GastNode::_process(const real_t delta) {
    if (is_gaze_tracking()) {
        Rect2 gaze_area = get_viewport()->get_visible_rect();
        Vector2 gaze_center_point = Vector2(gaze_area.position.x + gaze_area.size.x / 2.0,
                                            gaze_area.position.y + gaze_area.size.y / 2.0);

        // Get the distance between the camera and this node.
        Camera* camera = get_viewport()->get_camera();
        Transform global_transform = get_global_transform();
        float distance = camera->get_global_transform().origin.distance_to(global_transform.origin);

        // Update the node's position to match the center of the gaze area.
        Vector3 updated_position = camera->project_position(gaze_center_point, distance);
        global_transform.origin = updated_position;
        set_global_transform(global_transform);
    }
}

bool
GastNode::handle_ray_cast_input(const String &ray_cast_name, Vector2 relative_collision_point) {
    Input *input = Input::get_singleton();
    InputMap *input_map = InputMap::get_singleton();

    String node_path = get_path();

    float x_percent = relative_collision_point.x;
    float y_percent = relative_collision_point.y;

    // Check for click actions
    String ray_cast_click_action = get_click_action_from_node_name(ray_cast_name);
    bool press_in_progress = false;
    bool hovering = true;

    if (input_map->has_action(ray_cast_click_action)) {
        press_in_progress = input->is_action_pressed(ray_cast_click_action);

        if (input->is_action_just_pressed(ray_cast_click_action)) {
            hovering = false;
            GastManager::get_singleton_instance()->on_render_input_press(
                    node_path, ray_cast_name,
                    x_percent, y_percent);
        } else if (input->is_action_just_released(ray_cast_click_action)) {
            hovering = false;
            GastManager::get_singleton_instance()->on_render_input_release(
                    node_path, ray_cast_name,
                    x_percent, y_percent);
        }
    }
    if (hovering) {
        GastManager::get_singleton_instance()->on_render_input_hover(
                node_path,
                ray_cast_name,
                x_percent,
                y_percent);
    }

    // Check for scrolling actions
    bool did_scroll = false;
    float horizontal_scroll_delta = 0;
    float vertical_scroll_delta = 0;

    // Horizontal scrolls
    String ray_cast_horizontal_left_scroll_action = get_horizontal_left_scroll_action_from_node_name(
            ray_cast_name);
    String ray_cast_horizontal_right_scroll_action = get_horizontal_right_scroll_action_from_node_name(
            ray_cast_name);
    if (input_map->has_action(ray_cast_horizontal_left_scroll_action) && input->is_action_pressed
            (ray_cast_horizontal_left_scroll_action)) {
        did_scroll = true;
        horizontal_scroll_delta = -input->get_action_strength(
                ray_cast_horizontal_left_scroll_action);
    } else if (input_map->has_action(ray_cast_horizontal_right_scroll_action) &&
            input->is_action_pressed(ray_cast_horizontal_right_scroll_action)) {
        did_scroll = true;
        horizontal_scroll_delta = input->get_action_strength(
                ray_cast_horizontal_right_scroll_action);
    }

    // Vertical scrolls
    String ray_cast_vertical_down_scroll_action = get_vertical_down_scroll_action_from_node_name(
            ray_cast_name);
    String ray_cast_vertical_up_scroll_action = get_vertical_up_scroll_action_from_node_name(
            ray_cast_name);
    if (input_map->has_action(ray_cast_vertical_down_scroll_action) && input->is_action_pressed
            (ray_cast_vertical_down_scroll_action)) {
        did_scroll = true;
        vertical_scroll_delta = -input->get_action_strength(ray_cast_vertical_down_scroll_action);
    } else if (input_map->has_action(ray_cast_vertical_up_scroll_action) && input->is_action_pressed
            (ray_cast_vertical_up_scroll_action)) {
        did_scroll = true;
        vertical_scroll_delta = input->get_action_strength(ray_cast_vertical_up_scroll_action);
    }

    if (did_scroll) {
        GastManager::get_singleton_instance()->on_render_input_scroll(node_path, ray_cast_name,
                                                                      x_percent, y_percent,
                                                                      horizontal_scroll_delta,
                                                                      vertical_scroll_delta);
    }

    return press_in_progress;
}

bool GastNode::intersects_ray(Vector3 ray_origin, Vector3 ray_direction, Vector3 *intersection) {
    // NOTE: This only handles RECTANGULAR projection type for now
    if (!projection_mesh->is_rectangular_projection_mesh()) {
        return false;
    }

    // Get the mesh's aabb
    MeshInstance *mesh_instance = projection_mesh->get_mesh_instance();
    if (!mesh_instance) {
        return false;
    }

    AABB mesh_aabb = mesh_instance->get_aabb();
    Vector3 aabb_global_position = mesh_instance->to_global(mesh_aabb.position);

    // Generate a plane from 3 of the aabb's endpoints.
    Vector3 first_endpoint = aabb_global_position;
    Vector3 second_endpoint = Vector3(aabb_global_position.x,
                                      aabb_global_position.y + mesh_aabb.size.y,
                                      aabb_global_position.z);
    Vector3 third_endpoint = Vector3(aabb_global_position.x + mesh_aabb.size.x,
                                     aabb_global_position.y + mesh_aabb.size.y,
                                     aabb_global_position.z);
    Plane plane(first_endpoint, second_endpoint, third_endpoint);
    return plane.intersects_ray(ray_origin, ray_direction, intersection);
}

Vector2 GastNode::get_relative_collision_point(Vector3 absolute_collision_point) {
    Vector3 local_point = to_local(absolute_collision_point);
    if (projection_mesh->is_rectangular_projection_mesh()) {
        auto *rectangular_projection_mesh =
                Object::cast_to<RectangularProjectionMesh>(projection_mesh);
        return rectangular_projection_mesh->get_relative_collision_point(local_point);
    } else {
        // TODO: Implement for other projection mesh types.
        return kInvalidCoordinate;
    }
}

}  // namespace gast
