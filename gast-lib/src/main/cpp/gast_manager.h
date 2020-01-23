#ifndef GAST_MANAGER_H
#define GAST_MANAGER_H

#include <core/String.hpp>
#include <gen/ExternalTexture.hpp>
#include <gen/MeshInstance.hpp>

namespace gast {

    namespace {
        using namespace godot;
    }  // namespace

    class GastManager {
    public:
        static GastManager *get_singleton_instance();

        static void delete_singleton_instance();

        int get_external_texture_id(const String& mesh_name, const String& texture_param_name);

    private:
        ExternalTexture *
        get_external_texture(const String& mesh_name, const String& texture_param_name);

        MeshInstance *get_mesh_instance(const String& mesh_name);

        GastManager();

        ~GastManager();

        static GastManager *singleton_instance;
    };
}  // namespace gast

#endif // GAST_MANAGER_H
