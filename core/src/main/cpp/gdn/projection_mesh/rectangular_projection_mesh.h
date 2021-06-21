#ifndef RECTANGULAR_PROJECTION_MESH_H
#define RECTANGULAR_PROJECTION_MESH_H

#include <core/Vector2.hpp>

#include "projection_mesh.h"

namespace gast {

namespace {
using namespace godot;
const Vector2 kDefaultMeshSize = Vector2(2.0, 1.125);
const float kDefaultGradientHeightRatio = 0.0f;
}

class RectangularProjectionMesh : public ProjectionMesh {
GODOT_CLASS(RectangularProjectionMesh, ProjectionMesh)

public:
    RectangularProjectionMesh();
    RectangularProjectionMesh(Vector2 mesh_size, bool is_curved);
    ~RectangularProjectionMesh();

    void _init();

    static void _register_methods();

    inline void set_mesh_size(Vector2 mesh_size) {
        if (this->mesh_size == mesh_size) {
            return;
        }
        this->mesh_size = mesh_size;
        update_projection_mesh();
        update_listener();
    }

    inline Vector2 get_mesh_size() {
        return mesh_size;
    }

    inline void set_curved(bool is_curved) {
        if (this->is_curved == is_curved) {
            return;
        }
        this->is_curved = is_curved;
        update_projection_mesh();
        update_listener();
    }

    inline bool get_curved() {
        return this->is_curved;
    }

  Vector2 get_relative_collision_point(Vector3 local_collision_point) override {
      Vector2 relative_collision_point = kInvalidCoordinate;

      // Normalize the collision point.
      Vector2 node_size = get_mesh_size();
      if (node_size.width > 0 && node_size.height > 0) {
          float max_x = node_size.width / 2;
          float min_x = -max_x;
          float max_y = node_size.height / 2;
          float min_y = -max_y;
          relative_collision_point = Vector2((local_collision_point.x - min_x) / node_size.width,
                                             (local_collision_point.y - min_y) / node_size.height);

          // Adjust the y coordinate to match the Android view coordinates system.
          relative_collision_point.y = 1 - relative_collision_point.y;
      }

      return relative_collision_point;
  }

    inline float get_gradient_height_ratio() {
        return gradient_height_ratio;
    }

    inline void set_gradient_height_ratio(float ratio) {
        if (this->gradient_height_ratio == ratio) {
            return;
        }
        this->gradient_height_ratio = std::min(1.0f, std::max(0.0f, ratio));
        get_shader_material()->set_shader_param(
                kGastGradientHeightRatioParamName, gradient_height_ratio);
    }

private:
    Vector2 mesh_size;
    bool is_curved;
    float gradient_height_ratio;

    void update_projection_mesh();
};

}  // namespace gast
#endif //RECTANGULAR_PROJECTION_MESH_H
