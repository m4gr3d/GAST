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
const float kDefaultAlpha = 1;
}  // namespace

ProjectionMesh::~ProjectionMesh() {
    mesh_instance = nullptr;
};

ProjectionMesh::ProjectionMesh(ProjectionMesh::ProjectionMeshType projection_mesh_type) :
        projection_mesh_type(projection_mesh_type),
        mesh_instance(MeshInstance::_new()),
        shader_material_ref(Ref<ShaderMaterial>(ShaderMaterial::_new())),
        gaze_tracking(kDefaultGazeTracking),
        render_on_top(kDefaultRenderOnTop),
        alpha(kDefaultAlpha),
        stereo_mode(StereoMode::kMono) {}

ProjectionMesh::ProjectionMesh() : ProjectionMesh(ProjectionMeshType::RECTANGULAR) {}

void ProjectionMesh::_init() {}

void ProjectionMesh::_process(const real_t delta) {}

void ProjectionMesh::_register_methods() {
    register_method("_process", &ProjectionMesh::_process);
}

}  // namespace gast

