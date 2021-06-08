#ifndef PROJECTION_MESH_UTILS_H
#define PROJECTION_MESH_UTILS_H

#include "gen/Mesh.hpp"

namespace gast {

namespace {
using namespace godot;

enum class StereoMode {
    kMono,
    kTopBottom,
    kLeftRight,
    kModeCount,
};

struct StereoModeDisplayParameters {
    Vector2 texture_scale = Vector2(1.0, 1.0);
    Vector2 left_texture_offset = Vector2(0.0, 0.0);
    Vector2 right_texture_offset = Vector2(0.0, 0.0);
};

struct SamplingTransforms {
    Transform left;
    Transform right;
};

Transform get_transform_from_translation(Vector2 translation) {
    auto transform = Transform();
    transform.translate(translation.x, translation.y, 0);
    return transform;
}

Transform get_transform_from_scale(Vector2 scale) {
    auto transform = Transform();
    transform.scale(Vector3(scale.x, scale.y, 1));
    return transform;
}

StereoModeDisplayParameters get_stereo_mode_display_parameters(StereoMode stereo_mode) {
    StereoModeDisplayParameters parameters;
    switch (stereo_mode) {
        case StereoMode::kMono:
            parameters.texture_scale = Vector2(1.0, 1.0);
            parameters.left_texture_offset = Vector2(0.0, 0.0);
            parameters.right_texture_offset = Vector2(0.0, 0.0);
            break;
        case StereoMode::kTopBottom: {
            parameters.texture_scale = Vector2(1.0, 0.5);
            parameters.left_texture_offset = Vector2(0.0, 0.0);
            parameters.right_texture_offset = Vector2(0.0, 0.5);
            break;
        }
        case StereoMode::kLeftRight: {
            parameters.texture_scale = Vector2(0.5, 1.0);
            parameters.left_texture_offset = Vector2(0.0, 0.0);
            parameters.right_texture_offset = Vector2(0.5, 0.0);
            break;
        }
        case StereoMode::kModeCount: {
            break;
        }
    }
    return parameters;
}

SamplingTransforms get_sampling_transforms(StereoMode stereo_mode) {
    StereoModeDisplayParameters stereo_mode_display_params =
            get_stereo_mode_display_parameters(stereo_mode);

    Transform texture_scale_transform = get_transform_from_scale(
            stereo_mode_display_params.texture_scale);
    Transform left_texture_offset_transform = get_transform_from_translation(
            stereo_mode_display_params.left_texture_offset);
    Transform right_texture_offset_transform = get_transform_from_translation(
            stereo_mode_display_params.right_texture_offset);

    Transform left_sampling_transform = left_texture_offset_transform * texture_scale_transform;
    Transform right_sampling_transform =
            right_texture_offset_transform * texture_scale_transform;

    SamplingTransforms sampling_transforms;
    sampling_transforms.left = left_sampling_transform;
    sampling_transforms.right = right_sampling_transform;
    return sampling_transforms;
}

const char *kShaderCode = R"GAST_SHADER(
shader_type spatial;
render_mode unshaded, depth_draw_opaque, specular_disabled, shadows_disabled, ambient_light_disabled;

uniform samplerExternalOES gast_texture;
uniform mat4 left_eye_sampling_transform;
uniform mat4 right_eye_sampling_transform;
uniform bool enable_billboard;
uniform float gradient_height_ratio;

void vertex() {
	if (enable_billboard) {
		MODELVIEW_MATRIX = INV_CAMERA_MATRIX * mat4(CAMERA_MATRIX[0],CAMERA_MATRIX[1],CAMERA_MATRIX[2],WORLD_MATRIX[3]);
	}
}

void fragment() {
    // Find world camera position (different for each eye)
    vec3 world_camera = (CAMERA_MATRIX * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    // Find projection position
    vec3 world_projection = ((PROJECTION_MATRIX * CAMERA_MATRIX) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    // Find difference between projection and camera on X-Axis
    vec3 diff = world_camera - world_projection;
    float x_diff = diff.x;

    vec2 new_uv = UV;
    if(x_diff <= 0.0f) {
        // Right Eye
        new_uv = (right_eye_sampling_transform * vec4(UV.x, UV.y, 0.0, 1.0)).xy;
    } else {
        // Left Eye
        new_uv = (left_eye_sampling_transform * vec4(UV.x, UV.y, 0.0, 1.0)).xy;
    }

	vec4 texture_color = texture(gast_texture, new_uv);
	float target_alpha = COLOR.a * texture_color.a;
	if (gradient_height_ratio >= 0.05) {
		float gradient_mask = min((1.0 - UV.y) / gradient_height_ratio, 1.0);
		target_alpha = target_alpha * gradient_mask;
	}
	ALPHA = target_alpha;
	ALBEDO = texture_color.rgb * target_alpha;
}
)GAST_SHADER";

const char *kDisableDepthTestRenderMode = "render_mode depth_test_disable;";
const char *kCullFrontRenderMode = "render_mode cull_front;";
const char *kGastLeftEyeSamplingTransformName = "left_eye_sampling_transform";
const char *kGastRightEyeSamplingTransformName = "right_eye_sampling_transform";

const char *kShaderCustomDefines = R"GAST_DEFINES(
#ifdef ANDROID_ENABLED
#extension GL_OES_EGL_image_external : enable
#extension GL_OES_EGL_image_external_essl3 : enable
#else
#define samplerExternalOES sampler2D
#endif
)GAST_DEFINES";
}  // namespace


static inline Array create_curved_screen_surface_array(
        Vector2 mesh_size, float curved_screen_radius, size_t curved_screen_resolution) {
    const float horizontal_angle =
            2.0f * std::atan(mesh_size.x * 0.5f / curved_screen_radius);
    const size_t vertical_resolution = curved_screen_resolution;
    const size_t horizontal_resolution = curved_screen_resolution;
    Array arr = Array();
    arr.resize(Mesh::ARRAY_MAX);
    PoolVector3Array vertices = PoolVector3Array();
    PoolVector2Array uv = PoolVector2Array();
    PoolIntArray indices = PoolIntArray();

    for (size_t row = 0; row < vertical_resolution; row++) {
        for (size_t col = 0; col < horizontal_resolution; col++) {
            const float x_percent = static_cast<float>(col) /
                                    static_cast<float>(horizontal_resolution - 1U);
            const float y_percent = static_cast<float>(row) /
                                    static_cast<float>(vertical_resolution - 1U);
            const float angle = x_percent * horizontal_angle - (horizontal_angle / 2.0f);
            const float x_pos = sin(angle);
            const float z_pos = -cos(angle) + cos(horizontal_angle / 4.0f);
            Vector3 out_pos =
                    Vector3(
                            x_pos * curved_screen_radius,
                            (y_percent - 0.5) * mesh_size.y,
                            z_pos * curved_screen_radius);
            Vector2 out_uv = Vector2(x_percent, 1 - y_percent);
            vertices.append(out_pos);
            uv.append(out_uv);
        }
    }

    for (size_t row = 0; row < vertical_resolution - 1; row++) {
        for (size_t col = 0; col < horizontal_resolution - 1; col++) {
            uint32_t offset = col + row * horizontal_resolution;

            /*
             Our vertex array contains the list of all vertices grouped by rows.
             For example, for row_count = 2 (= col_count = horizontal_resolution):
                [(A {index=0}, B {index=1}), (C {index=2}, D {index=3})]

             To generate the triangles in clockwise winding order, we need the following sequence:

             C ---- D
             |\     |
             |  \   |
             |    \ |
             A ---- B

             vertices => [A,C,B], [B,C,D]
             indices  => [0,2,1], [1,2,3]

             ==> First triangle: offset, offset + horizontal_resolution, offset + 1
             ==> Second triangle: offset + 1, offset + horizontal_resolution, offset + horizontal_resolution + 1
             */
            // Add the first triangle
            indices.append(offset);
            indices.append(offset + horizontal_resolution);
            indices.append(offset + 1);

            // Add the second triangle
            indices.append(offset + 1);
            indices.append(offset + horizontal_resolution);
            indices.append(offset + horizontal_resolution + 1);
        }
    }

    arr[Mesh::ARRAY_VERTEX] = vertices;
    arr[Mesh::ARRAY_TEX_UV] = uv;
    arr[Mesh::ARRAY_INDEX] = indices;

    return arr;
}

static inline Array create_spherical_surface_array(
        float size, size_t band_count, size_t sector_count) {
    float degrees_to_radians = M_PI / 180.0f;
    float longitude_start = -180.f * degrees_to_radians;
    float longitude_end = 180.f * degrees_to_radians;
    float latitude_start = -90.f * degrees_to_radians;
    float latitude_end = 90.f * degrees_to_radians;
    band_count = std::max(static_cast<size_t>(2), band_count);
    sector_count = std::max(static_cast<size_t>(3), sector_count);

    const size_t vertices_per_ring = sector_count + 1;
    const size_t vertices_per_band = band_count + 1;

    Array arr;
    arr.resize(Mesh::ARRAY_MAX);
    PoolVector3Array vertices = PoolVector3Array();
    PoolVector2Array uvs = PoolVector2Array();
    PoolIntArray indices = PoolIntArray();

    const float sector_angle =
            (longitude_end - longitude_start) / static_cast<float>(sector_count);
    const Vector3 scale = 0.5f * Vector3(size, size, size);

    const float delta_angle =
            (latitude_end - latitude_start) / static_cast<float>(band_count);

    for (size_t ring = 0; ring < vertices_per_band; ring++) {
        const float latitude_angle =
                latitude_start + delta_angle * static_cast<float>(ring);
        const float ring_radius = cosf(latitude_angle);
        const float sphere_y = sinf(latitude_angle);

        for (size_t s = 0; s < vertices_per_ring; s++) {
            const float radians =
                    longitude_start + sector_angle * static_cast<float>(s);
            const Vector2 ring_pt = Vector2(cosf(radians), sinf(radians));
            const Vector3 sphere_pt = Vector3(ring_radius * ring_pt[1], sphere_y,
                                              ring_radius * -ring_pt[0]);
            const Vector3 pos = Vector3(scale * sphere_pt);

            const float ts = static_cast<float>(s) / static_cast<float>(sector_count);
            const float tt =
                    1.0f - (static_cast<float>(ring) / static_cast<float>(band_count));
            const Vector2 uv = Vector2(ts, tt);

            vertices.append(pos);
            uvs.append(uv);
        }
    }

    const int ring_offset = static_cast<int>(vertices_per_ring);
    for (int band = 0; band < band_count; band++) {
        const int first_band_vertex = static_cast<int>(band * ring_offset);
        for (int s = 0; s < sector_count; s++) {
            const int v = static_cast<int>(first_band_vertex + s);
            indices.append(v);
            indices.append(v + 1U);
            indices.append(v + ring_offset);

            indices.append(v + 1U);
            indices.append(v + ring_offset + 1U);
            indices.append(v + ring_offset);
        }
    }

    arr[Mesh::ARRAY_VERTEX] = vertices;
    arr[Mesh::ARRAY_TEX_UV] = uvs;
    arr[Mesh::ARRAY_INDEX] = indices;
    return arr;
}

}  // namespace gast

#endif //PROJECTION_MESH_UTILS_H
