package org.godotengine.plugin.gast.xrapp

import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.Gravity
import android.view.View
import android.view.ViewGroup
import android.widget.FrameLayout.LayoutParams
import androidx.annotation.CallSuper
import androidx.annotation.IdRes
import androidx.annotation.LayoutRes
import androidx.appcompat.app.AppCompatActivity
import org.godotengine.godot.Godot
import org.godotengine.godot.GodotHost
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.plugin.GodotPluginRegistry
import org.godotengine.godot.xr.XRMode
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.R
import org.godotengine.plugin.gast.projectionmesh.RectangularProjectionMesh
import org.godotengine.plugin.gast.view.GastFrameLayout
import java.util.*
import kotlin.system.exitProcess

/**
 * Container activity.
 *
 * Host and provide Gast related functionality for the driving app.
 */
abstract class GastActivity : AppCompatActivity(), GodotHost {

    companion object {
        private val TAG = GastActivity::class.java.simpleName
    }

    private val enableXR = isXREnabled()
    private val appPlugin = GastAppPlugin(enableXR)

    private var godotFragment : Godot? = null
    private var gastFrameLayout: GastFrameLayout? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        if (enableXR) {
            super.setContentView(R.layout.godot_app_layout)
            val currentFragment =
                supportFragmentManager.findFragmentById(R.id.godot_fragment_container)
            if (currentFragment is Godot) {
                Log.v(TAG, "Reusing existing Godot fragment instance.")
                godotFragment = currentFragment
            } else {
                Log.v(TAG, "Creating new Godot fragment instance.")
                godotFragment = Godot()
                supportFragmentManager
                    .beginTransaction()
                    .replace(R.id.godot_fragment_container, godotFragment!!)
                    .setPrimaryNavigationFragment(godotFragment)
                    .commitNowAllowingStateLoss()
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        onGodotForceQuit(godotFragment)
    }

    override fun onGodotForceQuit(instance: Godot?) {
        if (!enableXR) {
            return
        }

        if (instance === godotFragment) {
            exitProcess(0)
        }
    }

    private fun getGastContainerView(): ViewGroup? {
        if (!enableXR) {
            return null
        }

        if (godotFragment == null) {
            throw IllegalStateException("This must be called after super.onCreate(...)")
        }

        // Add this point, Godot::onCreate(...) should have already been called giving us access
        // the registered plugins.
        val gastPlugin = getGastManager()
        return gastPlugin.rootView
    }

    override fun <T : View?> findViewById(@IdRes id: Int): T {
        if (!enableXR) {
            return super.findViewById(id)
        }

        val container = gastFrameLayout ?: return super.findViewById(id)
        return container.findViewById(id) ?: super.findViewById(id)
    }

    override fun setContentView(@LayoutRes layoutResID: Int) {
        if (!enableXR || godotFragment == null) {
            super.setContentView(layoutResID)
            return
        }

        setContentView(layoutInflater.inflate(layoutResID, getGastContainerView(), false))
    }

    override fun setContentView(view: View) {
        if (!enableXR || godotFragment == null) {
            super.setContentView(view)
            return
        }

        if (view.parent != null) {
            (view.parent as ViewGroup).removeView(view)
        }

        // Setup the layout params for the container view
        val viewLayoutParams = view.layoutParams
        val containerLayoutParams =
            LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT, Gravity.CENTER)
        if (viewLayoutParams == null || viewLayoutParams.width == LayoutParams.MATCH_PARENT) {
            containerLayoutParams.width =
                resources.getDimensionPixelSize(R.dimen.default_container_width)
        }

        if (viewLayoutParams == null || viewLayoutParams.height == LayoutParams.MATCH_PARENT) {
            containerLayoutParams.height =
                resources.getDimensionPixelSize(R.dimen.default_container_height)
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
        containerView?.addView(gastFrameLayout, 0, containerLayoutParams)
    }

    @CallSuper
    override fun onGodotSetupCompleted() {
        super.onGodotSetupCompleted()
        GodotPlugin.registerPluginWithGodotNative(appPlugin, appPlugin)
    }

    @CallSuper
    override fun onGodotMainLoopStarted() {
        // Complete setup and initializing of the GastFrameLayout
        super.onGodotMainLoopStarted()

        val gastNode = GastNode(getGastManager(), "GastActivityContainer")
        val projectionMesh = gastNode.getProjectionMesh()
        if (projectionMesh is RectangularProjectionMesh) {
            projectionMesh.setCurved(false)
        }

        runOnUiThread {
            gastFrameLayout?.initialize(getGastManager(), gastNode)
        }
    }

    override fun onNewIntent(intent: Intent?) {
        super.onNewIntent(intent)
        godotFragment?.onNewIntent(intent)
    }

    @CallSuper
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        godotFragment?.onActivityResult(requestCode, resultCode, data)
    }

    @CallSuper
    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String?>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        godotFragment?.onRequestPermissionsResult(requestCode, permissions, grantResults)
    }

    override fun onBackPressed() {
        godotFragment?.onBackPressed() ?: super.onBackPressed()
    }

    private fun getGastManager(): GastManager {
        val gastPlugin = GodotPluginRegistry.getPluginRegistry().getPlugin("gast-core")
        if (gastPlugin !is GastManager) {
            throw IllegalStateException("Unable to retrieve the Gast plugin.")
        }

        return gastPlugin
    }

    /**
     * Override the engine starting parameters to indicate we want VR mode.
     */
    override fun getCommandLine(): List<String> {
        return if (enableXR)
            Collections.singletonList(XRMode.OVR.cmdLineArg)
        else
            Collections.emptyList()
    }

    protected open fun isXREnabled() = false

    /**
     * Enable passthrough
     */
    fun startPassthrough() {
        if (enableXR) {
            appPlugin.startPassthrough(godotFragment)
        }
    }

    /**
     * Disable passthrough
     */
    fun stopPassthrough() {
        if (enableXR) {
            appPlugin.stopPassthrough(godotFragment)
        }
    }

}
