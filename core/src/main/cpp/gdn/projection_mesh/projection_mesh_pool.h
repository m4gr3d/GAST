#ifndef PROJECTION_MESH_POOL_H
#define PROJECTION_MESH_POOL_H

#include <map>

#include "projection_mesh.h"

namespace gast {

class ProjectionMeshPool {
public:
    ProjectionMeshPool() = default;
    ~ProjectionMeshPool() {
        projection_mesh_map.clear();
    }

    template<class T>
    inline T *get_or_create_projection_mesh() {
        if (projection_mesh_map.find(T::___get_class_name()) == projection_mesh_map.end()) {
            T *projection_mesh = T::_new();
            projection_mesh_map[T::___get_class_name()] = projection_mesh;
            return projection_mesh;
        }
        return Object::cast_to<T>(projection_mesh_map[T::___get_class_name()]);
    }

private:
    std::map<const char*, ProjectionMesh*> projection_mesh_map;
};

}  // namespace gast
#endif //PROJECTION_MESH_POOL_H
