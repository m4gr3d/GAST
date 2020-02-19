@file:JvmName("GastManager")

package com.google.vr.youtube.gast

import android.app.Activity
import android.util.Log
import android.widget.FrameLayout
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.ConcurrentLinkedQueue
import javax.microedition.khronos.opengles.GL10

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

    interface GLDrawFrameListener {
        /**
         * Forward the GL draw frame callback.
         */
        fun onGLDrawFrame()
    }

    init {
        System.loadLibrary("gast")
    }

    private val glCallbackListeners = ConcurrentHashMap<String, GLCallbackListener>()
    private val glDrawFrameListeners = ConcurrentLinkedQueue<GLDrawFrameListener>()

    /**
     * Root parent for all GAST views.
     */
    val rootView = FrameLayout(godot)

    companion object {
        val TAG = GastManager::class.java.simpleName
    }

    override fun onGLGodotMainLoopStarted() {
        Log.d(TAG, "Initializing Gast manager")
        initialize()
    }

    override fun onMainCreateView(activity: Activity) = rootView

    override fun onMainDestroy() {
        Log.d(TAG, "Shutting down Gast manager")
        runOnGLThread { shutdown() }
    }

    override fun onGLDrawFrame(gl: GL10) {
        for (listener in glDrawFrameListeners) {
            listener.onGLDrawFrame()
        }
    }

    override fun getPluginMethods(): MutableList<String> =
        emptyList<String>().toMutableList()

    override fun getPluginName() = "Gast"

    override fun getPluginGDNativeLibrariesPaths() = setOf("godot/plugin/v2/gast/gastlib.gdnlib")

    fun addGLDrawFrameListener(listener: GLDrawFrameListener) {
        glDrawFrameListeners.add(listener)
    }

    fun removeGLDrawFrameListener(listener: GLDrawFrameListener) {
        glDrawFrameListeners.remove(listener)
    }

    fun addGLCallbackListener(nodePath: String, listener: GLCallbackListener) {
        glCallbackListeners[nodePath] = listener;
    }

    fun removeGLCallbackListener(nodePath: String) {
        glCallbackListeners.remove(nodePath)
    }

    external fun getExternalTextureId(nodePath: String): Int

    external fun unbindMeshInstance(nodePath: String)

    external fun bindMeshInstance(parentNodePath: String): String

    external fun updateMeshInstanceParent(nodePath: String, newParentNodePath: String): String

    external fun updateMeshInstanceVisibility(
        nodePath: String,
        shouldDuplicateParentVisibility: Boolean,
        visible: Boolean
    )

    external fun updateMeshInstanceSize(nodePath: String, width: Float, height: Float)

    external fun updateMeshInstanceTranslation(
        nodePath: String,
        xTranslation: Float,
        yTranslation: Float,
        zTranslation: Float
    )

    external fun updateMeshInstanceScale(nodePath: String, xScale: Float, yScale: Float)

    external fun updateMeshInstanceRotation(
        nodePath: String,
        xRotation: Float,
        yRotation: Float,
        zRotation: Float
    )

    private external fun initialize()

    private external fun shutdown()

    /**
     * Invoked by the native layer to forward the node's '_process' callback.
     */
    private fun onGLProcess(nodePath: String, delta: Float) {
        val listener = glCallbackListeners[nodePath] ?: return
        listener.onGLProcess(nodePath, delta)
    }

    private fun onGLInputHover(nodePath: String, xPercent: Float, yPercent: Float) {}

    private fun onGLInputPress(nodePath: String, xPercent: Float, yPercent: Float) {}

    private fun onGLInputRelease(nodePath: String, xPercent: Float, yPercent: Float) {}

}