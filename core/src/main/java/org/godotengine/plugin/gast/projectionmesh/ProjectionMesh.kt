package org.godotengine.plugin.gast.projectionmesh

sealed class ProjectionMesh(protected val meshPointer : Long, protected val nodePointer : Long) {

    fun isGazeTracking(): Boolean {
        return isGazeTracking(meshPointer)
    }

    private external fun isGazeTracking(meshPointer: Long): Boolean

    fun setGazeTracking(gazeTracking: Boolean) {
        setGazeTracking(meshPointer, gazeTracking)
    }

    private external fun setGazeTracking(meshPointer: Long, gazeTracking: Boolean)

    fun isRenderOnTop(): Boolean {
        return isRenderOnTop(meshPointer)
    }

    private external fun isRenderOnTop(meshPointer: Long): Boolean

    fun setRenderOnTop(enable: Boolean) {
        setRenderOnTop(meshPointer, enable)
    }

    private external fun setRenderOnTop(meshPointer: Long, enable: Boolean)

    fun updateAlpha(alpha: Float) {
        updateAlpha(meshPointer, alpha)
    }

    private external fun updateAlpha(meshPointer: Long, alpha: Float)
}
