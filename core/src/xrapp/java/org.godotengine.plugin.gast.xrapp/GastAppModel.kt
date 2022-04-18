package org.godotengine.plugin.gast.xrapp

import android.os.Build
import androidx.lifecycle.ViewModel
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.plugin.GodotPluginInfoProvider
import org.godotengine.godot.plugin.GodotPluginRegistry
import org.godotengine.godot.plugin.SignalInfo
import org.godotengine.godot.plugin.UsedByGodot
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.projectionmesh.RectangularProjectionMesh
import org.godotengine.plugin.gast.view.GastView
import org.godotengine.plugin.vr.openxr.OpenXRPlugin

/**
 * Godot plugin used to interact with the render (gdscript) logic on behalf of the driving app.
 */
class GastAppModel() : ViewModel(), GodotPluginInfoProvider {

    companion object {
        val START_PASSTHROUGH_SIGNAL = SignalInfo("start_passthrough")
        val STOP_PASSTHROUGH_SIGNAL = SignalInfo("stop_passthrough")

        val SIGNAL_INFOS = setOf(
            START_PASSTHROUGH_SIGNAL,
            STOP_PASSTHROUGH_SIGNAL,
        )
    }

    internal var enableXR = false

    override fun getPluginName() = "GastAppModel"

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

    internal fun getGastManager(): GastManager {
        val gastPlugin = GodotPluginRegistry.getPluginRegistry().getPlugin("gast-core")
        if (gastPlugin !is GastManager) {
            throw IllegalStateException("Unable to retrieve the Gast plugin.")
        }

        return gastPlugin
    }

    internal fun getOpenXRPlugin(): OpenXRPlugin {
        val openxrPlugin = GodotPluginRegistry.getPluginRegistry().getPlugin("OpenXR")
        if (openxrPlugin !is OpenXRPlugin) {
            throw IllegalStateException("Unable to retrieve the OpenXR plugin.")
        }

        return openxrPlugin
    }

    internal fun setCurved(view: GastView, curved: Boolean) {
        val gastNode = view.viewState.gastNode ?: return
        val projectionMesh = gastNode.getProjectionMesh()
        if (projectionMesh is RectangularProjectionMesh) {
            projectionMesh.setCurved(curved)
        }
    }
}