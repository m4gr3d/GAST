package org.godotengine.plugin.gast.projectionmesh

class CustomProjectionMesh(meshPointer: Long, nodePointer : Long) : ProjectionMesh(meshPointer, nodePointer) {

    /**
     * Set provided mesh as the current projection mesh.
     *
     * If either verticesRight, textureCoordsRight, or glDrawModeRight are null, the left eye mesh
     * will be used for the right eye as well.
     *
     * @param vertices Left eye vertex positions in 3D space [x1, y1, z1 ... xn, yn, zn]
     * @param textureCoords Left eye texture sampling coords [u1, v1, u2, v2 ... un, vn]
     * @param glDrawMode Left eye GL mesh primitive
     * @param verticesRight Right eye vertex positions in 3D space [x1, y1, z1 ... xn, yn, zn]
     * @param textureCoordsRight Right eye texture sampling coords [u1, v1, u2, v2 ... un, vn]
     * @param glDrawModeRight Right eye GL mesh primitive
     * @param stereoMode int representation of stereo mode, matches the
     * com.google.android.exoplayer2.C.StereoMode constants
     * @param uvOriginIsBottomLeft Whether the origin in mesh space is bottom left or top left
     */
    fun setCustomMesh(vertices: FloatArray,
                      textureCoords: FloatArray,
                      glDrawMode: Int,
                      verticesRight: FloatArray?,
                      textureCoordsRight: FloatArray?,
                      glDrawModeRight: Int?,
                      stereoMode: Int,
                      uvOriginIsBottomLeft: Boolean) {
        if (verticesRight == null || textureCoordsRight == null || glDrawModeRight == null) {
            nativeSetCustomMesh(
                meshPointer,
                vertices,
                textureCoords,
                glDrawMode,
                vertices,
                textureCoords,
                glDrawMode,
                stereoMode,
                uvOriginIsBottomLeft)
        } else {
            nativeSetCustomMesh(
                meshPointer,
                vertices,
                textureCoords,
                glDrawMode,
                verticesRight,
                textureCoordsRight,
                glDrawModeRight,
                stereoMode,
                uvOriginIsBottomLeft)
        }
    }

    private external fun nativeSetCustomMesh(meshPointer: Long,
                                             vertices: FloatArray,
                                             textureCoords: FloatArray,
                                             glDrawMode: Int,
                                             verticesRight: FloatArray,
                                             textureCoordsRight: FloatArray,
                                             glDrawModeRight: Int,
                                             stereoMode: Int,
                                             uvOriginIsBottomLeft: Boolean)
}
