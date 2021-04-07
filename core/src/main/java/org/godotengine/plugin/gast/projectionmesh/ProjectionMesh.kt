package org.godotengine.plugin.gast.projectionmesh

open class ProjectionMesh(meshPointer : Long) {

    private var meshPointer : Long

    init {
        this.meshPointer = meshPointer
    }

    fun getMeshPointer() : Long {
        return meshPointer
    }

    fun isGazeTracking(): Boolean {
        return isGazeTracking(getMeshPointer())
    }

    private external fun isGazeTracking(meshPointer: Long): Boolean

    fun setGazeTracking(gazeTracking: Boolean) {
        setGazeTracking(getMeshPointer(), gazeTracking)
    }

    private external fun setGazeTracking(meshPointer: Long, gazeTracking: Boolean)

    fun isRenderOnTop(): Boolean {
        return isRenderOnTop(getMeshPointer())
    }

    private external fun isRenderOnTop(meshPointer: Long): Boolean

    fun setRenderOnTop(enable: Boolean) {
        setRenderOnTop(getMeshPointer(), enable)
    }

    private external fun setRenderOnTop(meshPointer: Long, enable: Boolean)

    fun getGradientHeightRatio(): Float {
        return getGastNodeGradientHeightRatio(getMeshPointer())
    }

    private external fun getGastNodeGradientHeightRatio(meshPointer: Long): Float

    fun setGradientHeightRatio(ratio: Float) {
        setGastNodeGradientHeightRatio(getMeshPointer(), ratio)
    }

    private external fun setGastNodeGradientHeightRatio(meshPointer: Long, ratio: Float)

    fun updateAlpha(alpha: Float) {
        updateAlpha(getMeshPointer(), alpha)
    }

    private external fun updateAlpha(meshPointer: Long, alpha: Float)

    fun isRectangular() : Boolean {
        return nativeIsRectangular(meshPointer);
    }

    private external fun nativeIsRectangular(meshPointer: Long) : Boolean

    fun isEquirectangular() : Boolean {
        return nativeIsEquirectangular(meshPointer);
    }

    private external fun nativeIsEquirectangular(meshPointer: Long) : Boolean
}
