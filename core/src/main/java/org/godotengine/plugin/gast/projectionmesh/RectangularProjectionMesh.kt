package org.godotengine.plugin.gast.projectionmesh

class RectangularProjectionMesh(meshPointer: Long, nodePointer : Long)
    : ProjectionMesh(meshPointer, nodePointer) {

    fun setMeshSize(width: Float, height: Float) {
        nativeSetMeshSize(meshPointer, nodePointer, width, height)
    }

    private external fun nativeSetMeshSize(meshPointer: Long, nodePointer: Long, width: Float, height: Float)

    fun setCurved(curved : Boolean) {
        nativeSetCurved(meshPointer, nodePointer, curved)
    }

    private external fun nativeSetCurved(meshPointer: Long, nodePointer: Long, curved: Boolean)

    fun getGradientHeightRatio(): Float {
        return getGradientHeightRatio(meshPointer)
    }

    private external fun getGradientHeightRatio(meshPointer: Long): Float

    fun setGradientHeightRatio(ratio: Float) {
        setGradientHeightRatio(meshPointer, ratio)
    }

    private external fun setGradientHeightRatio(meshPointer: Long, ratio: Float)
}
