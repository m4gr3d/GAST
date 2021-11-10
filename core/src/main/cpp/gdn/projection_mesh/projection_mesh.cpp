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
    reset_meshes();
    reset_external_texture();
    projection_mesh_data_list.clear();
}

ProjectionMesh::ProjectionMesh(ProjectionMesh::ProjectionMeshType projection_mesh_type) :
        projection_mesh_type(projection_mesh_type),
        gaze_tracking(kDefaultGazeTracking),
        render_on_top(kDefaultRenderOnTop),
        alpha(kDefaultAlpha),
        has_transparency(kDefaultHasTransparency),
        stereo_mode(StereoMode::kMono),
        collidable(kDefaultCollidable),
        uv_origin_is_bottom_left(kDefaultUvOriginIsBottomLeft) {}

ProjectionMesh::ProjectionMesh() : ProjectionMesh(ProjectionMeshType::RECTANGULAR) {}

void ProjectionMesh::_init() {
    // Initialize the collision shape(s).
    for (int i = 0; i < get_mesh_count(); i++) {
        auto *mesh_data = new ProjectionMeshData();

        mesh_data->collision_shape = CollisionShape::_new();
        mesh_data->mesh_instance = MeshInstance::_new();
        mesh_data->collision_shape->add_child(mesh_data->mesh_instance);

        mesh_data->shape = Ref<Resource>();
        mesh_data->shader_material = Ref<ShaderMaterial>(ShaderMaterial::_new());

        projection_mesh_data_list.push_back(mesh_data);
    }
}

void ProjectionMesh::_register_methods() {
    register_method("set_gaze_tracking", &ProjectionMesh::set_gaze_tracking);
    register_method("is_gaze_tracking", &ProjectionMesh::is_gaze_tracking);
    register_method("set_render_on_top", &ProjectionMesh::set_render_on_top);
    register_method("is_render_on_top", &ProjectionMesh::is_render_on_top);
    register_method("set_collidable", &ProjectionMesh::set_collidable);
    register_method("is_collidable", &ProjectionMesh::is_collidable);

    register_property<ProjectionMesh, bool>("collidable", &ProjectionMesh::set_collidable,
                                            &ProjectionMesh::is_collidable, kDefaultCollidable);
    register_property<ProjectionMesh, bool>(
            "gaze_tracking",
            &ProjectionMesh::set_gaze_tracking,
            &ProjectionMesh::is_gaze_tracking, kDefaultGazeTracking);
    register_property<ProjectionMesh, bool>(
            "render_on_top",
            &ProjectionMesh::set_render_on_top,
            &ProjectionMesh::is_render_on_top, kDefaultRenderOnTop);
}

void ProjectionMesh::update_collision_shapes() const {
    int count = get_mesh_count();
    if (count <= 0) {
        return;
    }

    for (int i = 0; i < count; i++) {
        ProjectionMeshData *mesh_data = projection_mesh_data_list[i];
        CollisionShape * collision_shape = mesh_data->collision_shape;
        if (collidable && collision_shape->is_visible_in_tree() &&
            collision_shape->get_child_count() > 0) {
            collision_shape->set_shape(mesh_data->shape);
        } else {
            collision_shape->set_shape(Ref<Resource>());
        }
    }
}

bool ProjectionMesh::should_use_alpha_shader_code()  {
    return has_transparency || alpha < kAlphaThreshold || is_render_on_top();
}

inline String ProjectionMesh::generate_shader_code() {
    String shader_code = get_base_shader_code(should_use_alpha_shader_code());
    if (is_render_on_top()) {
        shader_code += kDisableDepthTestRenderMode;
    }
    return shader_code;
}

void ProjectionMesh::reset_meshes() const {
    for (int i = 0; i < get_mesh_count(); i++) {
        ProjectionMeshData *mesh_data = projection_mesh_data_list[i];
        mesh_data->mesh_instance->set_mesh(Ref<Resource>());
        mesh_data->collision_shape->set_shape(Ref<Resource>());
        mesh_data->shape = Ref<Resource>();
    }
}

CollisionShape *ProjectionMesh::get_collision_shape(int index) const {
    if (0 > index || index >= get_mesh_count()) {
        ALOGE("Invalid index: %d.", index);
        return nullptr;
    }

    ProjectionMeshData *mesh_data = projection_mesh_data_list[index];
    return mesh_data->collision_shape;
}

MeshInstance *ProjectionMesh::get_mesh_instance(int index) const {
    if (0 > index || index >= get_mesh_count()) {
        ALOGE("Invalid index: %d.", index);
        return nullptr;
    }

    ProjectionMeshData *mesh_data = projection_mesh_data_list[index];
    return mesh_data->mesh_instance;
}

void ProjectionMesh::set_mesh(int index, const Ref<Mesh>& mesh) const {
    if (0 > index || index >= get_mesh_count()) {
        ALOGE("Cannot set Mesh, invalid MeshInstance index: %d.", index);
        return;
    }

    ProjectionMeshData *mesh_data = projection_mesh_data_list[index];
    mesh_data->mesh_instance->set_mesh(mesh);
}

void ProjectionMesh::set_shader(int index, const Ref<Shader> &shader) const {
    if (0 > index || index >= get_mesh_count()) {
        ALOGE("Cannot set Shader, invalid index: %d.", index);
        return;
    }

    ProjectionMeshData *mesh_data = projection_mesh_data_list[index];
    if (!mesh_data->shader_material.is_valid()) {
        ALOGE("Cannot set Shader, invalid ShaderMaterial.");
        return;
    }
    mesh_data->shader_material->set_shader(shader);
    mesh_data->mesh_instance->set_surface_material(kDefaultSurfaceIndex, mesh_data->shader_material);
}

void ProjectionMesh::set_collision_shape(int index, const Ref<Shape>& collision_shape) const {
    if (0 > index || index >= get_mesh_count()) {
        ALOGE("Invalid index: %d.", index);
        return;
    }

    ProjectionMeshData *mesh_data = projection_mesh_data_list[index];
    mesh_data->shape = collision_shape;
    update_collision_shapes();
}

void ProjectionMesh::update_shader_code() {
    int mesh_count = get_mesh_count();
    if (mesh_count <= 0) {
        return;
    }

    String shader_code = this->generate_shader_code();
    for (int i = 0; i < mesh_count; i++) {
        ProjectionMeshData *mesh_data = projection_mesh_data_list[i];

        Ref<ShaderMaterial> shader_material = mesh_data->shader_material;
        if (shader_material.is_valid() && shader_material->get_shader().is_valid()) {
            if (shader_material->get_shader()->get_code() != shader_code) {
                shader_material->get_shader()->set_code(shader_code);
            }
        }
    }
}

void ProjectionMesh::update_sampling_transforms() {
    int mesh_count = get_mesh_count();
    if (mesh_count <= 0) {
        return;
    }

    SamplingTransforms sampling_transforms = get_sampling_transforms(stereo_mode,
                                                                     uv_origin_is_bottom_left);
    for (int i = 0; i < mesh_count; i++) {
        ProjectionMeshData *mesh_data = projection_mesh_data_list[i];

        Ref<ShaderMaterial> shader_material = mesh_data->shader_material;
        if (shader_material.is_valid()) {
            shader_material->set_shader_param(
                    kGastLeftEyeSamplingTransformName, sampling_transforms.left);
            shader_material->set_shader_param(
                    kGastRightEyeSamplingTransformName, sampling_transforms.right);
        }
    }
}

void ProjectionMesh::update_properties(ProjectionMesh *projection_mesh) {
    if (projection_mesh == nullptr) {
        return;
    }

    set_render_on_top(projection_mesh->is_render_on_top());
    set_gaze_tracking(projection_mesh->is_gaze_tracking());
    set_alpha(projection_mesh->alpha);
    set_has_transparency(projection_mesh->has_transparency);
    set_collidable(projection_mesh->is_collidable());
}

void ProjectionMesh::update_render_priority() const {
    int mesh_count = get_mesh_count();
    if (mesh_count <= 0) {
        return;
    }

    int render_priority = 0;
    if (render_on_top) {
        render_priority++;
        if (gaze_tracking) {
            render_priority++;
        }
    }

    for (int i = 0; i < mesh_count; i++) {
        ProjectionMeshData *mesh_data = projection_mesh_data_list[i];

        Ref<ShaderMaterial> shader_material = mesh_data->shader_material;
        if (shader_material.is_valid()) {
            shader_material->set_render_priority(render_priority);
        }
    }
}

void ProjectionMesh::update_shaders_param(const String& param, const Variant& value) const {
    for (int i = 0; i < get_mesh_count(); i++) {
        update_shader_param(i, param, value);
    }
}

void ProjectionMesh::update_shader_param(int index, const String &param,
                                         const Variant &value) const {
    if (index < 0 || index >= get_mesh_count()) {
        return;
    }

    ProjectionMeshData *mesh_data = projection_mesh_data_list[index];
    if (mesh_data->shader_material.is_valid()) {
        mesh_data->shader_material->set_shader_param(param, value);
    }
}


}  // namespace gast

