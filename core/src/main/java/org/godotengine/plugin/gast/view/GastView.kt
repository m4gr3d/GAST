package org.godotengine.plugin.gast.view

import android.view.View
import androidx.annotation.CallSuper
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode

/**
 * Represent an Android view rendered by a [GastNode].
 */
internal interface GastView {

    companion object {
        private fun fetchGastViewParent(view: View): GastView? {
            var viewParent = view.parent
            while (viewParent != null && viewParent !is GastView) {
                viewParent = viewParent.parent
            }
            return viewParent as GastView?
        }
    }

    val viewState: GastViewState

    @CallSuper
    fun initialize(gastManager: GastManager, gastNode: GastNode) {
        viewState.gastManager = gastManager
        viewState.gastNode = gastNode
        viewState.parent = null

        // Generate the surface backing this view
        gastNode.bindSurface()

        // Propagate to the children
        for (child in viewState.children) {
            child.initializeFromParent(this)
        }

        viewState.inputManager.onInitialize()
        viewState.renderManager.onInitialize()

        viewState.initialized = true
    }

    private fun initializeFromParent(parent: GastView) {
        parent.viewState.children.add(this)

        val gastManager = parent.viewState.gastManager?: return
        val parentGastNode = parent.viewState.gastNode ?: return

        val gastNode = GastNode(gastManager, parentGastNode.nodePath)
        initialize(gastManager, gastNode)
        viewState.parent = parent
    }

    @CallSuper
    fun shutdown() {
        // Propagate to the children
        for (child in viewState.children) {
            child.shutdown()
        }

        viewState.renderManager.onShutdown()
        viewState.inputManager.onShutdown()

        viewState.parent?.viewState?.children?.remove(this)

        viewState.gastNode?.release()
        viewState.gastNode = null

        viewState.gastManager = null
        viewState.parent = null
        viewState.initialized = false
    }

    @CallSuper
    fun onGastViewAttachedToWindow() {
        if (viewState.initialized) {
            return
        }

        // Get the GastView parent for this view
        val parent = fetchGastViewParent(viewState.view) ?: return
        initializeFromParent(parent)
    }

    @CallSuper
    fun onGastViewDetachedFromWindow() {
        if (!viewState.initialized) {
            return
        }

        shutdown()
    }

    @CallSuper
    fun onGastViewSizeChanged(width: Float, height: Float) {
        viewState.renderManager.onViewSizeChanged(width, height)
    }

    @CallSuper
    fun onGastViewVisibilityChanged(visible: Boolean) {
        viewState.renderManager.onVisibilityChanged(visible)
    }
}
