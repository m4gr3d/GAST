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

    void set_mesh_size(Vector2 size);

    Vector2 get_mesh_size() {
        return mesh_size;
    }

    void set_curved(bool is_curved);

    bool get_curved() {
        return this->is_curved;
    }

    Vector2 get_relative_collision_point(Vector3 local_collision_point) override;

    inline float get_gradient_height_ratio() {
        return gradient_height_ratio;
    }

    void set_gradient_height_ratio(float ratio);

    int get_mesh_count() const override;

    void update_properties(ProjectionMesh *projection_mesh) override;

protected:
    bool should_use_alpha_shader_code() override;

    void update_projection_mesh() override;

private:
    Vector2 mesh_size;
    bool is_curved;
    float gradient_height_ratio;
};

}  // namespace gast
#endif //RECTANGULAR_PROJECTION_MESH_H
