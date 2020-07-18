package org.godotengine.plugin.gast.extension

import android.annotation.SuppressLint
import android.app.Activity
import android.graphics.SurfaceTexture
import android.os.Build
import android.view.View
import androidx.annotation.CallSuper
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.plugin.GodotPluginRegistry
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastRenderListener
import org.godotengine.plugin.gast.input.GastInputListener
import java.util.concurrent.atomic.AtomicInteger

/**
 * Base class for GAST extension plugins.
 *
 * Provides common logic to create a GAST extension.
 */
abstract class GastExtension(godot: Godot) : GodotPlugin(godot),
    SurfaceTexture.OnFrameAvailableListener, GastRenderListener, GastInputListener {

    protected val gastManager: GastManager by lazy {
        GodotPluginRegistry.getPluginRegistry().getPlugin("gast-core") as GastManager
    }

    private val updateTextureImageCounter = AtomicInteger()
    protected var surfaceTexture: SurfaceTexture? = null
        private set

    /**
     * Initialize the [surfaceTexture] provided by this class to its children.
     * After this call, the [surfaceTexture] property will be valid.
     *
     * This method should be called on the UI thread.
     */
    @SuppressLint("Recycle")
    protected fun initSurfaceTexture(texName: Int) {
        surfaceTexture = SurfaceTexture(texName)
        surfaceTexture?.setOnFrameAvailableListener(this)
    }

    /**
     * Return true if the [surfaceTexture] is initialized.
     *
     * This method should be called on the UI thread.
     */
    protected fun isSurfaceTextureValid(): Boolean {
        val texture = surfaceTexture ?: return false
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            !texture.isReleased
        } else {
            true
        }
    }

    /**
     * Release the [surfaceTexture] provided by this class.
     *
     * This method should be called on the UI thread.
     */
    protected fun releaseSurfaceTexture() {
        surfaceTexture?.release()
        surfaceTexture = null
    }

    @CallSuper
    override fun onMainCreate(activity: Activity): View? {
        gastManager.registerGastRenderListener(this)
        gastManager.registerGastInputListener(this)
        return null
    }

    @CallSuper
    override fun onMainDestroy() {
        gastManager.unregisterGastRenderListener(this)
        gastManager.unregisterGastInputListener(this)
        releaseSurfaceTexture()
    }

    final override fun onFrameAvailable(surfaceTexture: SurfaceTexture) {
        updateTextureImageCounter.incrementAndGet()
    }

    final override fun onRenderDrawFrame() {
        var counter = updateTextureImageCounter.get()
        while (counter > 0) {
            surfaceTexture?.updateTexImage()
            counter = updateTextureImageCounter.decrementAndGet()
        }
    }

    override fun onMainInputAction(
        action: String,
        pressState: GastInputListener.InputPressState,
        strength: Float
    ) {
    }

    override fun onMainInputHover(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {

    }

    override fun onMainInputPress(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {

    }

    override fun onMainInputRelease(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {

    }

    override fun onMainInputScroll(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float,
        horizontalDelta: Float,
        verticalDelta: Float
    ) {
    }
}
