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
import kotlin.math.roundToInt

class GastFrameLayout(
    context: Context,
    attrs: AttributeSet?,
    @AttrRes defStyleAttr: Int,
    @StyleRes defStyleRes: Int
) : FrameLayout(context, attrs, defStyleAttr, defStyleRes) {

    private val inputHandler: GastViewInputHandler = GastViewInputHandler(this)
    private val onPreDrawListener = ViewTreeObserver.OnPreDrawListener {
        if (isDirty) {
            invalidate()
        }
        return@OnPreDrawListener true
    }

    private var textureWidth = DEFAULT_TEXTURE_DIMENSION
    private var textureHeight = DEFAULT_TEXTURE_DIMENSION

    constructor(
        context: Context,
        attrs: AttributeSet?,
        @AttrRes defStyleAttr: Int
    ) : this(context, attrs, defStyleAttr, 0)

    constructor(
        context: Context,
        attrs: AttributeSet?
    ) : this(context, attrs, 0)

    constructor(context: Context) : this(context, null)

    companion object {
        private val TAG = GastFrameLayout::class.java.simpleName
        private const val DEFAULT_TEXTURE_DIMENSION = 512
    }

    private var gastManager: GastManager? = null
    internal var gastNode: GastNode? = null

    fun initialize(gastManager: GastManager, gastNode: GastNode) {
        Log.d(TAG, "Initializing GastFrameLayout...")
        this.gastNode = gastNode
        gastNode.bindSurface()
        gastNode.setSurfaceTextureSize(textureWidth, textureHeight)

        gastManager.registerGastInputListener(inputHandler)
        viewTreeObserver.addOnPreDrawListener(onPreDrawListener)

        invalidate()
    }

    fun shutdown() {
        Log.d(TAG, "Shutting down GastFrameLayout...")
        viewTreeObserver.removeOnPreDrawListener(onPreDrawListener)
        gastManager?.unregisterGastInputListener(inputHandler)
        this.gastNode = null
    }

    private fun setTextureSize(width: Int, height: Int) {
        val density = resources.displayMetrics.scaledDensity
        textureWidth = (width / density).roundToInt()
        textureHeight = (height / density).roundToInt()
        gastNode?.setSurfaceTextureSize(textureWidth, textureHeight)

        Log.d(TAG, "Updating texture size for density $density: $textureWidth, $textureHeight")
        invalidate()
    }

    override fun draw(canvas: Canvas) {
        val surfaceCanvas = gastNode?.lockSurfaceCanvas(
            textureWidth / width.toFloat(),
            textureHeight / height.toFloat()
        ) ?: canvas
        super.draw(surfaceCanvas)
        gastNode?.unlockSurfaceCanvas()
    }

    override fun dispatchDraw(canvas: Canvas) {
        val surfaceCanvas = gastNode?.lockSurfaceCanvas(
            textureWidth / width.toFloat(),
            textureHeight / height.toFloat()
        ) ?: canvas
        super.dispatchDraw(surfaceCanvas)
        gastNode?.unlockSurfaceCanvas()
    }

    override fun onSizeChanged(width: Int, height: Int, oldWidth: Int, oldHeight: Int) {
        super.onSizeChanged(width, height, oldWidth, oldHeight)
        Log.d(TAG, "On size changed: $width, $height, $oldWidth, $oldHeight")
        if (width > 0 && height > 0) {
            setTextureSize(width, height)
        }
    }

    override fun onVisibilityChanged(changedView: View, visibility: Int) {
        super.onVisibilityChanged(changedView, visibility)
        gastManager?.runOnRenderThread {
            gastNode?.updateVisibility(false, visibility == View.VISIBLE)
        }
    }
}

