package org.godotengine.plugin.gast.view

import android.content.Context
import android.view.ViewGroup
import android.view.ViewTreeObserver
import androidx.annotation.CallSuper
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.GastRenderListener
import org.godotengine.plugin.gast.projectionmesh.RectangularProjectionMesh

/**
 * Represent a Android view rendered by a [GastNode].
 */
internal interface GastView : GastRenderListener {

    companion object {
        private const val DP_RATIO = 4f / 1000f

        fun fromPixelsToGodotDimensions(context: Context, dimensionInPixels: Float): Float {
            val metrics = context.resources.displayMetrics

            // First we convert the dimension back to dp.
            // dp = 160 * pixels / dpi
            // dpi = 160 * density
            // dp = (dpi / density) * (pixels / dpi)
            // dp = pixels / density
            val dimensionInDp = dimensionInPixels / metrics.density

            // Then we convert the dp dimension to Godot's dimensions.
            return dimensionInDp * DP_RATIO
        }

        fun fromGodotDimensionsToPixels(context: Context, godotDimensions: Float): Float {
            val dimensionInDp = godotDimensions / DP_RATIO
            return dimensionInDp * context.resources.displayMetrics.density
        }
    }

    private val inputHandler: GastViewInputHandler
        get() = GastViewInputHandler(viewState)

    private val onPreDrawListener: ViewTreeObserver.OnPreDrawListener
        get() = ViewTreeObserver.OnPreDrawListener {
            if (viewState.view.isDirty) {
                viewState.view.invalidate()
            }
            return@OnPreDrawListener true
        }

    val viewState: GastViewState

    @CallSuper
    fun initialize(gastManager: GastManager, gastNode: GastNode, isRoot: Boolean = true) {
        viewState.gastManager = gastManager
        viewState.gastNode = gastNode
        viewState.isRoot = isRoot
        gastNode.bindSurface()

        gastManager.registerGastInputListener(inputHandler)
        gastManager.registerGastRenderListener(this)
        viewState.view.viewTreeObserver.addOnPreDrawListener(onPreDrawListener)

        // Propagate to the children
        if (viewState.view is ViewGroup) {
            val viewGroup = viewState.view as ViewGroup
            for (index in 0 until viewGroup.childCount) {
                val child = viewGroup.getChildAt(index)
                if (child is GastView) {
                    child.initializeFromParent(this)
                }
            }
        }

        viewState.view.invalidate()
        viewState.initialized = true
    }

    private fun initializeFromParent(parent: GastView) {
        val gastManager = parent.viewState.gastManager?: return
        val parentGastNode = parent.viewState.gastNode ?: return
        val gastNode = GastNode(gastManager, parentGastNode.nodePath)
        initialize(gastManager, gastNode, false)
    }

    @CallSuper
    fun shutdown() {
        // Propagate to the children
        if (viewState.view is ViewGroup) {
            val viewGroup = viewState.view as ViewGroup
            for (index in 0 until viewGroup.childCount) {
                val child = viewGroup.getChildAt(index)
                if (child is GastView) {
                    child.shutdown()
                }
            }
        }

        viewState.view.viewTreeObserver.removeOnPreDrawListener(onPreDrawListener)

        viewState.gastManager?.unregisterGastRenderListener(this)
        viewState.gastManager?.unregisterGastInputListener(inputHandler)
        viewState.gastManager = null

        viewState.gastNode?.release()
        viewState.gastNode = null

        viewState.initialized = false
    }

    @CallSuper
    fun onGastViewAttachedToWindow() {
        if (viewState.initialized) {
            return
        }

        // Get the GastView parent for this view
        val parent = getGastViewParent() ?: return
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
    override fun onRenderDrawFrame() {
        updateGastNodeProperties()
    }

    @CallSuper
    fun onGastViewSizeChanged(width: Float, height: Float) {
        viewState.gastManager?.runOnRenderThread {
            if (viewState.gastNode?.getProjectionMeshType() == GastNode.ProjectionMeshType.RECTANGULAR) {
                val projectionMesh: RectangularProjectionMesh =
                    viewState.gastNode?.getProjectionMesh() as RectangularProjectionMesh
                projectionMesh.setMeshSize(
                    fromPixelsToGodotDimensions(viewState.view.context, width),
                    fromPixelsToGodotDimensions(viewState.view.context, height)
                )
            }
        } ?: return
    }

    @CallSuper
    fun onGastViewVisibilityChanged(visible: Boolean) {
        viewState.gastManager?.runOnRenderThread {
            viewState.gastNode?.updateVisibility(false, visible)
        }
    }

    /**
     * Update the spatial properties (scale, translation, rotation) of the backing gast node.
     */
    private fun updateGastNodeProperties() {
        // Update the node's translation
        viewState.gastNode?.updateLocalTranslation(
            fromPixelsToGodotDimensions(viewState.view.context, getViewTranslationX()),
            fromPixelsToGodotDimensions(viewState.view.context, getViewTranslationY()),
            fromPixelsToGodotDimensions(viewState.view.context, viewState.view.z)
        )

        // Update the node's scale
        viewState.gastNode?.updateLocalScale(viewState.view.scaleX, viewState.view.scaleY, 1f)

        // Update the node's rotation
        viewState.gastNode?.updateLocalRotation(
            viewState.view.rotationX,
            viewState.view.rotationY,
            0f
        )
    }

    private fun getViewTranslationX(): Float {
        // TODO: Update to incorporate non GastView parent x translation
        return if (true) viewState.view.translationX else viewState.view.x
    }

    private fun getViewTranslationY(): Float {
        // TODO: Update to incorporate non GastView parent y translation
        return if (true) viewState.view.translationY else viewState.view.y
    }

    private fun getGastViewParent(): GastView? {
        var viewParent = viewState.view.parent
        while (viewParent != null && viewParent !is GastView) {
            viewParent = viewParent.parent
        }
        return viewParent as GastView?
    }
}
