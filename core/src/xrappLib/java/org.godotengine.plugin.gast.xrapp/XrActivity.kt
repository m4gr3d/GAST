package org.godotengine.plugin.gast.xrapp

import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.Gravity
import android.view.View
import android.view.ViewGroup
import android.widget.FrameLayout.LayoutParams
import androidx.activity.viewModels
import androidx.annotation.CallSuper
import androidx.annotation.IdRes
import androidx.annotation.LayoutRes
import androidx.appcompat.app.AppCompatActivity
import org.godotengine.godot.Godot
import org.godotengine.godot.GodotHost
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.xr.XRMode
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.R
import org.godotengine.plugin.gast.input.action.GastActionListener
import org.godotengine.plugin.gast.projectionmesh.ProjectionMesh
import org.godotengine.plugin.gast.xrapp.view.XrFrameLayout
import org.godotengine.plugin.gast.xrapp.view.XrView
import org.godotengine.plugin.vr.openxr.OpenXRPlugin
import java.util.Collections
import kotlin.system.exitProcess

/**
 * Container activity.
 *
 * Host and provide Gast related functionality for the driving app.
 */
abstract class XrActivity :
    AppCompatActivity(),
    GodotHost,
    OpenXRPlugin.EventListener,
    GastActionListener {

    companion object {
        private val TAG = XrActivity::class.java.simpleName

        private const val BACK_BUTTON_ACTION = "back_button_action"
        private const val MENU_BUTTON_ACTION = "menu_button_action"
    }

    private val appModel: XrAppModel by viewModels()

    internal var godotFragment: Godot? = null
    private var xrView: XrView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        appModel.enableXr = isXrEnabled()

        if (appModel.enableXr) {
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

            appModel.getOpenXRPlugin().registerEventListener(this)
            appModel.getGastManager().registerGastActionListener(this)
        }
    }

    override fun onDestroy() {
        if (appModel.enableXr) {
            appModel.getGastManager().unregisterGastActionListener(this)
            appModel.getOpenXRPlugin().unregisterEventListener(this)
        }

        super.onDestroy()
        onGodotForceQuit(godotFragment)
    }

    override fun onGodotForceQuit(instance: Godot?) {
        if (!appModel.enableXr) {
            return
        }

        if (instance === godotFragment) {
            exitProcess(0)
        }
    }

    private fun getGastContainerView(): ViewGroup? {
        if (!appModel.enableXr) {
            return null
        }

        if (godotFragment == null) {
            throw IllegalStateException("This must be called after super.onCreate(...)")
        }

        // Add this point, Godot::onCreate(...) should have already been called giving us access
        // the registered plugins.
        val gastPlugin = appModel.getGastManager()
        return gastPlugin.rootView
    }

    override fun <T : View?> findViewById(@IdRes id: Int): T {
        if (!appModel.enableXr) {
            return super.findViewById(id)
        }

        val container = xrView ?: return super.findViewById(id)
        return container.viewState.view.findViewById(id) ?: super.findViewById(id)
    }

    override fun setContentView(@LayoutRes layoutResID: Int) {
        if (!appModel.enableXr || godotFragment == null) {
            super.setContentView(layoutResID)
            return
        }

        setContentView(layoutInflater.inflate(layoutResID, getGastContainerView(), false))
    }

    override fun setContentView(view: View) {
        if (!appModel.enableXr || godotFragment == null) {
            super.setContentView(view)
            return
        }

        if (view.parent != null) {
            (view.parent as ViewGroup).removeView(view)
        }

        // Setup the layout params for the container view
        val viewLayoutParams = view.layoutParams ?: LayoutParams(
            LayoutParams.WRAP_CONTENT,
            LayoutParams.WRAP_CONTENT,
            Gravity.CENTER
        )

        if (viewLayoutParams.width == LayoutParams.MATCH_PARENT) {
            viewLayoutParams.width =
                resources.getDimensionPixelSize(R.dimen.default_container_width)
        }

        if (viewLayoutParams.height == LayoutParams.MATCH_PARENT) {
            viewLayoutParams.height =
                resources.getDimensionPixelSize(R.dimen.default_container_height)
        }

        val containerView = getGastContainerView()

        if (xrView != null) {
            containerView?.removeView(xrView!!.viewState.view)
        }

        // Any view we want to render in VR needs to be added as a child to the containerView in
        // order to be hooked to the view lifecycle events.
        xrView = if (view is XrView) {
            view
        } else {
            // Wrap the content view in a XrFrameLayout view.
            XrFrameLayout(this).apply {
                addView(view)
            }
        }
        containerView?.addView(xrView!!.viewState.view, 0, viewLayoutParams)
    }

    @CallSuper
    override fun onGodotSetupCompleted() {
        super.onGodotSetupCompleted()
        GodotPlugin.registerPluginWithGodotNative(appModel, appModel)
    }

    @CallSuper
    override fun onGodotMainLoopStarted() {
        // Complete setup and initializing of the XrFrameLayout
        super.onGodotMainLoopStarted()

        val gastManager = appModel.getGastManager()
        gastManager.defaultDepthDrawMode = ProjectionMesh.DepthDrawMode.ALWAYS
        val gastNode = GastNode(gastManager, "GastActivityContainer")
        runOnUiThread {
            xrView?.initialize(gastManager, gastNode)
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

    /**
     * Override the engine starting parameters to indicate we want VR mode.
     */
    override fun getCommandLine(): List<String> {
        return if (appModel.enableXr && !appModel.enable2DDebugMode())
            Collections.singletonList(XRMode.OVR.cmdLineArg)
        else
            Collections.emptyList()
    }

    protected open fun isXrEnabled() = false

    /**
     * Enables back button presses via the B/Y buttons for controllers or middle finger pinch for
     * tracked hands.
     */
    protected open fun isBackButtonEnabled() = false

    /**
     * Enable passthrough
     */
    fun startPassthrough() {
        appModel.startPassthrough(godotFragment)
    }

    /**
     * Disable passthrough
     */
    fun stopPassthrough() {
        appModel.stopPassthrough(godotFragment)
    }

    @CallSuper
    override fun onFocusGained() {
    }

    @CallSuper
    override fun onFocusLost() {
    }

    @CallSuper
    override fun onHeadsetMounted() {
    }

    @CallSuper
    override fun onHeadsetUnmounted() {
    }

    @CallSuper
    override fun onSessionBegun() {
    }

    @CallSuper
    override fun onSessionEnding() {
    }

    override fun getInputActionsToMonitor() = setOf(BACK_BUTTON_ACTION, MENU_BUTTON_ACTION)

    override fun onMainInputAction(
        action: String,
        pressState: GastActionListener.InputPressState,
        strength: Float
    ) {
        if (action == BACK_BUTTON_ACTION
            && pressState == GastActionListener.InputPressState.JUST_RELEASED
            && isBackButtonEnabled()
        ) {
            onBackPressed()
        } else if (action == MENU_BUTTON_ACTION
            && pressState == GastActionListener.InputPressState.JUST_RELEASED
        ) {
            openOptionsMenu()
        }
    }

}
