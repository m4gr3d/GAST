package org.godotengine.plugin.gast.projectionmesh

sealed class ProjectionMesh(protected val meshPointer : Long, protected val nodePointer : Long) {

    // Mirrors enum DepthDrawMode in libs/godot-cpp/include/gen/SpatialMaterial.hpp
    internal enum class DepthDrawMode(internal val index: Int) {
        /**
         * Only draw depth for opaque geometry (not transparent).
         */
        OPAQUE(0),

        /**
         * Always draw depth (opaque and transparent).
         */
        ALWAYS(1),

        /**
         * Never draw depth.
         */
        NEVER(2),

        /**
         * Do opaque depth pre-pass for transparent geometry.
         */
        ALPHA_PREPASS(3);

        companion object {
            fun fromIndex(index: Int): DepthDrawMode {
                for (mode in DepthDrawMode.values()) {
                    if (mode.index == index) {
                        return mode
                    }
                }

                // Return the default
                return OPAQUE
            }
        }
    }

    internal fun setDepthDrawMode(mode: DepthDrawMode) {
        nativeSetDepthDrawMode(meshPointer, mode.index)
    }

    private external fun nativeSetDepthDrawMode(meshPoint: Long, modeIndex: Int)

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
