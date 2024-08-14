package org.godotengine.plugin.gast.xr.view

import android.content.Context
import android.graphics.Canvas
import android.graphics.Rect
import android.util.AttributeSet
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.widget.FrameLayout
import androidx.annotation.AttrRes
import androidx.core.view.contains
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode

/**
 * Provides access to a [Surface] instance for rendering.
 */
class XrSurfaceView(
    context: Context,
    attrs: AttributeSet?,
    @AttrRes defStyleAttr: Int
) : FrameLayout(context, attrs, defStyleAttr), XrView {

    private inner class XrSurfaceHolder : SurfaceHolder, SurfaceHolder.Callback2 {

        private val callbacks = HashSet<SurfaceHolder.Callback>()

        var delegateSurfaceHolder: SurfaceHolder? = null
            set(surfaceHolder) {
                if (field == surfaceHolder) {
                    return
                }
                field?.apply {
                    removeCallback(this@XrSurfaceHolder)
                    if (this@XrSurfaceView.isAttachedToWindow && this.surface != null) {
                        surfaceDestroyed(this)
                    }
                }

                field = surfaceHolder
                field?.apply {
                    addCallback(this@XrSurfaceHolder)
                    if (this@XrSurfaceView.isAttachedToWindow && this.surface != null) {
                        surfaceCreated(this)
                    }
                }
            }

        override fun addCallback(callback: SurfaceHolder.Callback) {
            callbacks.add(callback)
        }

        override fun removeCallback(callback: SurfaceHolder.Callback) {
            callbacks.remove(callback)
        }

        override fun isCreating() = delegateSurfaceHolder?.isCreating ?: false

        override fun setType(type: Int) {
            // Ignored - Deprecated in API level 15
        }

        override fun setFixedSize(width: Int, height: Int) {
            if (delegateSurfaceHolder != null) {
                delegateSurfaceHolder?.setFixedSize(width, height)
            } else {
                viewState.gastNode?.setSurfaceTextureSize(width, height)
            }
        }

        override fun setSizeFromLayout() {
            if (delegateSurfaceHolder != null) {
                delegateSurfaceHolder?.setSizeFromLayout()
            } else {
                TODO("Not yet implemented")
            }
        }

        override fun setFormat(format: Int) {
            // Ignored
        }

        override fun setKeepScreenOn(screenOn: Boolean) {
            this@XrSurfaceView.keepScreenOn = screenOn
        }

        override fun lockCanvas(): Canvas? {
            if (delegateSurfaceHolder != null) {
                return delegateSurfaceHolder?.lockCanvas()
            }
            return viewState.gastNode?.lockSurfaceCanvas()
        }

        override fun lockCanvas(dirty: Rect?): Canvas? {
            if (delegateSurfaceHolder != null) {
                return delegateSurfaceHolder?.lockCanvas(dirty)
            }
            return viewState.gastNode?.lockSurfaceCanvas(dirty)
        }

        override fun unlockCanvasAndPost(canvas: Canvas?) {
            if (delegateSurfaceHolder != null) {
                delegateSurfaceHolder?.unlockCanvasAndPost(canvas)
            } else {
                viewState.gastNode?.unlockSurfaceCanvas()
            }
        }

        override fun getSurfaceFrame(): Rect {
            if (delegateSurfaceHolder != null) {
                return delegateSurfaceHolder!!.surfaceFrame
            } else {
                TODO("Not yet implemented")
            }
        }

        override fun getSurface(): Surface? =
            delegateSurfaceHolder?.surface ?: viewState.gastNode?.bindSurface()

        override fun surfaceCreated(holder: SurfaceHolder) {
            for (callback in callbacks) {
                callback.surfaceCreated(this)
            }
        }

        override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
            for (callback in callbacks) {
                callback.surfaceChanged(this, format, width, height)
            }
        }

        override fun surfaceDestroyed(holder: SurfaceHolder) {
            for (callback in callbacks) {
                callback.surfaceDestroyed(this)
            }
        }

        override fun surfaceRedrawNeeded(holder: SurfaceHolder) {
            for (callback in callbacks) {
                if (callback is SurfaceHolder.Callback2) {
                    callback.surfaceRedrawNeeded(this)
                }
            }
        }

        fun onXrStateChanged(inXr: Boolean) {
            if (delegateSurfaceHolder != null) {
                return
            }
            if (inXr) {
                surfaceCreated(this)
            } else {
                surfaceDestroyed(this)
            }
        }
    }

    override val viewState = XrViewState(this)
    private val surfaceView = SurfaceView(context, attrs, defStyleAttr)
    private val surfaceHolder = XrSurfaceHolder()

    constructor(
        context: Context,
        attrs: AttributeSet?
    ) : this(context, attrs, 0)

    constructor(context: Context) : this(context, null)

    init {
        addSurfaceView()
    }

    fun getHolder(): SurfaceHolder = surfaceHolder

    override fun initialize(gastManager: GastManager, gastNode: GastNode) {
        super.initialize(gastManager, gastNode)

        onXrStateChanged(viewState.initialized)
    }

    override fun shutdown() {
        super.shutdown()

        onXrStateChanged(viewState.initialized)
    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        onXrViewAttachedToWindow()
    }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        onXrViewDetachedFromWindow()
    }

    private fun onXrStateChanged(inXr: Boolean) {
        if (inXr) {
            // If `surfaceView` is within the viewtree, it should be removed
            surfaceHolder.delegateSurfaceHolder = null
            removeView(surfaceView)
        } else {
            // If `surfaceView` is not within the viewtree and this view is still attached to the
            // viewtree, it should be added back
            addSurfaceView()
        }

        surfaceHolder.onXrStateChanged(inXr)
    }

    private fun addSurfaceView() {
        if (!contains(surfaceView)) {
            addView(surfaceView)
            surfaceHolder.delegateSurfaceHolder = surfaceView.holder
        }
    }
}
