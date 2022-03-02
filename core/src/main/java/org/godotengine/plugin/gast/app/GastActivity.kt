package org.godotengine.plugin.gast.app

import android.view.View
import android.view.ViewGroup
import androidx.annotation.LayoutRes
import org.godotengine.godot.FullScreenGodotApp
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.plugin.GodotPluginRegistry
import org.godotengine.godot.xr.XRMode
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.view.GastFrameLayout
import java.util.*

/**
 * Container activity.
 *
 * Host and provide Gast related functionality for the driving app.
 */
abstract class GastActivity : FullScreenGodotApp() {

    companion object {
        private val TAG = GastActivity::class.java.simpleName
    }

    private val appPlugin = GastAppPlugin(isXREnabled())
    private var gastFrameLayout: GastFrameLayout? = null

    private fun getGastContainerView(): ViewGroup? {
        if (godotFragment == null) {
            throw IllegalStateException("This must be called after super.onCreate(...)")
        }

        // Add this point, Godot::onCreate(...) should have already been called giving us access
        // the registered plugins.
        val gastPlugin = getGastManager()
        return gastPlugin.rootView
    }

    override fun setContentView(@LayoutRes layoutResID: Int) {
        if (godotFragment == null) {
            super.setContentView(layoutResID)
            return
        }

        setContentView(layoutInflater.inflate(layoutResID, getGastContainerView(), false))
    }

    override fun setContentView(view: View) {
        if (godotFragment == null) {
            super.setContentView(view)
            return
        }

        if (view.parent != null) {
            (view.parent as ViewGroup).removeView(view)
        }

        val containerView = getGastContainerView()

        if (gastFrameLayout != null) {
            containerView?.removeView(gastFrameLayout)
        }

        // Any view we want to render in VR needs to be added as a child to the containerView in
        // order to be hooked to the view lifecycle events.
        gastFrameLayout = if (view is GastFrameLayout) {
            view
        } else {
            // Wrap the content view in a GastFrameLayout view.
            GastFrameLayout(this).apply {
                addView(view)
            }
        }
        containerView?.addView(gastFrameLayout, 0)
    }

    override fun onGodotSetupCompleted() {
        super.onGodotSetupCompleted()
        GodotPlugin.registerPluginWithGodotNative(appPlugin, appPlugin)
    }

    override fun onGodotMainLoopStarted() {
        // Complete setup and initializing of the GastFrameLayout
        super.onGodotMainLoopStarted()

        val gastNode = GastNode(getGastManager(), "GastActivityContainer")

        runOnUiThread {
            gastFrameLayout?.initialize(getGastManager(), gastNode)
        }
    }

    private fun getGastManager(): GastManager {
        val gastPlugin = GodotPluginRegistry.getPluginRegistry().getPlugin("gast-core")
        if (gastPlugin !is GastManager) throw IllegalStateException("Unable to retrieve the Gast plugin.")

        return gastPlugin
    }

    /**
     * Override the engine starting parameters to indicate we want VR mode.
     */
    override fun getCommandLine(): List<String> {
        return if (isXREnabled())
            Collections.singletonList(XRMode.OVR.cmdLineArg)
        else
            Collections.emptyList()
    }

    protected fun isXREnabled() = true

}
