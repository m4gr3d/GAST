@file:JvmName("GastManager")

package org.godotengine.plugin.gast

import android.app.Activity
import android.util.Log
import android.widget.FrameLayout
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import java.util.concurrent.ConcurrentLinkedQueue
import javax.microedition.khronos.opengles.GL10

/**
 * GAST core plugin.
 *
 * Provides the functionality for rendering, interacting and manipulating content generated by the
 * Android system onto Godot textures.
 */
class GastManager(godot: Godot) : GodotPlugin(godot) {

    /**
     * Used to listen to events dispatched by the Gast plugin.
     */
    interface GastEventListener {
        /**
         * Forward the render draw frame callback.
         */
        fun onRenderDrawFrame()
    }

    init {
        System.loadLibrary("gast")
    }

    private val gastEventListeners = ConcurrentLinkedQueue<GastEventListener>()

    /**
     * Root parent for all GAST views.
     */
    val rootView = FrameLayout(godot)

    companion object {
        private val TAG = GastManager::class.java.simpleName
        private const val INVALID_SURFACE_INDEX = -1
    }

    override fun onGodotMainLoopStarted() {
        Log.d(TAG, "Initializing ${pluginName} manager")
        initialize()
    }

    override fun onMainCreateView(activity: Activity) = rootView

    override fun onMainDestroy() {
        Log.d(TAG, "Shutting down ${pluginName} manager")
        runOnRenderThread { shutdown() }
    }

    override fun onGLDrawFrame(gl: GL10) {
        for (listener in gastEventListeners) {
            listener.onRenderDrawFrame()
        }
    }

    override fun getPluginMethods(): MutableList<String> = emptyList<String>().toMutableList()

    override fun getPluginName() = "gast-core"

    override fun getPluginGDNativeLibrariesPaths() = setOf("godot/plugin/v1/gast/gastlib.gdnlib")

    fun addGastEventListener(listener: GastEventListener) {
        gastEventListeners += listener;
    }

    fun removeGastEventListener(listener: GastEventListener) {
        gastEventListeners -= listener
    }

    @JvmOverloads
    external fun getExternalTextureId(
        nodePath: String,
        surfaceIndex: Int = INVALID_SURFACE_INDEX
    ): Int

    /**
     * Create a Gast node with the given parent node and set it up.
     * @param parentNodePath - Path to the parent for the Gast node that will be created. The parent node must exist
     * @return The node path to the newly created Gast node
     */
    external fun acquireAndBindGastNode(parentNodePath: String): String

    /**
     * Unbind and release the Gast node with the given node path. This is the counterpart to [GastManager.acquireAndBindGastNode]
     */
    external fun unbindAndReleaseGastNode(nodePath: String)

    external fun updateGastNodeParent(nodePath: String, newParentNodePath: String): String

    external fun updateGastNodeVisibility(
        nodePath: String,
        shouldDuplicateParentVisibility: Boolean,
        visible: Boolean
    )

    external fun updateGastNodeSize(nodePath: String, width: Float, height: Float)

    external fun updateGastNodeLocalTranslation(
        nodePath: String,
        xTranslation: Float,
        yTranslation: Float,
        zTranslation: Float
    )

    external fun updateGastNodeLocalScale(nodePath: String, xScale: Float, yScale: Float)

    external fun updateGastNodeLocalRotation(
        nodePath: String,
        xRotation: Float,
        yRotation: Float,
        zRotation: Float
    )

    private external fun initialize()

    private external fun shutdown()

    private fun onRenderInputHover(nodePath: String, pointerId: String, xPercent: Float, yPercent: Float) {
        Log.v(TAG, "Received hover event for $nodePath from $pointerId at {$xPercent, $yPercent}")
    }

    private fun onRenderInputPress(nodePath: String, pointerId: String, xPercent: Float, yPercent: Float) {
        Log.v(TAG, "Received press event for $nodePath from $pointerId at {$xPercent, $yPercent}")
    }

    private fun onRenderInputRelease(nodePath: String, pointerId: String, xPercent: Float, yPercent: Float) {
        Log.v(TAG, "Received release event for $nodePath from $pointerId at {$xPercent, $yPercent}")
    }

}
