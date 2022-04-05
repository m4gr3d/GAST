package org.godotengine.plugin.gast.view

import android.content.Context
import android.graphics.Rect
import android.graphics.RectF
import android.os.Build
import android.util.Log
import android.view.ViewTreeObserver
import androidx.annotation.RequiresApi
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.GastRenderListener
import org.godotengine.plugin.gast.projectionmesh.RectangularProjectionMesh

/**
 * Manages [GastView]'s render related logic.
 */
internal class GastViewRenderManager(private val viewState: GastViewState): GastRenderListener {

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

        /**
         * Convert from the Android coordinate system to the Godot coordinate system
         *
         * Android coordinate system: (0, 0) -> (left, top)
         * Godot coordinate system: (0, 0) -> (width / 2, height / 2)
         */
        fun fromAndroidCoordinatesToGodotCoordinates(inAndroidCoordsRect: RectF,
                                                     outGodotCoordsRect: RectF = RectF()
        ): RectF {
            // x coord
            outGodotCoordsRect.left = inAndroidCoordsRect.centerX()
            // y coord
            outGodotCoordsRect.top = inAndroidCoordsRect.centerY()
            // width
            outGodotCoordsRect.right = inAndroidCoordsRect.right
            // height
            outGodotCoordsRect.bottom = inAndroidCoordsRect.bottom

            return outGodotCoordsRect
        }

        /**
         * Convert from the Godot coordinate system to the Android coordinate system
         */
        fun fromGodotCoordinatesToAndroidCoordinates(inGodotCoordsRect: RectF,
                                                     outAndroidCoordsRect: RectF = RectF()
        ): RectF {
            // x coord
            outAndroidCoordsRect.left = (2 * inGodotCoordsRect.left) - inGodotCoordsRect.right
            // y coord
            outAndroidCoordsRect.top = (2 * inGodotCoordsRect.top) - inGodotCoordsRect.bottom
            // width
            outAndroidCoordsRect.right = inGodotCoordsRect.right
            // height
            outAndroidCoordsRect.bottom = inGodotCoordsRect.bottom

            return outAndroidCoordsRect
        }
    }

    private val onPreDrawListener = ViewTreeObserver.OnPreDrawListener {
        if (viewState.view.isDirty) {
            viewState.view.invalidate()
        }
        return@OnPreDrawListener true
    }

    internal fun onInitialize() {
        viewState.gastManager?.registerGastRenderListener(this)
        viewState.view.viewTreeObserver.addOnPreDrawListener(onPreDrawListener)

        viewState.view.invalidate()
    }

    internal fun onShutdown() {
        viewState.view.viewTreeObserver.removeOnPreDrawListener(onPreDrawListener)

        viewState.gastManager?.unregisterGastRenderListener(this)
    }

    private var renderCount = 0

    @RequiresApi(Build.VERSION_CODES.Q)
    override fun onRenderDrawFrame() {
        if (renderCount == 0) {
            updateGastNodeProperties()
        }
        renderCount = (renderCount + 1) % 300
    }

    /**
     * Update the spatial properties (scale, translation, rotation) of the backing gast node.
     */
    @RequiresApi(Build.VERSION_CODES.Q)
    private fun updateGastNodeProperties() {
        // Update the node's translation
        val rect = Rect()
        var result = viewState.view.getGlobalVisibleRect(rect)
        if (result) {
            Log.d("FHK", "Global visible rect: $rect")
        } else {
            Log.d("FHK", "Unable to get the global visible rect")
        }

        result = viewState.view.getLocalVisibleRect(rect)
        if (result) {
            Log.d("FHK", "Local visible rect: $rect")
        } else {
            Log.d("FHK", "Unable to get the local visible rect")
        }

        val location = IntArray(2)
        viewState.view.getLocationInSurface(location)
        Log.d("FHK", "Location in surface: ${location.contentToString()}")

        viewState.view.getLocationInWindow(location)
        Log.d("FHK", "Location in window: ${location.contentToString()}")

        viewState.view.getLocationOnScreen(location)
        Log.d("FHK", "Location on screen: ${location.contentToString()}")

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

    internal fun onViewSizeChanged(width: Float, height: Float) {
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

    internal fun onVisibilityChanged(visible: Boolean) {
        viewState.gastManager?.runOnRenderThread {
            viewState.gastNode?.updateVisibility(false, visible)
        }
    }

    private fun getViewTranslationX(): Float {
        // TODO: Update to incorporate non GastView parent x translation
        return if (true) viewState.view.translationX else viewState.view.x
    }

    private fun getViewTranslationY(): Float {
        // TODO: Update to incorporate non GastView parent y translation
        return if (true) viewState.view.translationY else viewState.view.y
    }
}
