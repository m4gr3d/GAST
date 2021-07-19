package org.godotengine.plugin.gast.extension

import android.app.Activity
import android.view.View
import androidx.annotation.CallSuper
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.plugin.GodotPluginRegistry
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.input.GastInputListener
import org.godotengine.plugin.gast.input.action.GastActionListener

/**
 * Base class for GAST extension plugins.
 *
 * Provides common logic to create a GAST extension.
 */
abstract class GastExtension(godot: Godot) : GodotPlugin(godot), GastActionListener,
    GastInputListener {

    protected val gastManager: GastManager by lazy {
        GodotPluginRegistry.getPluginRegistry().getPlugin("gast-core") as GastManager
    }

    @CallSuper
    override fun onMainCreate(activity: Activity): View? {
        gastManager.registerGastInputListener(this)
        return null
    }

    @CallSuper
    override fun onMainDestroy() {
        gastManager.unregisterGastInputListener(this)
    }

    override fun onMainInputAction(
        action: String,
        pressState: GastActionListener.InputPressState,
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
