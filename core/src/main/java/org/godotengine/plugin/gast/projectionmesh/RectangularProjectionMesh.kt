package org.godotengine.plugin.gast.projectionmesh

class RectangularProjectionMesh(meshPointer: Long) : ProjectionMesh(meshPointer) {

    fun setMeshSize(width: Float, height: Float) {
        nativeSetMeshSize(getMeshPointer(), width, height)
    }

    private external fun nativeSetMeshSize(meshPointer: Long, width: Float, height: Float)

    fun setCurved(curved : Boolean) {
        nativeSetCurved(getMeshPointer(), curved)
    }

    private external fun nativeSetCurved(meshPointer: Long, curved: Boolean)
}
