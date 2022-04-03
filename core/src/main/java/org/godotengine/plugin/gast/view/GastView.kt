package org.godotengine.plugin.gast.view

import android.content.Context
import android.view.View
import android.view.ViewTreeObserver
import androidx.annotation.CallSuper
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.projectionmesh.RectangularProjectionMesh

/**
 * Represent a Android view rendered by a [GastNode].
 */
internal interface GastView {

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
        get() = GastViewInputHandler(this)

    private val onPreDrawListener: ViewTreeObserver.OnPreDrawListener
        get() = ViewTreeObserver.OnPreDrawListener {
            if (view.isDirty) {
                view.invalidate()
            }
            return@OnPreDrawListener true
        }

    val view: View
    var gastNode: GastNode?
    var gastManager: GastManager?

    @CallSuper
    fun initialize(gastManager: GastManager, gastNode: GastNode) {
        this.gastManager = gastManager
        this.gastNode = gastNode
        gastNode.bindSurface()

        gastManager.registerGastInputListener(inputHandler)
        view.viewTreeObserver.addOnPreDrawListener(onPreDrawListener)

        view.invalidate()
    }

    @CallSuper
    fun shutdown() {
        view.viewTreeObserver.removeOnPreDrawListener(onPreDrawListener)
        this.gastManager?.unregisterGastInputListener(inputHandler)
        this.gastNode = null
    }

    /**
     * Update the spatial properties (scale, translation, rotation) of the backing gast node.
     */
    @CallSuper
    fun updateGastNodeProperties() {
        val node = gastNode?: return

        gastManager?.runOnRenderThread {
            // Update the node's translation
            node.updateLocalTranslation(view.x, view.y, view.z)

            // Update the node's scale
            node.updateLocalScale(view.scaleX, view.scaleY, 1f)

            // Update the node's rotation
            node.updateLocalRotation(view.rotationX, view.rotationY, 0f)
        }
    }

    @CallSuper
    fun onGastViewSizeChanged(width: Float, height: Float) {
        gastManager?.runOnRenderThread {
            if (gastNode?.getProjectionMeshType() == GastNode.ProjectionMeshType.RECTANGULAR) {
                val projectionMesh : RectangularProjectionMesh =
                    gastNode?.getProjectionMesh() as RectangularProjectionMesh
                projectionMesh.setMeshSize(
                    fromPixelsToGodotDimensions(view.context, width),
                    fromPixelsToGodotDimensions(view.context, height))
            }
        } ?: return
    }

    @CallSuper
    fun onGastViewVisibilityChanged(visible: Boolean) {
        gastManager?.runOnRenderThread {
            gastNode?.updateVisibility(false, visible)
        }
    }
}
