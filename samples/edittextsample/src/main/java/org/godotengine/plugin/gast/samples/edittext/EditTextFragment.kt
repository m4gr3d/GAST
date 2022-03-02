package org.godotengine.plugin.gast.samples.edittext

import android.os.Bundle
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPluginRegistry
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.projectionmesh.RectangularProjectionMesh
import org.godotengine.plugin.gast.view.GastFrameLayout

/**
 * Initialize and render an EditText view in a VR environment.
 */
class EditTextFragment : Godot() {

    /**
     * Extension of a FrameLayout which leverages a {@link GastNode} node to render the view and its
     * children onto an OpenGL canvas.
     */
    private var editTextWrapperView: GastFrameLayout? = null
    private var messageWrapperView: GastFrameLayout? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Retrieve our container view which is a child of the activity's main content view.
        val gastPlugin = getGastManager()
        val containerView = gastPlugin.rootView

        // Any view we want to render in VR needs to be added as a child to the containerView in
        // order to be hooked to the view lifecycle events.
        layoutInflater.inflate(R.layout.edittext_layout, containerView)
        editTextWrapperView = containerView?.findViewById(R.id.edittext_wrapper_view)

        layoutInflater.inflate(R.layout.message_layout, containerView)
        messageWrapperView = containerView?.findViewById(R.id.message_wrapper_view)
    }

    // Signals that the Godot engine render loop has started. This is invoked on the render thread.
    override fun onGodotMainLoopStarted() {
        super.onGodotMainLoopStarted()

        // Generate a Godot node which has the ability to render/stream android views by leveraging
        // external GLES textures (https://source.android.com/devices/graphics/arch-st#ext_texture).
        // The second argument specifies the parent node the created node should be attached to.
        val gastNode = GastNode(getGastManager(), "EditTextContainer")
        gastNode.setGazeTracking(true)

        val messageGastNode = GastNode(getGastManager(), "MessageContainer")
        val projectionMesh = messageGastNode.getProjectionMesh()
        if (projectionMesh is RectangularProjectionMesh) {
            projectionMesh.setCurved(true)
        }

        runOnUiThread {
            editTextWrapperView?.initialize(getGastManager(), gastNode)
            messageWrapperView?.initialize(getGastManager(), messageGastNode)
        }
    }

    private fun getGastManager(): GastManager {
        val gastPlugin = GodotPluginRegistry.getPluginRegistry().getPlugin("gast-core")
        if (gastPlugin !is GastManager) throw IllegalStateException("Unable to retrieve the Gast plugin.")

        return gastPlugin
    }
}
