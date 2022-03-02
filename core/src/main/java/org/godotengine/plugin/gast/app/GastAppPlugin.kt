package org.godotengine.plugin.gast.app

import org.godotengine.godot.plugin.GodotPluginInfoProvider
import org.godotengine.godot.plugin.UsedByGodot

/**
 * Godot plugin used to interact with the render (gdscript) logic on behalf of the driving app.
 */
class GastAppPlugin(private val enableXR: Boolean) : GodotPluginInfoProvider {

    override fun getPluginName() = "GastAppPlugin"

    @UsedByGodot
    fun isXREnabled() = enableXR
}
