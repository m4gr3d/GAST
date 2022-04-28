package org.godotengine.plugin.gast.xr.view

import android.content.Context
import android.graphics.Canvas
import android.util.AttributeSet
import android.view.View
import androidx.annotation.AttrRes
import androidx.appcompat.widget.AppCompatImageView

/**
 * Derived implemented of [AppCompatImageView] that allows rendering and display of 3D models
 * when in xr mode.
 */
class XrModelView(
    context: Context,
    attrs: AttributeSet?,
    @AttrRes defStyleAttr: Int
) : AppCompatImageView(context, attrs, defStyleAttr), XrView {

    override val viewState = XrViewState(this)

    constructor(
        context: Context,
        attrs: AttributeSet?
    ) : this(context, attrs, 0)

    constructor(context: Context) : this(context, null)

    override fun draw(canvas: Canvas) {
        viewState.renderManager.drawHelper(canvas) {
            super.draw(it)
        }
    }

    override fun dispatchDraw(canvas: Canvas) {
        viewState.renderManager.drawHelper(canvas) {
            super.dispatchDraw(it)
        }
    }

    override fun onSizeChanged(width: Int, height: Int, oldWidth: Int, oldHeight: Int) {
        super.onSizeChanged(width, height, oldWidth, oldHeight)
        onXrViewSizeChanged(width.toFloat(), height.toFloat())
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
}
