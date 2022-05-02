package org.godotengine.plugin.gast.xr.view

import android.content.Context
import android.graphics.Canvas
import android.util.AttributeSet
import android.view.View
import androidx.annotation.AttrRes
import androidx.constraintlayout.widget.ConstraintLayout

/**
 * Derived implementation of [ConstraintLayout] that allows its subtree z-depth positioning in
 * 3D space.
 *
 * [XrConstraintLayout] can be nested within themselves and other view group allowing for UI that
 * breaks out of the 2D app window and can be interacted and animated in 3D space.
 *
 * [XrConstraintLayout] uses the [View.getZ] property as their z-component in 3D space relative to
 * their parent.
 */
class XrConstraintLayout(
    context: Context,
    attrs: AttributeSet?,
    @AttrRes defStyleAttr: Int
) : ConstraintLayout(context, attrs, defStyleAttr), XrView {

    override val viewState: XrViewState = XrViewState(this)

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
