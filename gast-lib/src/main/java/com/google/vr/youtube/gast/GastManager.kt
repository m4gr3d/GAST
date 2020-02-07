@file:JvmName("GastManager")

package com.google.vr.youtube.gast

import android.util.Log
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import java.util.concurrent.ConcurrentHashMap

class GastManager(godot: Godot) : GodotPlugin(godot) {

    /**
     * Registers for Godot callbacks. The callbacks occur on the GL thread.
     */
    interface GLCallbackListener {
        /**
         * Forward the '_process' callback of the Godot node with the given 'nodePath'
         */
        fun onGLProcess(nodePath: String, delta: Float)
    }

    init {
        System.loadLibrary("gast")
    }

    private val glCallbackListeners = ConcurrentHashMap<String, GLCallbackListener>()

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

    fun addGLCallbackListener(nodePath: String, listener : GLCallbackListener) {
        glCallbackListeners[nodePath] = listener;
    }

    fun removeGLCallbackListener(nodePath: String) {
        glCallbackListeners.remove(nodePath)
    }

    override fun getPluginGDNativeLibrariesPaths() = setOf("godot/plugin/v2/gast/gastlib.gdnlib")

    external fun getExternalTextureId(nodePath: String): Int

    external fun setupMeshInstance(nodePath: String)

    external fun createMeshInstance(parentNodePath: String): String

    /**
     * Invoked by the native layer to forward the node's '_process' callback.
     */
    private fun onGLProcess(nodePath: String, delta: Float) {
        val listener = glCallbackListeners[nodePath] ?: return
        listener.onGLProcess(nodePath, delta)
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