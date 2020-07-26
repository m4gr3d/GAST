package org.godotengine.plugin.gast.view

import android.content.Context
import android.graphics.Canvas
import android.util.AttributeSet
import android.view.View
import android.widget.FrameLayout
import androidx.annotation.AttrRes
import androidx.annotation.StyleRes
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode

class GastFrameLayout(
    context: Context,
    attrs: AttributeSet?,
    @AttrRes defStyleAttr: Int,
    @StyleRes defStyleRes: Int
) : FrameLayout(context, attrs, defStyleAttr, defStyleRes) {

    private val inputHandler: GastViewInputHandler = GastViewInputHandler(this)

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
    }

    private var gastManager: GastManager? = null
    internal var gastNode: GastNode? = null

    fun initialize(gastManager: GastManager, gastNode: GastNode) {
        this.gastNode = gastNode
        gastNode.bindSurface()

        gastManager.registerGastInputListener(inputHandler)
    }

    fun shutdown() {
        gastManager?.unregisterGastInputListener(inputHandler)
        this.gastNode = null
    }

    override fun draw(canvas: Canvas) {
        val surfaceCanvas = gastNode?.lockSurfaceCanvas() ?: return
        super.draw(surfaceCanvas)
        gastNode?.unlockSurfaceCanvas()

    }

    override fun dispatchDraw(canvas: Canvas) {
        val surfaceCanvas = gastNode?.lockSurfaceCanvas() ?: return
        super.dispatchDraw(surfaceCanvas)
        gastNode?.unlockSurfaceCanvas()
    }

    override fun onSizeChanged(width: Int, height: Int, oldWidth: Int, oldHeight: Int) {
        super.onSizeChanged(width, height, oldWidth, oldHeight)
        // TODO: complete
    }

    override fun onVisibilityChanged(changedView: View, visibility: Int) {
        super.onVisibilityChanged(changedView, visibility)
        gastManager?.runOnRenderThread {
            gastNode?.updateVisibility(false, visibility == View.VISIBLE)
        }
    }
}

