package org.godotengine.plugin.gast.view

import android.content.Context
import android.graphics.Canvas
import android.graphics.Rect
import android.util.AttributeSet
import android.view.Surface
import android.view.SurfaceHolder
import android.view.View
import androidx.annotation.AttrRes

class GastSurfaceView(
    context: Context,
    attrs: AttributeSet?,
    @AttrRes defStyleAttr: Int
) : View(context, attrs, defStyleAttr), SurfaceHolder, GastView {

    override val viewState = GastViewState(this)

    fun getHolder(): SurfaceHolder = this

    override fun addCallback(callback: SurfaceHolder.Callback?) {
        TODO("Not yet implemented")
    }

    override fun removeCallback(callback: SurfaceHolder.Callback?) {
        TODO("Not yet implemented")
    }

    override fun isCreating() = false

    override fun setType(type: Int) {
        // Ignored - Deprecated in API level 15
    }

    override fun setFixedSize(width: Int, height: Int) {
        viewState.gastNode?.setSurfaceTextureSize(width, height)
    }

    override fun setSizeFromLayout() {
        TODO("Not yet implemented")
    }

    override fun setFormat(format: Int) {
        // Ignored
    }

    override fun lockCanvas(): Canvas? = viewState.gastNode?.lockSurfaceCanvas()

    override fun lockCanvas(dirty: Rect?): Canvas? = viewState.gastNode?.lockSurfaceCanvas(dirty)

    override fun unlockCanvasAndPost(canvas: Canvas?) {
        viewState.gastNode?.unlockSurfaceCanvas()
    }

    override fun getSurfaceFrame(): Rect {
        TODO("Not yet implemented")
    }

    override fun getSurface(): Surface? = viewState.gastNode?.bindSurface()
}
