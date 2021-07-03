#include <gen/ArrayMesh.hpp>
#include <gen/Mesh.hpp>
#include <gen/QuadMesh.hpp>
#include <gen/Shader.hpp>
#include <utils.h>

#include "projection_mesh.h"

namespace gast {

namespace {
const float kDefaultAlpha = 1;
}  // namespace

ProjectionMesh::~ProjectionMesh() {
    reset_mesh();
    reset_external_texture();
    mesh_instance = nullptr;
};

ProjectionMesh::ProjectionMesh(ProjectionMesh::ProjectionMeshType projection_mesh_type) :
        projection_mesh_type(projection_mesh_type),
        mesh_instance(MeshInstance::_new()),
        shader_material_ref(Ref<ShaderMaterial>(ShaderMaterial::_new())),
        gaze_tracking(kDefaultGazeTracking),
        render_on_top(kDefaultRenderOnTop),
        alpha(kDefaultAlpha),
        stereo_mode(kDefaultStereoMode),
        uv_origin_is_bottom_left(kDefaultUvOriginIsBottomLeft) {}

ProjectionMesh::ProjectionMesh() : ProjectionMesh(ProjectionMeshType::RECTANGULAR) {}

void ProjectionMesh::_init() {}

void ProjectionMesh::_register_methods() {
    register_method("set_gaze_tracking", &ProjectionMesh::set_gaze_tracking);
    register_method("is_gaze_tracking", &ProjectionMesh::is_gaze_tracking);
    register_method("set_render_on_top", &ProjectionMesh::set_render_on_top);
    register_method("is_render_on_top", &ProjectionMesh::is_render_on_top);

    register_property<ProjectionMesh, bool>(
            "gaze_tracking",
            &ProjectionMesh::set_gaze_tracking,
            &ProjectionMesh::is_gaze_tracking, kDefaultGazeTracking);
    register_property<ProjectionMesh, bool>(
            "render_on_top",
            &ProjectionMesh::set_render_on_top,
            &ProjectionMesh::is_render_on_top, kDefaultRenderOnTop);
}

}  // namespace gast

