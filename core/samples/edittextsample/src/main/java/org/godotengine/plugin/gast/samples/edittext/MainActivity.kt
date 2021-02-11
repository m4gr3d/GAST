package org.godotengine.plugin.gast.samples.edittext

import android.os.Bundle
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPluginRegistry
import org.godotengine.godot.xr.XRMode
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.view.GastFrameLayout
import java.util.Arrays

/**
 * Initialize and render an EditText view in a VR environment.
 */
class MainActivity : Godot() {

    /**
     * Extension of a FrameLayout which leverages a {@link GastNode} node to render the view and its
     * children onto an OpenGL canvas.
     */
    private var editTextWrapperView: GastFrameLayout? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Retrieve our container view which is a child of the activity's main content view.
        val gastPlugin = getGastManager()
        val containerView = gastPlugin.rootView

        // Any view we want to render in VR needs to be added as a child to the containerView in
        // order to be hooked to the view lifecycle events.
        layoutInflater.inflate(R.layout.edittext_layout, containerView)
        editTextWrapperView = containerView.findViewById(R.id.wrapper_view)
    }

    // Signals that the Godot engine render loop has started. This is invoked on the render thread.
    override fun onGodotMainLoopStarted() {
        super.onGodotMainLoopStarted()

        // Generate a Godot node which has the ability to render/stream android views by leveraging
        // external GLES textures (https://source.android.com/devices/graphics/arch-st#ext_texture).
        // The second argument specifies the parent node the created node should be attached to.
        val gastNode = GastNode(getGastManager(), "EditTextContainer")

        runOnUiThread {
            editTextWrapperView?.initialize(getGastManager(), gastNode)
        }
    }

    private fun getGastManager(): GastManager {
        val gastPlugin = GodotPluginRegistry.getPluginRegistry().getPlugin("gast-core")
        if (gastPlugin !is GastManager) throw IllegalStateException("Unable to retrieve the Gast plugin.")

        return gastPlugin
    }

    // Override the engine starting parameters to indicate we want VR mode.
    override fun getCommandLine(): Array<String> {
        val original = super.getCommandLine()
        val updated: Array<String> = Arrays.copyOf(original, original.size + 1)
        updated[original.size] = XRMode.OVR.cmdLineArg
        return updated
    }
}
