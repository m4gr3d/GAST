package org.godotengine.plugin.gast.projectionmesh

class RectangularProjectionMesh(meshPointer: Long, nodePointer : Long)
    : ProjectionMesh(meshPointer, nodePointer) {

    fun setMeshSize(width: Float, height: Float) {
        nativeSetMeshSize(getMeshPointer(), width, height)
        updateGastNodeFromProjectionMesh()
    }

    private external fun nativeSetMeshSize(meshPointer: Long, width: Float, height: Float)

    fun setCurved(curved : Boolean) {
        nativeSetCurved(getMeshPointer(), curved)
        updateGastNodeFromProjectionMesh()
    }

    private external fun nativeSetCurved(meshPointer: Long, curved: Boolean)

    fun getGradientHeightRatio(): Float {
        return getGradientHeightRatio(getMeshPointer())
    }

    private external fun getGradientHeightRatio(meshPointer: Long): Float

    fun setGradientHeightRatio(ratio: Float) {
        setGradientHeightRatio(getMeshPointer(), ratio)
    }

    private external fun setGradientHeightRatio(meshPointer: Long, ratio: Float)
}
