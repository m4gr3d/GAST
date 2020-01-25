#include <gen/Engine.hpp>
#include <gen/MainLoop.hpp>
#include <gen/Material.hpp>
#include <gen/SceneTree.hpp>
#include <gen/ShaderMaterial.hpp>
#include <gen/Object.hpp>
#include <core/Godot.hpp>
#include <gen/Texture.hpp>
#include "gast_manager.h"
#include "utils.h"

namespace gast {

    namespace {
        constexpr int kInvalidTexId = -1;
    } // namespace

    GastManager *GastManager::singleton_instance = nullptr;

    GastManager *GastManager::get_singleton_instance() {
        if (singleton_instance == nullptr) {
            singleton_instance = new GastManager();
        }
        return singleton_instance;
    }

    void GastManager::delete_singleton_instance() {
        delete singleton_instance;
        singleton_instance = nullptr;
    }

    GastManager::GastManager() {}

    GastManager::~GastManager() {}

    int GastManager::get_external_texture_id(const String &mesh_name,
                                             const String &texture_param_name) {
        ExternalTexture *external_texture = get_external_texture(mesh_name, texture_param_name);
        int tex_id = external_texture == nullptr ? kInvalidTexId
                                                 : external_texture->get_external_texture_id();
        return tex_id;
    }

    ExternalTexture *GastManager::get_external_texture(const String &mesh_name,
                                                       const String &texture_param_name) {
        // Go through the mesh instance surface material and look for the external texture.
        ALOGV("Looking for external texture %s", texture_param_name.utf8().get_data());
        MeshInstance *mesh_instance = get_mesh_instance(mesh_name);
        if (!mesh_instance) {
            return nullptr;
        }

        for (int i = 0; i < mesh_instance->get_surface_material_count(); i++) {
            Ref<Material> material = mesh_instance->get_surface_material(i);
            if (material.is_null() || !material->is_class("ShaderMaterial")) {
                continue;
            }

            auto *shader_material = Object::cast_to<ShaderMaterial>(*material);
            Ref<Texture> texture = shader_material->get_shader_param(texture_param_name);
            if (texture.is_null() || !texture->is_class("ExternalTexture")) {
                continue;
            }

            auto *external_texture = Object::cast_to<ExternalTexture>(*texture);
            ALOGV("Found external texture %s", texture_param_name.utf8().get_data());
            return external_texture;
        }
        return nullptr;
    }

    MeshInstance *GastManager::get_mesh_instance(const godot::String &mesh_name) {
        MainLoop *main_loop = Engine::get_singleton()->get_main_loop();
        if (!main_loop || !main_loop->is_class("SceneTree")) {
            ALOGE("Unable to retrieve main loop.");
            return nullptr;
        }

        auto *scene_tree = Object::cast_to<SceneTree>(main_loop);

        // Retrieve the nodes that are part of the given group.
        ALOGV("Retrieving nodes for %s", mesh_name.utf8().get_data());
        Array nodes = scene_tree->get_nodes_in_group(mesh_name);
        if (nodes.empty()) {
            ALOGW("No nodes in group %s", mesh_name.utf8().get_data());
            return nullptr;
        }

        // Return the first node that is a mesh instance.
        for (int i = 0; i < nodes.size(); i++) {
            Node *node = nodes[i];

            // Check if the node is a mesh instance.
            if (!node || !node->is_class("MeshInstance")) {
                continue;
            }

            auto *mesh_instance = Object::cast_to<MeshInstance>(node);
            return mesh_instance;
        }
        return nullptr;
    }

}  // namespace gast