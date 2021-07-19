@file:JvmName("GastManager")

package org.godotengine.plugin.gast

import android.app.Activity
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.widget.FrameLayout
import androidx.annotation.Keep
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.plugin.gast.input.GastInputListener
import org.godotengine.plugin.gast.input.HoverEventData
import org.godotengine.plugin.gast.input.InputDispatcher
import org.godotengine.plugin.gast.input.InputEventData
import org.godotengine.plugin.gast.input.PressEventData
import org.godotengine.plugin.gast.input.ReleaseEventData
import org.godotengine.plugin.gast.input.ScrollEventData
import org.godotengine.plugin.gast.input.action.GastActionListener
import org.godotengine.plugin.gast.input.action.InputActionDispatcher
import java.util.ArrayDeque
import java.util.Queue
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.ConcurrentLinkedQueue
import java.util.concurrent.atomic.AtomicBoolean
import javax.microedition.khronos.opengles.GL10

/**
 * GAST core plugin.
 *
 * Provides the functionality for rendering, interacting and manipulating content generated by the
 * Android system onto Godot textures.
 */
@Keep
class GastManager(godot: Godot) : GodotPlugin(godot) {

    init {
        System.loadLibrary("gast")
    }

    private val gastRenderListeners = ConcurrentLinkedQueue<GastRenderListener>()
    private val gastInputListeners = ConcurrentLinkedQueue<GastInputListener>()

    private val gastActionListenersPerActions = ConcurrentHashMap<String, ArrayDeque<GastActionListener>>()

    private val mainThreadHandler = Handler(Looper.getMainLooper())
    private val initialized = AtomicBoolean(false)

    /**
     * Root parent for all GAST views.
     */
    val rootView = FrameLayout(activity!!)

    companion object {
        private val TAG = GastManager::class.java.simpleName
    }

    override fun onGodotMainLoopStarted() {
        Log.d(TAG, "Initializing $pluginName manager")
        initialize()
        initialized.set(true)

        updateMonitoredInputActions()
    }

    override fun onMainCreate(activity: Activity) = rootView

    override fun onMainDestroy() {
        Log.d(TAG, "Shutting down $pluginName manager")
        runOnRenderThread {
            initialized.set(false);
            shutdown()
        }
    }

    public override fun runOnRenderThread(action: Runnable) {
        super.runOnRenderThread(action)
    }

    override fun onGLDrawFrame(gl: GL10) {
        for (listener in gastRenderListeners) {
            listener.onRenderDrawFrame()
        }
    }

    override fun getPluginName() = "gast-core"

    override fun getPluginGDNativeLibrariesPaths() = setOf("godot/plugin/v1/gast/gastlib.gdnlib")

    /**
     * Register a [GastRenderListener] instance to be notified of rendering related events.
     */
    fun registerGastRenderListener(listener: GastRenderListener) {
        gastRenderListeners += listener
    }

    /**
     * Unregister a previously registered [GastRenderListener] instance.
     */
    fun unregisterGastRenderListener(listener: GastRenderListener) {
        gastRenderListeners -= listener
    }

    /**
     * Register a [GastActionListener] instance to be notified of input action related events.
     */
    fun registerGastActionListener(listener: GastActionListener) {
        val actionsToMonitor = listener.getInputActionsToMonitor()
        if (actionsToMonitor.isEmpty()) {
            return
        }

        for (action in actionsToMonitor) {
            val actionListeners = gastActionListenersPerActions.getOrPut(action) { ArrayDeque() }
            actionListeners.add(listener)
        }

        updateMonitoredInputActions()
    }

    /**
     * Unregister a previously registered [GastActionListener] instance.
     */
    fun unregisterGastActionListener(listener: GastActionListener) {
        val monitoredActions = listener.getInputActionsToMonitor()
        if (monitoredActions.isEmpty()) {
            return
        }

        for (action in monitoredActions) {
            val actionListeners = gastActionListenersPerActions.get(action) ?: continue
            actionListeners.remove(listener)
            if (actionListeners.isEmpty()) {
                gastActionListenersPerActions.remove(action)
            }
        }

        updateMonitoredInputActions()
    }

    /**
     * Register a [GastInputListener] instance to be notified of input related events.
     */
    fun registerGastInputListener(listener: GastInputListener) {
        gastInputListeners += listener
    }

    /**
     * Unregister a previously registered [GastInputListener] instance.
     */
    fun unregisterGastInputListener(listener: GastInputListener) {
        gastInputListeners -= listener
    }

    /**
     * Update the visibility for the given node.
     */
    fun updateVisibility(nodePath: String, visible: Boolean) {
        if (initialized.get()) {
            nativeUpdateNodeVisibility(nodePath, visible)
        }
    }

    private fun updateMonitoredInputActions() {
        if (initialized.get()) {
            // Update the list of input actions to monitor for the native code
            setInputActionsToMonitor(gastActionListenersPerActions.keys.toTypedArray())
        }
    }

    private inline fun dispatchInputEvent(
        listeners: Queue<GastInputListener>?,
        eventDataProvider : () -> InputEventData
    ) {
        if (listeners.isNullOrEmpty()) {
            return
        }

        val dispatcher = InputDispatcher.acquireInputDispatcher(
            listeners,
            eventDataProvider()
        )
        mainThreadHandler.post(dispatcher)
    }

    private fun dispatchInputActionEvent(
        listeners: Queue<GastActionListener>?,
        action: String,
        pressState: GastActionListener.InputPressState,
        strength: Float
    ) {
        if (listeners.isNullOrEmpty()) {
            return
        }

        val dispatcher =
            InputActionDispatcher.acquireInputActionDispatcher(
                listeners,
                action,
                pressState,
                strength
            )
        mainThreadHandler.post(dispatcher)
    }

    private external fun initialize()

    private external fun nativeUpdateNodeVisibility(nodePath: String, visible: Boolean)

    private external fun shutdown()

    private external fun setInputActionsToMonitor(inputActions: Array<String>)

    private fun onRenderInputAction(action: String, pressStateIndex: Int, strength: Float) {
        val pressState = GastActionListener.InputPressState.fromIndex(pressStateIndex)
        if (pressState == GastActionListener.InputPressState.INVALID) {
            return
        }

        dispatchInputActionEvent(
            gastActionListenersPerActions[action],
            action,
            pressState,
            strength
        )
    }

    private fun onRenderInputHover(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        dispatchInputEvent(gastInputListeners) {
            HoverEventData(nodePath, pointerId, xPercent, yPercent)
        }
    }

    private fun onRenderInputPress(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        dispatchInputEvent(gastInputListeners){
            PressEventData(nodePath, pointerId, xPercent, yPercent)
        }
    }

    private fun onRenderInputRelease(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        dispatchInputEvent(gastInputListeners){
            ReleaseEventData(nodePath, pointerId, xPercent, yPercent)
        }
    }

    private fun onRenderInputScroll(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float,
        horizontalDelta: Float,
        verticalDelta: Float
    ) {
        dispatchInputEvent(gastInputListeners) {
            ScrollEventData(
                nodePath,
                pointerId,
                xPercent,
                yPercent,
                horizontalDelta,
                verticalDelta
            )
        }
    }

}
