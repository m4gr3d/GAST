package org.godotengine.plugin.gast.view

import android.content.Context
import android.graphics.Canvas
import android.util.AttributeSet
import android.util.Log
import android.view.View
import android.view.ViewTreeObserver
import android.widget.FrameLayout
import androidx.annotation.AttrRes
import androidx.annotation.StyleRes
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.projectionmesh.RectangularProjectionMesh

class GastFrameLayout(
    context: Context,
    attrs: AttributeSet?,
    @AttrRes defStyleAttr: Int
) : FrameLayout(context, attrs, defStyleAttr) {

    private val inputHandler: GastViewInputHandler = GastViewInputHandler(this)
    private val onPreDrawListener = ViewTreeObserver.OnPreDrawListener {
        if (isDirty) {
            invalidate()
        }
        return@OnPreDrawListener true
    }

    private var textureWidth = MIN_TEXTURE_DIMENSION
    private var textureHeight = MIN_TEXTURE_DIMENSION

    constructor(
        context: Context,
        attrs: AttributeSet?
    ) : this(context, attrs, 0)

    constructor(context: Context) : this(context, null)

    companion object {
        private val TAG = GastFrameLayout::class.java.simpleName
        private const val MIN_TEXTURE_DIMENSION = 1
        private const val DP_RATIO = 4f / 1000f

        fun fromPixelsToGodotDimensions(context:Context, dimensionInPixels: Float): Float {
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
            val dimensionInPixels = dimensionInDp * context.resources.displayMetrics.density
            return dimensionInPixels
        }
    }

    private var gastManager: GastManager? = null
    internal var gastNode: GastNode? = null

    fun initialize(gastManager: GastManager, gastNode: GastNode) {
        Log.d(TAG, "Initializing GastFrameLayout...")
        this.gastManager = gastManager
        this.gastNode = gastNode
        gastNode.bindSurface()

        gastManager.registerGastInputListener(inputHandler)
        viewTreeObserver.addOnPreDrawListener(onPreDrawListener)

        updateTextureSizeIfNeeded()
        invalidate()
    }

    fun shutdown() {
        Log.d(TAG, "Shutting down GastFrameLayout...")
        viewTreeObserver.removeOnPreDrawListener(onPreDrawListener)
        gastManager?.unregisterGastInputListener(inputHandler)
        this.gastNode = null
        textureHeight = MIN_TEXTURE_DIMENSION
        textureWidth = MIN_TEXTURE_DIMENSION
    }

    private fun updateTextureSizeIfNeeded() {
        // Update the texture size
        val widthInPixels = width
        val heightInPixels = height
        if ((textureWidth != widthInPixels || textureHeight != heightInPixels)
            && widthInPixels >= MIN_TEXTURE_DIMENSION && heightInPixels >= MIN_TEXTURE_DIMENSION
        ) {
            gastNode?.setSurfaceTextureSize(widthInPixels, heightInPixels) ?: return
            gastManager?.runOnRenderThread {
                if (gastNode?.getProjectionMeshType() == GastNode.ProjectionMeshType.RECTANGULAR) {
                    val projectionMesh : RectangularProjectionMesh =
                        gastNode?.getProjectionMesh() as RectangularProjectionMesh
                    projectionMesh.setMeshSize(
                        fromPixelsToGodotDimensions(context, width.toFloat()),
                        fromPixelsToGodotDimensions(context, height.toFloat()))
                }
            } ?: return

            Log.d(TAG, "Updating texture size to X - $widthInPixels, Y - $heightInPixels")
            textureWidth = widthInPixels
            textureHeight = heightInPixels
            invalidate()
        }
    }

    override fun draw(canvas: Canvas) {
        updateTextureSizeIfNeeded()
        val surfaceCanvas = gastNode?.lockSurfaceCanvas() ?: canvas
        super.draw(surfaceCanvas)
        gastNode?.unlockSurfaceCanvas()
    }

    override fun dispatchDraw(canvas: Canvas) {
        val surfaceCanvas = gastNode?.lockSurfaceCanvas() ?: canvas
        super.dispatchDraw(surfaceCanvas)
        gastNode?.unlockSurfaceCanvas()
    }

    override fun onSizeChanged(width: Int, height: Int, oldWidth: Int, oldHeight: Int) {
        super.onSizeChanged(width, height, oldWidth, oldHeight)
        Log.d(TAG, "On size changed: $width, $height, $oldWidth, $oldHeight")
        updateTextureSizeIfNeeded()
    }

    override fun onVisibilityChanged(changedView: View, visibility: Int) {
        super.onVisibilityChanged(changedView, visibility)
        gastManager?.runOnRenderThread {
            gastNode?.updateVisibility(false, visibility == View.VISIBLE)
        }
    }
}

