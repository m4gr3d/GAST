#ifndef PROJECTION_MESH_H
#define PROJECTION_MESH_H

#include <core/Ref.hpp>
#include <core/Vector2.hpp>
#include <gen/ExternalTexture.hpp>
#include <gen/MeshInstance.hpp>
#include <gen/Resource.hpp>
#include <gen/Shader.hpp>
#include <gen/ShaderMaterial.hpp>
#include <gen/Shape.hpp>

#include "projection_mesh_utils.h"
#include "utils.h"

namespace gast {

namespace {
using namespace godot;
const char *kGastTextureParamName = "gast_texture";
const char *kGastEnableBillBoardParamName = "enable_billboard";
const char *kGastGradientHeightRatioParamName = "gradient_height_ratio";
const char *kGastNodeAlphaParamName = "node_alpha";
const int kDefaultSurfaceIndex = 0;
const Vector2 kInvalidCoordinate = Vector2(-1, -1);
}

class ProjectionMesh : public Resource {
GODOT_CLASS(ProjectionMesh, Resource)

public:
    ProjectionMesh();
    ~ProjectionMesh();

    void _init();

    static void _register_methods();

    // Mirrors src/main/java/org/godotengine/plugin/gast/GastNode#ProjectionMeshType
    enum ProjectionMeshType {
        RECTANGULAR = 0,
        EQUIRECTANGULAR = 1,
        MESH = 2,
    };

    inline ProjectionMeshType get_projection_mesh_type() const {
        return projection_mesh_type;
    }

    inline bool is_rectangular_projection_mesh() const {
        return get_projection_mesh_type() == ProjectionMeshType::RECTANGULAR;
    }

    inline bool is_equirectangular_projection_mesh() const {
        return get_projection_mesh_type() == ProjectionMeshType::EQUIRECTANGULAR;
    }

    inline MeshInstance* get_mesh_instance() {
        return this->mesh_instance;
    }

    inline Ref<Shape> get_collision_shape() {
        return this->collision_shape_ref;
    }

    inline void set_external_texture(Ref<ExternalTexture> external_texture) {
        if (!shader_material_ref.is_valid()) {
            ALOGE("Cannot set ExternalTexture, invalid ShaderMaterial.");
            return;
        }
        shader_material_ref->set_shader_param(kGastTextureParamName, external_texture);
    }

    inline ExternalTexture* get_external_texture() {
        if (!shader_material_ref.is_valid()) {
            return nullptr;
        }

        ExternalTexture *external_texture = nullptr;
        Ref<Texture> texture = shader_material_ref->get_shader_param(kGastTextureParamName);
        if (texture.is_valid()) {
            external_texture = Object::cast_to<ExternalTexture>(*texture);
        }
        return external_texture;
    }

    inline void set_render_on_top(bool enable) {
        if (this->render_on_top == enable) {
            return;
        }
        this->render_on_top = enable;
        if (shader_material_ref.is_valid() && shader_material_ref->get_shader().is_valid()) {
            shader_material_ref->get_shader()->set_code(this->generate_shader_code());
        }
        update_render_priority();
    }

    inline bool is_render_on_top() {
        return this->render_on_top;
    }

    inline void set_gaze_tracking(bool gaze_tracking) {
        if (this->gaze_tracking == gaze_tracking) {
            return;
        }
        this->gaze_tracking = gaze_tracking;
        update_render_priority();
        if (shader_material_ref.is_valid()) {
            shader_material_ref->set_shader_param(kGastEnableBillBoardParamName, gaze_tracking);
        }
    }

    inline bool is_gaze_tracking() {
        return gaze_tracking;
    }

    inline void update_render_priority() {
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

    inline void set_alpha(float alpha) {
        if (this->alpha == alpha) {
            return;
        }
        this->alpha = alpha;
        shader_material_ref->set_shader_param(kGastNodeAlphaParamName, alpha);
    }

    inline StereoMode get_stereo_mode() {
        return stereo_mode;
    }

    inline void set_stereo_mode() {
        // TODO: Implement
    }

    inline void reset_mesh() {
        mesh_instance->set_mesh(Ref<Resource>());
        collision_shape_ref = Ref<Resource>();
    }

    inline void reset_external_texture() {
        set_external_texture(Ref<Resource>());
    }

  virtual Vector2 get_relative_collision_point(Vector3 local_collision_point) {
        return kInvalidCoordinate;
    }

 private:
    ProjectionMeshType projection_mesh_type;
    MeshInstance *mesh_instance;
    Ref<ShaderMaterial> shader_material_ref;
    Ref<Shape> collision_shape_ref;
    StereoMode stereo_mode;
    bool render_on_top;
    bool gaze_tracking;
    float alpha;

protected:
    ProjectionMesh(ProjectionMeshType projection_mesh_type);

    inline void set_shader(Ref<Shader> shader) {
        if (!shader_material_ref.is_valid()) {
            ALOGE("Cannot set Shader, invalid ShaderMaterial.");
            return;
        }
        shader_material_ref->set_shader(shader);
    }

    inline Ref<ShaderMaterial> get_shader_material() {
        return shader_material_ref;
    }

    inline void set_mesh(Ref<Mesh> mesh) {
        if (mesh_instance == nullptr) {
            ALOGE("Cannot set Mesh, invalid MeshInstance.");
            return;
        }
        mesh_instance->set_mesh(mesh);
    }

    inline void set_collision_shape(Ref<Shape> collision_shape) {
        collision_shape_ref = collision_shape;
    }

    virtual inline String generate_shader_code() {
        String shader_code = kMonoShaderCode;
        if (is_render_on_top()) {
            shader_code += kDisableDepthTestRenderMode;
        }
        return shader_code;
    }
};

}  // namespace gast
#endif //PROJECTION_MESH_H
