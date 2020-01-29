@file:JvmName("GastManager")

package com.google.vr.youtube.gast

object GastManager {

    init {
        System.loadLibrary("gastlib")
    }

    val DEFAULT_GROUP_NAME = "gambit_view_container"

    @JvmStatic
    external fun getExternalTextureId(nodeGroupName: String, externalTextureParamName: String): Int

    external fun registerCallback()

    external fun unregisterCallback()

    private fun onGLProcess(delta: Float) {}

    private fun onGLInputHover(xPercent: Float, yPercent: Float) {}

    private fun onGLInputPress(xPercent: Float, yPercent: Float) {}

    private fun onGLInputRelease(xPercent: Float, yPercent: Float) {}
}