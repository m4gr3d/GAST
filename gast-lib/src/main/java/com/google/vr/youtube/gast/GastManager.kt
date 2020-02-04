@file:JvmName("GastManager")

package com.google.vr.youtube.gast

import android.util.Log
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin

class GastManager(godot: Godot) : GodotPlugin(godot) {

    init {
        System.loadLibrary("gast")
    }

    companion object {
        val TAG = GastManager::class.java.simpleName
    }

    override fun onGLGodotMainLoopStarted() {
        Log.d(TAG, "Initializing Gast manager")
        initialize()
    }

    override fun onMainDestroy() {
        Log.d(TAG, "Shutting down Gast manager")
        runOnGLThread { shutdown() }
    }

    override fun getPluginGDNativeLibrariesPaths() = setOf("godot/plugin/v2/gast/gastlib.gdnlib")

    external fun getExternalTextureId(nodePath: String): Int

    external fun setupMeshInstance(nodePath: String)

    external fun createMeshInstance(parentNodePath: String): String

    private fun onGLProcess(nodePath: String, delta: Float) {
        Log.d("FHK", "Received gl process callback from $nodePath")
    }

    private external fun initialize()

    private external fun shutdown()

    private fun onGLInputHover(nodePath: String, xPercent: Float, yPercent: Float) {}

    private fun onGLInputPress(nodePath: String, xPercent: Float, yPercent: Float) {}

    private fun onGLInputRelease(nodePath: String, xPercent: Float, yPercent: Float) {}

    override fun getPluginMethods(): MutableList<String> =
        emptyList<String>().toMutableList()

    override fun getPluginName() = "Gast"
}