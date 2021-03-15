#ifndef UTILS_H
#define UTILS_H

#include <android/log.h>
#include <core/Godot.hpp>
#include <core/String.hpp>
#include <core/Variant.hpp>
#include <gen/Mesh.hpp>
#include <gen/Node.hpp>
#include <gen/Object.hpp>

#define LOG_TAG "GAST"

#define ALOG_ASSERT(_cond, ...) \
    if (!(_cond)) __android_log_assert("conditional", LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

/** Auxiliary macros */
#define __JNI_METHOD_BUILD(package, class_name, method) \
    Java_ ## package ## _ ## class_name ## _ ## method
#define __JNI_METHOD_EVAL(package, class_name, method) \
    __JNI_METHOD_BUILD(package, class_name, method)

/**
 * Expands the JNI signature for a JNI method.
 *
 * Requires to redefine the macros JNI_PACKAGE_NAME and JNI_CLASS_NAME.
 * Not doing so will raise preprocessor errors during build.
 *
 * JNI_PACKAGE_NAME must be the JNI representation Java class package name.
 * JNI_CLASS_NAME must be the JNI representation of the Java class name.
 *
 * For example, for the class com.example.package.SomeClass:
 * JNI_PACKAGE_NAME: com_example_package
 * JNI_CLASS_NAME: SomeClass
 *
 * Note that underscores in Java package and class names are replaced with "_1"
 * in their JNI representations.
 */
#define JNI_METHOD(method) \
    __JNI_METHOD_EVAL(JNI_PACKAGE_NAME, JNI_CLASS_NAME, method)

/**
 * Expands a Java class name using slashes as package separators into its
 * JNI type string representation.
 *
 * For example, to get the JNI type representation of a Java String:
 * JAVA_TYPE("java/lang/String")
 */
#define JAVA_TYPE(class_name) "L" class_name ";"

/**
 * Default definitions for the macros required in JNI_METHOD.
 * Used to raise build errors if JNI_METHOD is used without redefining them.
 */
#define JNI_CLASS_NAME "Error: JNI_CLASS_NAME not redefined"
#define JNI_PACKAGE_NAME "Error: JNI_PACKAGE_NAME not redefined"

namespace gast {

using namespace godot;

static inline const char *get_node_tag(const String &node_path) {
    return node_path.utf8().get_data();
}

static inline const char *get_node_tag(const Node& node) {
    return get_node_tag(node.get_path());
}

/**
* Converts JNI jstring to Godot String.
* @param source Source JNI string. If null an empty string is returned.
* @param env JNI environment instance.
* @return Godot string instance.
*/
static inline String jstring_to_string(JNIEnv *env, jstring source) {
    if (env && source) {
        const char *const source_utf8 = env->GetStringUTFChars(source, NULL);
        if (source_utf8) {
            String result(source_utf8);
            env->ReleaseStringUTFChars(source, source_utf8);
            return result;
        }
    }
    return String();
}

/**
 * Converts Godot String to JNI jstring.
 * @param source Source Godot String.
 * @param env JNI environment instance.
 * @return JNI string instance.
 */
static inline jstring string_to_jstring(JNIEnv *env, const String &source) {
    if (env) {
        return env->NewStringUTF(source.utf8().get_data());
    }
    return nullptr;
}

static inline Node *get_node_from_variant(Variant variant) {
    if (variant.get_type() != Variant::OBJECT) {
        return nullptr;
    }

    Node *node = Object::cast_to<Node>(variant);
    return node;
}

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

    uint32_t vertex_offset = 0;
    for (size_t row = 0; row < vertical_resolution - 1; row++) {
        // Add last index from the previous row another time to produce a degenerate triangle.
        if (row > 0) {
            int last_index = indices[indices.size() - 1];
            indices.append(last_index);
        }

        for (size_t col = 0; col < horizontal_resolution; col++) {
            // Add indices for this vertex and the vertex beneath it.
            indices.append(vertex_offset);
            indices.append(static_cast<uint32_t>(vertex_offset + horizontal_resolution));

            // Move to the vertex in the next column.
            if (col < horizontal_resolution - 1) {
                // Move from left-to-right on even rows, right-to-left on odd rows.
                if (row % 2 == 0) {
                    vertex_offset++;
                } else {
                    vertex_offset--;
                }
            }
        }

        // Move to the vertex in the next row.
        vertex_offset = vertex_offset + static_cast<int>(horizontal_resolution);
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

#endif // UTILS_H
