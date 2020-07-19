package org.godotengine.plugin.gast.webview

import android.content.Context
import android.graphics.Canvas
import android.util.AttributeSet
import android.view.View
import android.widget.FrameLayout
import androidx.annotation.AttrRes
import androidx.annotation.StyleRes
import org.godotengine.godot.Godot
import org.godotengine.plugin.gast.GastNode

internal class GastFrameLayout(
    context: Context,
    attrs: AttributeSet?,
    @AttrRes defStyleAttr: Int,
    @StyleRes defStyleRes: Int
) : FrameLayout(context, attrs, defStyleAttr, defStyleRes) {

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

    private var godot: Godot? = null
    private var gastNode: GastNode? = null

    fun initialize(godot: Godot, gastNode: GastNode) {
        this.godot = godot
        this.gastNode = gastNode
    }

    override fun draw(canvas: Canvas) {
        super.draw(canvas)
    }

    override fun dispatchDraw(canvas: Canvas) {
        super.dispatchDraw(canvas)
    }

    override fun onVisibilityChanged(changedView: View, visibility: Int) {
        super.onVisibilityChanged(changedView, visibility)
        godot?.runOnRenderThread {
            gastNode?.updateVisibility(false, visibility == View.VISIBLE)
        }
    }
}
