#ifndef CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_RECTANGULAR_PROJECTION_MESH_H_
#define CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_RECTANGULAR_PROJECTION_MESH_H_

#include <core/Vector2.hpp>

#include "projection_mesh.h"

namespace gast {

namespace {
using namespace godot;
}

class RectangularProjectionMesh : public ProjectionMesh {
public:
    RectangularProjectionMesh();
    RectangularProjectionMesh(Vector2 mesh_size, bool is_curved);
    ~RectangularProjectionMesh();


    inline void set_mesh_size(Vector2 mesh_size) {
        if (this->mesh_size == mesh_size) {
            return;
        }
        this->mesh_size = mesh_size;
        update_projection_mesh();
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
    }

    inline bool get_curved() {
        return this->is_curved;
    }

  Vector2 get_relative_collision_point(Vector3 local_collision_point) override {
      Vector2 relative_collision_point = kInvalidCoordinate;

      // Normalize the collision point.
      // TODO: A rectangular projection mesh is a quad so we only worry about the x,y coordinates??
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

private:
    Vector2 mesh_size;
    bool is_curved;

    static inline Vector2 get_default_mesh_size() {
        return Vector2(2.0, 1.125);
    }

    void update_projection_mesh();

    String generate_shader_code();
};

}  // namespace gast
#endif //CORE_SRC_MAIN_CPP_GDN_PROJECTION_MESH_RECTANGULAR_PROJECTION_MESH_H_
