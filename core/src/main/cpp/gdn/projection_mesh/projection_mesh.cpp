#include <gen/ArrayMesh.hpp>
#include <gen/Mesh.hpp>
#include <gen/QuadMesh.hpp>
#include <gen/Shader.hpp>
#include <utils.h>

#include "projection_mesh.h"

namespace gast {

namespace {
const bool kDefaultGazeTracking = false;
const bool kDefaultRenderOnTop = false;
const float kDefaultGradientHeightRatio = 0.0f;
const float kDefaultAlpha = 1;
}  // namespace

ProjectionMesh::~ProjectionMesh() = default;

ProjectionMesh::ProjectionMeshType ProjectionMesh::get_projection_mesh_type() const {
    return projection_mesh_type;
}

ProjectionMesh::ProjectionMesh(
        ProjectionMesh::ProjectionMeshType projection_mesh_type,
        MeshInstance *mesh_instance,
        Ref<ShaderMaterial> shader_material_ref) :
    projection_mesh_type(projection_mesh_type),
    mesh_instance(mesh_instance),
    shader_material_ref(shader_material_ref),
    gaze_tracking(kDefaultGazeTracking),
    render_on_top(kDefaultRenderOnTop),
    alpha(kDefaultAlpha),
    gradient_height_ratio(kDefaultGradientHeightRatio) {
    // TODO: Implement other stereo modes.
    stereo_mode = StereoMode::kMono;
}

}  // namespace gast

