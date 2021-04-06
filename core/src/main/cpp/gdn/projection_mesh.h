#ifndef CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_H_
#define CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_H_

#include <core/Vector2.hpp>
#include <gen/MeshInstance.hpp>

namespace gast {

namespace {
using namespace godot;
}

class ProjectionMesh {
 public:
  ~ProjectionMesh();

    // Specifies whether a texture is stereoscopic and if so, how it's divided into left+right eyes.
    enum class StereoMode {
        kMono,
        kTopBottom,
        kLeftRight,
        kModeCount,
    };

    // Mirrors src/main/java/org/godotengine/plugin/gast/GastNode#ProjectionMeshType
    enum ProjectionMeshType {
        RECTANGULAR = 0,
        EQUIRECTANGULAR = 1,
        MESH = 2,
    };

    struct RectangularProjectionMeshConfig {
        bool is_curved;
    };

    struct EquirectangularProjectionMeshConfig {
        StereoMode stereo_mode;
    };

    class ProjectionMeshBuilder {
     public:
      static ProjectionMesh build_rectangular_projection_mesh(bool is_curved);
      static ProjectionMesh build_equirectangular_projection_mesh(StereoMode stereo_mode);
    };

    ProjectionMeshType get_projection_mesh_type() const;

    inline bool is_rectangular_projection_mesh() const {
        return get_projection_mesh_type() == ProjectionMeshType::RECTANGULAR;
    }

    inline bool is_equirectangular_projection_mesh() const {
        return get_projection_mesh_type() == ProjectionMeshType::EQUIRECTANGULAR;
    }

    RectangularProjectionMeshConfig get_rectangular_projection_mesh_config();
    EquirectangularProjectionMeshConfig get_equirectangular_projection_mesh_config();

    bool operator==(const ProjectionMesh& other) {
        if (this->projection_mesh_type != other.projection_mesh_type) {
            return false;
        }
        if (this->projection_mesh_type == ProjectionMeshType::RECTANGULAR) {
            return this->rectangular_config.is_curved == other.rectangular_config.is_curved;
        } else if (this->projection_mesh_type == ProjectionMeshType::EQUIRECTANGULAR) {
            return this->equirectangular_config.stereo_mode == other.equirectangular_config.stereo_mode;
        }
        return false;
    }

 private:
  ProjectionMesh(RectangularProjectionMeshConfig rectangular_config);
  ProjectionMesh(EquirectangularProjectionMeshConfig equirectangular_config);

  ProjectionMeshType projection_mesh_type;
  RectangularProjectionMeshConfig rectangular_config;
  EquirectangularProjectionMeshConfig equirectangular_config;
};

}  // namespace gast
#endif //CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_H_
