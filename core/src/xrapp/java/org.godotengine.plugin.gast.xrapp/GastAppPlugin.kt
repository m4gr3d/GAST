package org.godotengine.plugin.gast.xrapp

import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.plugin.GodotPluginInfoProvider
import org.godotengine.godot.plugin.SignalInfo
import org.godotengine.godot.plugin.UsedByGodot

/**
 * Godot plugin used to interact with the render (gdscript) logic on behalf of the driving app.
 */
class GastAppPlugin(private val enableXR: Boolean) : GodotPluginInfoProvider {

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

    internal fun startPassthrough(godot: Godot?) {
        if (godot == null || !enableXR) {
            return
        }
        GodotPlugin.emitSignal(godot, pluginName, START_PASSTHROUGH_SIGNAL)
    }

    internal fun stopPassthrough(godot: Godot?) {
        if (godot == null || !enableXR) {
            return
        }
        GodotPlugin.emitSignal(godot, pluginName, STOP_PASSTHROUGH_SIGNAL)
    }
}
