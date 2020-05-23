#include <gen/BoxShape.hpp>
#include <gen/Engine.hpp>
#include <gen/MainLoop.hpp>
#include <gen/Material.hpp>
#include <gen/SceneTree.hpp>
#include <gen/ShaderMaterial.hpp>
#include <gen/Object.hpp>
#include <core/Godot.hpp>
#include <core/NodePath.hpp>
#include <core/Vector2.hpp>
#include <core/Vector3.hpp>
#include <gen/Texture.hpp>
#include <gen/Viewport.hpp>
#include <gen/Resource.hpp>
#include <gen/ResourceLoader.hpp>
#include <gen/QuadMesh.hpp>
#include "gast_manager.h"
#include "utils.h"

namespace gast {

    namespace {
    constexpr float kBoxShapeZExtent = 0.01;
        constexpr int kInvalidTexId = -1;
        const char *kDefaultTextureParamName = "gast_texture";
        const char *kGastNodeGroupName = "gast_node_group";
    } // namespace

    GastManager *GastManager::singleton_instance_ = nullptr;
    bool GastManager::gdn_initialized_ = false;
    bool GastManager::jni_initialized_ = false;

    jobject GastManager::callback_instance_ = nullptr;
    jmethodID GastManager::on_render_input_hover_ = nullptr;
    jmethodID GastManager::on_render_input_press_ = nullptr;
    jmethodID GastManager::on_render_input_release_ = nullptr;

    GastManager::GastManager() {}

    GastManager::~GastManager() {
        reusable_pool_.clear();
    }

    GastManager *GastManager::get_singleton_instance() {
        ALOG_ASSERT(gdn_initialized_ && jni_initialized_,
                    "GastManager is not properly initialized.");
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

    void GastManager::gdn_initialize() {
        gdn_initialized_ = true;
    }

    void GastManager::gdn_shutdown() {
        gdn_initialized_ = false;
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

        on_render_input_hover_ = env->GetMethodID(callback_class, "onRenderInputHover",
                                                  "(Ljava/lang/String;FF)V");
        ALOG_ASSERT(on_render_input_hover_ != nullptr, "Unable to find onRenderInputHover");

        on_render_input_press_ = env->GetMethodID(callback_class, "onRenderInputPress",
                                                  "(Ljava/lang/String;FF)V");
        ALOG_ASSERT(on_render_input_press_ != nullptr, "Unable to find onRenderInputPress");

        on_render_input_release_ = env->GetMethodID(callback_class, "onRenderInputRelease",
                                                    "(Ljava/lang/String;FF)V");
        ALOG_ASSERT(on_render_input_release_ != nullptr, "Unable to find onRenderInputRelease");
    }

    void GastManager::unregister_callback(JNIEnv *env) {
        if (callback_instance_) {
            env->DeleteGlobalRef(callback_instance_);
            callback_instance_ = nullptr;
            on_render_input_hover_ = nullptr;
            on_render_input_press_ = nullptr;
            on_render_input_release_ = nullptr;
        }
    }

    int GastManager::get_external_texture_id(const String &node_path, int surface_index) {
        ExternalTexture *external_texture = get_external_texture(node_path, surface_index);
        int tex_id = external_texture == nullptr ? kInvalidTexId
                                                 : external_texture->get_external_texture_id();
        return tex_id;
    }

    ExternalTexture *GastManager::get_external_texture(const String &node_path, int surface_index) {
        StaticBody* gast_node = get_gast_node(node_path);
        if (!gast_node) {
            return nullptr;
        }

        // Go through the mesh instance surface material and look for the default external texture param.
        MeshInstance *mesh_instance = get_mesh_instance_from_gast_node(*gast_node);
        if (!mesh_instance) {
            return nullptr;
        }

        // Retrieve the mesh resource
        Ref<Mesh> mesh = mesh_instance->get_mesh();
        if (mesh.is_null()) {
            return nullptr;
        }

        ExternalTexture* external_texture = nullptr;

        if (surface_index == kInvalidSurfaceIndex) {
            // Look through all the mesh's surfaces.
            for (int i = 0; i < mesh->get_surface_count(); i++) {
                external_texture = get_external_texture(mesh, i);
            }
        } else {
            if (surface_index < mesh->get_surface_count()) {
                // Look through the surface with the given index.
                external_texture = get_external_texture(mesh, surface_index);
            }
        }
        if (external_texture) {
            ALOGV("Found external Gast texture for node %s", node_path.utf8().get_data());
        }
        return external_texture;
    }

    ExternalTexture *GastManager::get_external_texture(godot::Ref<godot::Mesh> mesh,
                                                       int surface_index) {
        ExternalTexture* external_texture = nullptr;

        Ref<Material> material = mesh->surface_get_material(surface_index);
        if (material.is_valid() && material->is_class("ShaderMaterial")) {
            auto *shader_material = Object::cast_to<ShaderMaterial>(*material);
            Ref<Texture> texture = shader_material->get_shader_param(kDefaultTextureParamName);
            if (texture.is_valid() && texture->is_class("ExternalTexture")) {
                external_texture = Object::cast_to<ExternalTexture>(*texture);
            }
        }

        return external_texture;
    }

    StaticBody* GastManager::get_gast_node(const godot::String &node_path) {
        Node *node = get_node(node_path);
        if (!node || !node->is_class("StaticBody")) {
            ALOGW("Unable to find a StaticBody node with path %s", node_path.utf8().get_data());
            return nullptr;
        }

        auto* static_body = Object::cast_to<StaticBody>(node);
        if (!static_body || !static_body->is_in_group(kGastNodeGroupName)) {
            return nullptr;
        }

        return static_body;
    }

    Node *GastManager::get_node(const godot::String &node_path) {
        if (node_path.empty()) {
            ALOGE("Invalid node path argument: %s", node_path.utf8().get_data());
            return nullptr;
        }

        NodePath node_path_obj(node_path);
        MainLoop *main_loop = Engine::get_singleton()->get_main_loop();
        if (!main_loop || !main_loop->is_class("SceneTree")) {
            ALOGW("Unable to retrieve main loop.");
            return nullptr;
        }

        auto *scene_tree = Object::cast_to<SceneTree>(main_loop);
        Node *node = scene_tree->get_root()->get_node_or_null(node_path_obj);
        return node;
    }

    String GastManager::acquire_and_bind_gast_node(const godot::String &parent_node_path) {
        ALOGV("Retrieving node's parent with path %s", parent_node_path.utf8().get_data());
        Node *parent_node = get_node(parent_node_path);
        if (!parent_node) {
            ALOGE("Unable to retrieve parent node with path %s",
                  parent_node_path.utf8().get_data());
            return "";
        }

        StaticBody* static_body;

        // Check if we have one already setup in the reusable pool, otherwise create a new one.
        if (reusable_pool_.size() == 0) {
            ALOGV("Creating a new Gast node.");
            // Creating a new static body node
            static_body = StaticBody::_new();

            // Add a CollisionShape to the static body node
            CollisionShape* collision_shape = CollisionShape::_new();
            static_body->add_child(collision_shape);

            // Add a mesh instance to the collision shape node
            MeshInstance* mesh_instance = MeshInstance::_new();
            collision_shape->add_child(mesh_instance);

            // Add the new node to the Gast node group. This is how we keep track of the nodes
            // that are created and managed by this plugin.
            static_body->add_to_group(kGastNodeGroupName);
        } else {
            static_body = reusable_pool_.back();
            reusable_pool_.pop_back();
        }

        if (!bind_gast_node(*static_body)) {
            ALOGE("Unable to setup Gast node.");
            return "";
        }

        if (static_body->get_parent() != nullptr) {
            ALOGV("Removing Gast node parent.");
            static_body->get_parent()->remove_child(static_body);
        }

        ALOGV("Adding the Gast node to the parent node.");
        parent_node->add_child(static_body);
        static_body->set_owner(parent_node);

        return (String) static_body->get_path();
    }

    bool GastManager::bind_gast_node(godot::StaticBody &static_body) {
        // Load the script resource.
        ALOGV("Loading script resource.");
        Ref<Resource> script_res = ResourceLoader::get_singleton()->load(
            "res://godot/plugin/v1/gast/GastNodeScript.gdns", "", true);
        if (script_res.is_null() || !script_res->is_class("NativeScript")) {
            ALOGE("Unable to load native script resource.");
            return false;
        }

        // Load the box shape resource.
        ALOGV("Loading GAST box shape resource.");
        Ref<Resource> gast_box_shape_res = ResourceLoader::get_singleton()->load(
            "res://godot/plugin/v1/gast/gast_box_shape.tres");
        if (gast_box_shape_res.is_null() || !gast_box_shape_res->is_class("BoxShape")) {
            ALOGE("Unable to load box shape resource.");
            return false;
        }

        // Load the gast mesh resource.
        ALOGV("Loading GAST mesh resource.");
        Ref<Resource> gast_mesh_res = ResourceLoader::get_singleton()->load(
            "res://godot/plugin/v1/gast/gast_quad_mesh.tres");
        if (gast_mesh_res.is_null() || !gast_mesh_res->is_class("QuadMesh")) {
            ALOGE("Unable to load quad mesh resource.");
            return false;
        }

        ALOGV("Setting up native script resource.");
        static_body.set_script(*script_res);

        ALOGV("Setting up GAST box shape resource.");
        CollisionShape* collision_shape = get_collision_shape_from_gast_node(static_body);
        if (!collision_shape) {
            return false;
        }
        collision_shape->set_shape(gast_box_shape_res);

        ALOGV("Setting up GAST mesh resource.");
        MeshInstance* mesh_instance = get_mesh_instance_from_gast_node(static_body);
        if (!mesh_instance) {
            return false;
        }
        mesh_instance->set_mesh(gast_mesh_res);

        return true;
    }

    void GastManager::unbind_gast_node(godot::StaticBody &static_body) {
        // Unset the GAST mesh resource
        MeshInstance* mesh_instance = get_mesh_instance_from_gast_node(static_body);
        if (mesh_instance) {
            mesh_instance->set_mesh(Ref<Resource>());
        }

        // Unset the box shape resource
        CollisionShape* collision_shape = get_collision_shape_from_gast_node(static_body);
        if (collision_shape) {
            collision_shape->set_shape(Ref<Resource>());
        }

        // Unset the native script resource
        static_body.set_script(nullptr);
    }

    void GastManager::unbind_and_release_gast_node(const godot::String &node_path) {
        // Remove the Gast node from its parent and move it to the reusable pool.
        StaticBody *gast_node = get_gast_node(node_path);
        if (!gast_node) {
            return;
        }

        unbind_gast_node(*gast_node);

        // Remove the Gast node from its parent.
        if (gast_node->get_parent() != nullptr) {
            gast_node->get_parent()->remove_child(gast_node);
            gast_node->set_owner(nullptr);
        }

        // Move the Gast node to the reusable pool.
        reusable_pool_.push_back(gast_node);
    }

    void GastManager::on_render_input(const String &node_path, const Ref<godot::InputEvent> event) {
        // Process input event, and forward results using the methods below.
    }

    void
    GastManager::on_render_input_hover(const String &node_path, float x_percent, float y_percent) {
        if (callback_instance_ && on_render_input_hover_) {
            JNIEnv *env = godot::android_api->godot_android_get_env();
            env->CallVoidMethod(callback_instance_, on_render_input_hover_,
                                string_to_jstring(env, node_path), x_percent, y_percent);
        }
    }

    void
    GastManager::on_render_input_press(const String &node_path, float x_percent, float y_percent) {
        if (callback_instance_ && on_render_input_press_) {
            JNIEnv *env = godot::android_api->godot_android_get_env();
            env->CallVoidMethod(callback_instance_, on_render_input_press_,
                                string_to_jstring(env, node_path), x_percent, y_percent);
        }
    }

    void GastManager::on_render_input_release(const String &node_path, float x_percent,
                                              float y_percent) {
        if (callback_instance_ && on_render_input_release_) {
            JNIEnv *env = godot::android_api->godot_android_get_env();
            env->CallVoidMethod(callback_instance_, on_render_input_release_,
                                string_to_jstring(env, node_path), x_percent, y_percent);
        }
    }

    String GastManager::update_gast_node_parent(const String &node_path,
                                                const String &new_parent_node_path) {
        Node *node = get_gast_node(node_path);
        if (!node) {
            ALOGW("Unable to retrieve Gast node with path %s", node_path.utf8().get_data());
            return node_path;
        }

        // Check if current parent differs from new one.
        if (node->get_parent() != nullptr) {
            String parent_node_path = node->get_parent()->get_path();
            if (parent_node_path == new_parent_node_path) {
                ALOGV("Current parent is same as newly proposed one.");
                return node_path;
            }
        }

        // Check if the new parent exists.
        Node *new_parent = get_node(new_parent_node_path);
        if (!new_parent) {
            ALOGW("Unable to retrieve new parent node with path %s",
                  new_parent_node_path.utf8().get_data());
            return node_path;
        }

        // Perform the update
        if (node->get_parent() != nullptr) {
            node->get_parent()->remove_child(node);
        }

        new_parent->add_child(node);
        node->set_owner(new_parent);

        return (String) node->get_path();
    }

    void GastManager::update_gast_node_visibility(const godot::String &node_path,
                                                  bool should_duplicate_parent_visibility,
                                                  bool visible) {
        Spatial *gast_node = get_gast_node(node_path);
        if (!gast_node) {
            ALOGW("Unable to retrieve Gast node with path %s", node_path.utf8().get_data());
            return;
        }

        bool is_visible = should_duplicate_parent_visibility ? gast_node->is_visible_in_tree()
                                                             : gast_node->is_visible();
        if (is_visible != visible) {
            gast_node->set_visible(visible);
        }
    }

    void GastManager::update_gast_node_size(const godot::String &node_path, float width,
                                            float height) {
        // We need to update both the size of the box shape and the size of the quad mesh resources.
        StaticBody* gast_node = get_gast_node(node_path);
        if (!gast_node) {
            ALOGW("Unable to retrieve Gast node with path %s", node_path.utf8().get_data());
            return;
        }

        CollisionShape* collision_shape = get_collision_shape_from_gast_node(*gast_node);
        MeshInstance* mesh_instance = get_mesh_instance_from_gast_node(*gast_node);
        if (!collision_shape || !mesh_instance) {
            ALOGW("Invalid Gast node %s. Aborting...", node_path.utf8().get_data());
            return;
        }

        // Retrieve the box shape
        Ref<Shape> shape_ref = collision_shape->get_shape();
        if (shape_ref.is_null() || !shape_ref->is_class("BoxShape")) {
            ALOGE("Unable to access box shape resource for %s", node_path.utf8().get_data());
            return;
        }

        // Retrieve the quad mesh.
        Ref<Mesh> mesh_ref = mesh_instance->get_mesh();
        if (mesh_ref.is_null() || !mesh_ref->is_class("QuadMesh")) {
            ALOGE("Unable to access mesh resource for %s", node_path.utf8().get_data());
            return;
        }

        auto* box_shape = Object::cast_to<BoxShape>(*shape_ref);
        if (!box_shape) {
            ALOGE("Failed cast to BoxShape.");
            return;
        }

        auto *quad_mesh = Object::cast_to<QuadMesh>(*mesh_ref);
        if (!quad_mesh) {
            ALOGE("Failed cast to QuadMesh.");
            return;
        }

        quad_mesh->set_size(Vector2(width, height));
        // The X & Y extends are half of the regular with and height, and the Z extent is fixed.
        box_shape->set_extents(Vector3(width / 2.0f, height / 2.0f, kBoxShapeZExtent));
    }

    void GastManager::update_gast_node_local_translation(const godot::String &node_path,
                                                         float x_translation,
                                                         float y_translation,
                                                         float z_translation) {
        Spatial *gast_node = get_gast_node(node_path);
        if (!gast_node) {
            ALOGW("Unable to retrieve Gast node with path %s", node_path.utf8().get_data());
            return;
        }

        gast_node->set_translation(Vector3(x_translation, y_translation, z_translation));
    }

    void GastManager::update_gast_node_local_scale(const godot::String &node_path, float x_scale,
                                                   float y_scale) {
        Spatial *gast_node = get_gast_node(node_path);
        if (!gast_node) {
            ALOGW("Unable to retrieve Gast node with path %s", node_path.utf8().get_data());
            return;
        }

        gast_node->set_scale(Vector3(x_scale, y_scale, 1));
    }

    void GastManager::update_gast_node_local_rotation(const godot::String &node_path,
                                                      float x_rotation, float y_rotation,
                                                      float z_rotation) {
        Spatial *gast_node = get_gast_node(node_path);
        if (!gast_node) {
            ALOGW("Unable to retrieve Gast node with path %s", node_path.utf8().get_data());
            return;
        }

        gast_node->set_rotation_degrees(Vector3(x_rotation, y_rotation, z_rotation));
    }

}  // namespace gast
