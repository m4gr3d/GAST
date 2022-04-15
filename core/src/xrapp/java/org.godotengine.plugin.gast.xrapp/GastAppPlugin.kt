package org.godotengine.plugin.gast.xrapp

import android.os.Build
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.plugin.GodotPluginInfoProvider
import org.godotengine.godot.plugin.SignalInfo
import org.godotengine.godot.plugin.UsedByGodot

/**
 * Godot plugin used to interact with the render (gdscript) logic on behalf of the driving app.
 */
class GastAppPlugin(private val enableXR: Boolean) : ViewModel(), GodotPluginInfoProvider {

    class GastAppPluginFactory(private val enableXR: Boolean) : ViewModelProvider.Factory {
        override fun <T : ViewModel?> create(modelClass: Class<T>): T {
            return GastAppPlugin(enableXR) as T
        }
    }

    companion object {
        val START_PASSTHROUGH_SIGNAL = SignalInfo("start_passthrough")
        val STOP_PASSTHROUGH_SIGNAL = SignalInfo("stop_passthrough")

        val SIGNAL_INFOS = setOf(
            START_PASSTHROUGH_SIGNAL,
            STOP_PASSTHROUGH_SIGNAL,
        )
    }

    override fun getPluginName() = "GastAppPlugin"

    override fun getPluginSignals() = SIGNAL_INFOS

    @UsedByGodot
    private fun isXREnabled() = enableXR

    @UsedByGodot
    internal fun enable2DDebugMode(): Boolean {
        return Build.BRAND != "Oculus"
    }

    internal fun startPassthrough(godot: Godot?) {
        if (godot == null || !enableXR || enable2DDebugMode()) {
            return
        }
        GodotPlugin.emitSignal(godot, pluginName, START_PASSTHROUGH_SIGNAL)
    }

    internal fun stopPassthrough(godot: Godot?) {
        if (godot == null || !enableXR || enable2DDebugMode()) {
            return
        }
        GodotPlugin.emitSignal(godot, pluginName, STOP_PASSTHROUGH_SIGNAL)
    }
}
