@file:JvmName("GastManager")

package com.google.vr.youtube.gast

import android.util.Log

object GastManager {

    init {
        System.loadLibrary("gastlib")
    }

    external fun initialize()

    external fun shutdown()

    @JvmStatic
    external fun getExternalTextureId(nodePath: String): Int

    @JvmStatic
    external fun setupMeshInstance(nodePath: String)

    @JvmStatic
    external fun createMeshInstance(parentNodePath: String) : String

    private fun onGLProcess(nodePath: String, delta: Float) {
        Log.d("FHK", "Received gl process callback from $nodePath")
    }

    private fun onGLInputHover(nodePath: String, xPercent: Float, yPercent: Float) {}

    private fun onGLInputPress(nodePath: String, xPercent: Float, yPercent: Float) {}

    private fun onGLInputRelease(nodePath: String, xPercent: Float, yPercent: Float) {}
}