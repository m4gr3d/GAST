@file:JvmName("GastManager")

package com.google.vr.youtube.gast

object GastManager {

    init {
        System.loadLibrary("gastlib")
    }

    @JvmStatic
    external fun getExternalTextureId(nodePath: String): Int

    @JvmStatic
    external fun setupMeshInstance(nodePath: String)

    @JvmStatic
    external fun createMeshInstance(parentNodePath: String) : String

    external fun registerCallback()

    external fun unregisterCallback()

    private fun onGLProcess(nodePath: String, delta: Float) {}

    private fun onGLInputHover(nodePath: String, xPercent: Float, yPercent: Float) {}

    private fun onGLInputPress(nodePath: String, xPercent: Float, yPercent: Float) {}

    private fun onGLInputRelease(nodePath: String, xPercent: Float, yPercent: Float) {}
}