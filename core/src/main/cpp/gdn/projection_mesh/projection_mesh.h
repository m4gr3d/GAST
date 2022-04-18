#ifndef PROJECTION_MESH_H
#define PROJECTION_MESH_H

#include <core/Array.hpp>
#include <core/Ref.hpp>
#include <core/Vector2.hpp>
#include <gen/CollisionShape.hpp>
#include <gen/ExternalTexture.hpp>
#include <gen/MeshInstance.hpp>
#include <gen/Resource.hpp>
#include <gen/Shader.hpp>
#include <gen/ShaderMaterial.hpp>
#include <gen/Shape.hpp>
#include <gen/SpatialMaterial.hpp>
#include <vector>

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
const bool kDefaultCollidable = true;
const bool kDefaultGazeTracking = false;
const bool kDefaultRenderOnTop = false;
const bool kDefaultHasTransparency = true;
// This threshold is used to help determine when we should enable transparency in the shader.
const float kAlphaThreshold = 0.94f;
const bool kDefaultUvOriginIsBottomLeft = false;
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

    ProjectionMeshType get_projection_mesh_type() const {
        return projection_mesh_type;
    }

    bool is_custom_projection_mesh() const {
        return get_projection_mesh_type() == ProjectionMeshType::MESH;
    }

    bool is_rectangular_projection_mesh() const {
        return get_projection_mesh_type() == ProjectionMeshType::RECTANGULAR;
    }

    bool is_equirectangular_projection_mesh() const {
        return get_projection_mesh_type() == ProjectionMeshType::EQUIRECTANGULAR;
    }

    void set_external_texture(Ref<ExternalTexture> external_texture) {
        update_shaders_param(kGastTextureParamName, external_texture);
    }

    void set_render_on_top(bool enable) {
        if (this->render_on_top == enable) {
            return;
        }
        this->render_on_top = enable;
        update_shader_code();
        update_render_priority();
    }

    bool is_render_on_top() const {
        return this->render_on_top;
    }

    void set_depth_draw_mode(SpatialMaterial::DepthDrawMode mode) {
        if (this->depth_draw_mode == mode) {
            return;
        }

        this->depth_draw_mode = mode;
        update_shader_code();
    }

    bool is_collidable() const {
        return this->collidable;
    }

    void set_gaze_tracking(bool gaze_tracking) {
        if (this->gaze_tracking == gaze_tracking) {
            return;
        }
        this->gaze_tracking = gaze_tracking;
        update_render_priority();
        update_shaders_param(kGastEnableBillBoardParamName, gaze_tracking);
    }

    bool is_gaze_tracking() const {
        return gaze_tracking;
    }

    void update_render_priority() const;

    void set_alpha(float alpha) {
        if (this->alpha == alpha) {
            return;
        }
        this->alpha = alpha;
        update_shader_code();
        update_shaders_param(kGastNodeAlphaParamName, alpha);
    }

    void set_has_transparency(bool has_transparency) {
        if (this->has_transparency == has_transparency) {
            return;
        }
        this->has_transparency = has_transparency;
        update_shader_code();
    }

    void set_collidable(bool collidable) {
        if (this->collidable == collidable) {
            return;
        }

        this->collidable = collidable;
        update_collision_shapes();
    }

    StereoMode get_stereo_mode() const {
        return stereo_mode;
    }

    void set_stereo_mode(StereoMode stereo_mode) {
        if (this->stereo_mode == stereo_mode) {
            return;
        }

        this->stereo_mode = stereo_mode;
        update_sampling_transforms();
    }

    void set_uv_origin_is_bottom_left(bool uv_origin_is_bottom_left) {
        if (this->uv_origin_is_bottom_left == uv_origin_is_bottom_left) {
            return;
        }
        this->uv_origin_is_bottom_left = uv_origin_is_bottom_left;
        update_sampling_transforms();
    }

    void reset_meshes() const;

    void reset_external_texture() {
        set_external_texture(Ref<Resource>());
    }

    virtual Vector2 get_relative_collision_point(Vector3 local_collision_point) {
        return kInvalidCoordinate;
    }

    virtual int get_mesh_count() const {
        return 0;
    }

    virtual void update_projection_mesh() {}

    CollisionShape * get_collision_shape(int index) const;

    MeshInstance *get_mesh_instance(int index) const;

    Array get_shader_materials() const;

    void update_collision_shapes() const;

    virtual void update_properties(ProjectionMesh *projection_mesh);

protected:
    struct ProjectionMeshData {
        CollisionShape *collision_shape;
        MeshInstance *mesh_instance;
        Ref<Shape> shape;
        Ref<ShaderMaterial> shader_material;
    };

    void update_shaders_param(const String& param, const Variant& value) const;

    void update_shader_param(int index, const String& param, const Variant& value) const;

    void update_shader_code();

    virtual bool should_use_alpha_shader_code();

    ProjectionMesh(ProjectionMeshType projection_mesh_type);

    void set_shader(int index, const Ref<Shader>& shader) const;

    void set_mesh(int index, const Ref<Mesh>& mesh) const;

    void set_collision_shape(int index, const Ref<Shape>& collision_shape) const;

    virtual String generate_shader_code();

    void update_sampling_transforms();

private:
    ProjectionMeshType projection_mesh_type;
    std::vector<ProjectionMeshData*> projection_mesh_data_list{};
    StereoMode stereo_mode;

    SpatialMaterial::DepthDrawMode depth_draw_mode = SpatialMaterial::DEPTH_DRAW_OPAQUE_ONLY;
    bool render_on_top;
    bool gaze_tracking;
    bool uv_origin_is_bottom_left;
    float alpha;
    bool has_transparency;
    bool collidable;
};

}  // namespace gast
#endif //PROJECTION_MESH_H
