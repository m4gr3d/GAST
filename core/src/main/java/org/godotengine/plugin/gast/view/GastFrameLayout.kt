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
import org.godotengine.plugin.gast.input.GastInputListener

class GastFrameLayout(
    context: Context,
    attrs: AttributeSet?,
    @AttrRes defStyleAttr: Int,
    @StyleRes defStyleRes: Int
) : FrameLayout(context, attrs, defStyleAttr, defStyleRes), GastInputListener {

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
    private var gastNode: GastNode? = null

    fun initialize(gastManager: GastManager, gastNode: GastNode) {
        this.gastNode = gastNode
        gastNode.bindSurface()

        gastManager.registerGastInputListener(this)
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

    override fun onMainInputAction(
        action: String,
        pressState: GastInputListener.InputPressState,
        strength: Float
    ) {
        TODO("Not yet implemented")
    }

    override fun onMainInputHover(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        TODO("Not yet implemented")
    }

    override fun onMainInputPress(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        TODO("Not yet implemented")
    }

    override fun onMainInputRelease(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        TODO("Not yet implemented")
    }

    override fun onMainInputScroll(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float,
        horizontalDelta: Float,
        verticalDelta: Float
    ) {
        TODO("Not yet implemented")
    }
}

