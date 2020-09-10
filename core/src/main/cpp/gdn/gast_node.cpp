#include "gast_node.h"
#include "gast_manager.h"

#include <core/Array.hpp>
#include <core/String.hpp>
#include <core/NodePath.hpp>
#include <gen/Input.hpp>
#include <gen/InputEventScreenDrag.hpp>
#include <gen/InputEventScreenTouch.hpp>
#include <gen/Material.hpp>
#include <gen/Node.hpp>
#include <gen/Resource.hpp>
#include <gen/ResourceLoader.hpp>
#include <gen/SceneTree.hpp>
#include <gen/Shape.hpp>
#include <gen/Texture.hpp>

namespace gast {

namespace {
const Vector2 kDefaultSize = Vector2(2.0, 1.0);
const int kDefaultSurfaceIndex = 0;
const bool kDefaultCurveValue = false;
const char *kGastCurvedParamName = "curve_flag";
const char *kGastTextureParamName = "gast_texture";
}

GastNode::GastNode() {}

GastNode::~GastNode() {}

void GastNode::_register_methods() {
    register_method("_enter_tree", &GastNode::_enter_tree);
    register_method("_exit_tree", &GastNode::_exit_tree);
    register_method("_input_event", &GastNode::_input_event);
    register_method("_physics_process", &GastNode::_physics_process);

    register_method("set_size", &GastNode::set_size);
    register_method("get_size", &GastNode::get_size);
    register_method("set_collidable", &GastNode::set_collidable);
    register_method("is_collidable", &GastNode::is_collidable);
    register_method("set_curved", &GastNode::set_curved);
    register_method("is_curved", &GastNode::is_curved);
    register_method("get_external_texture_id", &GastNode::get_external_texture_id);

    register_property<GastNode, bool>("collidable", &GastNode::set_collidable,
                                      &GastNode::is_collidable, true);
    register_property<GastNode, bool>("curved", &GastNode::set_curved, &GastNode::is_curved, kDefaultCurveValue);
    register_property<GastNode, Vector2>("size", &GastNode::set_size, &GastNode::get_size,
                                         kDefaultSize);
}

void GastNode::_init() {
    ALOGV("Initializing GastNode class.");

    // Add a CollisionShape to the static body node
    CollisionShape *collision_shape = CollisionShape::_new();
    add_child(collision_shape);

    // Add a mesh instance to the collision shape node
    MeshInstance *mesh_instance = MeshInstance::_new();
    collision_shape->add_child(mesh_instance);
}

void GastNode::_enter_tree() {
    ALOGV("Entering tree.");

//    // Load the script resource.
//    ALOGV("Loading script resource.");
//    Ref<Resource> script_res = ResourceLoader::get_singleton()->load(
//            "res://godot/plugin/v1/gast/GastNode.gdns", "", true);
//    if (script_res.is_null() || !script_res->is_class("NativeScript")) {
//        ALOGE("Unable to load native script resource.");
//        return;
//    }

    // Load the gast mesh resource.
    ALOGV("Loading GAST mesh resource.");
    Ref<Resource> gast_mesh_res = ResourceLoader::get_singleton()->load(
            "res://godot/plugin/v1/gast/gast_plane_mesh.tres");
    if (gast_mesh_res.is_null() || !gast_mesh_res->is_class(PlaneMesh::___get_class_name())) {
        ALOGE("Unable to load mesh resource.");
        return;
    }
    auto *mesh = Object::cast_to<PlaneMesh>(*gast_mesh_res);
    if (!mesh) {
        ALOGE("Failed cast to %s.", PlaneMesh::___get_class_name());
        return;
    }

    // Load the box shape resource.
    ALOGV("Loading GAST shape resource.");
    Ref<Resource> gast_shape_res = ResourceLoader::get_singleton()->load(
            "res://godot/plugin/v1/gast/gast_concave_polygon_shape.tres");
    if (gast_shape_res.is_null() || !gast_shape_res->is_class(ConcavePolygonShape::___get_class_name())) {
        ALOGE("Unable to load shape resource.");
        return;
    }
    auto *shape = Object::cast_to<ConcavePolygonShape>(*gast_shape_res);
    if (!shape) {
        ALOGE("Failed cast to %s.", ConcavePolygonShape::___get_class_name());
        return;
    }

//    ALOGV("Setting up native script resource.");
//    static_body.set_script(*script_res);

    ALOGV("Setting up GAST mesh resource.");
    MeshInstance *mesh_instance = get_mesh_instance();
    if (!mesh_instance) {
        return;
    }
    mesh_instance->set_mesh(mesh);

    ALOGV("Setting up GAST shape resource.");
    CollisionShape *collision_shape = get_collision_shape();
    if (!collision_shape) {
        return;
    }
    shape->set_faces(mesh->get_faces());
    collision_shape->set_shape(shape);
}

void GastNode::_exit_tree() {
    ALOGV("Exiting tree.");

    // Unset the GAST mesh resource
    MeshInstance *mesh_instance = get_mesh_instance();
    if (mesh_instance) {
        mesh_instance->set_mesh(Ref<Resource>());
    }

    // Unset the box shape resource
    CollisionShape *collision_shape = get_collision_shape();
    if (collision_shape) {
        collision_shape->set_shape(Ref<Resource>());
    }

    // Unset the native script resource
//    static_body.set_script(nullptr);
}

Vector2 GastNode::get_size() {
    Vector2 size;
    MeshInstance *mesh_instance = get_mesh_instance();
    if (mesh_instance) {
        Ref<Mesh> mesh_ref = mesh_instance->get_mesh();
        if (mesh_ref.is_valid() && mesh_ref->is_class(PlaneMesh::___get_class_name())) {
            auto *mesh = Object::cast_to<PlaneMesh>(*mesh_ref);
            if (mesh) {
                size = mesh->get_size();
            }
        }
    }
    return size;
}

void GastNode::set_size(Vector2 size) {
    // We need to update both the size of the box shape and the size of the quad mesh resources.
    CollisionShape *collision_shape = get_collision_shape();
    MeshInstance *mesh_instance = get_mesh_instance();
    if (!collision_shape || !mesh_instance) {
        ALOGW("Invalid GastNode %s. Aborting...", get_path_as_string());
        return;
    }

    // Retrieve the shape
    Ref<Shape> shape_ref = collision_shape->get_shape();
    if (shape_ref.is_null() || !shape_ref->is_class(ConcavePolygonShape::___get_class_name())) {
        ALOGE("Unable to access shape resource for %s", get_path_as_string());
        return;
    }

    // Retrieve the mesh.
    Ref<Mesh> mesh_ref = mesh_instance->get_mesh();
    if (mesh_ref.is_null() || !mesh_ref->is_class(PlaneMesh::___get_class_name())) {
        ALOGE("Unable to access mesh resource for %s", get_path_as_string());
        return;
    }

    auto *shape = Object::cast_to<ConcavePolygonShape>(*shape_ref);
    if (!shape) {
        ALOGE("Failed cast to %s.", ConcavePolygonShape::___get_class_name());
        return;
    }

    auto *mesh = Object::cast_to<PlaneMesh>(*mesh_ref);
    if (!mesh) {
        ALOGE("Failed cast to %s.", PlaneMesh::___get_class_name());
        return;
    }

    mesh->set_size(size);
    shape->set_faces(mesh->get_faces());
}

int GastNode::get_external_texture_id(int surface_index) {
    if (surface_index == kInvalidSurfaceIndex) {
        // Default to the first one
        surface_index = kDefaultSurfaceIndex;
    }

    ExternalTexture *external_texture = get_external_texture(surface_index);
    int tex_id = external_texture == nullptr ? kInvalidTexId
                                             : external_texture->get_external_texture_id();
    return tex_id;
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

void GastNode::_physics_process(const real_t delta) {
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

void GastNode::set_curved(bool curved) {
    ShaderMaterial *shader_material = get_shader_material(kDefaultSurfaceIndex);
    if (!shader_material) {
        return;
    }

    shader_material->set_shader_param(kGastCurvedParamName, curved);
}

bool GastNode::is_curved() {
    ShaderMaterial *shader_material = get_shader_material(kDefaultSurfaceIndex);
    if (!shader_material) {
        return kDefaultCurveValue;
    }

    return shader_material->get_shader_param(kGastCurvedParamName);
}

ExternalTexture *GastNode::get_external_texture(int surface_index) {
    ShaderMaterial *shader_material = get_shader_material(surface_index);
    if (!shader_material) {
        return nullptr;
    }

    ExternalTexture *external_texture = nullptr;
    Ref<Texture> texture = shader_material->get_shader_param(kGastTextureParamName);
    if (texture.is_valid() && texture->is_class(ExternalTexture::___get_class_name())) {
        external_texture = Object::cast_to<ExternalTexture>(*texture);
    }

    if (external_texture) {
        ALOGV("Found external GastNode texture for node %s", get_path_as_string());
    }
    return external_texture;
}

ShaderMaterial *GastNode::get_shader_material(int surface_index) {
    PlaneMesh* plane_mesh = get_plane_mesh();
    if (!plane_mesh) {
        return nullptr;
    }

    ShaderMaterial *shader_material = nullptr;

    if (surface_index < plane_mesh->get_surface_count()) {
        Ref<Material> material = plane_mesh->surface_get_material(surface_index);
        if (material.is_valid() && material->is_class(ShaderMaterial::___get_class_name())) {
            shader_material = Object::cast_to<ShaderMaterial>(*material);
        }
    }

    return shader_material;
}

void GastNode::handle_ray_cast_input(const RayCast &ray_cast) {
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

Vector2 GastNode::get_relative_collision_point(Vector3 absolute_collision_point) {
    Vector2 relative_collision_point = kInvalidCoordinate;

    // Turn the collision point into local space
    Vector3 local_point = to_local(absolute_collision_point);

    // Normalize the collision point. A Gast node is a flat quad so we only worry about
    // the x,y coordinates
    Vector2 node_size = GastManager::get_singleton_instance()->get_gast_node_size(get_path());
    if (node_size.width > 0 && node_size.height > 0) {
        float max_x = node_size.width / 2;
        float min_x = -max_x;
        float max_y = node_size.height / 2;
        float min_y = -max_y;
        if (local_point.x >= min_x && local_point.x <= max_x && local_point.y >= min_y &&
            local_point.y <= max_y) {
            relative_collision_point = Vector2((local_point.x - min_x) / node_size.width,
                                               (local_point.y - min_y) / node_size.height);

            // Adjust the y coordinate to match the Android view coordinates system.
            relative_collision_point.y = 1 - relative_collision_point.y;
        }
    }

    return relative_collision_point;
}

}  // namespace gast
