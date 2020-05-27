@file:JvmName("GastManager")

package org.godotengine.plugin.gast

import android.app.Activity
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.widget.FrameLayout
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.plugin.gast.input.InputDispatcher
import org.godotengine.plugin.gast.input.GastInputListener
import org.godotengine.plugin.gast.input.InputType
import java.util.concurrent.ConcurrentLinkedQueue
import javax.microedition.khronos.opengles.GL10

/**
 * GAST core plugin.
 *
 * Provides the functionality for rendering, interacting and manipulating content generated by the
 * Android system onto Godot textures.
 */
class GastManager(godot: Godot) : GodotPlugin(godot) {

    init {
        System.loadLibrary("gast")
    }

    private val gastEventListeners = ConcurrentLinkedQueue<GastRenderListener>()
    private val gastInputListeners = ConcurrentLinkedQueue<GastInputListener>()

    private val mainThreadHandler = Handler(Looper.getMainLooper())

    /**
     * Root parent for all GAST views.
     */
    val rootView = FrameLayout(godot)

    companion object {
        private val TAG = GastManager::class.java.simpleName
        private const val INVALID_SURFACE_INDEX = -1
    }

    override fun onGodotMainLoopStarted() {
        Log.d(TAG, "Initializing $pluginName manager")
        initialize()
    }

    override fun onMainCreateView(activity: Activity) = rootView

    override fun onMainDestroy() {
        Log.d(TAG, "Shutting down $pluginName manager")
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

    fun addGastEventListener(listener: GastRenderListener) {
        gastEventListeners += listener
    }

    fun removeGastEventListener(listener: GastRenderListener) {
        gastEventListeners -= listener
    }

    fun addGastInputListener(listener: GastInputListener) {
        gastInputListeners += listener
    }

    fun removeGastInputListener(listener: GastInputListener) {
        gastInputListeners -= listener
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

    external fun setGastNodeCollidable(nodePath: String, collidable: Boolean)

    external fun isGastNodeCollidable(nodePath: String): Boolean

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

    private fun dispatchInputEvent(
        inputType: InputType,
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        val dispatcher = InputDispatcher.acquireInputDispatcher(
            gastInputListeners,
            inputType,
            nodePath,
            pointerId,
            xPercent,
            yPercent
        )
        mainThreadHandler.post(dispatcher)
    }

    private external fun initialize()

    private external fun shutdown()

    private fun onRenderInputHover(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        dispatchInputEvent(InputType.HOVER, nodePath, pointerId, xPercent, yPercent)
    }

    private fun onRenderInputPress(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        dispatchInputEvent(InputType.PRESS, nodePath, pointerId, xPercent, yPercent)
    }

    private fun onRenderInputRelease(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        dispatchInputEvent(InputType.RELEASE, nodePath, pointerId, xPercent, yPercent)
    }

}
