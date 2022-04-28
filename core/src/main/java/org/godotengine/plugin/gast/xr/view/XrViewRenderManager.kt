package org.godotengine.plugin.gast.xr.view

import android.content.Context
import android.graphics.Canvas
import android.util.Log
import android.view.View
import android.view.ViewTreeObserver
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.GastRenderListener
import org.godotengine.plugin.gast.projectionmesh.RectangularProjectionMesh
import kotlin.math.max

/**
 * Manages [XrView]'s render related logic.
 */
internal class XrViewRenderManager(private val viewState: XrViewState) : GastRenderListener {

    companion object {
        private val TAG = XrViewRenderManager::class.java.simpleName

        private const val DP_RATIO = 4f / 1000f
        private const val MIN_CHILD_ELEVATION = 1f // pixels

        private const val TRANSLATION_X_INDEX = 0
        private const val TRANSLATION_Y_INDEX = 1
        private const val TRANSLATION_Z_INDEX = 2

        private const val MIN_TEXTURE_DIMENSION = 1

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
         */
        fun fromAndroidXToGodotX(
            locationXInParent: Int,
            viewWidth: Int,
            parentWidth: Int
        ): Float {
            return locationXInParent + (viewWidth / 2f) - (parentWidth / 2f)
        }

        fun fromAndroidYToGodotY(
            locationYInParent: Int,
            viewHeight: Int,
            parentHeight: Int
        ): Float {
            return (parentHeight / 2f) - (locationYInParent + (viewHeight / 2f))
        }

        /**
         * Convert from the Godot coordinate system to the Android coordinate system
         */
        fun fromGodotXToAndroidX(
            locationXInParent: Int,
            viewWidth: Int,
            parentWidth: Int
        ): Float {
            return (parentWidth / 2f) + locationXInParent - (viewWidth / 2f)
        }

        fun fromGodotYToAndroidY(
            locationYInParent: Int,
            viewHeight: Int,
            parentHeight: Int
        ): Float {
            return (parentHeight / 2f) - locationYInParent - (viewHeight / 2f)
        }
    }

    private val onPreDrawListener = ViewTreeObserver.OnPreDrawListener {
        if (viewState.view.isDirty) {
            viewState.view.invalidate()
        }
        return@OnPreDrawListener true
    }

    private val locationInParent = FloatArray(3)

    private var textureWidth = MIN_TEXTURE_DIMENSION
    private var textureHeight = MIN_TEXTURE_DIMENSION

    internal fun onInitialize() {
        viewState.gastManager?.registerGastRenderListener(this)
        viewState.view.viewTreeObserver.addOnPreDrawListener(onPreDrawListener)

        viewState.view.invalidate()
        updateTextureSizeIfNeeded()
    }

    internal fun onShutdown() {
        viewState.view.viewTreeObserver.removeOnPreDrawListener(onPreDrawListener)

        viewState.gastManager?.unregisterGastRenderListener(this)

        textureHeight = MIN_TEXTURE_DIMENSION
        textureWidth = MIN_TEXTURE_DIMENSION
    }

    internal fun drawHelper(originalCanvas: Canvas, drawTask: (Canvas) -> Unit) {
        val surfaceCanvas = viewState.gastNode?.lockSurfaceCanvas() ?: originalCanvas
        drawTask(surfaceCanvas)
        viewState.gastNode?.unlockSurfaceCanvas()
    }

    override fun onRenderDrawFrame() {
        updateTextureSizeIfNeeded()
        computeLocationInParent()
        updateGastNodeProperties()
    }

    private fun updateTextureSizeIfNeeded() {
        // Update the texture size
        val widthInPixels = viewState.view.width
        val heightInPixels = viewState.view.height
        if ((textureWidth != widthInPixels || textureHeight != heightInPixels)
            && widthInPixels >= MIN_TEXTURE_DIMENSION
            && heightInPixels >= MIN_TEXTURE_DIMENSION
            && viewState.gastNode != null
            && viewState.gastManager != null
        ) {
            viewState.gastNode?.setSurfaceTextureSize(widthInPixels, heightInPixels) ?: return

            Log.d(TAG, "Updating texture size to X - $widthInPixels, Y - $heightInPixels")
            textureWidth = widthInPixels
            textureHeight = heightInPixels
            viewState.view.invalidate()

            onViewSizeChanged(viewState.view.width.toFloat(), viewState.view.height.toFloat())
        }
    }

    /**
     * Update the spatial properties (scale, translation, rotation) of the backing gast node.
     */
    private fun updateGastNodeProperties() {
        viewState.gastNode?.updateLocalTranslation(
            fromPixelsToGodotDimensions(
                viewState.view.context,
                locationInParent[TRANSLATION_X_INDEX]
            ),
            fromPixelsToGodotDimensions(
                viewState.view.context,
                locationInParent[TRANSLATION_Y_INDEX]
            ),
            fromPixelsToGodotDimensions(
                viewState.view.context,
                locationInParent[TRANSLATION_Z_INDEX]
            )
        )

        // Update the node's scale
        viewState.gastNode?.updateLocalScale(viewState.view.scaleX, viewState.view.scaleY, 1f)

        // Update the node's rotation
        viewState.gastNode?.updateLocalRotation(
            viewState.view.rotationX,
            viewState.view.rotationY,
            0f
        )

        // Update the node's alpha value
        viewState.gastNode?.updateAlpha(viewState.view.alpha)
    }

    internal fun onViewSizeChanged(width: Float, height: Float) {
        updateTextureSizeIfNeeded()
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

    private fun computeLocationInParent() {
        if (isRoot()) {
            locationInParent[TRANSLATION_X_INDEX] = viewState.view.translationX
            locationInParent[TRANSLATION_Y_INDEX] = viewState.view.translationY
        } else {
            val parent = viewState.parent ?: return

            var locationX = viewState.view.left
            var locationY = viewState.view.top

            var viewParent = viewState.view.parent
            while (viewParent is View && viewParent != parent) {
                val view = viewParent as View

                locationX += view.left
                locationY += view.top
                viewParent = view.parent
            }

            locationInParent[TRANSLATION_X_INDEX] = fromAndroidXToGodotX(
                locationX,
                viewState.view.width,
                parent.viewState.view.width
            )
            locationInParent[TRANSLATION_Y_INDEX] = fromAndroidYToGodotY(
                locationY,
                viewState.view.height,
                parent.viewState.view.height
            )
        }
        locationInParent[TRANSLATION_Z_INDEX] = max(viewState.view.z, MIN_CHILD_ELEVATION)
    }

    private fun isRoot() = viewState.parent == null
}
