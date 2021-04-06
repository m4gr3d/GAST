#include <gen/QuadMesh.hpp>
#include <utils.h>

#include "projection_mesh.h"

namespace gast {

ProjectionMesh::ProjectionMesh(ProjectionMesh::RectangularProjectionMeshConfig rectangular_config) :
        projection_mesh_type(ProjectionMeshType::RECTANGULAR),
        rectangular_config(rectangular_config),
        equirectangular_config(EquirectangularProjectionMeshConfig()) {}

ProjectionMesh::ProjectionMesh(
        ProjectionMesh::EquirectangularProjectionMeshConfig equirectangular_config) :
        projection_mesh_type(ProjectionMeshType::EQUIRECTANGULAR),
        equirectangular_config(equirectangular_config),
        rectangular_config(RectangularProjectionMeshConfig()) {}

ProjectionMesh::~ProjectionMesh() = default;

ProjectionMesh::ProjectionMeshType ProjectionMesh::get_projection_mesh_type() const {
    return projection_mesh_type;
}

ProjectionMesh::RectangularProjectionMeshConfig
ProjectionMesh::get_rectangular_projection_mesh_config() {
    if (get_projection_mesh_type() != ProjectionMeshType::RECTANGULAR) {
        ALOGE("Requested rectangular config for non-rectangular mesh type");
    }
    return rectangular_config;
}

ProjectionMesh::EquirectangularProjectionMeshConfig
ProjectionMesh::get_equirectangular_projection_mesh_config() {
    if (get_projection_mesh_type() != ProjectionMeshType::EQUIRECTANGULAR) {
        ALOGE("Requested equirectangular config for non-equirectangular mesh type");
    }
    return equirectangular_config;
}

ProjectionMesh
ProjectionMesh::ProjectionMeshBuilder::build_rectangular_projection_mesh(bool is_curved) {
    RectangularProjectionMeshConfig rectangular_config;
    rectangular_config.is_curved = is_curved;
    return ProjectionMesh(rectangular_config);
}

ProjectionMesh
ProjectionMesh::ProjectionMeshBuilder::build_equirectangular_projection_mesh(StereoMode stereo_mode) {
    EquirectangularProjectionMeshConfig equirectangular_config;
    equirectangular_config.stereo_mode = stereo_mode;
    return ProjectionMesh(equirectangular_config);
}

}  // namespace gast
