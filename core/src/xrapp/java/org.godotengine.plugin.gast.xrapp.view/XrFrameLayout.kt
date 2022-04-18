package org.godotengine.plugin.gast.xrapp.view

import android.content.Context
import android.graphics.Canvas
import android.util.AttributeSet
import android.util.Log
import android.view.View
import android.widget.FrameLayout
import androidx.annotation.AttrRes
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode

/**
 * Derived implementation of [FrameLayout] that allows its subtree z-depth positioning in 3D space.
 *
 * [XrFrameLayout] can be nested within themselves and other view group allowing for UI that
 * breaks out of the 2D app window and can be interacted and animated in 3D space.
 *
 * [XrFrameLayout] uses the [View.getZ] property as their z-component in 3D space relative to
 * their parent.
 */
class XrFrameLayout(
    context: Context,
    attrs: AttributeSet?,
    @AttrRes defStyleAttr: Int
) : FrameLayout(context, attrs, defStyleAttr), XrView {

    companion object {
        private val TAG = XrFrameLayout::class.java.simpleName
        const val MIN_TEXTURE_DIMENSION = 1
    }

    private var textureWidth = MIN_TEXTURE_DIMENSION
    private var textureHeight = MIN_TEXTURE_DIMENSION

    override val viewState: XrViewState = XrViewState(this)

    constructor(
        context: Context,
        attrs: AttributeSet?
    ) : this(context, attrs, 0)

    constructor(context: Context) : this(context, null)

    @JvmOverloads
    override fun initialize(gastManager: GastManager, gastNode: GastNode) {
        Log.d(TAG, "Initializing XrFrameLayout...")
        super.initialize(gastManager, gastNode)
        updateTextureSizeIfNeeded()
    }

    override fun shutdown() {
        Log.d(TAG, "Shutting down XrFrameLayout...")
        super.shutdown()
        textureHeight = MIN_TEXTURE_DIMENSION
        textureWidth = MIN_TEXTURE_DIMENSION
    }

    override fun draw(canvas: Canvas) {
        updateTextureSizeIfNeeded()
        val surfaceCanvas = viewState.gastNode?.lockSurfaceCanvas() ?: canvas
        super.draw(surfaceCanvas)
        viewState.gastNode?.unlockSurfaceCanvas()
    }

    override fun dispatchDraw(canvas: Canvas) {
        val surfaceCanvas = viewState.gastNode?.lockSurfaceCanvas() ?: canvas
        super.dispatchDraw(surfaceCanvas)
        viewState.gastNode?.unlockSurfaceCanvas()
    }

    override fun onSizeChanged(width: Int, height: Int, oldWidth: Int, oldHeight: Int) {
        super.onSizeChanged(width, height, oldWidth, oldHeight)
        Log.d(TAG, "On size changed: $width, $height, $oldWidth, $oldHeight")
        updateTextureSizeIfNeeded()
    }

    override fun onVisibilityChanged(changedView: View, visibility: Int) {
        super.onVisibilityChanged(changedView, visibility)
        onXrViewVisibilityChanged(visibility == View.VISIBLE)
    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        onXrViewAttachedToWindow()
    }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        onXrViewDetachedFromWindow()
    }

    private fun updateTextureSizeIfNeeded() {
        // Update the texture size
        val widthInPixels = width
        val heightInPixels = height
        if ((textureWidth != widthInPixels || textureHeight != heightInPixels)
            && widthInPixels >= MIN_TEXTURE_DIMENSION
            && heightInPixels >= MIN_TEXTURE_DIMENSION
            && viewState.gastNode != null
            && viewState.gastManager != null
        ) {
            viewState.gastNode?.setSurfaceTextureSize(widthInPixels, heightInPixels) ?: return
            onXrViewSizeChanged(width.toFloat(), height.toFloat())

            Log.d(TAG, "Updating texture size to X - $widthInPixels, Y - $heightInPixels")
            textureWidth = widthInPixels
            textureHeight = heightInPixels
            invalidate()
        }
    }
}

