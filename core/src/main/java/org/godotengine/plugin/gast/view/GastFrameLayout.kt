package org.godotengine.plugin.gast.view

import android.content.Context
import android.graphics.Canvas
import android.util.AttributeSet
import android.util.Log
import android.view.View
import android.widget.FrameLayout
import androidx.annotation.AttrRes
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode

class GastFrameLayout(
    context: Context,
    attrs: AttributeSet?,
    @AttrRes defStyleAttr: Int
) : FrameLayout(context, attrs, defStyleAttr), GastView {

    companion object {
        private val TAG = GastFrameLayout::class.java.simpleName
        const val MIN_TEXTURE_DIMENSION = 1
    }

    private var textureWidth = MIN_TEXTURE_DIMENSION
    private var textureHeight = MIN_TEXTURE_DIMENSION

    override val view: View = this
    override var gastNode: GastNode? = null
    override var gastManager: GastManager? = null

    constructor(
        context: Context,
        attrs: AttributeSet?
    ) : this(context, attrs, 0)

    constructor(context: Context) : this(context, null)

    override fun initialize(gastManager: GastManager, gastNode: GastNode) {
        Log.d(TAG, "Initializing GastFrameLayout...")
        super.initialize(gastManager, gastNode)
        updateTextureSizeIfNeeded()
    }

    override fun shutdown() {
        Log.d(TAG, "Shutting down GastFrameLayout...")
        super.shutdown()
        textureHeight = MIN_TEXTURE_DIMENSION
        textureWidth = MIN_TEXTURE_DIMENSION
    }

    override fun draw(canvas: Canvas) {
        updateTextureSizeIfNeeded()
        updateGastNodeProperties()
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
        super.onGastViewVisibilityChanged(visibility == View.VISIBLE)
    }

    private fun updateTextureSizeIfNeeded() {
        // Update the texture size
        val widthInPixels = width
        val heightInPixels = height
        if ((textureWidth != widthInPixels || textureHeight != heightInPixels)
            && widthInPixels >= MIN_TEXTURE_DIMENSION
            && heightInPixels >= MIN_TEXTURE_DIMENSION
            && gastNode != null
            && gastManager != null
        ) {
            gastNode?.setSurfaceTextureSize(widthInPixels, heightInPixels) ?: return
            onGastViewSizeChanged(width.toFloat(), height.toFloat())

            Log.d(TAG, "Updating texture size to X - $widthInPixels, Y - $heightInPixels")
            textureWidth = widthInPixels
            textureHeight = heightInPixels
            invalidate()
        }
    }
}

