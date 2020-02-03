#include <gen/Engine.hpp>
#include <gen/MainLoop.hpp>
#include <gen/Material.hpp>
#include <gen/SceneTree.hpp>
#include <gen/ShaderMaterial.hpp>
#include <gen/Object.hpp>
#include <core/Godot.hpp>
#include <core/NodePath.hpp>
#include <gen/Texture.hpp>
#include <gen/Viewport.hpp>
#include <gen/Mesh.hpp>
#include <gen/Resource.hpp>
#include <gen/ResourceLoader.hpp>
#include <gen/QuadMesh.hpp>
#include "gast_manager.h"
#include "utils.h"

namespace gast {

    namespace {
        constexpr int kInvalidTexId = -1;
        const char *kDefaultTextureParamName = "sampler_texture";
    } // namespace

    GastManager *GastManager::singleton_instance_ = nullptr;
    bool GastManager::gdn_initialized_ = false;
    bool GastManager::jni_initialized_ = false;

    jobject GastManager::callback_instance_ = nullptr;
    jmethodID GastManager::on_gl_process_ = nullptr;
    jmethodID GastManager::on_gl_input_hover_ = nullptr;
    jmethodID GastManager::on_gl_input_press_ = nullptr;
    jmethodID GastManager::on_gl_input_release_ = nullptr;

    GastManager::GastManager() {}

    GastManager::~GastManager() {}

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

        on_gl_process_ = env->GetMethodID(callback_class, "onGLProcess", "(Ljava/lang/String;F)V");
        ALOG_ASSERT(on_gl_process_ != nullptr, "Unable to find onGLProcess");

        on_gl_input_hover_ = env->GetMethodID(callback_class, "onGLInputHover",
                                              "(Ljava/lang/String;FF)V");
        ALOG_ASSERT(on_gl_input_hover_ != nullptr, "Unable to find onGLInputHover");

        on_gl_input_press_ = env->GetMethodID(callback_class, "onGLInputPress",
                                              "(Ljava/lang/String;FF)V");
        ALOG_ASSERT(on_gl_input_press_ != nullptr, "Unable to find onGLInputPress");

        on_gl_input_release_ = env->GetMethodID(callback_class, "onGLInputRelease",
                                                "(Ljava/lang/String;FF)V");
        ALOG_ASSERT(on_gl_input_release_ != nullptr, "Unable to find onGLInputRelease");
    }

    void GastManager::unregister_callback(JNIEnv *env) {
        if (callback_instance_) {
            env->DeleteGlobalRef(callback_instance_);
            callback_instance_ = nullptr;
            on_gl_process_ = nullptr;
            on_gl_input_hover_ = nullptr;
            on_gl_input_press_ = nullptr;
            on_gl_input_release_ = nullptr;
        }
    }

    int GastManager::get_external_texture_id(const String &node_path) {
        ExternalTexture *external_texture = get_external_texture(node_path);
        int tex_id = external_texture == nullptr ? kInvalidTexId
                                                 : external_texture->get_external_texture_id();
        return tex_id;
    }

    ExternalTexture *GastManager::get_external_texture(const String &node_path) {
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

        for (int i = 0; i < mesh->get_surface_count(); i++) {
            Ref<Material> material = mesh->surface_get_material(i);
            if (material.is_null() || !material->is_class("ShaderMaterial")) {
                continue;
            }

            auto *shader_material = Object::cast_to<ShaderMaterial>(*material);
            Ref<Texture> texture = shader_material->get_shader_param(kDefaultTextureParamName);
            if (texture.is_null() || !texture->is_class("ExternalTexture")) {
                continue;
            }

            auto *external_texture = Object::cast_to<ExternalTexture>(*texture);
            ALOGV("Found external sampler texture for node %s", node_path.utf8().get_data());
            return external_texture;
        }
        return nullptr;
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

    String GastManager::create_mesh_instance(const godot::String &parent_node_path) {
        ALOGV("Retrieving node's parent with path %s", parent_node_path.utf8().get_data());
        Node *parent_node = get_node(parent_node_path);
        if (!parent_node) {
            ALOGE("Unable to retrieve parent node with path %s",
                  parent_node_path.utf8().get_data());
            return "";
        }

        ALOGV("Creating a new mesh instance.");
        MeshInstance *mesh_instance = MeshInstance::_new();
        if (mesh_instance->get_parent() != nullptr) {
            ALOGV("Removing mesh instance parent.");
            mesh_instance->get_parent()->remove_child(mesh_instance);
        }

        ALOGV("Adding the mesh instance to the parent node.");
        parent_node->add_child(mesh_instance);
        mesh_instance->set_owner(parent_node);

        return (String) mesh_instance->get_path();
    }

    void GastManager::setup_mesh_instance(const godot::String &node_path) {
        ALOGV("Retrieving mesh instance with path %s", node_path.utf8().get_data());
        MeshInstance *mesh_instance = get_mesh_instance(node_path);
        if (!mesh_instance) {
            ALOGE("Unable to retrieve mesh instance with path %s", node_path.utf8().get_data());
            return;
        }

        // Load the gast mesh resource.
        ALOGV("Loading GAST mesh resource.");
        Ref<Resource> gast_mesh_res = ResourceLoader::get_singleton()->load(
                "res://plugin_artifacts/addons/gastlib/gast_quad_mesh.tres");
        if (gast_mesh_res.is_null() || !gast_mesh_res->is_class("QuadMesh")) {
            ALOGE("Unable to load the target resource.");
            return;
        }

        ALOGV("Setting up GAST mesh resource.");
        mesh_instance->set_mesh(gast_mesh_res);

        // Load the script resource.
        ALOGV("Loading script resource.");
        Ref<Resource> script_res = ResourceLoader::get_singleton()->load(
                "res://plugin_artifacts/addons/gastlib/MeshInstanceProxy.gdns", "", true);
        if (script_res.is_null() || !script_res->is_class("NativeScript")) {
            ALOGE("Unable to load native script resource.");
            return;
        }

        ALOGV("Setting up native script resource.");
        mesh_instance->set_script(*script_res);

        // Set the node to processing
        mesh_instance->set_process(true);
        mesh_instance->set_process_input(true);
    }

    void GastManager::on_gl_process(const String &node_path, float delta) {
        if (callback_instance_ && on_gl_process_) {
            JNIEnv *env = godot::android_api->godot_android_get_env();
            env->CallVoidMethod(callback_instance_, on_gl_process_,
                                string_to_jstring(env, node_path), delta);
        }
    }

    void GastManager::on_gl_input(const String &node_path, const Ref<godot::InputEvent> event) {
        // Process input event, and forward results using the methods below.
    }

    void
    GastManager::on_gl_input_hover(const String &node_path, float x_percent, float y_percent) {
        if (callback_instance_ && on_gl_input_hover_) {
            JNIEnv *env = godot::android_api->godot_android_get_env();
            env->CallVoidMethod(callback_instance_, on_gl_input_hover_,
                                string_to_jstring(env, node_path), x_percent, y_percent);
        }
    }

    void
    GastManager::on_gl_input_press(const String &node_path, float x_percent, float y_percent) {
        if (callback_instance_ && on_gl_input_press_) {
            JNIEnv *env = godot::android_api->godot_android_get_env();
            env->CallVoidMethod(callback_instance_, on_gl_input_press_,
                                string_to_jstring(env, node_path), x_percent, y_percent);
        }
    }

    void GastManager::on_gl_input_release(const String &node_path, float x_percent,
                                          float y_percent) {
        if (callback_instance_ && on_gl_input_release_) {
            JNIEnv *env = godot::android_api->godot_android_get_env();
            env->CallVoidMethod(callback_instance_, on_gl_input_release_,
                                string_to_jstring(env, node_path), x_percent, y_percent);
        }
    }

}  // namespace gast