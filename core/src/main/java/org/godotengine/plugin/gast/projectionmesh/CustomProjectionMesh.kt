package org.godotengine.plugin.gast.projectionmesh

class CustomProjectionMesh(meshPointer: Long, nodePointer : Long) : ProjectionMesh(meshPointer, nodePointer) {

    fun setCustomMesh(vertices: FloatArray,
                      textureCoords: FloatArray,
                      glDrawMode: Int,
                      verticesRight: FloatArray?,
                      textureCoordsRight: FloatArray?,
                      glDrawModeRight: Int?,
                      stereoMode: Int) {
        if (verticesRight == null || textureCoordsRight == null || glDrawModeRight == null) {
            nativeSetCustomMesh(
                meshPointer,
                vertices,
                textureCoords,
                glDrawMode,
                vertices,
                textureCoords,
                glDrawMode,
                stereoMode)
        } else {
            nativeSetCustomMesh(
                meshPointer,
                vertices,
                textureCoords,
                glDrawMode,
                verticesRight,
                textureCoordsRight,
                glDrawModeRight,
                stereoMode)
        }
    }

    private external fun nativeSetCustomMesh(meshPointer: Long,
                                             vertices: FloatArray,
                                             textureCoords: FloatArray,
                                             glDrawMode: Int,
                                             verticesRight: FloatArray,
                                             textureCoordsRight: FloatArray,
                                             glDrawModeRight: Int,
                                             stereoMode: Int)
}
