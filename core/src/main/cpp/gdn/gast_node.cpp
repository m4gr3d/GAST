#include "gast_node.h"
#include "gast_manager.h"
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

namespace {
const Vector2 kDefaultSize = Vector2(2.0, 1.125);
const int kDefaultSurfaceIndex = 0;
const char *kGastEnableBillBoardParamName = "enable_billboard";
const char *kGastTextureParamName = "gast_texture";
const char *kGastGradientHeightRatioParamName = "gradient_height_ratio";
const char *kGastNodeAlphaParamName = "node_alpha";
const float kDefaultAlpha = 1;
const float kCurvedScreenRadius = 6.0f;
const size_t kCurvedScreenResolution = 20;
const float kEquirectSphereSize = 1.0f;
const size_t kEquirectSphereMeshBandCount = 80;
const size_t kEquirectSphereMeshSectorCount = 80;

const char *kShaderCode = R"GAST_SHADER(
shader_type spatial;
render_mode unshaded, depth_draw_opaque, specular_disabled, shadows_disabled, ambient_light_disabled;

uniform samplerExternalOES gast_texture;
uniform bool enable_billboard;
uniform float gradient_height_ratio;
uniform float node_alpha = 1.0;

void vertex() {
	if (enable_billboard) {
		MODELVIEW_MATRIX = INV_CAMERA_MATRIX * mat4(CAMERA_MATRIX[0],CAMERA_MATRIX[1],CAMERA_MATRIX[2],WORLD_MATRIX[3]);
	}
}

void fragment() {
	vec4 texture_color = texture(gast_texture, UV);
	float target_alpha = COLOR.a * texture_color.a * node_alpha;
	if (gradient_height_ratio >= 0.05) {
		float gradient_mask = min((1.0 - UV.y) / gradient_height_ratio, 1.0);
		target_alpha = target_alpha * gradient_mask;
	}
	ALPHA = target_alpha;
	ALBEDO = texture_color.rgb * target_alpha;
}

)GAST_SHADER";

const char *kDisableDepthTestRenderMode = "render_mode depth_test_disable;";
const char *kCullFrontRenderMode = "render_mode cull_front;";

const char *kShaderCustomDefines = R"GAST_DEFINES(
#ifdef ANDROID_ENABLED
#extension GL_OES_EGL_image_external : enable
#extension GL_OES_EGL_image_external_essl3 : enable
#else
#define samplerExternalOES sampler2D
#endif
)GAST_DEFINES";
}

GastNode::GastNode() : collidable(kDefaultCollidable), curved(kDefaultCurveValue),
                       gaze_tracking(kDefaultGazeTracking),
                       render_on_top(kDefaultRenderOnTop),
                       alpha(kDefaultAlpha),
                       gradient_height_ratio(kDefaultGradientHeightRatio),
                       mesh_size(kDefaultSize),
                       projection_mesh_type(ProjectionMeshType::RECTANGULAR){}

GastNode::~GastNode() = default;

void GastNode::_register_methods() {
    register_method("_enter_tree", &GastNode::_enter_tree);
    register_method("_exit_tree", &GastNode::_exit_tree);
    register_method("_input_event", &GastNode::_input_event);
    register_method("_process", &GastNode::_process);
    register_method("_notification", &GastNode::_notification);

    register_method("set_size", &GastNode::set_size);
    register_method("get_size", &GastNode::get_size);
    register_method("set_collidable", &GastNode::set_collidable);
    register_method("is_collidable", &GastNode::is_collidable);
    register_method("set_curved", &GastNode::set_curved);
    register_method("is_curved", &GastNode::is_curved);
    register_method("set_gaze_tracking", &GastNode::set_gaze_tracking);
    register_method("is_gaze_tracking", &GastNode::is_gaze_tracking);
    register_method("set_render_on_top", &GastNode::set_render_on_top);
    register_method("is_render_on_top", &GastNode::is_render_on_top);
    register_method("set_gradient_height_ratio", &GastNode::set_gradient_height_ratio);
    register_method("get_gradient_height_ratio", &GastNode::get_gradient_height_ratio);
    register_method("get_external_texture_id", &GastNode::get_external_texture_id);

    register_property<GastNode, bool>("collidable", &GastNode::set_collidable,
                                      &GastNode::is_collidable, kDefaultCollidable);
    register_property<GastNode, bool>("curved", &GastNode::set_curved, &GastNode::is_curved,
                                      kDefaultCurveValue);
    register_property<GastNode, bool>("gaze_tracking", &GastNode::set_gaze_tracking,
                                      &GastNode::is_gaze_tracking, kDefaultGazeTracking);
    register_property<GastNode, bool>("render_on_top", &GastNode::set_render_on_top,
                                      &GastNode::is_render_on_top, kDefaultRenderOnTop);
    register_property<GastNode, Vector2>("size", &GastNode::set_size, &GastNode::get_size,
                                         kDefaultSize);
    register_property<GastNode, float>("gradient_height_ratio",
                                       &GastNode::set_gradient_height_ratio,
                                       &GastNode::get_gradient_height_ratio,
                                       kDefaultGradientHeightRatio);
}

void GastNode::_init() {
    ALOGV("Initializing GastNode class.");

    // Add a CollisionShape to the static body node
    CollisionShape *collision_shape = CollisionShape::_new();
    add_child(collision_shape);

    // Add a mesh instance to the collision shape node
    MeshInstance *mesh_instance = MeshInstance::_new();
    collision_shape->add_child(mesh_instance);

    rectangular_surface = QuadMesh::_new();
    spherical_surface_array = create_spherical_surface_array(
            kEquirectSphereSize, kEquirectSphereMeshBandCount, kEquirectSphereMeshSectorCount);
}

void GastNode::_enter_tree() {
    ALOGV("Entering tree for %s.", get_node_tag(*this));

    // Create the shader object
    Shader *shader = Shader::_new();
    shader->set_custom_defines(kShaderCustomDefines);
    shader->set_code(generate_shader_code());

    // Create the external texture
    ExternalTexture *external_texture = ExternalTexture::_new();

    // Create the shader material.
    ALOGV("Creating GAST shader material.");
    ShaderMaterial *shader_material = ShaderMaterial::_new();
    shader_material->set_shader(shader);
    shader_material->set_shader_param(kGastTextureParamName, external_texture);

    shader_material_ref = Ref<ShaderMaterial>(shader_material);

    update_mesh_and_collision_shape();
    update_render_priority();
}

void GastNode::_exit_tree() {
    ALOGV("Exiting tree.");
    reset_mesh_and_collision_shape();
}

void GastNode::reset_mesh_and_collision_shape() {
    // Unset the GAST mesh resource
    MeshInstance *mesh_instance = get_mesh_instance();
    if (mesh_instance) {
        mesh_instance->set_mesh(Ref<Resource>());
    }

    // Unset the box shape resource
    update_collision_shape();
}

void GastNode::update_mesh_dimensions_and_collision_shape() {
    MeshInstance *mesh_instance = get_mesh_instance();
    auto *mesh = get_mesh();
    if (!mesh_instance || !mesh) {
        ALOGE("Unable to access mesh resource for %s", get_node_tag(*this));
        return;
    }

    Mesh::PrimitiveType primitive;
    Array mesh_surface_array;
    if (projection_mesh_type == ProjectionMeshType::RECTANGULAR) {
        if (is_curved()) {
            primitive = Mesh::PRIMITIVE_TRIANGLE_STRIP;
            mesh_surface_array = create_curved_screen_surface_array(
                    mesh_size, kCurvedScreenRadius, kCurvedScreenResolution);
        } else {
            primitive = Mesh::PRIMITIVE_TRIANGLES;
            rectangular_surface->set_size(mesh_size);
            mesh_surface_array = rectangular_surface->get_mesh_arrays();
        }
    } else if (projection_mesh_type == ProjectionMeshType::EQUIRECTANGULAR) {
        primitive = Mesh::PRIMITIVE_TRIANGLES;
        mesh_surface_array = spherical_surface_array;
    }

    auto *array_mesh = Object::cast_to<ArrayMesh>(mesh);
    if (!array_mesh) {
        ALOGE("Failed to cast mesh to %s.", ArrayMesh::___get_class_name());
        return;
    }
    for (int i = 0; i < array_mesh->get_surface_count(); i++) {
        array_mesh->surface_remove(i);
    }
    array_mesh->add_surface_from_arrays(primitive, mesh_surface_array);

    // Generate updated shader code for the type of projection being used.
    shader_material_ref->get_shader()->set_code(generate_shader_code());
    ALOGV("Setting up GAST shader material resource.");
    mesh_instance->set_surface_material(kDefaultSurfaceIndex, shader_material_ref);

    update_collision_shape();
}

void GastNode::update_mesh_and_collision_shape() {
    Mesh *mesh = ArrayMesh::_new();

    ALOGV("Setting up GAST mesh resource.");
    MeshInstance *mesh_instance = get_mesh_instance();
    if (!mesh_instance || !mesh) {
        return;
    }
    mesh_instance->set_mesh(mesh);

    ALOGV("Setting up GAST shape resource.");
    update_mesh_dimensions_and_collision_shape();
}

void GastNode::update_render_priority() {
    ShaderMaterial *shader_material = *shader_material_ref;
    if (!shader_material) {
        return;
    }

    int render_priority = 0;
    if (render_on_top) {
        render_priority++;
        if (gaze_tracking) {
            render_priority++;
        }
    }
    shader_material->set_render_priority(render_priority);
}

Vector2 GastNode::get_size() {
    return mesh_size;
}

void GastNode::set_size(Vector2 size) {
    this->mesh_size = size;
    update_mesh_dimensions_and_collision_shape();
}

void GastNode::update_collision_shape() {
    CollisionShape *collision_shape = get_collision_shape();
    if (!collision_shape) {
        ALOGW("Unable to retrieve collision shape for %s. Aborting...", get_node_tag(*this));
        return;
    }

    Mesh *mesh = get_mesh();
    if (!is_visible_in_tree() || !collidable || !mesh) {
        collision_shape->set_shape(Ref<Resource>());
    } else {
        if (is_curved()) {
            // TODO: Use `create_trimesh_shape()` instead after resolving why the shape doesn't detect collisions.
            collision_shape->set_shape(mesh->create_convex_shape());
        } else {
            collision_shape->set_shape(mesh->create_convex_shape());
        }
    }
}

void GastNode::update_shader_params() {
    ShaderMaterial *shader_material = get_shader_material();
    if (!shader_material) {
        return;
    }

    shader_material->set_shader_param(kGastEnableBillBoardParamName, gaze_tracking);
    shader_material->set_shader_param(kGastGradientHeightRatioParamName, gradient_height_ratio);
    shader_material->set_shader_param(kGastNodeAlphaParamName, alpha);
}

int GastNode::get_external_texture_id(int surface_index) {
    if (surface_index == kInvalidSurfaceIndex) {
        // Default to the first one
        surface_index = kDefaultSurfaceIndex;
    }

    ExternalTexture *external_texture = get_external_texture(surface_index);
    int tex_id = external_texture == nullptr ? kInvalidTexId
                                             : external_texture->get_external_texture_id();
    ALOGV("Retrieved tex id %d", tex_id);
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

ExternalTexture *GastNode::get_external_texture(int surface_index) {
    ShaderMaterial *shader_material = get_shader_material();
    if (!shader_material) {
        return nullptr;
    }

    ExternalTexture *external_texture = nullptr;
    Ref<Texture> texture = shader_material->get_shader_param(kGastTextureParamName);
    if (texture.is_valid()) {
        external_texture = Object::cast_to<ExternalTexture>(*texture);
    }

    if (external_texture) {
        ALOGV("Found external GastNode texture for node %s", get_node_tag(*this));
    }
    return external_texture;
}

bool
GastNode::handle_ray_cast_input(const String &ray_cast_path, Vector2 relative_collision_point,
                                bool was_press_in_progress) {
    Input *input = Input::get_singleton();
    String node_path = get_path();

    float x_percent = relative_collision_point.x;
    float y_percent = relative_collision_point.y;

    // Check for click actions
    String ray_cast_click_action = get_click_action_from_node_path(ray_cast_path);
    const bool press_in_progress = input->is_action_pressed(ray_cast_click_action);
    if (press_in_progress == was_press_in_progress) {
        GastManager::get_singleton_instance()->on_render_input_hover(node_path, ray_cast_path,
                                                                     x_percent, y_percent);
    } else {
        if (press_in_progress) {
            GastManager::get_singleton_instance()->on_render_input_press(node_path, ray_cast_path,
                                                                         x_percent, y_percent);
        } else {
            GastManager::get_singleton_instance()->on_render_input_release(node_path, ray_cast_path,
                                                                           x_percent, y_percent);
        }
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

    return press_in_progress;
}

String GastNode::generate_shader_code() const {
    String shader_code = kShaderCode;
    if (render_on_top) {
        shader_code += kDisableDepthTestRenderMode;
    }
    if (projection_mesh_type == ProjectionMeshType::EQUIRECTANGULAR) {
        shader_code += kCullFrontRenderMode;
    }
    return shader_code;
}

Vector2 GastNode::get_relative_collision_point(Vector3 absolute_collision_point) {
    Vector2 relative_collision_point = kInvalidCoordinate;

    // Turn the collision point into local space
    Vector3 local_point = to_local(absolute_collision_point);

    // Normalize the collision point. A Gast node is a flat quad so we only worry about
    // the x,y coordinates
    Vector2 node_size = get_size();
    if (node_size.width > 0 && node_size.height > 0) {
        float max_x = node_size.width / 2;
        float min_x = -max_x;
        float max_y = node_size.height / 2;
        float min_y = -max_y;
        relative_collision_point = Vector2((local_point.x - min_x) / node_size.width,
                                           (local_point.y - min_y) / node_size.height);

        // Adjust the y coordinate to match the Android view coordinates system.
        relative_collision_point.y = 1 - relative_collision_point.y;
    }

    return relative_collision_point;
}

}  // namespace gast
