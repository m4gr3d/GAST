package org.godotengine.plugin.gast.xr.view

import android.util.Log
import android.view.View
import androidx.annotation.CallSuper
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode

/**
 * Represent an Android view rendered by a [GastNode].
 */
internal interface XrView {

    companion object {
        private val TAG = XrView::class.java.simpleName

        private fun fetchXrViewParent(view: View): XrView? {
            var viewParent = view.parent
            while (viewParent != null && viewParent !is XrView) {
                viewParent = viewParent.parent
            }
            return viewParent as XrView?
        }
    }

    val viewState: XrViewState

    @CallSuper
    fun initialize(gastManager: GastManager, gastNode: GastNode) {
        Log.d(TAG, "Initializing XrView $this...")
        viewState.initialize(gastManager, gastNode)

        // Generate the surface backing this view
        gastNode.bindSurface()

        // Propagate to the children
        for (child in viewState.children) {
            child.initializeFromParent(this)
        }

        viewState.inputManager.onInitialize()
        viewState.renderManager.onInitialize()
    }

    private fun initializeFromParent(parent: XrView) {
        parent.viewState.children.add(this)

        val gastManager = parent.viewState.gastManager?: return
        val parentGastNode = parent.viewState.gastNode ?: return

        val gastNode = GastNode(gastManager, parentGastNode.nodePath)
        initialize(gastManager, gastNode)
        viewState.parent = parent
    }

    @CallSuper
    fun shutdown() {
        Log.d(TAG, "Shutting down XrView $this...")

        // Propagate to the children
        for (child in viewState.children) {
            child.shutdown()
        }

        viewState.renderManager.onShutdown()
        viewState.inputManager.onShutdown()

        viewState.parent?.viewState?.children?.remove(this)

        viewState.shutdown()
    }

    @CallSuper
    fun onXrViewAttachedToWindow() {
        if (viewState.initialized) {
            return
        }

        // Get the XrView parent for this view
        val parent = fetchXrViewParent(viewState.view) ?: return
        initializeFromParent(parent)
    }

    @CallSuper
    fun onXrViewDetachedFromWindow() {
        if (!viewState.initialized) {
            return
        }

        shutdown()
    }

    @CallSuper
    fun onXrViewSizeChanged(width: Float, height: Float) {
        Log.d(TAG, "On size changed: $width, $height")
        viewState.renderManager.onViewSizeChanged(width, height)
    }

    @CallSuper
    fun onXrViewVisibilityChanged(visible: Boolean) {
        viewState.renderManager.onVisibilityChanged(visible)
    }

    fun setGazeTracking(enable: Boolean) {
        viewState.gazeTracking = enable
    }
}
