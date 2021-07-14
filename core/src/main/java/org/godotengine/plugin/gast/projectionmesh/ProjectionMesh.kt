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

    fun setHasTransparency(hasTransparency: Boolean) {
        setHasTransparency(meshPointer, hasTransparency)
    }

    private external fun setHasTransparency(meshPointer: Long, hasTransparency: Boolean)

    /**
     * @param stereoMode int representation of stereo mode, see StereoMode enum in
     * src/main/cpp/gdn/projection_mesh/projection_mesh_utils.h. This reflects the
     * com.google.android.exoplayer2.C.StereoMode constants.
     */
    fun setStereoMode(stereoMode: Int) {
        setStereoMode(meshPointer, stereoMode)
    }

    private external fun setStereoMode(meshPointer: Long, stereoMode: Int)
}
