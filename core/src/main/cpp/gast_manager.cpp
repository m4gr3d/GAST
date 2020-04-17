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
        constexpr int kInvalidTexId = -1;
        const char *kDefaultTextureParamName = "gast_texture";
        const char *kGastNodeGroupName = "gast_node_group";
        const char *kNodeWithDefaultGastShaderGroupName = "node_with_default_gast_shader";
    } // namespace

    GastManager *GastManager::singleton_instance_ = nullptr;
    bool GastManager::gdn_initialized_ = false;
    bool GastManager::jni_initialized_ = false;

    jobject GastManager::callback_instance_ = nullptr;
    jmethodID GastManager::on_render_process_ = nullptr;
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

        on_render_process_ = env->GetMethodID(callback_class, "onRenderProcess", "(Ljava/lang/String;F)V");
        ALOG_ASSERT(on_render_process_ != nullptr, "Unable to find onRenderProcess");

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
            on_render_process_ = nullptr;
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
        // Go through the mesh instance surface material and look for the default external texture param.
        MeshInstance *mesh_instance = get_mesh_instance(node_path);
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

    MeshInstance *GastManager::get_mesh_instance(const godot::String &node_path) {
        Node *node = get_node(node_path);
        if (!node || !node->is_class("MeshInstance")) {
            ALOGW("Unable to find a MeshInstance node with path %s", node_path.utf8().get_data());
            return nullptr;
        }

        auto *mesh_instance = Object::cast_to<MeshInstance>(node);
        return mesh_instance;
    }

    Spatial *GastManager::get_spatial_node(const gast::String &node_path) {
        Node *node = get_node(node_path);
        if (!node || !node->is_class("Spatial")) {
            ALOGW("Unable to find a Spatial node with path %s",
                  node_path.utf8().get_data());
            return nullptr;
        }

        auto* spatial_node = Object::cast_to<Spatial>(node);
        return spatial_node;
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

    String GastManager::acquire_and_bind_quad_mesh_instance(const godot::String &parent_node_path) {
        ALOGV("Retrieving node's parent with path %s", parent_node_path.utf8().get_data());
        Node *parent_node = get_node(parent_node_path);
        if (!parent_node) {
            ALOGE("Unable to retrieve parent node with path %s",
                  parent_node_path.utf8().get_data());
            return "";
        }

        MeshInstance *mesh_instance;

        // Check if we have one already setup in the reusable pool, otherwise create a new one.
        if (reusable_pool_.size() == 0) {
            ALOGV("Creating a new mesh instance.");
            mesh_instance = MeshInstance::_new();

            // Add the mesh instance to the Gast node group. This is how we keep track of the mesh
            // instances that are created and managed by this plugin.
            mesh_instance->add_to_group(kGastNodeGroupName);
        } else {
            mesh_instance = reusable_pool_.back();
            reusable_pool_.pop_back();
        }

        if (!bind_mesh_instance(*mesh_instance)) {
            ALOGE("Unable to setup mesh instance.");
            return "";
        }

        if (mesh_instance->get_parent() != nullptr) {
            ALOGV("Removing mesh instance parent.");
            mesh_instance->get_parent()->remove_child(mesh_instance);
        }

        ALOGV("Adding the mesh instance to the parent node.");
        parent_node->add_child(mesh_instance);
        mesh_instance->set_owner(parent_node);

        return (String) mesh_instance->get_path();
    }

    bool GastManager::bind_mesh_instance(const godot::String &node_path) {
        ALOGV("Retrieving mesh instance with path %s", node_path.utf8().get_data());
        MeshInstance *mesh_instance = get_mesh_instance(node_path);
        if (!mesh_instance) {
            ALOGE("Unable to retrieve mesh instance with path %s",
                  node_path.utf8().get_data());
            return false;
        }

        return bind_mesh_instance(*mesh_instance);
    }

    bool GastManager::bind_mesh_instance(godot::MeshInstance &mesh_instance) {
        // Check if the mesh instance has a valid Gast shader.
        Ref<Mesh> current_mesh = mesh_instance.get_mesh();
        bool has_valid_gast_shader = is_mesh_with_gast_shader(current_mesh);

        // Load the script resource.
        ALOGV("Loading script resource.");
        Ref<Resource> script_res = ResourceLoader::get_singleton()->load(
                "res://godot/plugin/v1/gast/MeshInstanceProxy.gdns", "", true);
        if (script_res.is_null() || !script_res->is_class("NativeScript")) {
            ALOGE("Unable to load native script resource.");
            return false;
        }

        if (!has_valid_gast_shader) {
            // Load the gast mesh resource.
            ALOGV("Loading GAST mesh resource.");
            Ref<Resource> gast_mesh_res = ResourceLoader::get_singleton()->load(
                    "res://godot/plugin/v1/gast/gast_quad_mesh.tres");
            if (gast_mesh_res.is_null() || !gast_mesh_res->is_class("QuadMesh")) {
                ALOGE("Unable to load the target resource.");
                return false;
            }

            ALOGV("Setting up GAST mesh resource.");
            mesh_instance.set_mesh(gast_mesh_res);
            mesh_instance.add_to_group(kNodeWithDefaultGastShaderGroupName);
        }

        ALOGV("Setting up native script resource.");
        mesh_instance.set_script(*script_res);

        // Set the node to processing
        mesh_instance.set_process(true);
        mesh_instance.set_process_input(true);

        return true;
    }

    bool GastManager::is_mesh_with_gast_shader(Ref<Mesh> mesh) {
        if (mesh.is_null()) {
            return false;
        }

        // Go through the mesh material
        for (int i = 0; i < mesh->get_surface_count(); i++) {
            // Check the mesh has a valid Gast external texture.
            auto *external_texture = get_external_texture(mesh, i);
            if (external_texture) {
                return true;
            }
        }

        return false;
    }

    void GastManager::unbind_mesh_instance(godot::MeshInstance &mesh_instance) {
        // Stop the node's processing.
        mesh_instance.set_process(false);
        mesh_instance.set_process_input(false);

        if (mesh_instance.is_in_group(kNodeWithDefaultGastShaderGroupName)) {
            // Unset the GAST mesh resource
            mesh_instance.set_mesh(Ref<Resource>());
            mesh_instance.remove_from_group(kNodeWithDefaultGastShaderGroupName);
        }

        // Unset the native script resource
        mesh_instance.set_script(nullptr);
    }

    void GastManager::unbind_mesh_instance(const godot::String &node_path) {
        MeshInstance *mesh_instance = get_mesh_instance(node_path);
        if (!mesh_instance) {
            return;
        }

        unbind_mesh_instance(*mesh_instance);
    }

    void GastManager::unbind_and_release_quad_mesh_instance(const godot::String &node_path) {
        // Remove the mesh instance from its parent and move it to the reusable pool.
        MeshInstance *mesh_instance = get_mesh_instance(node_path);
        if (!mesh_instance) {
            return;
        }

        // Check if the mesh instance is in the correct group.
        if (!mesh_instance->is_in_group(kGastNodeGroupName)) {
            ALOGW("Mesh instance is not in the target group. Aborting...");
            return;
        }

        unbind_mesh_instance(*mesh_instance);

        // Remove the mesh instance children.
        int64_t child_count = mesh_instance->get_child_count();
        for (int i = 0; i < child_count; i++) {
            mesh_instance->remove_child(mesh_instance->get_child(i));
        }

        // Remove the mesh instance from its parent.
        if (mesh_instance->get_parent() != nullptr) {
            mesh_instance->get_parent()->remove_child(mesh_instance);
            mesh_instance->set_owner(nullptr);
        }

        // Move the mesh instance to the reusable pool.
        reusable_pool_.push_back(mesh_instance);
    }

    void GastManager::on_render_process(const String &node_path, float delta) {
        if (callback_instance_ && on_render_process_) {
            JNIEnv *env = godot::android_api->godot_android_get_env();
            env->CallVoidMethod(callback_instance_, on_render_process_,
                                string_to_jstring(env, node_path), delta);
        }
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

    String GastManager::update_node_parent(const String &node_path,
                                           const String &new_parent_node_path) {
        Node *node = get_node(node_path);
        if (!node) {
            ALOGW("Unable to retrieve node with path %s", node_path.utf8().get_data());
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

    void GastManager::update_spatial_node_visibility(const godot::String &node_path,
                                                     bool should_duplicate_parent_visibility,
                                                     bool visible) {
        Spatial *spatial_node = get_spatial_node(node_path);
        if (!spatial_node) {
            ALOGW("Unable to retrieve spatial node with path %s", node_path.utf8().get_data());
            return;
        }

        bool is_visible = should_duplicate_parent_visibility ? spatial_node->is_visible_in_tree()
                                                             : spatial_node->is_visible();
        if (is_visible != visible) {
            spatial_node->set_visible(visible);
        }
    }

    void GastManager::update_quad_mesh_instance_size(const godot::String &node_path, float width,
                                                     float height) {
        MeshInstance *mesh_instance = get_mesh_instance(node_path);
        if (!mesh_instance) {
            ALOGW("Unable to retrieve mesh instance with path %s", node_path.utf8().get_data());
            return;
        }

        // Retrieve and resize the quad mesh.
        Ref<Mesh> mesh_ref = mesh_instance->get_mesh();
        if (mesh_ref.is_null() || !mesh_ref->is_class("QuadMesh")) {
            ALOGE("Unable to access mesh resource for %s", node_path.utf8().get_data());
            return;
        }

        auto *mesh = Object::cast_to<QuadMesh>(*mesh_ref);
        if (!mesh) {
            ALOGE("Failed cast to QuadMesh.");
            return;
        }

        mesh->set_size(Vector2(width, height));
    }

    void GastManager::update_spatial_node_local_translation(const godot::String &node_path,
                                                            float x_translation,
                                                            float y_translation,
                                                            float z_translation) {
        Spatial *spatial_node = get_spatial_node(node_path);
        if (!spatial_node) {
            ALOGW("Unable to retrieve spatial node with path %s", node_path.utf8().get_data());
            return;
        }

        spatial_node->set_translation(Vector3(x_translation, y_translation, z_translation));
    }

    Vector3 GastManager::get_spatial_node_global_translation(const gast::String &node_path) {
        Spatial* spatial_node = get_spatial_node(node_path);
        if (!spatial_node) {
            ALOGE("Unable to retrieve spatial node with path %s", node_path.utf8().get_data());
            return Vector3();
        }

        return spatial_node->to_global(spatial_node->get_translation());
    }

    void GastManager::update_spatial_node_local_scale(const godot::String &node_path, float x_scale,
                                                      float y_scale) {
        Spatial *spatial_node = get_spatial_node(node_path);
        if (!spatial_node) {
            ALOGW("Unable to retrieve spatial node with path %s", node_path.utf8().get_data());
            return;
        }

        spatial_node->set_scale(Vector3(x_scale, y_scale, 1));
    }

    void GastManager::update_spatial_node_local_rotation(const godot::String &node_path,
                                                         float x_rotation, float y_rotation,
                                                         float z_rotation) {
        Spatial *spatial_node = get_spatial_node(node_path);
        if (!spatial_node) {
            ALOGW("Unable to retrieve spatial node with path %s", node_path.utf8().get_data());
            return;
        }

        spatial_node->set_rotation_degrees(Vector3(x_rotation, y_rotation, z_rotation));
    }

}  // namespace gast